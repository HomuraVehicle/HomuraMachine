#ifndef HMR_MACHINE_IO_INC
#define HMR_MACHINE_IO_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/future.hpp>
#include<homuraLib_v2/old/hmrVMC1.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Com.hpp"
#include"System_base.hpp"
#include"Message_base.hpp"
#include"Message.hpp"
#include"Device.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			//=== 基本定義 ===
		#define RF_BaudRate 9600
		#define RFDebug_BaudRate 38400
		#define MP_BaudRate 38400


			template<typename io_device_>
			struct cIO :public message_host_interface, public io_device_{
				typedef cIO<io_device_> this_type;
			private:
				//ロックフラグ
				bool Lock;
			private:
				//レジスタ系
				xc32::interrupt_uart<RF0_uart_register_> Uart0;
				xc32::interrupt_uart<RF1_uart_register_> Uart1;
				RF0_power PinPowerUart0;
				RF1_power PinPowerUart1;
			private:
				//ModuleID
				typedef enum{
					null = 0x00,
					mobile_phone = 0x01,
					rf_module = 0x02
				}rf_module_mode;
			private:
				//=== モード制御機能 ===
				typedef enum{ NormalMode, SleepMode, RoamingMode }mode;
				//デバイスモードを変更する
				void mode_set(mode Mode_);
				//デバイスモードを取得する
				volatile mode mode_get(void);
			private:
				//VMC1
				VMC1* pVMC;
				//受け取り用データ
				idata IData;
				bool IPacketMode = 0;
				odata OData;
				bool OPacketMode = 0;
				//メッセージホルダー
				cMessage Message;
			public:
				cIO(){}
				bool lock(){
					if(is_lock())return false;
					Lock = true;

					//通信関連機能の初期化
					pVMC = com::createVMC1();
					vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					com::initialize();

					devmng::courier::uart::fput_set_interrupt();//streamVMC_set_fput_interrupt_flag();//set_interrupt_fputflag(Stream_VMC);//割り込み予約
					devmng::interrupt_enable_streamVMC_fget_interrupt();// enable_interrupt_fget(Stream_VMC);
					devmng::interrupt_enable_streamVMC_fput_interrupt();// enable_interrupt_fput(Stream_VMC);


					return false;
				}
				bool is_lock()const{ return Lock; }
				void unlock(){
					Lock = false;

					//割り込み禁止
					devmng::courier::uart::fget_disable_interrupt();
					devmng::courier::uart::fput_disable_interrupt();

					//通信関連の終端化処理
					vmc1_finalize(pVMC);
					com::finalize();
					com::releaseVMC1(pVMC);
				}
			public:
				virtual void regist(message::element& rElement_){
					Message.regist(rElement_);
				}
			public:
				void operator()(void){
					//受信可能なデータがある場合
					if(!com::in_empty()){
						//受信データを取得
						com::in_move_pop(&IData);

						//中身が入っている場合
						if(idata_is_construct(&IData)){
							//Packetをまだ開いていない場合
							if(!IPacketMode){
								//Packetをここで開く
								IPacketMode = true;

								//受信用メッセージモジュール準備
								Message.setup_listen();
							}

							//Packet終了IDだった場合
							if(IData.ID == HMR_COM_PACTRMNID){
								//Packetをここで閉じる
								IPacketMode = false;

								//メッセージ破棄
								idata_destruct(&IData);
							} else{
								//メッセージ処理;
								Message.listen(&IData);
							}
						}
					}

					//早すぎるので待機
					delay_ms(5);

					//送信待ちのPacketがなく、comの送信バッファがいっぱいでもないとき
					if(!com::isWaitSendPacket() && !com::out_full()){
						//Packetをまだ開いていない場合
						if(!OPacketMode){
							//Packetをここで開く
							OPacketMode = true;
							//送信用メッセージモジュール準備
							Message.setup_talk();
						}

						//送信用データを準備する
						odata_format(&OData);

						//送信データの取得に失敗した場合
						if(Message.talk(&OData)){
							//中身が作成されていれば破棄
							if(odata_is_construct(&OData))odata_destruct(&OData);
							//Packetをここで閉じる
							OPacketMode = false;
							OData.ID = HMR_COM_PACTRMNID;
						}

						//送信データを送り出す
						com::out_move_push(&OData);
					}

					//送信割り込みが切られていて、かつ送信可能状態のときには、送信割り込みをオンにする
					if(!devmng::courier::uart::fput_is_interrupt_enable()){
						if(vmc1_can_send(pVMC))devmng::interrupt_enable_streamVMC_fput_interrupt();//uart1_enable_fput_interrupt();
					}
				}
			public://old functions
				   //=== rf module uart select ===
				   //現在アクティブなID
				rf_module_mode rf_module_uart_ModuleID = rf_module_mode::null;
				//ModuleIDのモジュールで初期化する
				void rf_module_uart_initialize(rf_module_mode ModuleID_, uint16 BaudRate_){
					if(ModuleID_ == rf_module_mode::mobile_phone){
						rf_module_uart_ModuleID = rf_module_mode::mobile_phone;
						Device.MobilePhone_uart.initialize(Device.MobilePhone_uart_register);
						Device.MobilePhone_uart.config(Device.MobilePhone_uart_Baudrate, Device.MobilePhone_uart_flowctrl, courier::uart::CourierTxInterruptFunc, courier::uart::CourierRxInterruptFunc);
						Device.MobilePhone_uart.lock();
						//Device.RF_uart.release();
					} else if(ModuleID_ == rf_module_mode::rf_module){
						rf_module_uart_ModuleID = rf_module_mode::rf_module;
						Device.RF_uart.initialize(Device.RF_uart_register);
						Device.RF_uart.config(Device.RF_uart_Baudrate, Device.RF_uart_flowctrl, courier::uart::CourierTxInterruptFunc, courier::uart::CourierRxInterruptFunc);
						Device.RF_uart.lock();
						//Device.MobilePhone_uart.release();
					}
				}
				//ModuleIDに電源を変更する
				void rf_module_uart_power_switch(bool onoff, rf_module_mode ModuleID_){
					if(onoff && ModuleID_ == rf_module_mode::mobile_phone){
						Device.PinMobilePhonePower(1);
						Device.PinRFPower(0);
					} else if(onoff && ModuleID_ == rf_module_mode::rf_module){
						Device.PinMobilePhonePower(0);
						Device.PinRFPower(1);
					} else{
						Device.PinMobilePhonePower(0);
						Device.PinRFPower(0);
					}
				}
				//現在のModuleIDを取得する
				rf_module_mode rf_module_uart_getModuleID(void){ return rf_module_uart_ModuleID; }
				// main stream 割り込みの許可禁止をいじくってよいかを返す関数
				bool interrupt_can_enable_streamVMC_fget_interrupt(){
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamVMC_fget_interrupt(){
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				bool interrupt_can_enable_streamVMC_fput_interrupt(){
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamVMC_fput_interrupt(){
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}

				// main stream の割り込み許可禁止関数
				bool interrupt_enable_streamVMC_fget_interrupt(){
					if(interrupt_can_enable_streamVMC_fget_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_enable();
						return true;
					} else{
						return false;
					}
				}
				bool interrupt_disable_streamVMC_fget_interrupt(){
					if(interrupt_can_disable_streamVMC_fget_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_disable();
						return true;
					} else{
						return false;
					}
				}
				bool interrupt_enable_streamVMC_fput_interrupt(){
					if(interrupt_can_enable_streamVMC_fput_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_enable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_enable();
						return true;
					} else{
						return false;
					}
				}
				bool interrupt_disable_streamVMC_fput_interrupt(){
					if(interrupt_can_disable_streamVMC_fput_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_disable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_disable();
						return true;
					} else{
						return false;
					}
				}

				uint8 fget(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)return Device.MobilePhone_uart.recv_data();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)return Device.RF_uart.recv_data();
					else return 0;
				}
				void fget_set_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_set_flag();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_set_flag();
				}
				void fget_clear_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_clear_flag();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_clear_flag();
				}
				void fget_enable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_enable();
				}
				void fget_disable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_disable();
				}
				bool fget_is_interrupt_enable(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)return Device.MobilePhone_uart.recv_is_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)return Device.RF_uart.recv_is_enable();
					else return false;
				}

				void fput(uint8 data_){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_data(data_);
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_data(data_);
				}
				if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_set_flag();
				void fput_set_interrupt(){
				else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_set_flag();
				}
				void fput_clear_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_clear_flag();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_clear_flag();
				}
				void fput_enable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_enable();
				}
				void fput_disable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_disable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_disable();
				}
				bool fput_is_interrupt_enable(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)return Device.MobilePhone_uart.send_is_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)return Device.RF_uart.send_is_enable();
					else return false;
				}
			public:
				//現在の通信先が全二重通信対応かを返す
				bool isFullDuplex(void);
			};
		}
	}
}
#
#endif
