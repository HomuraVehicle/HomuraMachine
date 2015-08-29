#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEMESSAGE_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEMESSAGE_INC 102

/*
===hmrDeviceManage===

v1_02/131019 amby
	メッセージ関数の追加 その他の機能はインターフェースへ…

v1_01/131013 hmIto
	インターフェースを大幅に変更＆整理
		モードからクロック速度指定を除外
*/

#include<homuraLib_v2/type.hpp>
#include<hmLib_v3_06/cstring.h>
#include"Device.hpp"

namespace hmr{
	namespace machine{
		namespace mihara{
			namespace devmngmsg{
				void initialize(void);
				void finalize(void);

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
