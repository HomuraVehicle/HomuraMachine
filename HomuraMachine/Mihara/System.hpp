#ifndef HMR_MACHINE_MIHARA_SYSTEM_INC
#define HMR_MACHINE_MIHARA_SYSTEM_INC 100
#
#include"System_base.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			struct cSystem : public system_host_interface{
				typedef cSystem this_type;
			private:
				systems::chain Chain;
			private:
				//Message Mode
				bool Info_i;				// ��񑗐M���[�h��
				bool SleepMode_i;			// sleep mode ��
				bool KillCom_i;				// kill command �� 
				bool SleepModeCodeFail;		// sleep mode exe �󗝂�����Code�F�����s
				bool RoamingModeCodeFail;	// roaming mode exe �󗝂��������s
				bool KillCodeFail;			// normal mode exe �󗝂��������s

				// ���b�N�p�̃R�[�h
				unsigned char _devmngmsg_sleep_Code = 0;	//���b�N�����p�̃R�[�h
				unsigned char _devmngmsg_roaming_Code = 0;	//���b�N�����p�̃R�[�h
				unsigned char _devmngmsg_kill_Code = 0;	//���b�N�����p�̃R�[�h

				uint16 _devmngmsg_sleep_sec_rem;
				uint16 _devmngmsg_sleep_sec_nonRem;
				unsigned char _devmngmsg_clockMode;
				uint16 _devmngmsg_roaming_sec_interval;
			private:
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						_devmngmsg_Mode.Info_i = true;
						return dt;
					}
				}InformTask;
			private:
				//�ʐM��̃N���X
				struct message_client : public message_client_interface{
				private:
					this_type& Ref;
				public:
					message_client(this_type& Ref_) :Ref(Ref_){
						_devmngmsg_Mode.Info_i = false;
						_devmngmsg_Mode.SleepMode_i = false;
						_devmngmsg_Mode.RoamingMode_i = false;
						_devmngmsg_Mode.NormalMode_i = false;
						_devmngmsg_Mode.KillCom_i = false;
						_devmngmsg_Mode.ClockMode_i = false;
						_devmngmsg_Mode.ClockModeFail = false;
						_devmngmsg_Mode.SleepModeCodeFail = false;
						_devmngmsg_Mode.RoamingModeCodeFail = false;
						_devmngmsg_Mode.KillCodeFail = false;
					}
				public:
					bool listen(hmLib::cstring Str){

						//�f�[�^�T�C�Y�m�F
						if(hmLib::cstring_size(&Str) == 0)return true;

						//1byte�ڂŃ��[�h�𕪂���
						switch(hmLib::cstring_getc(&Str, 0)){

							// Infomation mode
						case 0xA0:
							_devmngmsg_Mode.Info_i = true;
							return false;

							// �e��ҋ@���[�h�ݒ�
						case 0x10:
							if(hmLib::cstring_size(&Str) == 1)return true;
							switch(hmLib::cstring_getc(&Str, 1)){
							case 0x00:// normal 
								devmng::mode_set(devmng::NormalMode);

								_devmngmsg_Mode.NormalMode_i = true;
								return false;

							case 0x10:// sleep mode
								if(hmLib::cstring_size(&Str) != 6)return true;
								_devmngmsg_sleep_sec_nonRem = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								_devmngmsg_sleep_sec_rem = ((uint16)hmLib::cstring_getc(&Str, 4) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 5) * 256;
								// sleep mode�@�ݒ菀��
								devmng::sleep_setInterval(_devmngmsg_sleep_sec_nonRem, _devmngmsg_sleep_sec_rem);
								//devmng::sleep_getInterval(&_devmngmsg_sleep_sec_nonRem, &_devmngmsg_sleep_sec_rem); 
								// code �擾
								_devmngmsg_sleep_Code = service::lockcode();

								_devmngmsg_Mode.SleepMode_i = true;
								return false;

							case 0x11:// sleep execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code �̃`�F�b�N
								if(hmLib::cstring_getc(&Str, 2) == _devmngmsg_sleep_Code){// ��v
																						  // inform �֐��̃Z�b�g

																						  // sleep �̎��s
									devmng::mode_set(devmng::SleepMode);
								} else{
									// ���s�ʒm
									_devmngmsg_Mode.SleepModeCodeFail = true;
								}
								return false;

							case 0x20:// roaming mode 
								if(hmLib::cstring_size(&Str) != 4)return true;
								_devmngmsg_roaming_sec_interval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								// sleep mode�@�ݒ菀��
								devmng::roaming_setInterval(_devmngmsg_roaming_sec_interval);
								//devmng::roaming_getInterval(&_devmngmsg_roaming_sec_interval); 
								// code �擾
								_devmngmsg_roaming_Code = service::lockcode();
								_devmngmsg_Mode.RoamingMode_i = true;
								return false;

							case 0x21:// roaming execute mode
								if(hmLib::cstring_size(&Str) != 3)return true;
								// code �̃`�F�b�N
								if(hmLib::cstring_getc(&Str, 2) == _devmngmsg_roaming_Code){// ��v
																							// inform �֐��̃Z�b�g

																							// roaming �̎��s
									devmng::mode_set(devmng::RoamingMode);
								} else{
									// ���s�ʒm
									_devmngmsg_Mode.RoamingModeCodeFail = true;
								}
								return false;
							}

						case 0x20:
							_devmngmsg_kill_Code = service::lockcode();
							_devmngmsg_Mode.KillCom_i = true;
							return false;

						case 0x21:
							if(hmLib::cstring_size(&Str) != 2)return true;
							// code �̃`�F�b�N
							if(hmLib::cstring_getc(&Str, 1) == _devmngmsg_kill_Code){// ��v
																					 // kill �̎��s
								devmng::kill();
							} else{
								// ���s�ʒm
								_devmngmsg_Mode.KillCodeFail = true;
							}
							return false;

						case 0x30:
							if(hmLib::cstring_size(&Str) != 2)return true;
							// mode �̃`�F�b�N
							_devmngmsg_clockMode = hmLib::cstring_getc(&Str, 1);
							if(_devmngmsg_clockMode == 0){
								devmng::clock_set(devmng::NormalClock);
							} else if(_devmngmsg_clockMode == 1){
								devmng::clock_set(devmng::LowClock);
							} else if(_devmngmsg_clockMode == 2){
								devmng::clock_set(devmng::HighClock);
							} else{
								_devmngmsg_Mode.ClockModeFail = true;
								return false;
							}
							// clock mode ��
							_devmngmsg_Mode.ClockMode_i = true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						devmng::mode mode;
						devmng::clock clockMode;
						//hmLib_uint16 sumadc;
						if(_devmngmsg_Mode.Info_i){
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

							_devmngmsg_Mode.Info_i = false;
							return false;

						} else if(_devmngmsg_Mode.NormalMode_i){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0x00);

							_devmngmsg_Mode.NormalMode_i = false;
							return false;

						} else if(_devmngmsg_Mode.SleepMode_i){
							service::cstring_construct_safe(pStr, 7);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0x10);
							hmLib::cstring_putc(pStr, 2, (unsigned char)(_devmngmsg_sleep_sec_nonRem & 0x00FF));
							hmLib::cstring_putc(pStr, 3, (unsigned char)((_devmngmsg_sleep_sec_nonRem >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 4, (unsigned char)(_devmngmsg_sleep_sec_rem & 0x00FF));
							hmLib::cstring_putc(pStr, 5, (unsigned char)((_devmngmsg_sleep_sec_rem >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 6, _devmngmsg_sleep_Code);

							_devmngmsg_Mode.SleepMode_i = false;
							return false;

						} else if(_devmngmsg_Mode.SleepModeCodeFail){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0xF1);

							_devmngmsg_Mode.SleepModeCodeFail = false;
							return false;

						} else if(_devmngmsg_Mode.RoamingMode_i){
							service::cstring_construct_safe(pStr, 6);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0x20);
							hmLib::cstring_putc(pStr, 2, (unsigned char)(_devmngmsg_roaming_sec_interval & 0x00FF));
							hmLib::cstring_putc(pStr, 3, (unsigned char)((_devmngmsg_roaming_sec_interval >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 4, _devmngmsg_roaming_Code);

							//�ύX��̒ʐM��i��Ԃ�
							if(devmng::rf_module_uart_getModuleID() == devmng::rf_module){
								hmLib::cstring_putc(pStr, 5, 0x01); // ����RF�Ȃ玟��MP
							} else{
								hmLib::cstring_putc(pStr, 5, 0x00); // ����MP�Ȃ玟��RF 
							}

							_devmngmsg_Mode.RoamingMode_i = false;
							return false;

						} else if(_devmngmsg_Mode.RoamingModeCodeFail){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x10);
							hmLib::cstring_putc(pStr, 1, 0xF2);

							_devmngmsg_Mode.RoamingModeCodeFail = false;
							return false;

						} else if(_devmngmsg_Mode.KillCom_i){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x20);
							hmLib::cstring_putc(pStr, 1, _devmngmsg_kill_Code);

							_devmngmsg_Mode.KillCom_i = false;
							return false;
						} else if(_devmngmsg_Mode.KillCodeFail){
							service::cstring_construct_safe(pStr, 1);
							hmLib::cstring_putc(pStr, 1, 0xF2);

							_devmngmsg_Mode.KillCodeFail = false;
							return false;

						} else if(_devmngmsg_Mode.ClockMode_i){
							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x30);
							hmLib::cstring_putc(pStr, 1, _devmngmsg_clockMode);

							_devmngmsg_Mode.ClockMode_i = false;
							return false;

						} else if(_devmngmsg_Mode.ClockModeFail){
							service::cstring_construct_safe(pStr, 1);
							hmLib::cstring_putc(pStr, 0, 0xF3);

							_devmngmsg_Mode.ClockModeFail = false;
							return false;
						}

						return true;
					}
					void setup_listen(void){ return; }
					void setup_talk(void){ return; }
				}MessageClient;
				message::element MessageElement;
			public:
				//system host function
				void regist(systems::element& rElement_){
					Chain.push_back(rElement_);
				}
			public:
				cSystem(unsigned char ID_):MessageClient(*this), MessageElement(message_client_holder(ID_,MessageClient)){
					//�^�X�N�o�^
					service::task::quick_start(InformTask, 5);
				}
			};
		}
	}
}
#
#endif