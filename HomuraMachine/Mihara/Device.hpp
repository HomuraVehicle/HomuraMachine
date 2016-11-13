#ifndef HMR_MACHINE_MIHARA_DEVICE_INC
#define HMR_MACHINE_MIHARA_DEVICE_INC 100
#
#include<XC32/clock.hpp>
#include<XC32/interrupt.hpp>
#include<XC32/sfr/uart1.hpp>
#include<XC32/sfr/uart2.hpp>
#include<XC32/sfr/uart3.hpp>
#include<XC32/sfr/uart4.hpp>
#include<XC32/sfr/uart5.hpp>
#include<XC32/sfr/uart6.hpp>
#include<XC32/sfr/spi1.hpp>
#include<XC32/sfr/spi2.hpp>
#include<XC32/sfr/spi3.hpp>
#include<XC32/sfr/i2c5.hpp>
#include<XC32/sfr/timer1.hpp>
#include<XC32/sfr/timer2.hpp>
#include<XC32/sfr/timer3.hpp>
#include<XC32/sfr/timer4.hpp>
#include<XC32/sfr/timer5.hpp>
#include<XC32/sfr/timer6.hpp>
#include<XC32/sfr/timer7.hpp>
#include<XC32/sfr/timer8.hpp>
#include<XC32/device.hpp>
#include<XC32/pin.hpp>
#include<XC32/sfr/port.hpp>
#include<XC32/wdt.hpp>
#include<XC32/timer.hpp>
#include<XC32/uart.hpp>
#include<XC32/adc.hpp>
#include<XC32/i2c.hpp>
#include<XC32/spi.hpp>
#include<XC32/sfr/adc1.hpp>
#include<homuraLib_v2/machine/device/kk10.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			struct cDevice {
				typedef device::kk10 my_device;
			private:
				//‹¤—Ladc
				struct shared_adc_identifer{};
				typedef xc32::async_interrupt_adc<xc32::sfr::adc_block, shared_adc_identifer> default_adc;
				//‹¤—Li2c
				struct shared_i2c5_default_identifer{};
				typedef xc32::shared_i2c<xc32::sfr::i2c5,shared_i2c5_default_identifer> shared_i2c5;
				//‹¤—Lspi
				struct shared_spi2_default_identifer{};
				typedef xc32::shared_spi<xc32::sfr::spi2,shared_spi2_default_identifer> shared_spi2;
			private:
				my_device KK10;
				default_adc ADC;
				shared_i2c5 I2C5;
				shared_spi2 SPI2;
