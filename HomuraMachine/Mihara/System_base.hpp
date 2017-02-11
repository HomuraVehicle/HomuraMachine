#ifndef HMR_MACHINE_MIHARA_SYSTEMBASE_INC
#define HMR_MACHINE_MIHARA_SYSTEMBASE_INC 100
#
/*!
@brief cSystemを利用する上での共通規約の定義。

cSystemは、モジュールの電源やモード管理用クラスです。スリープモード時等に電源の節約が必要なモジュールは、必ずcSystemに対してクライアントを送りこみ、モードについての通知を受けられるようにする必要があります。
このファイルでは、cSystemを利用するうえで各モジュールに共通のモードやインターフェース等が定義されています。
*/
#include<XCBase/chain.hpp>
namespace hmr {
	namespace machine {
		namespace mihara{
			namespace systems{
				namespace mode{
					/*!
					@brief ほむらのモジュール類のモード分類です。
					@detail クライアントは、ホストから以下のモードを通知される可能性があります。*/
					enum type{
						/*! センサーはONで、自動的に観測処理を行う。*/
						observe, 
						/*! センサーはONだが、観測処理は通信で受け取る命令を待つ。*/
						passive, 
						/*! センサーはOFF。*/
						sleep 
					};
				}
			}
			/*!
			@brief システムホストからモードを受け取るために各モジュールが送り込む、クライアントのインターフェースです。*/
			struct system_client_interface: public ::xc::chain_element{
				virtual void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_) = 0;
			};

			namespace systems{
				typedef ::xc::chain<system_client_interface> chain;
			}

			/*!
			@brief クライアントにモードを通知する、システムホストからのインターフェースです。*/
			struct system_interface{
				virtual void regist(system_client_interface& rElement_) = 0;
			};
		}
	}
}
#
#endif
