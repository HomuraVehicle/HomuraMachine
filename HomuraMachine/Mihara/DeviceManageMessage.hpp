#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGEMESSAGE_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGEMESSAGE_INC 102

/*
===hmrDeviceManage===

v1_02/131019 amby
	���b�Z�[�W�֐��̒ǉ� ���̑��̋@�\�̓C���^�[�t�F�[�X�ցc

v1_01/131013 hmIto
	�C���^�[�t�F�[�X���啝�ɕύX������
		���[�h�����N���b�N���x�w�������O
*/

#include<homuraLib_v2/type.hpp>
#include<hmLib/cstring.h>
#include"Device.hpp"

namespace hmr{
	namespace machine{
		namespace mihara{
			namespace devmngmsg{
				void initialize(void);
				void finalize(void);

				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void setup_listen(void);
				void setup_talk(void);
			}
		}
	}
}

#
#endif
