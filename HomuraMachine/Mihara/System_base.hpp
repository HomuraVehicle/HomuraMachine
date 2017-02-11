#ifndef HMR_MACHINE_MIHARA_SYSTEMBASE_INC
#define HMR_MACHINE_MIHARA_SYSTEMBASE_INC 100
#
/*!
@brief cSystem�𗘗p�����ł̋��ʋK��̒�`�B

cSystem�́A���W���[���̓d���⃂�[�h�Ǘ��p�N���X�ł��B�X���[�v���[�h�����ɓd���̐ߖ񂪕K�v�ȃ��W���[���́A�K��cSystem�ɑ΂��ăN���C�A���g�𑗂肱�݁A���[�h�ɂ��Ă̒ʒm���󂯂���悤�ɂ���K�v������܂��B
���̃t�@�C���ł́AcSystem�𗘗p���邤���Ŋe���W���[���ɋ��ʂ̃��[�h��C���^�[�t�F�[�X������`����Ă��܂��B
*/
#include<XCBase/chain.hpp>
namespace hmr {
	namespace machine {
		namespace mihara{
			namespace systems{
				namespace mode{
					/*!
					@brief �قނ�̃��W���[���ނ̃��[�h���ނł��B
					@detail �N���C�A���g�́A�z�X�g����ȉ��̃��[�h��ʒm�����\��������܂��B*/
					enum type{
						/*! �Z���T�[��ON�ŁA�����I�Ɋϑ��������s���B*/
						observe, 
						/*! �Z���T�[��ON�����A�ϑ������͒ʐM�Ŏ󂯎�閽�߂�҂B*/
						passive, 
						/*! �Z���T�[��OFF�B*/
						sleep 
					};
				}
			}
			/*!
			@brief �V�X�e���z�X�g���烂�[�h���󂯎�邽�߂Ɋe���W���[�������荞�ށA�N���C�A���g�̃C���^�[�t�F�[�X�ł��B*/
			struct system_client_interface: public ::xc::chain_element{
				virtual void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_) = 0;
			};

			namespace systems{
				typedef ::xc::chain<system_client_interface> chain;
			}

			/*!
			@brief �N���C�A���g�Ƀ��[�h��ʒm����A�V�X�e���z�X�g����̃C���^�[�t�F�[�X�ł��B*/
			struct system_interface{
				virtual void regist(system_client_interface& rElement_) = 0;
			};
		}
	}
}
#
#endif
