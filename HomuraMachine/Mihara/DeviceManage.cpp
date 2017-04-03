#ifndef HMR_DEVICEMANAGE_C_INC
#define HMR_DEVICEMANAGE_C_INC 200
#
#include<XC32Lib/i2c.hpp>
#include<XC32Lib/spi.hpp>
#include<XC32Lib/uart.hpp>
#include<homuraLib_v2/machine/service/delay.hpp>
#include<homuraLib_v2/machine/service/exclusive_delay.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Device.hpp"
#include"DeviceManage.hpp"
#include"DeviceManage_forInertial.hpp"
#include"DeviceManage_forCamera.hpp"
#include"DeviceManage_forCO2.hpp"
#include"DeviceManage_forSDcard.hpp"
#include"DeviceManage_forCourier.hpp"
#include"Com.hpp"

namespace hmr {
	namespace machine {
		namespace service{
			namespace{
				//�֐��쓮�^�̃^�X�N�z�X�g
				hmr::task::functional_host<> TaskHost;
			}
			//service�̃^�X�N�Ƃ��ēo�^
			hmr::task::host_interface& Task(TaskHost);
		}
		namespace mihara {
			namespace devmng {
				class device :public mihara::cDevice::devmng_device {
				public:

//=================================================================
					//====rf module====
					pinRF_Power PinRFPower;
					pinMobilePhone_Power PinMobilePhonePower;
					rf_uart_register RF_uart_register;
					xc32::interrupt_uart<rf_uart_register> RF_uart;
					static const uint16 RF_uart_Baudrate=9600;
					static const xc32::uart::flowcontrol::mode RF_uart_flowctrl=xc32::uart::flowcontrol::mode::rts_cts_control;
					mobilephone_uart_register MobilePhone_uart_register;
					xc32::interrupt_uart<mobilephone_uart_register> MobilePhone_uart;
					static const uint16 MobilePhone_uart_Baudrate=38400;
					static const xc32::uart::flowcontrol::mode MobilePhone_uart_flowctrl=xc32::uart::flowcontrol::mode::rts_cts_control;
//=================================================================
				}Device;

				//++++++++++++++++++ protected�錾 +++++++++++++++++
				//=== ��{�֐� ===
				//�f�o�C�X�̓d���Ǘ�
				void power(bool onoff);

				//=== ���[�h����@�\ ===
				//���[�h����@�\������
				void mode_initialize(mode Mode_);
				//���[�h����@�\�I�[��
				void mode_finalize();
				//�N���b�N�ύX�̒ʒm�֐�
				void mode_informClockChange(void);

				//=== �N���b�N����@�\ ===
				//�N���b�N����@�\������
				void clock_initialize(clock Clock_);
				//�N���b�N����@�\�I�[��
				void clock_finalize();
				//�N���b�N���x�̕ύX�֐�
				void clock_restartClock(clock Clock_);
				//�N���b�N���x��f�t�H���g�ɕύX
				void clock_restartClockDefault();

				//=== �X���[�v���[�h���� ===
				//�X���[�v���[�h���䏉����
				void sleep_initialize(void);
				//�X���[�v���[�h����I�[��
				void sleep_finalize(void);
				//�X���[�v�ɓ��鏈��
				void sleep_start();
				//�X���[�v����o�鏈��
				void sleep_stop(void);
				//�X���[�v���䏈���֐�
				void sleep_interrupt_function(sint16 Interval_);
				//�X���[�v���̊��荞�݋֎~�֐�
				void sleep_disable_interrupt(void);
				//�X���[�v���̊��荞�ݕ��A�֐�
				void sleep_enable_interrupt(void);


				//=== ���[�~���O���[�h���� ===
				//���[�~���O���[�h���䏉����
				void roaming_initialize(void);
				//���[�~���O���[�h����I�[��
				void roaming_finalize(void);
				//���[�~���O�ɓ��鏈��
				void roaming_start(void);
				//���[�~���O����o�鏈��
				void roaming_stop(void);
				//���[�~���O���䏈���֐�
				void roaming_interrupt_function(sint16 Interval_);

				//=== RF module uart select ===
				//ModuleID�̃��W���[���ŏ���������
				void rf_module_uart_initialize(rf_module_mode ModuleID_, uint16 BaudRate_);
				//ModuleID�ɓd����ύX����
				void rf_module_uart_power_switch(bool onoff, rf_module_mode ModuleID_);
				//���݂�ModuleID��擾����
				rf_module_mode rf_module_uart_getModuleID(void);

				//=== ���荞�݊֐����� ===
				void interrupt_initialize(void);
				void interrupt_finalize(void);

				//++++++++++++++++++ ���� +++++++++++++++++
				//=== ���[�h����@�\ ===
				//�f�o�C�X�̌����[�h
				volatile mode mode_Mode=NormalMode;

