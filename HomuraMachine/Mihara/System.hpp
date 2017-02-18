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
						//!���ݒʐM�\��IO
						io_mode CurIO;
						//!���݂̒ʐM���[�h OFF�̎���X�C�b�`���O����CurIO�Ƃ͈قȂ�
						io_mode IOMode;
						io::mode_selector_interface* pIOModeSelector;
					public:
						io_mode getSelectIO()const{ return CurIO; }
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
									SensorMode = SystemMode_;
								}
							}
						}
						void regist(system_client_interface& rElement_){
							Chain.push_back(rElement_);
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
					status()
						: PinRedLEDLock(PinRedLED)
						, WDTLock(WDT){
						PinRedLED(false);
						WDT.disable();
					}
				}Status;
				struct mode_interface{
				public:
					//!@brief ���[�h�J�ڌ�A�ŏ��̏���
					virtual void start(status& Status) = 0;
					//!@brief ���[�h���̃^�X�N�Ăяo�����̏���
					virtual void task(hmr::task::duration Duration, status& Status) = 0;
					//!@brief ���[�h���̃��C�����[�v�Ăяo�����̏���
					virtual void work(status& Status) = 0;
				};
				struct normal_mode : public mode_interface{
				private:
					unsigned int Cnt = 0;
				public:
					//!@brief ���[�h�J�ڌ�A�ŏ��̏���
					virtual void start(status& Status){
						Status.WDT.enable();
					}
					//!@brief ���[�h���̃^�X�N�Ăяo�����̏���
					virtual void task(hmr::task::duration Duration, status& Status){
						if(Cnt == 0 || (Cnt == 2 && Status.IOMode.getSelectIO() == io::mode::module_phone)){
							Status.PinRedLED(true);
						} else{
							Status.PinRedLED(false);
						}
						Cnt = (Cnt + 1)%4;
					}
					//!@brief ���[�h���̃��C�����[�v�Ăяo�����̏���
					virtual void work(status& Status){
						Status.WDT.clear();
					}
				public:
					normal_mode() :Cnt(0){}
				};
				struct sleep_mode : public mode_interface{
				private:
					unsigned int Cnt;
					//!@brief �S�̂̃��[�h�J�ڂ̊�ƂȂ�C���^�[�o��
					unsigned int Interval;
					//!@brief Rem�i�ʐM�N�����ԁj�̒��� Interval�ƈ�v����ƁASleep����Roaming�̂�
					unsigned int RemPhaseLength;
					//!@brief �ʐM�@�����؂�ւ��T���̊Ԋu 0,���Ɛ؂�ւ��Ȃ�
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
								Status.IOMode.set(Status.getSelectIO());
							} else{
								Status.IOMode.set(Status.getSelectIO() == io::mode::module_phone ? io::mode::module_rf : io::mode::module_phone);
							}
							Status.setSystemMode(systems::mode::passive);
						} else{
							Status.setIOMode(io::mode::module_null);
							Status.setSystemMode(systems::mode::sleep);
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
								Status.setIOMode(Status.IOMode.getSelectIO());
								Status.setSystemMode(systems::mode::passive);
							} else{
								Status.setIOMode(io::mode::module_null);
								Status.setSystemMode(systems::mode::sleep);
							}
						} else if(PreDefaultIO != DefaultIO){
							if(DefaultIO){
								Status.IOMode.set(Status.getSelectIO());
							} else{
								Status.IOMode.set(Status.getSelectIO() == io::mode::module_phone ? io::mode::module_rf : io::mode::module_phone);
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
				sleep_mode RoamingSleepMode;
			private:
				void kill(){
					PinDevicePower(false);
				}
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
				//�ʐM��̃N���X
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
					bool Info_i;				// ��񑗐M���[�h��
					bool SleepMode_i;			// sleep mode ��
					bool RoamingMode_i;
					bool NormalMode_i;
					bool KillCom_i;				// kill command �� 
					bool SleepModeCodeFail;		// sleep mode exe �󗝂�����Code�F�����s
					bool RoamingModeCodeFail;	// roaming mode exe �󗝂��������s
					bool KillCodeFail;			// normal mode exe �󗝂��������s
					// ���b�N�p�̃R�[�h
					unsigned char SleepCode = 0;	//���b�N�����p�̃R�[�h
					unsigned char RoamingCode = 0;	//���b�N�����p�̃R�[�h
					unsigned char KillCode = 0;	//���b�N�����p�̃R�[�h
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

						//�^�X�N�o�^
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~message_client(){
						InformTaskHandler.stop();
					}
				public:
					bool listen(hmLib::cstring Str){

						//�f�[�^�T�C�Y�m�F
						if(hmLib::cstring_size(&Str) == 0)return true;

						//1byte�ڂŃ��[�h�𕪂���
						switch(hmLib::cstring_getc(&Str, 0)){

							// Infomation mode
						case 0xA0:
							Info_i = true;
							return false;

							// �e��ҋ@���[�h�ݒ�
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
								// sleep mode�@�ݒ菀��
								Ref.SleepMode.set(SleepSecRem, SleepSecRem+ SleepSecNonRem, SleepSecRem, 0);
								//devmng::sleep_getInterval(&SleepSecNonRem, &SleepSecRem); 
								// code �擾
								SleepCode = hmr::lockcode();

								SleepMode_i = true;
								return false;

							case 0x11:// sleep execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code �̃`�F�b�N
								if(hmLib::cstring_getc(&Str, 2) == SleepCode){// ��v
									Ref.pMode = &Ref.SleepMode;
									Ref.pMode->start(Ref.Status);
								} else{
									// ���s�ʒm
									SleepModeCodeFail = true;
								}
								return false;

							case 0x20:// roaming mode 
								if(hmLib::cstring_size(&Str) != 4)return true;
								RoamingSecInterval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								// sleep mode�@�ݒ菀��
								Ref.RoamingMode.set(0, RoamingSecInterval*2, RoamingSecInterval*2, RoamingSecInterval);
								//devmng::roaming_getInterval(&RoamingSecInterval); 
								// code �擾
								RoamingCode = hmr::lockcode();
								RoamingMode_i = true;
								return false;

							case 0x21:// roaming execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code �̃`�F�b�N
								if(hmLib::cstring_getc(&Str, 2) == RoamingCode){// ��v
									Ref.pMode = &Ref.RoamingMode;
									Ref.pMode->start(Ref.Status);
								} else{
									// ���s�ʒm
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
							// code �̃`�F�b�N
							if(hmLib::cstring_getc(&Str, 1) == KillCode){// ��v
								Ref.kill();
							} else{
								// ���s�ʒm
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

							//�ύX��̒ʐM��i��Ԃ�
							if(devmng::rf_module_uart_getModuleID() == devmng::rf_module){
								hmLib::cstring_putc(pStr, 5, 0x01); // ����RF�Ȃ玟��MP
							} else{
								hmLib::cstring_putc(pStr, 5, 0x00); // ����MP�Ȃ玟��RF 
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
				cSystem(unsigned char ID_, io_interface& MessageHost_, service_interface& Service_)
					: PinDevicePower()
					, PinDevicePowerLock(PinDevicePower)
					, MessageClient(*this, ID_, Service_)
					, SystemTask(*this)
					, pMode(&NormalMode){

					SystemTaskHandler = Service_.task().quick_start(SystemTask, 5);
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
