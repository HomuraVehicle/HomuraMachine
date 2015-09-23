#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFORCOURIER_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFORCOURIER_INC 100
#
#include<homuraLib_v2/type.hpp>
#include<XC32Lib/sfr/interrupt.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng{
				namespace courier{
					namespace uart{
						uint8 fget();
						void fget_set_interrupt();
						void fget_clear_interrupt();
						void fget_enable_interrupt();
						void fget_disable_interrupt();
						bool fget_is_interrupt_enable();

						void fput(uint8 data_);
						void fput_set_interrupt();
						void fput_clear_interrupt();
						void fput_enable_interrupt();
						void fput_disable_interrupt();
						bool fput_is_interrupt_enable();

						class tx_interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern tx_interrupt_function CourierTxInterruptFunc;
						class rx_interrupt_function:public xc32::sfr::interrupt::function{
						private:
							virtual void operator()(void);
						};
						extern rx_interrupt_function CourierRxInterruptFunc;
					}
				}
			}
		}
	}
}
#
#endif
