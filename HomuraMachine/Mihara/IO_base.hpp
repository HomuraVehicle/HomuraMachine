#ifndef HMR_MACHINE_MIHARA_IOBASE_INC
#define HMR_MACHINE_MIHARA_IOBASE_INC 100
#
/*!
@brief cUI�𗘗p�����ł̋��ʋK��̒�`�B

cIO�́A�قނ�̒ʐM�Ǘ��p�N���X�ł��BcIO���̂́AcSystem�Ȃǂ̐���N���X���烂�[�h�ʒm���󂯂邱�ƂŎ��M�̒ʐM���[�h��ύX���܂��B
���̃t�@�C���ł́AcIO�Ƒ��̐���N���X����̒ʒm���邤���ł̃��[�h��ʒm��i������`����Ă��܂��B
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
					@brief �قނ�̒ʐM���[�h���ށB*/
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
