#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFORINERTIAL_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFORINERTIAL_INC 100
#
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng{
				namespace inertial{
					namespace axcel{
						bool i2c_start(uint8 address_,bool IO_);
						void i2c_stop();
						bool i2c_write(uint8 data_);
						uint8 i2c_read(bool EndFlag_);
						bool i2c_restart(uint8 address_,bool IO_);
					}

					namespace compass{
						bool i2c_start(uint8 address_,bool IO_);
						void i2c_stop();
						bool i2c_write(uint8 data_);
						uint8 i2c_read(bool EndFlag_);
						bool i2c_restart(uint8 address_,bool IO_);
					}

					namespace gyro{
						bool i2c_start(uint8 address_,bool IO_);
						void i2c_stop();
						bool i2c_write(uint8 data_);
						uint8 i2c_read(bool EndFlag_);
						bool i2c_restart(uint8 address_,bool IO_);
					}
				}
			}
		}
	}
}
#
#endif
