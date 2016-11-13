#ifndef HMR_MACHINE_MIHARA_DRIVE_INC
#define HMR_MACHINE_MIHARA_DRIVE_INC 100
#
/*
===hmrDrive===
���[�^�[�A�}�j���s�����[�^�n���R���g���[��

=== hmr::machine::mihara::drive ===
v1_00/141121 hmIto
	cpp��
=== hmrDrive ===
v0_00/130112 iwahori
	�t�@�C�����쐬�E���[�^�[�̃��b�Z�[�W�֐���talk��listen�ɕ���
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace motor {
				//*********************** �쓮
				void initialize();
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

