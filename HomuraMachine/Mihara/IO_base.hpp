#ifndef HMR_MACHINE_MIHARA_IOBASE_INC
#define HMR_MACHINE_MIHARA_IOBASE_INC 100
#
/*!
@brief cUIを利用する上での共通規約の定義。

cIOは、ほむらの通信管理用クラスです。cIO自体は、cSystemなどの制御クラスからモード通知を受けることで自信の通信モードを変更します。
このファイルでは、cIOと他の制御クラスからの通知するうえでのモードや通知手段等が定義されています。
*/
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
				virtual void setup_talk(void) = 0;
				virtual bool talk(cstring* pStr) = 0;
				virtual void setup_listen(void) = 0;
				virtual bool listen(cstring Str) = 0;
			};

			struct io_interface{
				virtual void regist(message_client_interface& Client_) = 0;
			};

			namespace io{
				namespace mode{
					/*!
					@brief ほむらの通信モード分類。*/
					enum type{
						module_null = 0x00,
						module_phone = 0x01,
						module_rf = 0x02
					};
				}
				//module selector
				struct mode_selector_interface{
					virtual bool setModuleMode(mode::type Mode_) = 0;
					virtual mode::type getModuleMode()const = 0;
				};
			}
		}
}
#
#endif