				//���[�h����@�\������
				void mode_initialize(mode Mode_) {
					//�w��̃f�t�H���g���[�h�ŊJ�n
					mode_Mode=Mode_;
				}
				//���[�h����@�\�I�[��
				void mode_finalize() {
					mode_Mode=NormalMode;
				}
				//�f�o�C�X���[�h��ύX����
				void mode_set(mode Mode_) {
					switch(Mode_) {
					case SleepMode:
						switch(mode_Mode) {
						case RoamingMode:
							roaming_stop();
							break;
						default:
							break;
						}

						//?????
						mode_Mode=SleepMode;
						sleep_start();

						return;
					case RoamingMode:
						switch(mode_Mode) {
						case SleepMode:
							sleep_stop();
							break;
						default:
							break;
						}

						//?????
						mode_Mode=RoamingMode;
						roaming_start();

						return;
					default:
						switch(mode_Mode) {
						case RoamingMode:
							roaming_stop();
							break;
						case SleepMode:
							sleep_stop();
							break;
						default:
							break;
						}

						//?????
						mode_Mode=NormalMode;

						return;
					}

				}
				//�f�o�C�X���[�h��擾����
				volatile mode mode_get(void) { return mode_Mode; }
				//�N���b�N�ύX�̒ʒm�֐�
				void mode_informClockChange(void) {
					//�X���[�v���m�������������̓p�X����
					if(sleep_isSleep() && !sleep_isRem())return;

					//�N���b�N���[�h��������ɔ��f
					clock_restartClockDefault();
				}
				//=== �N���b�N����@�\ ===
				//�f�o�C�X�̌��N���b�N
				volatile clock clock_Clock;
				//�N���b�N����@�\������
				void clock_initialize(clock Clock_) {
					//�w��N���b�N�ɕύX
					clock_Clock=Clock_;

					//�������ɃN���b�N���x���f
					clock_restartClockDefault();
				}
				//�N���b�N����@�\�I�[��
				void clock_finalize() {
//					hmr_clock_finalize();
				}
				//�N���b�N���x�̕ύX�֐�
				void clock_restartClock(clock Clock_) {
					
					//��x�N���b�N��I��
//					hmr_clock_finalize();

					switch(Clock_) {
					case LowClock:
//						hmr_clock_initialize(CLOCK_SLOW);
						break;
					case HighClock:
//						hmr_clock_initialize(CLOCK_FAST);
						break;
					default:
//						hmr_clock_initialize(CLOCK_NORMAL);
						break;
					}

					//�N���b�N�ύX��ق��̃��W���[���ɂ�ʒm
					//	hmr_uart1_initialize(hmr_uart1_getModuleID(),);
					//	hmr_uart2_initialize(38400);
					//	hmr_adc_initialize();
					
				}
				//�N���b�N���x��f�t�H���g�ɕύX
				void clock_restartClockDefault() {
					clock_restartClock(clock_Clock);
				}
				//�N���b�N��ύX����
				void clock_set(clock Clock_) {
					//���łɐݒ�ς݂Ȃ疳��
					if(clock_Clock==Clock_)return;

					//�ύX�𔽉f���āA���[�h����ɒʒm
					clock_Clock=Clock_;
					mode_informClockChange();
				}
				//�N���b�N��擾����
				volatile clock clock_get(void) { return clock_Clock; }

