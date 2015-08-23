#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFUNCCOURIER_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFUNCCOURIER_INC 100
#
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace courier{
				void uart_enable_fget_interrupt();
				void uart_disable_fget_interrupt();
				void uart_enable_fput_interrupt();
				void uart_disable_fput_interrupt();
			}
		}
	}
}
#
#endif
