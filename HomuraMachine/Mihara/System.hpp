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

				uint16 SleepSecRem;
				uint16 SleepSecNonRem;
				uint16 RoamingSecInterval;
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
				public:
					message_client(this_type& Ref_) :Ref(Ref_), InformTask(*this){
						Info_i = false;
						SleepMode_i = false;
						RoamingMode_i = false;
						NormalMode_i = false;
						KillCom_i = false;
						SleepModeCodeFail = false;
						RoamingModeCodeFail = false;
						KillCodeFail = false;

						//�^�X�N�o�^
						service::task::quick_start(InformTask, 5);
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
								devmng::mode_set(devmng::NormalMode);

								NormalMode_i = true;
								return false;

							case 0x10:// sleep mode
								if(hmLib::cstring_size(&Str) != 6)return true;
								SleepSecNonRem = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								SleepSecRem = ((uint16)hmLib::cstring_getc(&Str, 4) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 5) * 256;
								// sleep mode�@�ݒ菀��
								devmng::sleep_setInterval(SleepSecNonRem, SleepSecRem);
								//devmng::sleep_getInterval(&SleepSecNonRem, &SleepSecRem); 
								// code �擾
								_devmngmsg_sleep_Code = service::lockcode();

								SleepMode_i = true;
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
									SleepModeCodeFail = true;
								}
								return false;

							case 0x20:// roaming mode 
								if(hmLib::cstring_size(&Str) != 4)return true;
								RoamingSecInterval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
								// sleep mode�@�ݒ菀��
								devmng::roaming_setInterval(RoamingSecInterval);
								//devmng::roaming_getInterval(&RoamingSecInterval); 
								// code �擾
								_devmngmsg_roaming_Code = service::lockcode();
								RoamingMode_i = true;
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
									RoamingModeCodeFail = true;
								}
								return false;
							}

						case 0x20:
							_devmngmsg_kill_Code = service::lockcode();
							KillCom_i = true;
							return false;

						case 0x21:
							if(hmLib::cstring_size(&Str) != 2)return true;
							// code �̃`�F�b�N
							if(hmLib::cstring_getc(&Str, 1) == _devmngmsg_kill_Code){// ��v
																					 // kill �̎��s
								devmng::kill();
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
							hmLib::cstring_putc(pStr, 6, _devmngmsg_sleep_Code);

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
							hmLib::cstring_putc(pStr, 4, _devmngmsg_roaming_Code);

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
							hmLib::cstring_putc(pStr, 1, _devmngmsg_kill_Code);

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
				void regist(systems::element& rElement_){
					Chain.push_back(rElement_);
				}
			public:
				cSystem(unsigned char ID_):MessageClient(*this), MessageElement(message_client_holder(ID_,MessageClient)){
				}
			};
		}
	}
}
#
#endif