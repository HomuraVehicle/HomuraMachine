#ifndef HMR_MACHINE_MIHARA_GPS_INC
#define HMR_MACHINE_MIHARA_GPS_INC 100
#
/*
GPS�֘A���W���[��
=== GPS ===
v1_00/141115 hmIto
	CPP��
===hmrGPS===
v0_03/131108 hmIto
	�^�X�N�ɂ̂���GPS�����ۂɐ؂��ւ������܂����������C��
v0_02/131019 iwahori
	�^�X�N�V�X�e���p��task_setup_talk��task_interrupt���쐬
v0_01/130105 iwahori
	work�֐���setup_lisen��setup_talk�ɕ���
v0_00/121208 hmIto
	���{�֐��쐬
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
