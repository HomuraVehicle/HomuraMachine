#ifndef HMR_MACHINE_MIHARA_SERVICE_CPP_INC
#define HMR_MACHINE_MIHARA_SERVICE_CPP_INC
#
#include<homuraLib_v2/machine/service/delay.hpp>
#include<homuraLib_v2/machine/service/exclusive_delay.hpp>
#include"Service.hpp"
namespace hmr{
	namespace machine{
		namespace mihara {
			xc32::delay_ms_timer<cService::delay_timer_register> cService::delay_timer;
			xc32::delay_ms_timer<cService::exclusive_delay_timer_register> cService::exclusive_delay_timer;
//			cService::clock_timer cService::Clock;
//			cService::task_host_type cService::TaskHost;
		}
		namespace service {
			void delay_ms(unsigned int ms_) {
				mihara::cService::delay_timer(ms_);
			}
			void exclusive_delay_ms(unsigned int ms_) {
				xc32::interrupt::lock_guard Lock(xc32::interrupt::Mutex);
				mihara::cService::exclusive_delay_timer(ms_);
			}
//			hmr::chrono::clock_interface& Chrono(mihara::cService::clock());
		}
	}
}
#
#endif
