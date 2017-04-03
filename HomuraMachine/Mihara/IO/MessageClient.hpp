#ifndef HMR_MACHINE_MIHARA_IO_MESSAGECLIENT_INC
#define HMR_MACHINE_MIHARA_IO_MESSAGECLIENT_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/chain.hpp>
#include"Com.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			struct message_client_interface : public ::xc::chain_element{
			public:
				typedef hmLib::cstring cstring;
				typedef com::did_t did_t;
			private:
				did_t ID;
			public:
				message_client_interface(com::did_t ID_) :ID(ID_){}
			public:
				did_t id()const{ return ID; }
			public:
				virtual void setup_talk(void) = 0;
				virtual bool talk(cstring* pStr) = 0;
				virtual void setup_listen(void) = 0;
				virtual bool listen(cstring Str) = 0;
			};
		}
	}
}
#
#endif
