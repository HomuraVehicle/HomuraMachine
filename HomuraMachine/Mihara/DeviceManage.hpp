#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGE_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGE_INC 200
#
#include<homuraLib_v2/type.hpp>
/*
Device����p�̋@�\��񋟂���
	Kill�@�\
		�������g�Ƃ��ׂĂ̂̓d����؂�
	�X���[�v�@�\
		����I�ɓd�����I��/�I�t���J��Ԃ�
	���[�~���O�@�\
		Roaming���[�h�ɓ���ƁARF(RoamingTime)=>MB(RoamingTime)=>RF�̂悤�ɁA�g�p�ʐM�@�̑J�ڂ��J��Ԃ��B
		RoamingTime�͓����ϐ��Ō��肷��i�R�}���h�Őݒ�\�j
	�N���b�N���[�h�ύX�@�\
		�ݒ�N���b�N���x��ύX����
=== hmr::machine::mihara::DeviceManage ===
v2_00/141117 hmito
	C++��
===hmrDeviceManage===
v1_03/131026 amby
	���荞�݋��֐��R��ǉ��A���̃N���X�ŊǗ�����
v1_02/131019 hmIto
	clockmode��clock�ɕύX
	isSleep, isRoaming��ǉ�
v1_01/131013 hmIto
	�C���^�[�t�F�[�X��啝�ɕύX������
		���[�h����N���b�N���x�w������O
*/
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng {
				typedef void(*vFp_v)(void);

				//=== ��{��` ===
				#define RF_BaudRate 9600
				#define RFDebug_BaudRate 38400
				#define MP_BaudRate 38400

				//=== ��{����֐� ===
				//�f�o�C�X�������֐�
				void initialize(void);
				//�f�o�C�X�I�[���֐�
				void finalize(void);
				//�f�o�C�X�����I��
				void kill(void);
				//���݂̒ʐM�悪�S��d�ʐM�Ή�����Ԃ�
				bool isFullDuplex(void);
				//clear_wdt
				void clear_wdt(void);
				//ModuleID
				typedef enum  {
					null=0x00,
					mobile_phone=0x01,
					rf_module=0x02
				}rf_module_mode;
				//���݂�ModuleID���擾����
				rf_module_mode rf_module_uart_getModuleID(void);


				//=== ���[�h����@�\ ===
				typedef enum{ NormalMode, SleepMode, RoamingMode }mode;
				//�f�o�C�X���[�h��ύX����
				void mode_set(mode Mode_);
				//�f�o�C�X���[�h���擾����
				volatile mode mode_get(void);

				//=== �N���b�N����@�\ ===
				typedef enum{ LowClock, NormalClock, HighClock }clock;
				//�N���b�N��ύX����
				void clock_set(clock Clock_);
				//�N���b�N���擾����
				volatile clock clock_get(void);

				//=== �X���[�v���[�h���� ===
				//�X���[�v���[�h�̃p�����[�^���擾����
				void sleep_getInterval(sint16* secNonRem, sint16* secRem);
				//�X���[�v���[�h�̃p�����[�^��ݒ肷��
				void sleep_setInterval(sint16 secNonRem, sint16 secRem);
				//���������N�����ɌĂяo�����ʒm�֐���o�^�ł��邩
				bool sleep_can_registSleepInformFunction();
				//���������N�����ɌĂяo�����ʒm�֐���o�^����
				bool sleep_registSleepInformFunction(vFp_v InformSleep, vFp_v InformWakeUp);
				//�����X���[�v�ɓ���/�o��ۂ̒ʒm�֐���o�^�ł��邩
				bool sleep_can_registRemInformFunction();
				//�����X���[�v�ɓ���/�o��ۂ̒ʒm�֐���o�^����
				bool sleep_registRemInformFunction(vFp_v InformRem, vFp_v InformNonRem);
				//�X���[�v��Ԃ��ǂ������擾����
				volatile bool sleep_isSleep(void);
				//�����X���[�v��Ԃ��ǂ������擾����
				volatile bool sleep_isRem(void);

				//=== ���[�~���O���[�h���� ===
				//���[�~���O���[�h�̃p�����[�^���擾����
				void roaming_getInterval(sint16* secInterval);
				//���[�~���O���[�h�̃p�����[�^��ݒ肷��
				void roaming_setInterval(sint16 secInterval);
				//���[�~���O��Ԃ��ǂ������擾����
				volatile bool roaming_isRoaming(void);

				//=== ���荞�݊֐� ===
				//�^�X�N�A�X���[�v�A����у��[�~���O���s�p�֐�
				void interrupt_function(sint16 Interval_);
				//���荞�݂�main���[�v�ւ̈Ϗ������֐�
				bool interrupt_yield(void);

				//=== ���荞�ݐ���p�֐�
				// ���荞�݋��ς��Ă悢���ǂ����̃`�F�b�N�֐��@�璷�߂��������E�E
				bool interrupt_can_enable_streamVMC_fget_interrupt(void);
				bool interrupt_can_disable_streamVMC_fget_interrupt(void);
				bool interrupt_can_enable_streamVMC_fput_interrupt(void);
				bool interrupt_can_disable_streamVMC_fput_interrupt(void);

/*				bool interrupt_can_enable_streamCMR_fget_interrupt(void);
				bool interrupt_can_disable_streamCMR_fget_interrupt(void);
				bool interrupt_can_enable_streamCMR_fput_interrupt(void);
				bool interrupt_can_disable_streamCMR_fput_interrupt(void);

				bool interrupt_can_enable_timerCMR_interrupt(void);
				bool interrupt_can_disable_timerCMR_interrupt(void);
*/
				bool interrupt_can_enable_timerDevmng_interrupt(void);
				bool interrupt_can_disable_timerDevmng_interrupt(void);

				// main stream ����M���荞�݋��֐�
				bool interrupt_enable_streamVMC_fget_interrupt(void);
				bool interrupt_disable_streamVMC_fget_interrupt(void);
				bool interrupt_enable_streamVMC_fput_interrupt(void);
				bool interrupt_disable_streamVMC_fput_interrupt(void);

/*				// camera stream ����M���荞�݋��֐�
				bool interrupt_enable_streamCMR_fget_interrupt(void);
				bool interrupt_disable_streamCMR_fget_interrupt(void);
				bool interrupt_enable_streamCMR_fput_interrupt(void);
				bool interrupt_disable_streamCMR_fput_interrupt(void);
				
				// timer CMR�@����M���荞�݋��֐�
				bool interrupt_enable_timerCMR_interrupt(void);
				bool interrupt_disable_timerCMR_interrupt(void);
				*/

				// timer Device Manager ����M���荞�݋��֐�
				bool interrupt_enable_timerDevmng_interrupt(void);
				bool interrupt_disable_timerDevmng_interrupt(void);

			}
		}
	}
}

#
#endif
