#ifndef HMR_MACHINE_IO_INC
#define HMR_MACHINE_IO_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/future.hpp>
#include<homuraLib_v2/old/hmrVMC1.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/gate.hpp>
#include"System_base.hpp"
#include"Device.hpp"
#include"IO_base.hpp"
#include"IO/Message.hpp"
#include"IO/ComClient.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename io_device_>
			struct cIO :public io_interface, public io::mode_selector_interface, public io_device_ {
				typedef cIO<io_device_> this_type;
			private:
				struct cDualUart : public io_device_ {
				private:
					//レジスタ系
					xc32::interrupt_uart<typename io_device_::RF0_uart_register> RFUart;
					xc32::interrupt_uart<typename io_device_::RF1_uart_register> PhoneUart;
					typename io_device_::RF0_power PinPowerRFUart;
					typename io_device_::RF1_power PinPowerPhoneUart;
					//現在のモード
					io::mode::type Mode;
					//文字列変換モジュールVMC1
					VMC1* pVMC;
				private:
					//送受信用タスク VMCと割り込みレジスタをつなぐ
					struct tx_interrupt :public xc32::sfr::interrupt::function {
					private:
						cDualUart& Ref;
					public:
						tx_interrupt(cDualUart& Ref_) :Ref(Ref_) {}
						void operator()(void) {
							if (Ref.getMode() == io::mode::module_rf) {
								//送信可能なら、送信
								Ref.RFUart.send_data(vmc1_send(Ref.pVMC));

								//送信可能でなくなった場合は、割り込みを切る
								if (!vmc1_can_send(Ref.pVMC)) {
									Ref.RFUart.send_disable();
								}
							} else if (Ref.getMode() == io::mode::module_phone) {
								//送信可能なら、送信
								Ref.PhoneUart.send_data(vmc1_send(Ref.pVMC));

								//送信可能でなくなった場合は、割り込みを切る
								if (!vmc1_can_send(Ref.pVMC)) {
									Ref.PhoneUart.send_disable();
								}
							}
						}
					};
					struct rx_interrupt :public xc32::sfr::interrupt::function {
					private:
						cDualUart& Ref;
					public:
						rx_interrupt(cDualUart& Ref_) :Ref(Ref_) {}
						void operator()(void) {
							if (Ref.getMode() == io::mode::module_rf) {
								//データを受信し、Comに処理させる
								vmc1_recv(Ref.pVMC, Ref.RFUart.recv_data());
							} else if (Ref.getMode() == io::mode::module_phone) {
								//データを受信し、Comに処理させる
								vmc1_recv(Ref.pVMC, Ref.PhoneUart.recv_data());
							}
						}
					};
				private:
					tx_interrupt TxInterrupt;
					rx_interrupt RxInterrupt;
				public:
					cDualUart()
						: TxInterrupt(*this)
						, RxInterrupt(*this)
						, Mode(io::mode::type::module_null) {
						PinPowerRFUart.lock();
						PinPowerPhoneUart.lock();

						//通信関連機能の初期化
						pVMC = com::createVMC1();
						vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					}
					~cDualUart() {
						//VMC解放
						vmc1_finalize(pVMC);
						com::releaseVMC1(pVMC);

						setMode(io::mode::type::module_null);
						PinPowerRFUart.unlock();
						PinPowerPhoneUart.unlock();
					}
				public:
					//モジュール選択
					void setMode(io::mode::type ModuleMode_) {
						//同じモードなら、無視
						if (ModuleMode_ == Mode)return;

						Mode = ModuleMode_;

						switch (Mode) {
						case io::mode::type::module_phone:
							PinPowerRFUart(false);
							RFUart.unlock();
							PinPowerPhoneUart(true);
							PhoneUart.lock(38400, xc32::uart::flowcontrol::rts_cts_control, TxInterrupt, RxInterrupt);
							PhoneUart.send_disable();
							PhoneUart.recv_enable();
							return;
						case io::mode::type::module_rf:
							PinPowerPhoneUart(false);
							PhoneUart.unlock();
							PinPowerRFUart(true);
							RFUart.lock(9600, xc32::uart::flowcontrol::rts_cts_control, TxInterrupt, RxInterrupt);
							RFUart.send_disable();
							RFUart.recv_enable();
							return;
						default:
							PinPowerRFUart(false);
							RFUart.unlock();
							PinPowerPhoneUart(false);
							PhoneUart.unlock();
							return;
						}
					}
					//現モジュール取得
					io::mode::type getMode()const { return Mode; }
					//どこも使ってなければ、falseを返す
					operator bool()const { return Mode != io::mode::type::module_null; }
					//Uart駆動関数
					void operator()(void) {
						//送信割り込みが切られていて、かつ送信可能状態のときには、送信割り込みをオンにする
						if (vmc1_can_send(pVMC)) {
							if (Mode == io::mode::module_rf && !RFUart.send_is_enable()) {
								//enableにした瞬間割り込みが入るのを防ぐため、一度クリア
								RFUart.send_clear_flag();
								RFUart.send_enable();
							} else if (Mode == io::mode::module_phone && !PhoneUart.send_is_enable()) {
								//enableにした瞬間割り込みが入るのを防ぐため、一度クリア
								PhoneUart.send_clear_flag();
								PhoneUart.send_enable();
							}
						}
					}
				};
			private:
				systems::io_agent_interface* pSystemAgent;
				struct com_client : public io::com_client_interface {
				private:
					this_type& Ref;
				public:
					com_client(this_type& Ref_) :Ref(Ref_) {}
				public://io_com_client_interface
					virtual void inform_timeout() {
						Ref.pSystemAgent->timeout();
					}
					virtual bool is_fullduplex()const {
						return io::mode::module_phone == Ref.Uart.getMode();
					}
				}ComClient;
			private:
				struct system_client : public system_client_interface {
				private:
					this_type& Ref;
				public:
					system_client(this_type& Ref_) :Ref(Ref_) {}
				public://system_clien_interface
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_) {
						if (NewMode_ == systems::mode::sleep)com::wdt_disable();
						else {
							com::wdt_restart();
							com::wdt_enable();
						}
					}
				}SystemClient;
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
				cIO(service_interface& Service)
					: pSystemAgent(0)
					, ComClient(*this)
					, SystemClient(*this)
					, Uart()
					, IData()
					, IPacketMode(false)
					, OData()
					, OPacketMode(false)
					, Message() {
					com::initialize(Service, ComClient);
				}
				~cIO() {
					//通信関連の終端化処理
					com::finalize();
				}
			public:
				//message_host_interface
				virtual void regist(message_client_interface& Client_) {
					Message.regist(Client_);
				}
			public:
				//module_selector_interface
				virtual bool setModuleMode(io::mode::type Mode_) {
					Uart.setMode(Mode_);
					return false;
				}
				virtual io::mode::type getModuleMode()const {
					return Uart.getMode();
				}
			public:
				system_client_interface& getSystemClient() { return SystemClient; }
				void operator()(void) {
					//スリープなら終了
					if (Uart.getMode() == io::mode::module_null)return;

					//受信可能なデータがある場合
					if (!com::in_empty()) {
						//受信データを取得
						com::in_move_pop(&IData);

						//中身が入っている場合
						if (idata_is_construct(&IData)) {
							//Packetをまだ開いていない場合
							if (!IPacketMode) {
								//Packetをここで開く
								IPacketMode = true;

								//受信用メッセージモジュール準備
								Message.setup_listen();
							}

							//Packet終了IDだった場合
							if (IData.ID == HMR_COM_PACTRMNID) {
								//Packetをここで閉じる
								IPacketMode = false;

								//メッセージ破棄
								idata_destruct(&IData);
							} else {
								//メッセージ処理;
								Message.listen(&IData);
							}
						}
					}

					//早すぎるので待機
					//delay_ms(5);

					//送信待ちのPacketがなく、comの送信バッファがいっぱいでもないとき
					if (!com::isWaitSendPacket() && !com::out_full()) {
						//Packetをまだ開いていない場合
						if (!OPacketMode) {
							//Packetをここで開く
							OPacketMode = true;
							//送信用メッセージモジュール準備
							Message.setup_talk();
						}

						//送信用データを準備する
						odata_format(&OData);

						//送信データの取得に失敗した場合
						if (Message.talk(&OData)) {
							//中身が作成されていれば破棄
							if (odata_is_construct(&OData))odata_destruct(&OData);
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
				void regis_system_agent(systems::io_agent_interface& IOAgent_) {
					pSystemAgent = &IOAgent_;
				}
			};
		}
	}
}
#
#endif
