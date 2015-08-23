#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEFUNCSPRITE_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEFUNCSPRITE_INC 100
#
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace sprite{
				void uart_enable_fget_interrupt();
				void uart_disable_fget_interrupt();
				void uart_enable_fput_interrupt();
				void uart_disable_fput_interrupt();
				void timer_enable_interrupt();
				void timer_disable_interrupt();
			}
		}
	}
}
#
#endif