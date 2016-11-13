#ifndef HMR_MACHINE_MIHARA_BATTERY_INC
#define HMR_MACHINE_MIHARA_BATTERY_INC 100
#
/*
�o�b�e���[�Ǘ��p�R�}���h

=== Battery ===

=== hmrBattery ===
v0_02/131019 iwahori
	�^�X�N�V�X�e���p��task_setup_talk��task_interrupt���쐬
v0_01/130105 iwahori
	work�֐���setup_lisen��setup_talk�ɕ���
v0_00/121208 hmIto
	���{�֐��쐬
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace battery {
				//batttery�Ď��p�֐��Q
				void initialize();
				void finalize();
				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void setup_listen(void);
				void setup_talk(void);
				void task_setup_talk(void);
			}
		}
	}
}
#
#endif