				//=== �X���[�v���[�h���� ===
				//�X���[�v/�o���ʒm�p�֐�
				typedef struct {
					vFp_v InformSleep;
					vFp_v InformWakeUp;
				}sleep_InformSleepFn;
				const unsigned int sleep_InformSleepFnBufSize=16;
				sleep_InformSleepFn sleep_InformSleepFnBuf[sleep_InformSleepFnBufSize];
				unsigned char sleep_InformSleepFnBufPos=0;
				//����/�m�������ʒm�p�֐�
				typedef struct {
					vFp_v InformRem;
					vFp_v InformNonRem;
				}sleep_InformRemFn;
				const unsigned int sleep_InformRemFnBufSize=16;
				sleep_InformRemFn sleep_InformRemFnBuf[sleep_InformRemFnBufSize];
				unsigned char sleep_InformRemFnBufPos=0;
				//�X���[�v���̎��ԃJ�E���g�n
				sint16 sleep_secNonRem=540;
				sint16 sleep_secRem=60;
				sint16 sleep_secCnt=0;
				//�X���[�v��ԊǗ��p�t���O
				volatile bool sleep_IsSleep=0;
				volatile bool sleep_IsRem=0;
				bool sleep_RequestEndRem=0;
				//�X���[�v���[�h���䏉����
				void sleep_initialize(void) {
					sleep_InformSleepFnBufPos=0;
					sleep_InformRemFnBufPos=0;

					sleep_secNonRem=540;
					sleep_secRem=60;
					sleep_secCnt=0;
					sleep_IsSleep=0;
					sleep_IsRem=0;
					sleep_RequestEndRem=0;
				}
				//�X���[�v���[�h����I�[��
				void sleep_finalize(void) {
					//�X���[�v���Ȃ��~
					if(sleep_IsSleep)sleep_stop();

					sleep_InformSleepFnBufPos=0;
					sleep_InformRemFnBufPos=0;

					sleep_secNonRem=540;
					sleep_secRem=60;
					sleep_secCnt=0;
					sleep_IsSleep=0;
					sleep_IsRem=0;
				}
				//�����X���[�v��Ԃ��ǂ�����擾����
				volatile bool sleep_isRem(void) { return sleep_IsRem && sleep_IsSleep; }
				//�X���[�v��Ԃ��ǂ�����擾����
				volatile bool sleep_isSleep(void) { return sleep_IsSleep; }
				//�X���[�v���[�h�̃p�����[�^��擾����
				void sleep_getInterval(sint16* secNonRem, sint16* secRem) {
					*secNonRem=sleep_secNonRem;
					*secRem=sleep_secRem;
				}
				//�X���[�v���[�h�̃p�����[�^��ݒ肷��
				void sleep_setInterval(sint16 secNonRem, sint16 secRem) {
					if(secNonRem<0)secNonRem=0;
					if(secRem<1)secRem=1;

					sleep_secNonRem=secNonRem;
					sleep_secRem=secRem;
				}
				//���������N�����ɌĂяo�����ʒm�֐���o�^�ł��邩
				bool sleep_can_registSleepInformFunction() { return sleep_InformSleepFnBufPos!=sleep_InformSleepFnBufSize; }
				//���������N�����ɌĂяo�����ʒm�֐���o�^����
				bool sleep_registSleepInformFunction(vFp_v InformSleep, vFp_v InformWakeUp) {
					if(!sleep_can_registSleepInformFunction())return true;
					sleep_InformSleepFnBuf[sleep_InformSleepFnBufPos].InformSleep=InformSleep;
					sleep_InformSleepFnBuf[sleep_InformSleepFnBufPos].InformWakeUp=InformWakeUp;
					++sleep_InformSleepFnBufPos;

					return false;
				}
				//�����X���[�v�ɓ���/�o��ۂ̒ʒm�֐���o�^�ł��邩
				bool sleep_can_registRemInformFunction() { return sleep_InformRemFnBufPos!=sleep_InformRemFnBufSize; }
				//�����X���[�v�ɓ���/�o��ۂ̒ʒm�֐���o�^����
				bool sleep_registRemInformFunction(vFp_v InformRem, vFp_v InformNonRem) {
					if(!sleep_can_registRemInformFunction())return true;
					sleep_InformRemFnBuf[sleep_InformRemFnBufPos].InformRem=InformRem;
					sleep_InformRemFnBuf[sleep_InformRemFnBufPos].InformNonRem=InformNonRem;
					++sleep_InformRemFnBufPos;

					return false;
				}
				//�X���[�v�֓��鏈��
				void sleep_start() {
					sleep_InformSleepFn* Itr;

					//���łɃX���[�v���Ȃ�I��
					if(sleep_IsSleep)return;

					//�J�E���^�[����Z�b�g
					sleep_secCnt=0;
					sleep_IsRem=0;

					//�X���[�v��ԂɕύX
					sleep_IsSleep=1;

					// ���荞�݋֎~
					sleep_disable_interrupt();

					//Inform�֐�
					for(Itr=sleep_InformSleepFnBuf; Itr!=sleep_InformSleepFnBuf+sleep_InformSleepFnBufSize; ++Itr) {
						//if(sleep_InformSleepFnBuf->InformSleep)sleep_InformSleepFnBuf->InformSleep();
						if(Itr->InformSleep)Itr->InformSleep();
					}

					//�d���𗎂Ƃ�
					power(0);

					//�N���b�N���x�𗎂Ƃ�
					clock_restartClock(LowClock);
				}
				//�X���[�v����o�鏈��
				void sleep_stop(void) {
					sleep_InformSleepFn* Itr;

					//���łɔ�X���[�v���Ȃ�I��
					if(!sleep_IsSleep)return;

					//�N���b�N���x��f�t�H���g�ɖ߂�
					clock_restartClockDefault();

					//�d����߂� ���łɃ��������Ȃ̂ŃJ�b�g
					//power(1);

					//�X���[�v��Ԃ���
					sleep_IsSleep=0;

					//Inform�֐�
					for(Itr=sleep_InformSleepFnBuf; Itr!=sleep_InformSleepFnBuf+sleep_InformSleepFnBufSize; ++Itr) {
						//if(sleep_InformSleepFnBuf->InformWakeUp)sleep_InformSleepFnBuf->InformWakeUp();
						if(Itr->InformWakeUp)Itr->InformWakeUp();
					}

					// ���荞�݋���
					sleep_enable_interrupt();

				}
				// �X���[�v���̊��荞�݋֎~
				void sleep_disable_interrupt() {
					// Main stream �̊��荞�݋֎~
					if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
					else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_disable();
					//streamVMC_disable_fput_interrupt();
					// Camera stream �̊��荞�݋֎~
//					Device.Camera1_uart.recv_disable();
//					Device.Camera2_uart.recv_disable();
					//streamCMR_disable_fput_interrupt();
					// �J���� ��Timer ���荞�݋֎~
//					Device.Camera_timer.clear_count();
//					Device.Camera_timer.stop();
				}
				// �X���[�v���̊��荞�ݕ��A
				void sleep_enable_interrupt() {
					// Main stream �̎�M���荞�݊J�n
					if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
					else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_enable();
					// Camera stream �̎�M���荞�݊J�n
//					Device.Camera1_uart.recv_enable();
//					Device.Camera2_uart.recv_enable();
					// �J���� ��Timer ���荞�݊J�n
//					Device.Camera_timer.clear_count();
//					Device.Camera_timer.start();
				}
				//�X���[�v���䏈�������֐�
				void sleep_interrupt_function(sint16 Interval_) {
					sleep_InformRemFn* Itr;

					//SleepCnt����Z
					sleep_secCnt+=Interval_;

					//������Ԃ̂Ƃ� 
					if(sleep_IsRem) {
						//���荞�݂ŋ}�ɓd���𗎂Ƃ��̂͊댯�Ȃ̂ŁA���N�G�X�g���������Ă���(���ۂ̏I���́Awork�֐��ɈϏ�)
						if(sleep_secCnt>=sleep_secRem)sleep_RequestEndRem=1;
					}//�m��������Ԃ̂Ƃ�
					else {
						if(sleep_secCnt>=sleep_secNonRem) {
							//�J�E���^�[�������
							sleep_secCnt=0;

							//�N���b�N���x��f�t�H���g�ɖ߂�
							clock_restartClockDefault();

							//�d��������
							power(1);
							// ����҂�
							service::exclusive_delay_ms(300);
							Device.PinRedLED(1);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(0);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(1);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(0);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(1);

							//������ԂɈڍs
							sleep_IsRem=1;

							//Inform�֐�
							for(Itr=sleep_InformRemFnBuf; Itr!=(sleep_InformRemFnBuf+sleep_InformRemFnBufSize); ++Itr) {
								//if(sleep_InformRemFnBuf->InformRem)sleep_InformRemFnBuf->InformRem();
								if(Itr->InformRem)Itr->InformRem();
							}

							// ���荞�݋���
							sleep_enable_interrupt();

						}
					}
				}
				//�X���[�v���䏈���ʏ�֐�
				void sleep_interrupt_yield(void) {
					sleep_InformRemFn* Itr;

					//���������I���v���֐�
					if(sleep_RequestEndRem) {
						//�t���O�������
						sleep_RequestEndRem=0;
						//�J�E���^�[�������
						sleep_secCnt=0;

						// ���荞�݂�֎~
						sleep_disable_interrupt();

						//�m��������ԂɈڍs
						sleep_IsRem=0;

						//Inform�֐�
						for(Itr=sleep_InformRemFnBuf; Itr!=sleep_InformRemFnBuf+sleep_InformRemFnBufSize; ++Itr) {
							//if(sleep_InformRemFnBuf->InformNonRem)sleep_InformRemFnBuf->InformNonRem();
							if(Itr->InformNonRem)Itr->InformNonRem();
						}

						//�N���b�N���x�𗎂Ƃ�
						clock_restartClock(LowClock);

						//�d���𗎂Ƃ�
						power(0);
					}
				}
				//=== ���[�~���O���[�h���� ===
				//���[�~���O�̊Ԋu����ϐ�
				sint16 roaming_secInterval=0;
				sint16 roaming_secCnt=0;
				//���[�~���O�̏�ԕϐ�
				volatile bool roaming_IsRoaming=0;
				//���[�~���O���[�h���䏉����
				void roaming_initialize(void) {
					roaming_secInterval=0;
					roaming_secCnt=0;
					roaming_IsRoaming=0;
				}
				//���[�~���O���[�h����I�[��
				void roaming_finalize(void) {
					//���[�~���O���Ȃ�A��~
					if(roaming_IsRoaming)roaming_stop();

					roaming_secInterval=0;
					roaming_secCnt=0;
					roaming_IsRoaming=0;
				}
				//���[�~���O���[�h�̃p�����[�^��擾����
				void roaming_getInterval(sint16* secInterval) {
					*secInterval=roaming_secInterval;
				}
				//���[�~���O���[�h�̃p�����[�^��ݒ肷��
				void roaming_setInterval(sint16 secInterval) {
					//���[�~���O�Ԋu��10�b�ȏ�ł���K�v
					if(secInterval<=10)secInterval=10;
					roaming_secInterval=secInterval;
				}
				//���[�~���O��Ԃ��ǂ�����擾����
				volatile bool roaming_isRoaming(void) { return roaming_IsRoaming; }
				//���[�~���O��Ԃɓ���
				void roaming_start(void) {
					//���łɃ��[�~���O���Ȃ疳��
					if(roaming_IsRoaming)return;

					//�J�E���^���Z�b�g
					roaming_secCnt=0;

					//���[�~���O��Ԃֈڍs
					roaming_IsRoaming=1;

					//�ʐM�f�o�C�X�؂�ւ�
					if(rf_module_uart_getModuleID()==rf_module_mode::rf_module) {
						Device.PinRedLED(1);
						rf_module_uart_initialize(rf_module_mode::mobile_phone, MP_BaudRate);
					} else {
						Device.PinRedLED(0);
						if(Device.Config_RFDebugMode())rf_module_uart_initialize(rf_module_mode::rf_module, RFDebug_BaudRate);
						else rf_module_uart_initialize(rf_module_mode::rf_module, RF_BaudRate);
					}
				}
				//���[�~���O��Ԃ���
				void roaming_stop(void) {
					//���łɃ��[�~���O���łȂ��Ȃ疳��
					if(!roaming_IsRoaming)return;

					//���[�~���O��Ԃ���
					roaming_IsRoaming=0;
				}
				//���[�~���O���䏈���֐�
				void roaming_interrupt_function(sint16 Interval_) {
					//�J�E���^�[��i�߂�
					roaming_secCnt+=Interval_;

					//�J�E���^�[���؂�ւ��l�𒴂����ꍇ�́A�ʐM�f�o�C�X�؂�ւ�
					if(roaming_secCnt>roaming_secInterval) {
						if(rf_module_uart_getModuleID()==rf_module_mode::rf_module) {
							Device.PinRedLED(1);
							rf_module_uart_initialize(rf_module_mode::mobile_phone, MP_BaudRate);
						} else {
							Device.PinRedLED(0);
							if(Device.Config_RFDebugMode())rf_module_uart_initialize(rf_module_mode::rf_module, RFDebug_BaudRate);
							else rf_module_uart_initialize(rf_module_mode::rf_module, RF_BaudRate);
						}
						roaming_secCnt=0;
					}
				}

