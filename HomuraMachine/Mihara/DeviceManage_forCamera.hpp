#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFORCAMERA_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFORCAMERA_INC 100
#
#include<homuraLib_v2/type.hpp>
#include<XC32/sfr/interrupt.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng{
				namespace camera{
					void setLED(bool val_);

					namespace timer{
						void clear_count();
						void clear_interrupt_flag();
						void disable_interrupt();
						void enable_interrupt();
						void initialize(uint32 MSec_);

						class interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern interrupt_function CameraInterruptFunc;
					}

					namespace uart_camera1{
						void initialize(uint32 BoudRate_);
						void disable();

						void fputc(uint8 data_);
						void fput_set_interrupt();
						void fput_clear_interrupt();
						void fput_enable_interrupt();
						void fput_disable_interrupt();

						uint8 fgetc();
						void fget_set_interrupt();
						void fget_clear_interrupt();
						void fget_enable_interrupt();
						void fget_disable_interrupt();

						class tx_interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern tx_interrupt_function Camera1TxInterruptFunc;
						class rx_interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern rx_interrupt_function Camera1RxInterruptFunc;
					}

					namespace uart_camera2{
						void initialize(uint32 BoudRate_);
						void disable();

						void fputc(uint8 data_);
						void fput_set_interrupt();
						void fput_clear_interrupt();
						void fput_enable_interrupt();
						void fput_disable_interrupt();

						uint8 fgetc();
						void fget_set_interrupt();
						void fget_clear_interrupt();
						void fget_enable_interrupt();
						void fget_disable_interrupt();

						class tx_interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern tx_interrupt_function Camera2TxInterruptFunc;
						class rx_interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern rx_interrupt_function Camera2RxInterruptFunc;
				}
				}
			}
		}
	}
}
#
#endif
