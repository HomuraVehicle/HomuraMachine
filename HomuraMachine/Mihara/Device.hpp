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
				//可読性を挙げるために、一度ここで宣言してから、各モジュールに割り当て
				static inline uint8 task_timer_ipl(){ return 2; }
				static inline uint8 camera1_uart_tx_ipl(){ return 6; }
				static inline uint8 camera1_uart_rx_ipl(){ return 7; }
				static inline uint8 camera2_uart_tx_ipl(){ return 6; }
				static inline uint8 camera2_uart_rx_ipl(){ return 7; }
				static inline uint8 camera_timer_ipl(){ return 2; }
				static inline uint8 rf_uart_tx_ipl(){ return 3; }
				static inline uint8 rf_uart_rx_ipl(){ return 4; }
				static inline uint8 mp_uart_tx_ipl(){ return 3; }
				static inline uint8 mp_uart_rx_ipl(){ return 4; }
				//以下の関数、管理者不明　そもそも、どこで使えばよい・・・？
				static inline uint8 main_ipl(){ return 1; }
				static inline uint8 lock_ipl(){ return 5; }
				static inline uint8 stronglock_ipl(){ return 7; }
			private:
				//共有adc
				struct shared_adc_identifer{};
				typedef xc32::async_interrupt_adc<xc32::sfr::adc_block, shared_adc_identifer> async_adc;
				//共有i2c
				struct shared_i2c5_default_identifer{};
				typedef xc32::shared_i2c<xc32::sfr::i2c5,shared_i2c5_default_identifer> shared_i2c;
				//共有spi
				struct shared_spi2_default_identifer{};
				typedef xc32::shared_spi<xc32::sfr::spi2,shared_spi2_default_identifer> shared_spi;
			private:
				my_device KK10;
			public:
				class io_device {
				protected:
					//courier stream tx interrupt priority level 3
					//courier stream rx interrupt priority level 4
					typedef xc32::output_pin<xc32::sfr::portC::pin15, true> pinGreenLED;
					typedef xc32::sfr::uart1 RF0_uart_register;
					static inline uint8 rf_uart_tx_ipl(){ return cDevice::rf_uart_tx_ipl(); }
					static inline uint8 rf_uart_rx_ipl(){ return cDevice::rf_uart_rx_ipl(); }
					typedef xc32::sfr::uart3 RF1_uart_register;
					static inline uint8 mp_uart_tx_ipl(){ return cDevice::mp_uart_tx_ipl(); }
					static inline uint8 mp_uart_rx_ipl(){ return cDevice::mp_uart_rx_ipl(); }
					typedef my_device::opinRF0Power RF0_power;
					typedef my_device::opinRF1Power RF1_power;
				};

				class debugger_device {
				protected:
					typedef xc32::sfr::uart3 debug_uart_register;
					typedef xc32::sfr::timer1 timer_register;
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
					static inline uint8 camera1_uart_tx_ipl(){ return cDevice::camera1_uart_tx_ipl(); }
					static inline uint8 camera1_uart_rx_ipl(){ return cDevice::camera1_uart_rx_ipl(); }
					static inline uint8 camera2_uart_tx_ipl(){ return cDevice::camera2_uart_tx_ipl(); }
					static inline uint8 camera2_uart_rx_ipl(){ return cDevice::camera2_uart_rx_ipl(); }
					static inline uint8 camera_timer_ipl(){ return cDevice::camera_timer_ipl(); }
					typedef my_device::opinHeadLightPower powerLight;
					typedef my_device::opinCamera0Power powerCamera;
					typedef my_device::opinCamera1Power powerCamera2;
				};

				class service_device {
				protected:
					typedef xc32::sfr::timer3 chrono_timer_register;
					typedef xc32::sfr::timer4 delay_timer_register;
					typedef xc32::sfr::timer5 exclusive_delay_timer_register;
				protected:
					typedef xc32::sfr::timer6 longtimer_register1;	//不使用？
					typedef xc32::sfr::timer7 longtimer_register2;	//不使用？
					typedef xc32::sfr::timer8 task_timer_register;
					static inline uint8 task_timer_ipl(){ return cDevice::task_timer_ipl(); }
				protected:
					typedef cDevice::async_adc async_adc;
					typedef my_device::opinADC0Power pinADC0Power;
					typedef my_device::opinADC1to4Power pinADC1to4Power;
				protected:
					typedef my_device::opin5VDCDC pin5VDCDC;
				protected:
					typedef cDevice::shared_i2c shared_i2c;
					typedef my_device::opinExternalI2C_Pw pinExternalI2C_Pw;
				protected:
					typedef cDevice::shared_spi shared_spi;
				};

				class system_device {
				protected:
					//=====DIP pin=====
					typedef my_device::ipinDip1 pinDip1;
					typedef my_device::ipinDip2 pinDip2;
					typedef my_device::ipinDip3 pinDip3;
					typedef my_device::ipinDip4 pinDip4;
					typedef my_device::opinRedLED pinRedLED;
					typedef my_device::opinDevicePower pinDevicePower;
				};

				class microphone_device {
				protected:
//					typedef xc32::sfr::timer1 timer_register;
//					typedef xc32::sfr::spi1 spi_register;
				};

				class co2_device {
				protected:
					typedef async_adc::analog_pin<my_device::apinCO2> apinData;
					typedef my_device::opinCO2PumpsPw powerPump;
					typedef my_device::opinCO2SensorPw powerSensor;
				};

				class battery_device {
				protected:
					typedef async_adc::analog_pin<my_device::apinMainBoardBattery> apinData1;
					typedef async_adc::analog_pin<my_device::apinRightMotorBattery> apinData2;
					typedef async_adc::analog_pin<my_device::apinLeftMotorBattery> apinData3;
				};

				class thermo_device {
				protected:
					typedef async_adc::analog_pin<my_device::apinThermo> apinData;
				};

				class motor_device {
				protected:
					typedef my_device::opinMotorLA pinMotorLA;
					typedef my_device::opinMotorLB pinMotorLB;
					typedef my_device::opinMotorRA pinMotorRA;
					typedef my_device::opinMotorRB pinMotorRB;
					typedef my_device::opinMotorPower pinMotorPower;
				};

				class inertial_device{
				protected:
					typedef xc32::sfr::i2c5 AcceleCompass_i2c;
					typedef xc32::sfr::i2c5 Gyro_i2c;
					typedef shared_i2c5_default_identifer AccelCompass_shaerd_i2c_identifer;
					typedef shared_i2c5_default_identifer Gyro_shared_i2c_identifer;
					typedef my_device::opinInertialPower powerInertial;
				};

				class logger_device{
				protected:
					//====SD card=====
					typedef my_device::opinSDPower pinSDPower;
					typedef my_device::opinSD_SPISelect pinSD_SPISelect;
					typedef xc32::sfr::spi2 sdcard_spi_register;
				};

			private:

			public:
				cDevice():KK10(){}
			};
		}
	}
}
#
#endif
