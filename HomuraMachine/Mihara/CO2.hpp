#ifndef HMR_MACHINE_MIHARA_CO2_INC
#define HMR_MACHINE_MIHARA_CO2_INC 100
#
/*
CO2���W���[�������p
=== hmr::machine::mihara::CO2 ===
v1_00/141111 hmIto
	C++��
=== hmrCO2 ===
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
			namespace co2 {
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
