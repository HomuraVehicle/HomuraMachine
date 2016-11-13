#ifndef HMR_MACHINE_MIHARA_SYSTEMBASE_INC
#define HMR_MACHINE_MIHARA_SYSTEMBASE_INC 100
#
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace system{
				namespace mode {
					enum type { drive, sleep };
				}
			}
			struct system_client_interface{
				virtual void operator()(system::mode::type NewMode_, system::mode::type PreMode_) = 0;
			};
			struct inform_host_interface{
				virtual void regist(system_client_interface& Client_) = 0;
			};
		}
	}
}
#
#endif