				//=== ��{����֐� ===
				//�f�o�C�X�������֐�
				void initialize(void) {
					Device.PinRedLED.lock();
					Device.Pin5VDCDC.lock();
					Device.PinADC0Power.lock();
					Device.PinADC1to4Power.lock();
//					Device.PinCamera0Power.lock();
//					Device.PinCamera1Power.lock();
//					Device.PinCameraLED.lock();
					Device.PinCO2PumpPower.lock();
					Device.PinCO2SensorPower.lock();
					Device.PinDevicePower.lock();
					Device.PinExtI2CPower.lock();
//					Device.PinHeadLightPower.lock();
					//Device.PinInertialPower.lock();
					Device.PinMobilePhonePower.lock();
					Device.PinMotorPower.lock();
					Device.PinMotorLA.lock();
					Device.PinMotorLB.lock();
					Device.PinMotorRA.lock();
					Device.PinMotorRB.lock();
					Device.PinRFPower.lock();
					Device.PinSDPower.lock();
					Device.Config_DefaultHighClock.lock();
					Device.Config_DefaultMP.lock();
					Device.Config_DisableWDT.lock();
					Device.Config_RFDebugMode.lock();

					//wdt������
					Device.WDT.lock();


					//Pic���̂̃p���[�����Ă���
					Device.PinDevicePower(1);
					Device.Pin5VDCDC(1);

					//�f�o�C�X�̓d���𗎂Ƃ�
					power(0);
					//���[�h�V�X�e��������
					mode_initialize(NormalMode);

					//�N���b�N������
					if(Device.Config_DefaultHighClock())clock_initialize(HighClock);
					else clock_initialize(NormalClock);

					//���荞�ݏ�����->�e���W���[�����������ɐݒ�
//					xc32::interrupt::initialize();
					xc32::sfr::interrupt::cpu_priority(Device.Main_IPL);
//					hmr_interrupt_initalize();

					//�ʐM�n���W���[��������
					if(Device.Config_DefaultMP()) {
						rf_module_uart_initialize(rf_module_mode::mobile_phone,Device.MobilePhone_uart_Baudrate);
						com::wdt_restart();
						com::wdt_enable();
					} else {
						if(Device.Config_RFDebugMode())rf_module_uart_initialize(rf_module_mode::rf_module,Device.RF_uart_Baudrate);
						else rf_module_uart_initialize(rf_module_mode::rf_module,Device.RF_uart_Baudrate);
						com::wdt_disable();
					}

					//=== Coamera
//					Device.Camera1_uart.initialize(Device.Camera1_uart_register);
//					Device.Camera1_uart.config(Device.Camera1_uart_Baudrate, Device.Camera1_uart_flowctrl, camera::uart_camera1::Camera1TxInterruptFunc, camera::uart_camera1::Camera1RxInterruptFunc);
//					Device.Camera1_uart.lock();
		
/*					Device.Camera2_uart.initialize(Device.Camera2_uart_register);
					Device.Camera2_uart.config(Device.Camera2_uart_Baudrate, Device.Camera2_uart_flowctrl, camera::uart_camera2::Camera2TxInterruptFunc, camera::uart_camera2::Camera2RxInterruptFunc);
					Device.Camera2_uart.lock();

					//=== SD card spi
					Device.SDcard_spi.initialize(Device.SDcard_spi_register);
					Device.SDcard_spi.config(true, );
					Device.SDcard_spi.lock();
*/
					//=== Inertial i2c
/*					Device.Axcel_i2c.config(Device.Axcel_i2c_clock, 0);
					Device.Axcel_i2c.lock();
					
					if(!Device.Compass_i2c.is_lock()){
						Device.Compass_i2c.config(Device.Compass_i2c_clock, 0);
						Device.Compass_i2c.lock();
					}

					if(!Device.Gyro_i2c.is_lock()) {
						Device.Gyro_i2c.config(Device.Gyro_i2c_clock, 0);
						Device.Gyro_i2c.lock();
					}
*/					
//					hmr_adc_initialize(); -> analog_pin����ݒ�ł���̂Ŋe���W���[�����s��

					//�f�o�C�X�̓d��������
					service::delay_ms(500);
					power(1);

					//devmng���W���[��������
					sleep_initialize();
					roaming_initialize();
					interrupt_initialize();

					//WatchDogTimer�J�n
					if(Device.Config_DisableWDT())Device.WDT.disable();
					else Device.WDT.enable();

					Device.PinRedLED(0);
//					Device.PinCameraLED(0);
					service::delay_ms(500);
					Device.PinRedLED(1);
//					Device.PinCameraLED(1);
					service::delay_ms(500);
					Device.PinRedLED(0);
//					Device.PinCameraLED(0);
					service::delay_ms(500);
					Device.PinRedLED(1);
//					Device.PinCameraLED(1);
					service::delay_ms(500);
					Device.PinRedLED(1);
//					Device.PinCameraLED(0);

				}
				//�f�o�C�X�I�[���֐�
				void finalize(void) {
					//WatchDogTimer�I��
					Device.WDT.disable();
					Device.WDT.unlock();

					//devmng���W���[���I�[��
					sleep_finalize();
					roaming_finalize();
					interrupt_finalize();

					//�ʐM�n���W���[���I�[��
					Device.RF_uart.recv_disable();
					Device.RF_uart.send_disable();
					Device.RF_uart.release();

					Device.MobilePhone_uart.recv_disable();
					Device.MobilePhone_uart.send_disable();
					Device.MobilePhone_uart.release();

//					Device.Camera1_uart.recv_disable();
//					Device.Camera1_uart.send_disable();
//					Device.Camera1_uart.release();

//					Device.Camera2_uart.recv_disable();
//					Device.Camera2_uart.send_disable();
//					Device.Camera2_uart.release();

//					Device.Axcel_i2c.unlock();
//					Device.Compass_i2c.unlock();
//					Device.Gyro_i2c.unlock();
//					hmr_spi2_finalize();
//					hmr_adc_finalize();

					//�f�o�C�X�̓d���𗎂Ƃ�
					power(0);

					//���荞�ݏI��
//					hmr_interrupt_finalize();

					//�N���b�N�I��
					clock_finalize();

					//���[�h�Ǘ��I��
					mode_finalize();

					power(0);
					Device.PinDevicePower(0);
					Device.WDT.disable();
					Device.WDT.unlock();
					//�f�o�C�X���̂�I��
//					hmr_device_finalize();
				}
				//�f�o�C�X�����I��
				void kill(void) {
					//���g�̓d����؂�
					Device.PinDevicePower(0);
				}
				//�f�o�C�X�̓d���Ǘ��֐�
				void power(bool onoff_) {
					bool onoff= (onoff_!=0);
					//�d���I�t�̏ꍇ�́A���W���[������؂�
					if(!onoff) {
//						Device.PinCamera0Power(0);
//						Device.PinCamera1Power(0);
//						Device.PinHeadLightPower(0);
						Device.PinCO2PumpPower(0);
						Device.PinCO2SensorPower(0);
					}

					rf_module_uart_power_switch(onoff, rf_module_uart_getModuleID());
					//Device.PinInertialPower(onoff);
					Device.PinADC0Power(onoff);
					Device.PinADC1to4Power(onoff);
					Device.PinMotorPower(onoff);
				}
				//���݂̒ʐM�悪�S��d�ʐM�Ή�����Ԃ�
				bool isFullDuplex(void) {
					return rf_module_uart_getModuleID()==rf_module_mode::mobile_phone || Device.Config_RFDebugMode();
				}
				//clear wdt
				void clear_wdt(void) { Device.WDT.clear(); }
				//=== rf module uart select ===
				//���݃A�N�e�B�u��ID
				rf_module_mode rf_module_uart_ModuleID = rf_module_mode::null;
				//ModuleID�̃��W���[���ŏ���������
				void rf_module_uart_initialize(rf_module_mode ModuleID_, uint16 BaudRate_) {
					if(ModuleID_==rf_module_mode::mobile_phone) {
						rf_module_uart_ModuleID = rf_module_mode::mobile_phone;
						Device.MobilePhone_uart.initialize(Device.MobilePhone_uart_register);
						Device.MobilePhone_uart.config(Device.MobilePhone_uart_Baudrate, Device.MobilePhone_uart_flowctrl, courier::uart::CourierTxInterruptFunc, courier::uart::CourierRxInterruptFunc);
						Device.MobilePhone_uart.lock();
						//Device.RF_uart.release();
					} else if(ModuleID_==rf_module_mode::rf_module) {
						rf_module_uart_ModuleID = rf_module_mode::rf_module;
						Device.RF_uart.initialize(Device.RF_uart_register);
						Device.RF_uart.config(Device.RF_uart_Baudrate, Device.RF_uart_flowctrl, courier::uart::CourierTxInterruptFunc, courier::uart::CourierRxInterruptFunc);
						Device.RF_uart.lock();
						//Device.MobilePhone_uart.release();
					}
				}
				//ModuleID�ɓd����ύX����
				void rf_module_uart_power_switch(bool onoff, rf_module_mode ModuleID_) {
					if(onoff && ModuleID_==rf_module_mode::mobile_phone) {
						Device.PinMobilePhonePower(1);
						Device.PinRFPower(0);
					} else if(onoff && ModuleID_==rf_module_mode::rf_module) {
						Device.PinMobilePhonePower(0);
						Device.PinRFPower(1);
					} else {
						Device.PinMobilePhonePower(0);
						Device.PinRFPower(0);
					}
				}
				//���݂�ModuleID��擾����
				rf_module_mode rf_module_uart_getModuleID(void) { return rf_module_uart_ModuleID; }

