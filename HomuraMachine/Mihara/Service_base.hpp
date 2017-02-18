#ifndef HMR_MACHINE_MIHARA_SERVICEBASE_INC
#define HMR_MACHINE_MIHARA_SERVICEBASE_INC 100
#
#include<homuraLib_v2/delay.hpp>
#include<homuraLib_v2/task.hpp>
namespace hmr{
	namespace machine{
		namespace mihara{
			struct service_interface{
				virtual hmr::delay_interface& delay() = 0;
				virtual hmr::task::host_interface& task() = 0;
			};
		}
	}
}
#
#endif
