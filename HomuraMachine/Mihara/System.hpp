#ifndef HMR_MACHINE_MIHARA_SYSTEM_INC
#define HMR_MACHINE_MIHARA_SYSTEM_INC 100
#
#include<XCBase/lock.hpp>
#include<homuraLib_v2/task.hpp>
#include<homuraLib_v2/lock_code.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"System_base.hpp"
#include"Service_base.hpp"
#include"IO_base.hpp"
#include"DeviceManage.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			template<typename system_device_>
			struct cSystem : public system_interface, public system_device_{
				typedef system_device_ my_device;
				typedef cSystem<system_device_> this_type;
			private:
				typename my_device::pinDevicePower PinDevicePower;
				xc::lock_guard<typename my_device::pinDevicePower> PinDevicePowerLock;
			private:
				typedef io::mode::type io_mode;
				typedef systems::mode::type system_mode;
				struct status{
				private:
					struct io_mode_holder{
					private:
						//!現在通信可能なIO
						io_mode CurIO;
						//!現在の通信モード OFFの時やスイッチング中はCurIOとは異なる
						io_mode IOMode;
						io::mode_selector_interface* pIOModeSelector;
					public:
						io_mode getSelectIO()const{ return CurIO; }
						void setSelectIO(bool UseAlternative=false){
							if(UseAlternative){
								set(CurIO == io::mode::module_phone ? io::mode::module_rf : io::mode::module_phone);
							} else{
								set(CurIO);
							}
						}
						io_mode get()const{ return IOMode; }
						void set(io_mode IOMode_){
							if(IOMode != IOMode_){
								pIOModeSelector->setModuleMode(IOMode_);
								IOMode = IOMode_;

								if(IOMode != io::mode::module_null){
									CurIO = IOMode;
								}
							}
						}
					public:
						io_mode_holder(io::mode_selector_interface& IOModeSelector_, io_mode IOMode = io_mode::module_null)
							: pIOModeSelector(&IOModeSelector_){
							set(IOMode);
						}
					};
					struct system_mode_holder{
					private:
						system_mode SensorMode;
						systems::chain Chain;
					public:
						system_mode get()const{ return SensorMode; }
						void set(system_mode SystemMode_){
							if(SensorMode != SystemMode_){
								for(systems::chain::iterator Itr = Chain.begin(); Itr != Chain.end(); ++Itr){
									(*Itr)(SystemMode_, SensorMode);
								}
								SensorMode = SystemMode_;
							}
						}
						void regist(system_client_interface& rElement_){
							rElement_(SensorMode, SensorMode);
							Chain.push_back(rElement_);
						}
					public:
						system_mode_holder(system_mode Mode_ = system_mode::sleep){
							set(Mode_);
						}
					};
				public:
					typename my_device::pinRedLED PinRedLED;
					xc32::wdt WDT;
					io_mode_holder IOMode;
					system_mode_holder SystemMode;
				private:
					xc::lock_guard<typename my_device::pinRedLED> PinRedLEDLock;
					xc::lock_guard<xc32::wdt> WDTLock;
				public:
					status(io::mode_selector_interface& IOModeSelector_, io_mode IOMode_ = io_mode::module_null, system_mode SystemMode_ = system_mode::sleep)
						: PinRedLEDLock(PinRedLED)
						, IOMode(IOModeSelector_, IOMode_)
						, SystemMode(SystemMode_)
						, WDTLock(WDT){
						PinRedLED(false);
						WDT.disable();
					}
				}Status;
				struct mode_interface{
				public:
					//!@brief モード遷移後、最初の処理
					virtual void start(status& Status) = 0;
					//!@brief モード中のタスク呼び出し時の処理
					virtual void task(hmr::task::duration Duration, status& Status) = 0;
					//!@brief モード中のメインループ呼び出し時の処理
					virtual void work(status& Status) = 0;
				};
				struct normal_mode : public mode_interface{
				private:
					unsigned int Cnt = 0;
				public:
					//!@brief モード遷移後、最初の処理
					virtual void start(status& Status){
						Status.IOMode.setSelectIO();
						Status.SystemMode.set(systems::mode::observe);
						Status.WDT.enable();
					}
					//!@brief モード中のタスク呼び出し時の処理
					virtual void task(hmr::task::duration Duration, status& Status){
						if(Cnt == 0 || (Cnt == 2 && Status.IOMode.getSelectIO() == io::mode::module_phone)){
							Status.PinRedLED(true);
						} else{
							Status.PinRedLED(false);
						}
						Cnt = (Cnt + 1)%4;
					}
					//!@brief モード中のメインループ呼び出し時の処理
					virtual void work(status& Status){
						Status.WDT.clear();
					}
				public:
					normal_mode() :Cnt(0){}
				};
				struct sleep_mode : public mode_interface{
				private:
					unsigned int Cnt;
					//!@brief 全体のモード遷移の基準となるインターバル
					unsigned int Interval;
					//!@brief Rem（通信起動時間）の長さ Intervalと一致すると、SleepせずRoamingのみ
					unsigned int RemPhaseLength;
					//!@brief 通信機自動切り替え探索の間隔 0,だと切り替えない
					unsigned int RoamingInterval;
				public:
					sleep_mode() :Cnt(0), RemPhaseLength(60), Interval(120), RoamingInterval(30){}
					void set(unsigned int Cnt_, unsigned int Interval_, unsigned int RemPhaseLength_, unsigned int RoamingInterval_){
						Cnt = Cnt_;
						Interval = Interval_;
						RemPhaseLength = RemPhaseLength_;
						RoamingInterval = RoamingInterval_;
					}
					void start(status& Status){
						Status.WDT.disable();

						if(Cnt<RemPhaseLength){
							if(RoamingInterval== 0 || (Cnt%RoamingInterval) % 2 == 0){
								Status.IOMode.setSelectIO();
							} else{
								Status.IOMode.setSelectIO(true);
							}
							Status.SystemMode.set(systems::mode::passive);
						} else{
							Status.IOMode.set(io::mode::module_null);
							Status.SystemMode.set(systems::mode::sleep);
						}
					}
					void task(hmr::task::duration Duration, status& Status){
						bool PrevIsRem = (Cnt<RemPhaseLength);
						bool PreDefaultIO = (RoamingInterval == 0 || (Cnt%RoamingInterval) % 2 == 0);
						Cnt = ((Cnt + Duration) % Interval);
						bool IsRem = (Cnt<RemPhaseLength);
						bool DefaultIO = (RoamingInterval == 0 || (Cnt%RoamingInterval) % 2 == 0);


						if(PrevIsRem != IsRem){
							if(IsRem){
								Status.IOMode.setSelectIO();
								Status.SystemMode.set(systems::mode::passive);
							} else{
								Status.IOMode.set(io::mode::module_null);
								Status.SystemMode.set(systems::mode::sleep);
							}
						} else if(PreDefaultIO != DefaultIO){
							if(DefaultIO){
								Status.IOMode.setSelectIO();
							} else{
								Status.IOMode.setSelectIO(true);
							}
						}

						if(Cnt<RemPhaseLength){
							if(Status.IOMode.getSelectIO() == io::mode::module_phone){
								Status.PinRedLED(Cnt % 3 == 0);
							} else{
								Status.PinRedLED(Cnt % 3 != 0);
							}
						} else{
							Status.PinRedLED(Cnt % 4<2);
						}
					}
					void work(status& Status){}
				};	
				mode_interface* pMode;
				normal_mode NormalMode;
				sleep_mode RoamingMode;
				sleep_mode SleepMode;
			private:
				void kill(){
					PinDevicePower(false);
				}
			private:
				struct io_agent : systems::io_agent_interface{
				private:
					this_type& Ref;
				public:
					io_agent(this_type& Ref_):Ref(Ref_){}
				public:
					virtual void timeout(){ Ref.kill(); }
				};
				io_agent IOAgent;
			public:
				systems::io_agent_interface& getIOAgent(){ return IOAgent; }
			public:
			private:
				class system_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					system_task(this_type& Ref_) :Ref(Ref_){}
					duration operator()(duration dt){
						Ref.pMode->task(dt, Ref.Status);
						return dt;
					}
				};
				system_task SystemTask;
				task::handler SystemTaskHandler;
			public:
				void operator()(void){
					pMode->work(Status);
				}
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
					task::handler InformTaskHandler;
				private:
					//Message Mode
					bool Info_i;				// 情報送信モード受理
					bool SleepMode_i;			// sleep mode 受理
					bool RoamingMode_i;
					bool NormalMode_i;
					bool KillCom_i;				// kill command 受理 
					bool SleepModeCodeFail;		// sleep mode exe 受理したがCode認識失敗
					bool RoamingModeCodeFail;	// roaming mode exe 受理したが失敗
					bool KillCodeFail;			// normal mode exe 受理したが失敗
					// ロック用のコード
					unsigned char SleepCode = 0;	//ロック解除用のコード
					unsigned char RoamingCode = 0;	//ロック解除用のコード
					unsigned char KillCode = 0;	//ロック解除用のコード
					//
					unsigned int SleepSecNonRem = 0;
					unsigned int SleepSecRem = 0;
					unsigned int RoamingSecInterval = 0;
				public:
					message_client(this_type& Ref_, com::did_t ID_, service_interface& Service_)
						: message_client_interface(ID_)
						, Ref(Ref_)
						, InformTask(*this)
						, SleepSecNonRem(0)
						, SleepSecRem(0)
						, RoamingSecInterval(0){
						Info_i = false;
						SleepMode_i = false;
						RoamingMode_i = false;
						NormalMode_i = false;
						KillCom_i = false;
						SleepModeCodeFail = false;
						RoamingModeCodeFail = false;
						KillCodeFail = false;

						//タスク登録
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~message_client(){
						InformTaskHandler.stop();
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
								Ref.pMode = &Ref.NormalMode;
								Ref.pMode->start(Ref.Status);

								NormalMode_i = true;
								return false;

							case 0x10:// sleep mode
								if(hmLib::cstring_size(&Str) != 6)return true;
								SleepSecNonRem = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								SleepSecRem = ((uint16)hmLib::cstring_getc(&Str, 4) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 5) * 256;
								// sleep mode　設定準備
								Ref.SleepMode.set(SleepSecRem, SleepSecRem+ SleepSecNonRem, SleepSecRem, 0);
								//devmng::sleep_getInterval(&SleepSecNonRem, &SleepSecRem); 
								// code 取得
								SleepCode = hmr::lockcode();

								SleepMode_i = true;
								return false;

							case 0x11:// sleep execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code のチェック
								if(hmLib::cstring_getc(&Str, 2) == SleepCode){// 一致
									Ref.pMode = &Ref.SleepMode;
									Ref.pMode->start(Ref.Status);
								} else{
									// 失敗通知
									SleepModeCodeFail = true;
								}
								return false;

							case 0x20:// roaming mode 
								if(hmLib::cstring_size(&Str) != 4)return true;
								RoamingSecInterval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								// sleep mode　設定準備
								Ref.RoamingMode.set(0, RoamingSecInterval*2, RoamingSecInterval*2, RoamingSecInterval);
								//devmng::roaming_getInterval(&RoamingSecInterval); 
								// code 取得
								RoamingCode = hmr::lockcode();
								RoamingMode_i = true;
								return false;

							case 0x21:// roaming execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code のチェック
								if(hmLib::cstring_getc(&Str, 2) == RoamingCode){// 一致
									Ref.pMode = &Ref.RoamingMode;
									Ref.pMode->start(Ref.Status);
								} else{
									// 失敗通知
									RoamingModeCodeFail = true;
								}
								return false;
							}

						case 0x20:
							KillCode = hmr::lockcode();
							KillCom_i = true;
							return false;

						case 0x21:
							if(hmLib::cstring_size(&Str) != 2)return true;
							// code のチェック
							if(hmLib::cstring_getc(&Str, 1) == KillCode){// 一致
								Ref.kill();
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
						//hmLib_uint16 sumadc;
						if(Info_i){
							service::cstring_construct_safe(pStr, 4);
							hmLib::cstring_putc(pStr, 0, 0xA0);

							if(Ref.pMode == &(Ref.NormalMode)){
								hmLib::cstring_putc(pStr, 1, 0x00);
							} else if(Ref.pMode == &(Ref.SleepMode)){
								hmLib::cstring_putc(pStr, 1, 0x10);
							} else if(Ref.pMode == &(Ref.RoamingMode)){
								hmLib::cstring_putc(pStr, 1, 0x20);
							} else {
								hmLib::cstring_putc(pStr, 1, 0xFF);
							}

							//クロックモード　廃止したので、現在未使用
							hmLib::cstring_putc(pStr, 2, 0x00);

							// RF or MP 
							if(Ref.Status.IOMode.getSelectIO() == io::mode::module_rf){
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
							if(Ref.Status.IOMode.getSelectIO() == io::mode::module_rf){
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
			public:
				//system host function
				void regist(system_client_interface& rElement_){
					Status.SystemMode.regist(rElement_);
				}
			public:
				cSystem(unsigned char ID_, io_interface& MessageHost_, service_interface& Service_, io::mode_selector_interface& IOModeSelectorInterface)
					: PinDevicePower()
					, PinDevicePowerLock(PinDevicePower)
					, Status(IOModeSelectorInterface)
					, IOAgent(*this)
					, SystemTask(*this)
					, MessageClient(*this, ID_, Service_)
					, pMode(&NormalMode){
					Status.IOMode.set(io::mode::module_phone);
					Status.SystemMode.set(systems::mode::observe);
					SystemTaskHandler = Service_.system_task().quick_start(SystemTask, 1);
					MessageHost_.regist(MessageClient);
					PinDevicePower(true);
					pMode->start(Status);
				}
				~cSystem(){
					SystemTaskHandler.stop();
				}
			};
		}
	}
}
#
#endif
