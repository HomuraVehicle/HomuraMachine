#ifndef HMR_MACHINE_MIHARA_MESSAGEBASE_INC
#define HMR_MACHINE_MIHARA_MESSAGEBASE_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/chain.hpp>
#include"Com.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			struct message_client_interface: public ::xc::chain_element{
			public:
				typedef hmLib::cstring cstring;
				typedef com::did_t did_t;
			private:
				did_t ID;
			public:
				message_client_interface(com::did_t ID_):ID(ID_){}
			public:
				virtual void setup_talk(void) = 0;
				virtual bool talk(cstring* pStr) = 0;
				virtual void setup_listen(void) = 0;
				virtual bool listen(cstring Str) = 0;
			};
			namespace message{
				typedef ::xc::chain<message_client_interface> chain;
			}
			struct message_host_interface{
				virtual void regist(message_client_interface& Client_) = 0;
			};

		}
	}
}
#
#endif
