#ifndef HMR_MACHINE_MIHARA_THERMO_INC
#define HMR_MACHINE_MIHARA_THERMO_INC 100
#
/*
バッテリー管理用コマンド
=== hmr::machine::mihara::thermo ===
v1_00/141121 hmIto
	cpp化
=== hmrThermo ===
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v0_01/130112 iwahori
	work関数をsetup_listenとsetup_talkに分離
v0_00/121208 hmIto
	基本関数作成
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace thermo {
				//batttery監視用関数群
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
