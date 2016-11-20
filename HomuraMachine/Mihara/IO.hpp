#ifndef HMR_MACHINE_IO_INC
#define HMR_MACHINE_IO_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/future.hpp>
#include<homuraLib_v2/old/hmrVMC1.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include<homuraLib_v2/gate.hpp>
#include"Com.hpp"
#include"System_base.hpp"
#include"Message_base.hpp"
#include"Message.hpp"
#include"Device.hpp"
#include"IO_base.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			template<typename io_device_>
			struct cIO :public message_host_interface, public io::module_selector_interface, public io_device_{
				typedef cIO<io_device_> this_type;
			private:
				struct tx_interrupt{
				private:
					this_type& Ref;
				public:
					void operator()(void){
						//送信可能なら、送信
						Ref.Uart.putc(vmc1_send(pVMC);

						//送信可能でなくなった場合は、割り込みを切る
						if(!vmc1_can_send(pVMC)){
							Ref.Uart.disable_tx_interrupt();
						}
					}
				};
				struct rx_interrput{
				private:
					this_type& Ref;
				public:
					void operator()(void){
						//データを受信し、Comに処理させる
						vmc1_recv(pVMC, Ref.Uart.getc());
					}
				};
			private:
				struct cDualUart: public io_device_{
					typedef cUartSwitcher<io_device_> this_type;
				public:
				private:
					tx_interrupt TxInterrupt;
					rx_interrupt RxInterrupt;
				private:
					//レジスタ系
					xc32::interrupt_uart<RF0_uart_register> RFUart;
					xc32::interrupt_uart<RF1_uart_register> PhoneUart;
					RF0_power PinPowerRFUart;
					RF1_power PinPowerPhoneUart;
					//現在のモード
					io::module_mode Mode;
				public:
					cDualUart(const tx_interrupt& TxInterrupt_, const rx_interrupt& RxInterrupt_)
						: TxInterrupt(TxInterrupt_)
						, RxInterrupt(RxInterrupt_)
						, Mode(module_null){
						PinPowerRFUart.lock();
						PinPowerPhoneUart.lock();
					}
					~cDualUart(){
						setMode(module_null);
						PinPowerRFUart.unlock();
						PinPowerPhoneUart.unlock();
					}
					void setMode(io::module_mode ModuleMode_){
						//同じモードなら、無視
						if(ModuleMode_ == Mode)return;

						Mode = ModuleMode_;
						switch(Mode){
						case io::module_phone:
							RFUart.unlock();
							PhoneUart.lock(38400, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PinPowerRFUartlock(false);
							PinPowerPhoneUartlock(true);
						case io::module_rf:
							RFUart.lock(9600, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PhoneUart.unlock();
							PinPowerRFUartlock(true);
							PinPowerPhoneUartlock(false);
						default:
							RFUart.unlock();
							PhoneUart.unlock();
							PinPowerRFUartlock(false);
							PinPowerPhoneUartlock(false);
						}
					}
					io::module_mode getMode()const{ return Mode; }
					operator bool()const{ return Mode == io::module_null; }
				public:
					//送信文字列を1byte取得する
					unsigned char getc(){
						switch(Mode){
						case io::module_rf:
							return RFUart.recv_data();
						case io::module_phone:
							return PhoneUart.recv_data();
						default:
							return 0;
						}
					}
					//受信文字列を1byte与える
					void putc(unsigned char c){
						switch(Mode){
						case io::module_rf:
							RFUart.send_data(c);
							return;
						case io::module_phone:
							PhoneUart.send_data(c);
							return;
						default:
							return;
						}
					}
					//tx interrupt enable
					void enable_tx_interrupt(){
						switch(Mode){
						case io::module_rf:
							RFUart.send_enable();
							return;
						case io::module_phone:
							PhoneUart.send_enable();
							return;
						default:
							return;
						}
					}
					//tx interrupt disable
					void disable_tx_interrupt(){
						switch(Mode){
						case io::module_rf:
							RFUart.send_disable();
							return;
						case io::module_phone:
							PhoneUart.send_disable();
							return;
						default:
							return;
						}
					}
				};
			private:
				//Uart
				cDualUart Uart;
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
				cIO(){
					//通信関連機能の初期化
					pVMC = com::createVMC1();
					vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					com::initialize();
				}
				~cIO{
					//通信関連の終端化処理
					vmc1_finalize(pVMC);
					com::finalize();
					com::releaseVMC1(pVMC);	
				}
			public:
				//message_host_interface
				virtual void regist(message::element& rElement_){
					Message.regist(rElement_);
				}
			public:
				//module_selector_interface
				virtual bool setModuleMode(io::module_mode Mode_){
					Uart.setMode(Mode_);
					return false;
				}
				virtual io::module_mode getModuleMode()const{
					return Uart.getMode();
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
						if(vmc1_can_send(pVMC))Uart.enable_tx_interrupt();
					}
				}
			};
		}
	}
}
#
#endif
