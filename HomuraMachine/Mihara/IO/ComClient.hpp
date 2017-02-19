#ifndef HMR_MACHINE_IOCOMCLIENT_INC
#define HMR_MACHINE_IOCOMCLIENT_INC 100
#
namespace hmr{
	namespace machine{
		namespace mihara{
			namespace io{
				struct com_client_interface{
					virtual void inform_timeout() = 0;
					virtual bool is_fullduplex()const = 0;
				};
			}
		}
	}
}
#
#endif
