#ifndef HMR_DEVICEMANAGEMESSAGE_C_INC
#define HMR_DEVICEMANAGEMESSAGE_C_INC 102
#
/*
===hmrDeviceManage===

v1_02/131019 amby
	���b�Z�[�W�֐�����A���̑��̋@�\�̓C���^�[�t�F�[�X�ցc

v1_01/131013 hmIto
	�C���^�[�t�F�[�X��啝�ɕύX������
		���[�h����N���b�N���x�w������O
*/

/*===hmrDeviceManage===
'D'
���[�h�擾�R�}���h
>> 0xA0 
<< 0xA0, DeviceManageMode, ClockModeSpeed  
	DeviceManageMode : 0x00:normal, 0x10:sleep, 0x20:roaming  
	ClockModeSpeed   : 0x00:normal, 0x01:low, 0x02:high  

Device Manage Mode �ݒ� 

�X���[�v�ݒ�R�}���h�i���s�͂���Ȃ��j
>> 0x10, 0x10, secSleepLength[2],secRemLength[2]
	secSleepLength�b�Ő����T�C�N���B�����AsecSleepLength�b����������ԁB
	secSleepLength��sint16�̐����A�܂�1�`32767�b��9���ԋ�������
<< 0x10, 0x10, secSleepLength[2],secRemLength[2],Code
	�ݒ�l�Ǝ��s�p�R�[�h���Ԃ��Ă���
<< 0x10, 0xF1
	�ݒ�l���ُ�ȏꍇ�́A0xF1���߂�

�X���[�v���s�R�}���h
>> 0x10, 0x11, Code
	���s�p�R�[�h�𑗐M���āA���s���m�F
		���O�̃X���[�v�ݒ�R�}���h�Ŏ擾�������s�p�R�[�h�̂ݗL��
<< �Ȃ��i�����j


���[�~���O�ݒ�R�}���h�i���s�͂���Ȃ��j
>> 0x10, 0x20, ModuleID[1],secRoamLength[1]
	ModuleID�̒ʐM���W���[����L���ɂ���
		ModuleID 0:RF, 1:MP
	secRoamLength�b�ҋ@������A���̃��W���[���ɖ߂�(���m�ɂ�secRoamLength�b���ƂɒT�������ς���)
	secRoamLength�ɂ�uint8�̐����A�܂�1�`255�b������
<< 0x10, 0x20,ModuleID,Code
	�ݒ�l�Ǝ��s�p�R�[�h���Ԃ��Ă���
<< 0x10, 0xF2
	�ݒ�l���ُ�ȏꍇ�́A0xF2���߂�
���[�~���O���s�R�}���h
>> 0x10, 0x21,Code
	���s�p�R�[�h�𑗐M���āA���s���m�F
		���O�̃X���[�v�ݒ�R�}���h�Ŏ擾�������s�p�R�[�h�̂ݗL��
<< �Ȃ��i���[�~���O�j

���[�h���Z�b�g�i�m�[�}�����[�h���s�j�R�}���h
>> 0x10, 0x00
	�m�[�}�����[�h
<< 0x10, 0x00


KILL�ݒ�R�}���h�i���s�͂���Ȃ��j
>> 0x20
<< 0x20,Code
	�ݒ�l�Ǝ��s�p�R�[�h���Ԃ��Ă���
<< 0xF2
	�ݒ�l���ُ�ȏꍇ�́A0xF1���߂�
KILL���s�R�}���h
>> 0x21,Code
	���s�p�R�[�h�𑗐M���āA���s���m�F
		���O��KILL�ݒ�R�}���h�Ŏ擾�������s�p�R�[�h�̂ݗL��
<< �Ȃ��i�i���j


�N���b�N�ݒ�p�R�}���h
>> 0x30, Speed
	Speed : 0:normal, 1:low, 2:high 	
<< 0x30, Speed 

initialize , sleep �Ƃ��̃C���t�H�[�����l����

*/
#include<homuraLib_v2/machine/service/lock_code.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"DeviceManageMessage.hpp"
#include"DeviceManage.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			namespace devmngmsg{
				namespace{
					// mode flag
					struct{
						unsigned Info_i : 1;	     // ��񑗐M���[�h��

						unsigned SleepMode_i : 1;  // sleep mode ��
						//unsigned SleepModeFail:1; // sleep mode �󗝎��s(���͂�������������)

						unsigned RoamingMode_i : 1;// roaming mode ��
						//unsigned RoamingModeFail:1; // roaming mode �󗝎��s

						unsigned NormalMode_i : 1; // normal mode ��
						unsigned KillCom_i : 1;    // kill command �� 

						unsigned ClockMode_i : 1;  // clock �ύX�R�}���h��
						unsigned ClockModeFail : 1;

						unsigned SleepModeCodeFail : 1;   // sleep mode exe �󗝂�����Code�F�����s
						unsigned RoamingModeCodeFail : 1; // roaming mode exe �󗝂��������s
						unsigned KillCodeFail : 1;		  // normal mode exe �󗝂��������s
					}_devmngmsg_Mode = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

					// ���b�N�p�̃R�[�h
					unsigned char _devmngmsg_sleep_Code = 0;	//���b�N�����p�̃R�[�h
					unsigned char _devmngmsg_roaming_Code = 0;	//���b�N�����p�̃R�[�h
					unsigned char _devmngmsg_kill_Code = 0;	//���b�N�����p�̃R�[�h

					uint16 _devmngmsg_sleep_sec_rem;
					uint16 _devmngmsg_sleep_sec_nonRem;
					unsigned char _devmngmsg_clockMode;
					uint16 _devmngmsg_roaming_sec_interval;
					//unsigned char _devmngmsg_roaming_ID;
				}
				bool listen(hmLib::cstring Str){

					//�f�[�^�T�C�Y�m�F
					if (hmLib::cstring_size(&Str) == 0)return true;

					//1byte�ڂŃ��[�h�𕪂���
					switch (hmLib::cstring_getc(&Str, 0)){

						// Infomation mode
					case 0xA0:
						_devmngmsg_Mode.Info_i = true;
						return false;

						// �e��ҋ@���[�h�ݒ�
					case 0x10:
						if (hmLib::cstring_size(&Str) == 1)return true;
						switch (hmLib::cstring_getc(&Str, 1)){
						case 0x00:// normal 
							devmng::mode_set(devmng::NormalMode);

							_devmngmsg_Mode.NormalMode_i = true;
							return false;

						case 0x10:// sleep mode
							if (hmLib::cstring_size(&Str) != 6)return true;
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
							if (hmLib::cstring_size(&Str) != 3)return true;
							// code �̃`�F�b�N
							if (hmLib::cstring_getc(&Str, 2) == _devmngmsg_sleep_Code){// ��v
								// inform �֐��̃Z�b�g

								// sleep �̎��s
								devmng::mode_set(devmng::SleepMode);
							}
							else{
								// ���s�ʒm
								_devmngmsg_Mode.SleepModeCodeFail = true;
							}
							return false;

						case 0x20:// roaming mode 
							if (hmLib::cstring_size(&Str) != 4)return true;
							_devmngmsg_roaming_sec_interval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
							// sleep mode�@�ݒ菀��
							devmng::roaming_setInterval(_devmngmsg_roaming_sec_interval);
							//devmng::roaming_getInterval(&_devmngmsg_roaming_sec_interval); 
							// code �擾
							_devmngmsg_roaming_Code = service::lockcode();
							_devmngmsg_Mode.RoamingMode_i = true;
							return false;

						case 0x21:// roaming execute mode
							if (hmLib::cstring_size(&Str) != 3)return true;
							// code �̃`�F�b�N
							if (hmLib::cstring_getc(&Str, 2) == _devmngmsg_roaming_Code){// ��v
								// inform �֐��̃Z�b�g

								// roaming �̎��s
								devmng::mode_set(devmng::RoamingMode);
							}
							else{
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
						if (hmLib::cstring_size(&Str) != 2)return true;
						// code �̃`�F�b�N
						if (hmLib::cstring_getc(&Str, 1) == _devmngmsg_kill_Code){// ��v
							// kill �̎��s
							devmng::kill();
						}
						else{
							// ���s�ʒm
							_devmngmsg_Mode.KillCodeFail = true;
						}
						return false;

					case 0x30:
						if (hmLib::cstring_size(&Str) != 2)return true;
						// mode �̃`�F�b�N
						_devmngmsg_clockMode = hmLib::cstring_getc(&Str, 1);
						if (_devmngmsg_clockMode == 0){
							devmng::clock_set(devmng::NormalClock);
						}
						else if (_devmngmsg_clockMode == 1){
							devmng::clock_set(devmng::LowClock);
						}
						else if (_devmngmsg_clockMode == 2){
							devmng::clock_set(devmng::HighClock);
						}
						else{
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
					if (_devmngmsg_Mode.Info_i){
						service::cstring_construct_safe(pStr, 4);
						hmLib::cstring_putc(pStr, 0, 0xA0);

						mode = devmng::mode_get();
						if (mode == devmng::NormalMode){
							hmLib::cstring_putc(pStr, 1, 0x00);
						}
						else if (mode == devmng::SleepMode){
							hmLib::cstring_putc(pStr, 1, 0x10);
						}
						else if (mode == devmng::RoamingMode){
							hmLib::cstring_putc(pStr, 1, 0x20);
						}

						clockMode = devmng::clock_get();
						if (clockMode == devmng::NormalClock){
							hmLib::cstring_putc(pStr, 2, 0x00);
						}
						else if (clockMode == devmng::LowClock){
							hmLib::cstring_putc(pStr, 2, 0x01);
						}
						else if (clockMode == devmng::HighClock){
							hmLib::cstring_putc(pStr, 2, 0x02);
						}

						// RF or MP 
						if (devmng::rf_module_uart_getModuleID() == devmng::rf_module){
							hmLib::cstring_putc(pStr, 3, 0x00);
						}
						else{
							hmLib::cstring_putc(pStr, 3, 0x01);
						}

						_devmngmsg_Mode.Info_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.NormalMode_i){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0x00);

						_devmngmsg_Mode.NormalMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.SleepMode_i){
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

					}
					else if (_devmngmsg_Mode.SleepModeCodeFail){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0xF1);

						_devmngmsg_Mode.SleepModeCodeFail = false;
						return false;

					}
					else if (_devmngmsg_Mode.RoamingMode_i){
						service::cstring_construct_safe(pStr, 6);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0x20);
						hmLib::cstring_putc(pStr, 2, (unsigned char)(_devmngmsg_roaming_sec_interval & 0x00FF));
						hmLib::cstring_putc(pStr, 3, (unsigned char)((_devmngmsg_roaming_sec_interval >> 8) & 0x00FF));
						hmLib::cstring_putc(pStr, 4, _devmngmsg_roaming_Code);

						//�ύX��̒ʐM��i��Ԃ�
						if (devmng::rf_module_uart_getModuleID() == devmng::rf_module){
							hmLib::cstring_putc(pStr, 5, 0x01); // ����RF�Ȃ玟��MP
						}
						else{
							hmLib::cstring_putc(pStr, 5, 0x00); // ����MP�Ȃ玟��RF 
						}

						_devmngmsg_Mode.RoamingMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.RoamingModeCodeFail){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0xF2);

						_devmngmsg_Mode.RoamingModeCodeFail = false;
						return false;

					}
					else if (_devmngmsg_Mode.KillCom_i){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x20);
						hmLib::cstring_putc(pStr, 1, _devmngmsg_kill_Code);

						_devmngmsg_Mode.KillCom_i = false;
						return false;
					}
					else if (_devmngmsg_Mode.KillCodeFail){
						service::cstring_construct_safe(pStr, 1);
						hmLib::cstring_putc(pStr, 1, 0xF2);

						_devmngmsg_Mode.KillCodeFail = false;
						return false;

					}
					else if (_devmngmsg_Mode.ClockMode_i){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x30);
						hmLib::cstring_putc(pStr, 1, _devmngmsg_clockMode);

						_devmngmsg_Mode.ClockMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.ClockModeFail){
						service::cstring_construct_safe(pStr, 1);
						hmLib::cstring_putc(pStr, 0, 0xF3);

						_devmngmsg_Mode.ClockModeFail = false;
						return false;
					}

					return true;
				}
				void setup_listen(void){ return; }
				void setup_talk(void){
					//_thermo_Mode.Data=_thermo_Mode.SendData;
					/*Debug*///	_thermo_Mode.Data=1;
				}
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						_devmngmsg_Mode.Info_i = true;
						return dt;
					}
				}InformTask;
				// initialize �֐�
				void initialize(){
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

//					devmng::initialize();

					//�^�X�N�o�^
					task::quick_start(InformTask, 5);
				}
				// finalize �֐�
				void finalize(){}
			}
		}
	}
}

#
#endif
