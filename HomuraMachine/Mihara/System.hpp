#ifndef HMR_MACHINE_MIHARA_SYSTEM_INC
#define HMR_MACHINE_MIHARA_SYSTEM_INC 100
#
#include<XCBase/lock.hpp>
#include"System_base.hpp"
#include"IO_base.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			template<typename system_device_>
			struct cSystem : public system_interface, public system_device_{
				typedef cSystem this_type;
			private:
				/*!@brief cSystemから制御するために使うモード分類。*/
				enum io_mode{
					/*!通信機器を通常状態で待機する。*/
					io_passive,
					/*!通信機器の定期的なON-OFFを繰り返す。*/
					io_sleep, 
					/*!通信機器の通信モードを切り替え続ける。*/
					io_roaming, 
					/*!通信機器の定期的なON-OFFを繰り返しつつ、通信モードも切り替え続ける。*/
					io_sleep_roaming 
				};
			private:
				systems::chain Chain;
			private:
				pinRedLED PinRedLED;
				pinDevicePower PinDevicePower;
				xc::lock_guard<pinRedLED> PinRedLEDLock;
				xc::lock_guard<pinDevicePower> PinDevicePowerLock;
			private:
				xc32::wdt WDT;
				xc::lock_guard<xc32::wdt> WDTLock;
			private:
				io::module_selector_interface* pIO;
			private:
				//モード制御クラス
				struct mode_driver{
				private:
					sensor_mode SensorMode;
					io_mode IOMode;
				private:
					uint16 SleepSecRem;
					uint16 SleepSecNonRem;
					uint16 RoamingSecInterval;
				public:
					mode_driver()
						: SensorMode(sensor_sleep)
						, IOMode(io_sleep)
						, SleepSecRem(0)
						, SleepSecNonRem(0)
						, RoamingSecInterval(0){
					}
				};
			private:
				//通信受領クラス
				struct message_client : public message_client_interface{
				private:
					this_type& Ref;
				public:
					struct inform_task :public hmr::task::client_interface{
					private:
						message_client& RefMessageClient;
					public:
						inform_task(message_client& RefMessageClient_) :RefMessageClient(RefMessageClient_){}
						duration operator()(duration dt){
							RefMessageClient.inform();
							return dt;
						}
					}InformTask;
				private:
					//Message Mode
					bool Info_i;				// 情報送信モード受理
					bool SleepMode_i;			// sleep mode 受理
					bool KillCom_i;				// kill command 受理 
					bool SleepModeCodeFail;		// sleep mode exe 受理したがCode認識失敗
					bool RoamingModeCodeFail;	// roaming mode exe 受理したが失敗
					bool KillCodeFail;			// normal mode exe 受理したが失敗
					// ロック用のコード
					unsigned char SleepCode = 0;	//ロック解除用のコード
					unsigned char RoamingCode = 0;	//ロック解除用のコード
					unsigned char KillCode = 0;	//ロック解除用のコード
				public:
					message_client(this_type& Ref_, service_interface& Service_) :Ref(Ref_), InformTask(*this){
						Info_i = false;
						SleepMode_i = false;
						RoamingMode_i = false;
						NormalMode_i = false;
						KillCom_i = false;
						SleepModeCodeFail = false;
						RoamingModeCodeFail = false;
						KillCodeFail = false;

						//タスク登録
						task::quick_start(InformTask, 5);
					}
				public:
					bool listen(hmLib::cstring Str){

						//データサイズ確認
						if(hmLib::cstring_size(&Str) == 0)return true;

						//1byte目でモードを分ける
						switch(hmLib::cstring_getc(&Str, 0)){

							// Infomation mode
						case 0xA0:
							Info_i = true;
							return false;

							// 各種待機モード設定
						case 0x10:
							if(hmLib::cstring_size(&Str) == 1)return true;
							switch(hmLib::cstring_getc(&Str, 1)){
							case 0x00:// normal 
								devmng::mode_set(devmng::NormalMode);

								NormalMode_i = true;
								return false;

							case 0x10:// sleep mode
								if(hmLib::cstring_size(&Str) != 6)return true;
								SleepSecNonRem = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								SleepSecRem = ((uint16)hmLib::cstring_getc(&Str, 4) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 5) * 256;
								// sleep mode　設定準備
								devmng::sleep_setInterval(SleepSecNonRem, SleepSecRem);
								//devmng::sleep_getInterval(&SleepSecNonRem, &SleepSecRem); 
								// code 取得
								SleepCode = service::lockcode();

								SleepMode_i = true;
								return false;

							case 0x11:// sleep execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code のチェック
								if(hmLib::cstring_getc(&Str, 2) == SleepCode){// 一致
																						  // inform 関数のセット

																						  // sleep の実行
									devmng::mode_set(devmng::SleepMode);
								} else{
									// 失敗通知
									SleepModeCodeFail = true;
								}
								return false;

							case 0x20:// roaming mode 
								if(hmLib::cstring_size(&Str) != 4)return true;
								RoamingSecInterval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								// sleep mode　設定準備
								devmng::roaming_setInterval(RoamingSecInterval);
								//devmng::roaming_getInterval(&RoamingSecInterval); 
								// code 取得
								RoamingCode = service::lockcode();
								RoamingMode_i = true;
								return false;

							case 0x21:// roaming execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code のチェック
								if(hmLib::cstring_getc(&Str, 2) == RoamingCode){// 一致
																							// inform 関数のセット

																							// roaming の実行
									devmng::mode_set(devmng::RoamingMode);
								} else{
									// 失敗通知
									RoamingModeCodeFail = true;
								}
								return false;
							}

						case 0x20:
							KillCode = service::lockcode();
							KillCom_i = true;
							return false;

						case 0x21:
							if(hmLib::cstring_size(&Str) != 2)return true;
							// code のチェック
							if(hmLib::cstring_getc(&Str, 1) == KillCode){// 一致
																					 // kill の実行
								devmng::kill();
							} else{
								// 失敗通知
								KillCodeFail = true;
							}
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						devmng::mode mode;
						devmng::clock clockMode;
						//hmLib_uint16 sumadc;
						if(Info_i){
							service::cstring_construct_safe(pStr, 4);
							hmLib::cstring_putc(pStr, 0, 0xA0);

							mode = devmng::mode_get();
							if(mode == devmng::NormalMode){
								hmLib::cstring_putc(pStr, 1, 0x00);
							} else if(mode == devmng::SleepMode){
								hmLib::cstring_putc(pStr, 1, 0x10);
							} else if(mode == devmng::RoamingMode){
								hmLib::cstring_putc(pStr, 1, 0x20);
							}

							clockMode = devmng::clock_get();
							if(clockMode == devmng::NormalClock){
								hmLib::cstring_putc(pStr, 2, 0x00);
							} else if(clockMode == devmng::LowClock){
								hmLib::cstring_putc(pStr, 2, 0x01);
							} else if(clockMode == devmng::HighClock){
								hmLib::cstring_putc(pStr, 2, 0x02);
							}

							// RF or MP 
							if(devmng::rf_module_uart_getModuleID() == devmng::rf_module){
								hmLib::cstring_putc(pStr, 3, 0x00);
							} else{
								hmLib::cstring_putc(pStr, 3, 0x01);
							}

							Info_i = false;
							return false;

						} else if(NormalMode_i){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0x00);

							NormalMode_i = false;
							return false;

						} else if(SleepMode_i){
							service::cstring_construct_safe(pStr, 7);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0x10);
							hmLib::cstring_putc(pStr, 2, (unsigned char)(SleepSecNonRem & 0x00FF));
							hmLib::cstring_putc(pStr, 3, (unsigned char)((SleepSecNonRem >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 4, (unsigned char)(SleepSecRem & 0x00FF));
							hmLib::cstring_putc(pStr, 5, (unsigned char)((SleepSecRem >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 6, SleepCode);

							SleepMode_i = false;
							return false;

						} else if(SleepModeCodeFail){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0xF1);

							SleepModeCodeFail = false;
							return false;

						} else if(RoamingMode_i){
							service::cstring_construct_safe(pStr, 6);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0x20);
							hmLib::cstring_putc(pStr, 2, (unsigned char)(RoamingSecInterval & 0x00FF));
							hmLib::cstring_putc(pStr, 3, (unsigned char)((RoamingSecInterval >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 4, RoamingCode);

							//変更先の通信手段を返す
							if(devmng::rf_module_uart_getModuleID() == devmng::rf_module){
								hmLib::cstring_putc(pStr, 5, 0x01); // 現状がRFなら次はMP
							} else{
								hmLib::cstring_putc(pStr, 5, 0x00); // 現状がMPなら次はRF 
							}

							RoamingMode_i = false;
							return false;

						} else if(RoamingModeCodeFail){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0xF2);

							RoamingModeCodeFail = false;
							return false;

						} else if(KillCom_i){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x20);
							hmLib::cstring_putc(pStr, 1, KillCode);

							KillCom_i = false;
							return false;
						} else if(KillCodeFail){
							service::cstring_construct_safe(pStr, 1);
							hmLib::cstring_putc(pStr, 1, 0xF2);

							KillCodeFail = false;
							return false;
						}

						return true;
					}
					void setup_listen(void){ return; }
					void setup_talk(void){ return; }
				public:
					void inform(){
						Info_i = true;
					}
				}MessageClient;
				message::element MessageElement;
			public:
				//system host function
				void regist(systems::value_type& rElement_){
					Chain.push_back(rElement_);
				}
			public:
				cSystem(unsigned char ID_, message_host& MessageHost_)
					: PinRedLED()
					, PinDevicePower()
					, PinRedLEDLock(PinRedLED)
					, PinDevicePowerLock(PinDevicePower)
					, MessageClient(*this)
					, MessageElement(message_client_holder(ID_,MessageClient)){
					MessageHost_.regist(MessageElement);
				}
				void operator()(void){
				}
			};
		}
	}
}
#
#endif
