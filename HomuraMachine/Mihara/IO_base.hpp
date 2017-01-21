#ifndef HMR_MACHINE_MIHARA_IOBASE_INC
#define HMR_MACHINE_MIHARA_IOBASE_INC 100
#
/*!
@brief cUI�𗘗p�����ł̋��ʋK��̒�`�B

cIO�́A�قނ�̒ʐM�Ǘ��p�N���X�ł��BcIO���̂́AcSystem�Ȃǂ̐���N���X���烂�[�h�ʒm���󂯂邱�ƂŎ��M�̒ʐM���[�h��ύX���܂��B
���̃t�@�C���ł́AcIO�Ƒ��̐���N���X����̒ʒm���邤���ł̃��[�h��ʒm��i������`����Ă��܂��B
*/
namespace hmr{
	namespace machine{
		namespace mihara{
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
