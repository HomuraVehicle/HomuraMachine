#ifndef HMR_MACHINE_MIHARA_SERVICE_CPP_INC
#define HMR_MACHINE_MIHARA_SERVICE_CPP_INC
#
#include<homuraLib_v2/machine/service/delay.hpp>
#include<homuraLib_v2/machine/service/exclusive_delay.hpp>
#include<homuraLib_v2/task.hpp>
#include"Service.hpp"
#include"System_base.hpp"
#include"Device.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			namespace{
				cService<cDevice::service_device>* pService = 0;
			}
			void delay_ms(unsigned int ms_){
				pService->delay_ms(ms_);
			}
			void exclusive_delay_ms(unsigned int ms_){
				pService->exclusive_delay_ms(ms_);
			}

			namespace task{
				bool start(hmr::task::client_interface& Client_, hmr::task::duration Interval_, hmr::task::duration Count_ = 0){return pService->task_start(Client_, Interval_, Count_); }
				void quick_start(hmr::task::client_interface& Client_, hmr::task::duration Interval_, hmr::task::duration Count_ = 0){pService->task_quick_start(Client_, Interval_, Count_); }
				bool is_start(hmr::task::client_interface& Client_){ return pService->task_is_start(Client_); }
				bool restart(hmr::task::client_interface& Client_, hmr::task::duration Interval_, hmr::task::duration Count_ = 0){ return pService->task_restart(Client_, Interval_, Count_); }
				void stop(hmr::task::client_interface& Client_){ pService->task_stop(Client_); }
			}
		}
		namespace mihara{
			namespace services{
				void initialize(cService<cDevice::service_device>& Service_){pService = &Service_;}
			}
		}
	}
}
#
#endif
