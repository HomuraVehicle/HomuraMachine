#ifndef HMR_MACHINE_MIHARA_DRIVE_INC
#define HMR_MACHINE_MIHARA_DRIVE_INC 100
#
/*
===hmrDrive===
モーター、マニュピュレータ系をコントロール

=== hmr::machine::mihara::drive ===
v1_00/141121 hmIto
	cpp化
=== hmrDrive ===
v0_00/130112 iwahori
	ファイルを作成・モーターのメッセージ関数をtalkとlistenに分解
*/
#include<hmLib_v3_06/cstring.h>
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace motor {
				//*********************** 駆動
				void initialize();
				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void setup_listen(void);
				void setup_talk(void);
			}
		}
	}
}
#
#endif

