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
				struct cDualUart: public io_device_{
				private:
					//レジスタ系
					xc32::interrupt_uart<RF0_uart_register> RFUart;
					xc32::interrupt_uart<RF1_uart_register> PhoneUart;
					RF0_power PinPowerRFUart;
					RF1_power PinPowerPhoneUart;
					//現在のモード
					io::module_mode Mode;
					//送信割り込み状態
					bool IsTxInterruptEnable;
					//文字列変換モジュールVMC1
					VMC1* pVMC;
				private:
					//送受信用タスク VMCと割り込みレジスタをつなぐ
					struct tx_interrupt{
					private:
						cDualUart& Ref;
					public:
						tx_interrupt(cDualUart& Ref_) :Ref(Ref_){}
						void operator()(void){
							//送信可能なら、送信
							Ref.putc(vmc1_send(Ref.pVMC);

							//送信可能でなくなった場合は、割り込みを切る
							if(!vmc1_can_send(Ref.pVMC)){
								Ref.disable_tx_interrupt();
							}
						}
					};
					struct rx_interrput{
					private:
						cDualUart& Ref;
					public:
						rx_interrput(cDualUart& Ref_) :Ref(Ref_){}
						void operator()(void){
							//データを受信し、Comに処理させる
							vmc1_recv(Ref.pVMC, Ref.getc());
						}
					};
				private:
					tx_interrupt TxInterrupt;
					rx_interrupt RxInterrupt;
				private:
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
							IsTxInterruptEnable = true;
							return;
						case io::module_phone:
							PhoneUart.send_enable();
							IsTxInterruptEnable = true;
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
							IsTxInterruptEnable = false;
							return;
						case io::module_phone:
							PhoneUart.send_disable();
							IsTxInterruptEnable = false;
							return;
						default:
							return;
						}
					}
					//interrupt再開を試みる
					void check_tx_interrupt(){
						//送信割り込みが切られていて、かつ送信可能状態のときには、送信割り込みをオンにする
						if(Mode != io::module_null && !IsTxInterruptEnable && vmc1_can_send(pVMC)){
							Uart.enable_tx_interrupt();
						}
					}
				public:
					cDualUart()
						: TxInterrupt(*this)
						, RxInterrupt(*this)
						, Mode(module_null){
						PinPowerRFUart.lock();
						PinPowerPhoneUart.lock();

						//通信関連機能の初期化
						pVMC = com::createVMC1();
						vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					}
					~cDualUart(){
						//VMC解放
						vmc1_finalize(pVMC);
						com::releaseVMC1(pVMC);

						setMode(module_null);
						PinPowerRFUart.unlock();
						PinPowerPhoneUart.unlock();
					}
				public:
					//モジュール選択
					void setMode(io::module_mode ModuleMode_){
						//同じモードなら、無視
						if(ModuleMode_ == Mode)return;

						Mode = ModuleMode_;

						IsTxInterruptEnable = false;
						switch(Mode){
						case io::module_phone:
							RFUart.unlock();
							PhoneUart.lock(38400, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PinPowerRFUart(false);
							PinPowerPhoneUart(true);
							check_tx_interrupt();
						case io::module_rf:
							PhoneUart.unlock();
							RFUart.lock(9600, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PinPowerRFUart(true);
							PinPowerPhoneUart(false);
							check_tx_interrupt();
						default:
							RFUart.unlock();
							PhoneUart.unlock();
							PinPowerRFUart(false);
							PinPowerPhoneUart(false);
						}
					}
					//現モジュール取得
					io::module_mode getMode()const{ return Mode; }
					//どこも使ってなければ、falseを返す
					operator bool()const{ return Mode != io::module_null; }
					//Uart駆動関数
					void operator()(void){
						check_tx_interrupt();
					}
				};
			private:
				//Uart
				cDualUart Uart;
				//受け取り用データ
				idata IData;
				bool IPacketMode;
				odata OData;
				bool OPacketMode;
				//メッセージホルダー
				cMessage Message;
			public:
				cIO()
					: Uart()
					, IData()
					, IPacketMode(false)
					, OData()
					, OPacketMode(false)
					, Message(){
					com::initialize();
				}
				~cIO{
					//通信関連の終端化処理
					com::finalize();
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

					//Uart駆動
					Uart();
				}
			};
		}
	}
}
#
#endif