				//=== ���荞�݊֐� ===
				//DevMng�p�^�C�}�[���荞�݊֐�
				class:public xc32::interrupt::function {
				private:
					virtual void operator()(void) {
						static bool flag = false;
						Device.Task_timer.clear_count();

						Device.PinRedLED(flag);
						flag = (!flag);

						//�f�o�C�X�}�l�[�W�̒�����s�����i������b�Ԋu�Ŏg�p�j
						interrupt_function(1);
					}
				}DevMngInterruptFunction;
				//�^�X�N�A�X���[�v�A����у��[�~���O���s�p�֐�
				void interrupt_function(sint16 Interval_) {
					//���[�h�ɂ���Ċ��荞�ݏ����𕪊�
					switch(mode_get()) {
					case SleepMode:
						sleep_interrupt_function(Interval_);
						break;
					case RoamingMode:
						roaming_interrupt_function(Interval_);
						break;
					default:
						service::TaskHost(Interval_);
						break;
					}
				}
				//���荞�݂�main���[�v�ւ̈Ϗ������֐�
				bool interrupt_yield(void) {
					sleep_interrupt_yield();

					//�X���[�v����O
					if(sleep_isSleep() && !sleep_isRem()) {
						//__asm__("PWRSAV #IDLE_MODE");
						//Idle();
						//Sleep();
						return true;
					}

					return false;
				}
				void interrupt_initialize() {
					Device.Task_timer.initialize(Device.Task_timer_register);
					Device.Task_timer.config(Device.TaskTimerMS, DevMngInterruptFunction);
					Device.Task_timer.lock();
				}
				void interrupt_finalize() {
					Device.Task_timer.stop();
					Device.Task_timer.clear_count();
					Device.Task_timer.release();
				}

