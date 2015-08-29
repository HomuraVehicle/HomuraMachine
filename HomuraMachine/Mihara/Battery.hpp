#ifndef HMR_MACHINE_MIHARA_BATTERY_INC
#define HMR_MACHINE_MIHARA_BATTERY_INC 100
#
/*
バッテリー管理用コマンド

=== Battery ===

=== hmrBattery ===
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v0_01/130105 iwahori
	work関数をsetup_lisenとsetup_talkに分離
v0_00/121208 hmIto
	基本関数作成
*/
#include<hmLib_v3_06/cstring.h>
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace battery {
				//batttery監視用関数群
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
