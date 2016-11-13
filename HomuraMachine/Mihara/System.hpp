#ifndef HMR_MACHINE_MIHARA_SYSTEM_INC
#define HMR_MACHINE_MIHARA_SYSTEM_INC 100
#
#include"System_base.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			struct cSystem : public system_host_interface{
				systems::chain Chain;
			public:
				void regist(systems::element& rElement_){
					Chain.push_back(rElement_);
				}
			};
		}
	}
}
#
#endif