#ifndef HMR_MACHINE_MIHARA_SERVICEBASE_INC
#define HMR_MACHINE_MIHARA_SERVICEBASE_INC 100
#
#include<homuraLib_v2/task.hpp>
namespace hmr{
	namespace machine{
		namespace mihara{
			void delay_ms(unsigned int ms_);
			void exclusive_delay_ms(unsigned int ms_);
			namespace task{
				bool start(hmr::task::client_interface& Client_, hmr::task::duration Interval_, hmr::task::duration Count_ = 0);
				void quick_start(hmr::task::client_interface& Client_, hmr::task::duration Interval_, hmr::task::duration Count_ = 0);
				bool is_start(hmr::task::client_interface& Client_);
				bool restart(hmr::task::client_interface& Client_, hmr::task::duration Interval_, hmr::task::duration Count_ = 0);
				void stop(hmr::task::client_interface& Client_);
			}
		}
	}
}
#
#endif
