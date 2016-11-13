#ifndef HMR_MACHINE_MIHARA_MESSAGEBASE_INC
#define HMR_MACHINE_MIHARA_MESSAGEBASE_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/chain.hpp>
#include"Com.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			typedef hmLib::cstring cstring;
			struct message_client_interface{
				virtual void setup_talk(void) = 0;
				virtual bool talk(cstring* pStr) = 0;
				virtual void setup_listen(void) = 0;
				virtual bool listen(cstring Str) = 0;
			};
			struct message_client_holder{
			private:
				com::did_t ID;
				message_client_interface* pClient;
			public:
				message_client_holder(com::did_t ID_, message_client_interface& rClient_) :ID(ID_), pClient(&rClient_){}
				message_client_holder(const message_client_holder& Other) :ID(Other.ID), pClient(Other.pClient){}
				message_client_holder& operator=(const message_client_holder& Other){
					if(this != &Other){
						ID = Other.ID;
						pClient = Other.pClient;
					}
					return *this;
				}
			public:
				com::did_t id()const{ return ID; }
				void setup_talk(void){ pClient->setup_talk(); }
				bool talk(cstring* pStr){ return pClient->talk(pStr); }
				void setup_listen(void){ pClient->setup_listen(); }
				bool listen(cstring Str){ return pClient->listen(Str); }
			};
			namespace message{
				typedef xc::chain<message_client_holder> chain;
				typedef typename chain::element element;
			}
			struct message_host_interface{
				virtual void regist(message::element& rElement_) = 0;
			};
		}
	}
}
#
#endif