//				my_device::opinDevicePower PinDevicePower;
//				xc32::wdt WDT;
//				xc::lock_guard<xc32::wdt> WDTLock;
			public:
				class io_device {
				protected:
					//courier stream tx interrupt priority level 3
					//courier stream rx interrupt priority level 4
					typedef xc32::sfr::uart1 RF0_uart_register;
					typedef xc32::sfr::uart3 RF1_uart_register;
					typedef my_device::opinRF0Power RF0_power;
					typedef my_device::opinRF1Power RF1_power;
				};

				class debugger_device {
				protected:
					typedef xc32::sfr::uart3 debug_uart_register;
					typedef xc32::sfr::timer1 timer_register;
					typedef xc32::output_pin<xc32::sfr::portC::pin15, true> pinGreenLED;
				};

				class gps_device {
				protected:
					typedef xc32::sfr::uart4 gps1_uart_register;
					typedef xc32::sfr::uart2 gps2_uart_register;
					typedef my_device::opinGPS0Power gps1_power;
					typedef my_device::opinGPS1Power gps2_power;
				};

				class sprite_device {
				protected:
					typedef xc32::sfr::uart5 uart_register;
					typedef my_device::opinHeadLightPower powerLight;
					typedef my_device::opinCamera0Power powerCamera;
					typedef my_device::opinCamera1Power powerCamera2;
				};

				class service_device {
				protected:
					typedef xc32::sfr::timer3 chrono_timer_register;
					typedef xc32::sfr::timer4 delay_timer_register;
					typedef xc32::sfr::timer5 exclusive_delay_timer_register;
				};

				class system_device {
				protected:
					typedef xc32::sfr::timer6 longtimer_register1;
					typedef xc32::sfr::timer7 longtimer_register2;
				};

				class microphone_device {
				protected:
//					typedef xc32::sfr::timer1 timer_register;
//					typedef xc32::sfr::spi1 spi_register;
				};

				class co2_device {
				protected:
					typedef default_adc::analog_pin<my_device::apinCO2> apinData;
					typedef my_device::opinCO2PumpsPw powerPump;
					typedef my_device::opinCO2SensorPw powerSensor;
				};

				class battery_device {
				protected:
					typedef default_adc::analog_pin<my_device::apinMainBoardBattery> apinData1;
					typedef default_adc::analog_pin<my_device::apinRightMotorBattery> apinData2;
					typedef default_adc::analog_pin<my_device::apinLeftMotorBattery> apinData3;
				};

				class thermo_device {
				protected:
					typedef default_adc::analog_pin<my_device::apinThermo> apinData;
				};

				class motor_device {
				protected:
					typedef my_device::opinMotorLA pinMotorLA;
					typedef my_device::opinMotorLB pinMotorLB;
					typedef my_device::opinMotorRA pinMotorRA;
					typedef my_device::opinMotorRB pinMotorRB;
				};
				class inertial_device{
				protected:
					typedef xc32::sfr::i2c5 AcceleCompass_i2c;
					typedef xc32::sfr::i2c5 Gyro_i2c;
					typedef shared_i2c5_default_identifer AccelCompass_shaerd_i2c_identifer;
					typedef shared_i2c5_default_identifer Gyro_shared_i2c_identifer;
					typedef my_device::opinInertialPower powerInertial;
				};

				class devmng_device {
				protected:
					typedef xc32::sfr::timer8 timer_register;
					typedef my_device::opinRedLED pinRedLED;
					typedef my_device::opinDevicePower pinDevicePower;

//================================================================================================
					//=====DIP pin=====
					typedef my_device::ipinDip1 pinDip1;
					typedef my_device::ipinDip2 pinDip2;
					typedef my_device::ipinDip3 pinDip3;
					typedef my_device::ipinDip4 pinDip4;

					//====rf module====
					typedef my_device::opinRF0Power pinRF_Power;
					typedef my_device::opinRF1Power pinMobilePhone_Power;
					typedef xc32::sfr::uart1 rf_uart_register;
					typedef xc32::sfr::uart3 mobilephone_uart_register;

					//====SD card=====
					typedef my_device::opinSDPower pinSDPower;
					typedef my_device::opinSD_SPISelect pinSD_SPISelect;
					typedef xc32::sfr::spi2 sdcard_spi_register;

					//====CO2 sensor====
					typedef my_device::opinCO2PumpsPw pinCO2PumpsPower;
					typedef my_device::opinCO2SensorPw pinCO2SensorPower;

					//====ADC====
					typedef my_device::opinADC0Power pinADC0Power;
					typedef my_device::opinADC1to4Power pinADC1to4Power;
					
					//====Motor====
					typedef my_device::opinMotorLA pinMotorLA;
					typedef my_device::opinMotorLB pinMotorLB;
					typedef my_device::opinMotorRA pinMotorRA;
					typedef my_device::opinMotorRB pinMotorRB;
					typedef my_device::opinMotorPower pinMotorPower;

					typedef my_device::opin5VDCDC pin5VDCDC;
					typedef my_device::opinExternalI2C_Pw pinExternalI2C_Pw;

//================================================================================================
				};
			public:
				cDevice():KK10()/*:WDTLock(WDT)*/ {
					I2C5.lock(xc32::i2c::clockmode::type::_400kHz,0);
					SPI2.lock(true,true,1);
					}
				~cDevice() {
					I2C5.unlock();
					SPI2.unlock();
				}
				void operator()(void) {
//					WDT.clear();
				}
			};
		}
	}
}
#
#endif
