#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFORSDCARD_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFORSDCARD_INC 100
#
#include<homuraLib_v2/type.hpp>
#include<XC32/sfr/interrupt.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng{
				namespace sdcard{
					void Power(bool val_);
					namespace spi{
						void write(uint8 data_);
						uint8 read(uint8 data_);
						bool in_data();
						void select(bool val_);
					}
				}
			}
		}
	}
}
#
#endif
