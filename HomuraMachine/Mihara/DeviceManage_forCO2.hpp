#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFORCO2_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFORCO2_INC 100
#
#include<homuraLib_v2/type.hpp>
#include<XC32Lib/sfr/interrupt.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng{
				namespace co2{
					void PumpPower(bool val_);
					void SensorPower(bool val_);
				}
			}
		}
	}
}
#
#endif