				// main stream ���荞�݂̋��֎~����������Ă悢����Ԃ��֐�
				bool interrupt_can_enable_streamVMC_fget_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamVMC_fget_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				bool interrupt_can_enable_streamVMC_fput_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamVMC_fput_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}

				// camera stream ���荞�݂̋��֎~����������Ă悢����Ԃ��֐�
				bool interrupt_can_enable_streamCMR_fget_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamCMR_fget_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				bool interrupt_can_enable_streamCMR_fput_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamCMR_fput_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}

				// camera timer  ���荞�݂̋��֎~����������Ă悢����Ԃ��֐�
				bool interrupt_can_enable_timerCMR_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_timerCMR_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				// DevMng timer �̊��荞�݂̋��֎~����������Ă悢����Ԃ��֐�
				bool interrupt_can_enable_timerDevmng_interrupt() {
					return true;
				}
				bool interrupt_can_disable_timerDevmng_interrupt() {
					return true;
				}

				// main stream �̊��荞�݋��֎~�֐�
				bool interrupt_enable_streamVMC_fget_interrupt() {
					if(interrupt_can_enable_streamVMC_fget_interrupt()) {
						if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
						else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_enable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_streamVMC_fget_interrupt() {
					if(interrupt_can_disable_streamVMC_fget_interrupt()) {
						if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
						else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_disable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_enable_streamVMC_fput_interrupt() {
					if(interrupt_can_enable_streamVMC_fput_interrupt()) {
						if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_enable();
						else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_enable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_streamVMC_fput_interrupt() {
					if(interrupt_can_disable_streamVMC_fput_interrupt()) {
						if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_disable();
						else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_disable();
						return true;
					} else {
						return false;
					}
				}

				// camera stream �̊��荞�݋��֎~�֐�
				bool interrupt_enable_streamCMR_fget_interrupt() {
					if(interrupt_can_enable_streamCMR_fget_interrupt()) {
//						Device.Camera1_uart.recv_enable();
//						Device.Camera2_uart.recv_enable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_streamCMR_fget_interrupt() {
					if(interrupt_can_disable_streamCMR_fget_interrupt()) {
//						Device.Camera1_uart.recv_disable();
//						Device.Camera2_uart.recv_disable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_enable_streamCMR_fput_interrupt() {
					if(interrupt_can_enable_streamCMR_fput_interrupt()) {
//						Device.Camera1_uart.send_enable();
//						Device.Camera2_uart.send_enable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_streamCMR_fput_interrupt() {
					if(interrupt_can_disable_streamCMR_fput_interrupt()) {
//						Device.Camera1_uart.send_disable();
//						Device.Camera2_uart.send_disable();
						return true;
					} else {
						return false;
					}
				}

				// timer camera �̊��荞�݋��֎~�֐�
				bool interrupt_enable_timerCMR_interrupt() {
					if(interrupt_can_enable_timerCMR_interrupt()) {
//						Device.Camera_timer.clear_count();
//						Device.Camera_timer.start();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_timerCMR_interrupt() {
					if(interrupt_can_disable_timerCMR_interrupt()) {
//						Device.Camera_timer.clear_count();
//						Device.Camera_timer.stop();
						return true;
					} else {
						return false;
					}
				}

				// timer Device manager �̊��荞�݋��֎~�֐�
				bool interrupt_enable_timerDevmng_interrupt() {
					if(interrupt_can_enable_timerDevmng_interrupt()) {
						Device.Task_timer.clear_count();
						Device.Task_timer.start();
//						timerDevmng_enable_interrupt();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_timerDevmng_interrupt() {
					if(interrupt_can_disable_timerDevmng_interrupt()) {
						Device.Task_timer.stop();
						Device.Task_timer.clear_count();
//						timerDevmng_disable_interrupt();
						return true;
					} else {
						return false;
					}
				}

//=================================Intertial===============================

//=================================SD card===============================
				namespace sdcard{
					void Power(bool val_){Device.PinSDPower(val_);}
					namespace spi{
						void write(uint8 data_){Device.SDcard_spi.write(data_);}
						uint8 read(uint8 data_){return Device.SDcard_spi.read(data_);}
						bool in_data(){return Device.SDcard_spi.in_data();}
						void select(bool val_){Device.PinSD_SPISelect(val_);}
					}
				}
//================================= CO2 ===============================
				namespace co2{
					void PumpPower(bool val_){Device.PinCO2PumpPower(val_);}
					void SensorPower(bool val_){Device.PinCO2SensorPower(val_);}
				}
//=================================Camera===============================
/*				namespace camera{
					void setLED(bool val_){Device.PinCameraLED(val_);}
					namespace timer{
						void clear_count(){Device.Camera_timer.clear_count();}
						void clear_interrupt_flag(){Device.Camera_timer.clear_flag();}
						void initialize(uint32 MSec_){
							if(!Device.Camera_timer.is_initialize())Device.Camera_timer.initialize(Device.Camera_timer_register);
							Device.Camera_timer.stop();
							if(Device.Camera_timer.is_lock())Device.Camera_timer.unlock();
							Device.Camera_timer.lock(MSec_, CameraInterruptFunc);
						}
						void enable_interrupt(){
							Device.Camera_timer.clear_count();
							Device.Camera_timer.start();
						}
						void disable_interrupt(){
							Device.Camera_timer.stop();
							Device.Camera_timer.clear_count();
						}
					}
					namespace uart_camera1 {
						void initialize(uint32 BoudRate_) {
							if(!Device.Camera1_uart.is_initialize())Device.Camera1_uart.initialize(Device.Camera1_uart_register);
							if(Device.Camera1_uart.is_lock())Device.Camera1_uart.unlock();
							Device.Camera1_uart.lock(Device.Camera1_uart_Baudrate, Device.Camera1_uart_flowctrl, uart_camera1::Camera1TxInterruptFunc, uart_camera1::Camera1RxInterruptFunc);
						}
						void disable() { Device.Camera1_uart_register.enable(0); }

						void fputc(uint8 data_){Device.Camera1_uart.send_data(data_);}
						void fput_set_interrupt(){Device.Camera1_uart.send_set_flag();}
						void fput_clear_interrupt(){Device.Camera1_uart.send_clear_flag();}
						void fput_enable_interrupt() { Device.Camera1_uart.send_enable(); }
						void fput_disable_interrupt() { Device.Camera1_uart.send_disable(); }

						uint8 fgetc(){return Device.Camera1_uart.recv_data();}
						void fget_set_interrupt(){Device.Camera1_uart.recv_set_flag();}
						void fget_clear_interrupt(){Device.Camera1_uart.recv_clear_flag();}
						void fget_enable_interrupt() { Device.Camera1_uart.recv_enable(); }
						void fget_disable_interrupt() { Device.Camera1_uart.recv_disable(); }
					}
					namespace uart_camera2 {
						void initialize(uint32 BoudRate_) {
							if(!Device.Camera2_uart.is_initialize())Device.Camera2_uart.initialize(Device.Camera2_uart_register);
							if(Device.Camera2_uart.is_lock())Device.Camera2_uart.unlock();
							Device.Camera2_uart.lock(Device.Camera2_uart_Baudrate, Device.Camera2_uart_flowctrl, uart_camera2::Camera2TxInterruptFunc, uart_camera2::Camera2RxInterruptFunc);
						}
						void disable() { Device.Camera2_uart_register.enable(0); }

						void fputc(uint8 data_) { Device.Camera2_uart.send_data(data_); }
						void fput_set_interrupt() { Device.Camera2_uart.send_set_flag(); }
						void fput_clear_interrupt() { Device.Camera2_uart.send_clear_flag(); }
						void fput_enable_interrupt() { Device.Camera2_uart.send_enable(); }
						void fput_disable_interrupt() { Device.Camera2_uart.send_disable(); }

						uint8 fgetc() { return Device.Camera2_uart.recv_data(); }
						void fget_set_interrupt() { Device.Camera2_uart.recv_set_flag(); }
						void fget_clear_interrupt() { Device.Camera2_uart.recv_clear_flag(); }
						void fget_enable_interrupt() { Device.Camera2_uart.recv_enable(); }
						void fget_disable_interrupt() { Device.Camera2_uart.recv_disable(); }
					}
				}*/
//=================================Courier===============================
				namespace courier{
					namespace uart{
						uint8 fget() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)return Device.MobilePhone_uart.recv_data();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)return Device.RF_uart.recv_data();
							else return 0;
						}
						void fget_set_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_set_flag();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_set_flag();
						}
						void fget_clear_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_clear_flag();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_clear_flag();
						}
						void fget_enable_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_enable();
						}
						void fget_disable_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_disable();
						}
						bool fget_is_interrupt_enable() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)return Device.MobilePhone_uart.recv_is_enable();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)return Device.RF_uart.recv_is_enable();
							else return false;
						}

						void fput(uint8 data_) {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_data(data_);
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_data(data_);
						}
						void fput_set_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_set_flag();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_set_flag();
						}
						void fput_clear_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_clear_flag();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_clear_flag();
						}
						void fput_enable_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_enable();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_enable();
						}
						void fput_disable_interrupt() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_disable();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.send_disable();
						}
						bool fput_is_interrupt_enable() {
							if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)return Device.MobilePhone_uart.send_is_enable();
							else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)return Device.RF_uart.send_is_enable();
							else return false;
						}
					}
				}
//=================================GPS module===============================
				namespace gps{
					namespace gps1{
						
					}
					namespace gps2{
						
					}
				}
				
			}
		}
	}
}
#
#endif
