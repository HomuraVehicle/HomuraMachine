#ifndef HMR_MACHINE_MIHARA_COURIER_INC
#define HMR_MACHINE_MIHARA_COURIER_INC 100
#
#include<hmLib_v3_06/type.h>
namespace hmr {
	namespace machine{
		namespace mihara {
			namespace courier {
				enum mode {null,rf,mp,rf_debug};
				//rf : RF:9600
				//mp : MP:38400
				//rf_debuf : RF:38400

				//初期化
				void initialize();
				//終端化
				void finalize();
				//タスク処理
				void work();
				//ModuleIDに電源を変更する
				void power_switch(boolian onoff, mode ModuleID_);
				//現在のModuleIDを取得する
				mode getModuleID(void);
			}
		}
	}
}
#
#endif
