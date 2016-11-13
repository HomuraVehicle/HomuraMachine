#ifndef HMR_MACHINE_MIHARA_SYSTEMBASE_INC
#define HMR_MACHINE_MIHARA_SYSTEMBASE_INC 100
#
#include<XCBase/chain.hpp>
namespace hmr {
	namespace machine {
		namespace mihara{
			namespace systems{
				namespace mode{
					enum type{ drive, sleep };
				}
			}
			struct system_client_interface{
				virtual void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_) = 0;
			};
			struct system_client_holder{
			private:
				system_client_interface* pClient;
			public:
				system_client_holder(system_client_interface& rClient_) :pClient(&rClient_){}
				system_client_holder(const system_client_holder& Other) :pClient(Other.pClient){}
				system_client_holder& operator=(const system_client_holder& Other){
					if(this != &Other){
						pClient = Other.pClient;
					}
					return *this;
				}
			public:
				void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_){
					pClient->operator()(NewMode_, PreMode_);
				}
			};
			namespace systems{
				typedef xc::chain<system_client_holder> chain;
				typedef typename chain::element element;
			}
			struct system_host_interface{
				virtual void regist(systems::element& rElement_) = 0;
			};
		}
	}
}
#
#endif
