#ifndef HMR_MACHINE_MIHARA_THERMO_INC
#define HMR_MACHINE_MIHARA_THERMO_INC 100
#
/*
�o�b�e���[�Ǘ��p�R�}���h
=== hmr::machine::mihara::thermo ===
v1_00/141121 hmIto
	cpp��
=== hmrThermo ===
v0_02/131019 iwahori
	�^�X�N�V�X�e���p��task_setup_talk��task_interrupt���쐬
v0_01/130112 iwahori
	work�֐���setup_listen��setup_talk�ɕ���
v0_00/121208 hmIto
	���{�֐��쐬
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace thermo {
				//batttery�Ď��p�֐��Q
				void initialize();
				void finalize();
				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void setup_listen(void);
				void setup_talk(void);
				void task_setup_listen(void);
				void task_setup_talk(void);
			}
		}
	}
}
#
#endif
