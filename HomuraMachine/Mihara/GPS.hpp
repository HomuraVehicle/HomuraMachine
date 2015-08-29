#ifndef HMR_MACHINE_MIHARA_GPS_INC
#define HMR_MACHINE_MIHARA_GPS_INC 100
#
/*
GPS関連モジュール
=== GPS ===
v1_00/141115 hmIto
	CPP化
===hmrGPS===
v0_03/131108 hmIto
	タスクにのってGPSが実際に切り替えられまくる問題を修正
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v0_01/130105 iwahori
	work関数をsetup_lisenとsetup_talkに分離
v0_00/121208 hmIto
	基本関数作成
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include<homuraLib_v2/machine/module/GPS52D.hpp>
#include<homuraLib_v2/machine/module/GPSSwitcher.hpp>
#include<HomuraMachine/Mihara/Device.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename gps_device_>
			class cGPS :public gps_device_{
			private:
				typedef module::cGPS52D<typename gps_device_::gps1_uart_register, typename gps_device_::gps1_power> gps1; 
				typedef module::cGPS52D<typename gps_device_::gps2_uart_register, typename gps_device_::gps2_power> gps2;
				gps1 GPS1;
				gps2 GPS2;
			public:
				typedef module::cGPSSwitcher<gps1, gps2> gps_switcher;
				gps_switcher GPSSwitcher;
			public:
				cGPS() :GPS1() , GPS2(), GPSSwitcher(GPS1, GPS2) {}
			};
			namespace gps {
				void initialize(cGPS<cDevice::gps_device>& rGPS_);
				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void setup_listen(void);
				void setup_talk(void);
				void task_setup_talk(void);
				void informSleep();
				void informAwake();
			}
		}
	}
}

#
#endif
