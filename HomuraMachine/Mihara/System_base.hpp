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
				struct inform_client_interface {
					virtual void inform_system_mode(system::mode::type Mode_,system::mode::type PreMode_)=0;
				};
				struct inform_host_interface {
					virtual void regist(system::inform_client_interface& Client_)=0;
				};
			}
		}
	}
}
#
#endif
