#ifndef HMR_MACHINE_MIHARA_MESSAGE_INC
#define HMR_MACHINE_MIHARA_MESSAGE_INC 300
#
/*
ID���ǂݏo���AID��������������listen�֐��ɓn��
talk�֐��ő��M���ׂ��f�[�^���󂯎����CID���t�����ĕԂ�

=== hmr::machine::mihara::Message ===
v3_00/141111 hmIto
	c++��
===hmrMessage===
v2_04/130212 hmIto
	msg_listen()
		idata��destruct�����Y���Ă����������C���B
		idata�̈��������ԃR�s�[����move�|�C���^�ɕύX�B
	msg_talk()
		true��FpTalk�����߂��Ă����ۂɁA������Str���m�ۂ����Ă����ꍇdestruct�����悤�C��
v2_03/130202 iwahori
	msg_setup_talk()�Ffor���[�v�̏����l��HMR_MESSAGE_BUFSIZE����_msg_BufSize�ɏC��
	msg_setup_listen()�Ffor���[�v�̏����l��HMR_MESSAGE_BUFSIZE����_msg_BufSize�ɏC��
v2_02/130127 iwahori
	msg_setup_talk()����_msg_Buf[i].FpSetupListen()�ƂȂ��Ă���������
	_msg_Buf[i].FpSetupTalk()�ɏC��
v2_01/130105 hmIto
	hmLib_v3_03�ɑΉ�
	SetupListen/SetupTalk�֐����ǉ�
		Listen��Talk���s�O�Ɉ��x�����Ă΂���
		�����M�t���O�̐ݒ��p�֐�
v2_00/121201 hmIto
	hmrCom_v2_00�ɑΉ�
	���b�Z�[�W�����֐����ATalk�֐���Listen�֐��ɕ���
v1_05/121128 hmIto
	hmCLib��hmLib�̓����ɔ����AhmLib_v3_03�ɑΉ������悤�ύX		
v1_04/121103 hmIto
	NonDeleteFlag�@�\�ɑΉ�
		msg_set_nondelete_mode();
v1_03/121013 hmIto
	���O����Message��msg�ɕύX
v1_02/121012 hmIto
	MessageType�\���̂�Sendable�֐����Ǘ������`�ɕύX
v1_01/121009 hmIto
	�C���^�[�t�F�C�X���傫���ύX
		msg_range�͔p�~
		work�֐��̈������|�C���^�ɕύX
v1_00/121005 hmIto
	�ЂƂ܂��A����
*/
#include<homuraLib_v2/type.hpp>
#include<hmLib/cstring.h>
#include"Com.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace message {
				typedef com::did_t did_t;
				typedef com::dsize_t dsize_t;
				typedef com::dsizeFp_v dsizeFp_v;
				typedef com::vFp_v vFp_v;
				//==============�ݒ��p=================//
				#define HMR_MESSAGE_BUFSIZE 32
				//=====================================//

				typedef bool(*bFp_s)(hmLib::cstring);				//Listen�p�֐�
				typedef bool(*bFp_ps)(hmLib::cstring*);	//Talk�p�֐�
				//message_t�\����
				typedef struct{
					did_t	ID;				//���b�Z�[�W���ʗpID
					vFp_v	FpSetupListen;	//���b�Z�[�W���M�����p�֐�
					bFp_s	FpListen;		//���b�Z�[�W���M�p�֐� �߂��l�̓��b�Z�[�W�����̉�
					vFp_v	FpSetupTalk;	//���b�Z�[�W���M�����p�֐�
					bFp_ps	FpTalk;			//���b�Z�[�W���M�p�֐� �߂��l�̓��b�Z�[�W�̗L��
				}message_t;
				//����������
				void initialize(void);
				//�I�[������
				void finalize(void);
				//DatType���o�^
				bool regist(
					did_t ID_
					,vFp_v FpSetupListen_
					,bFp_s FpListen_
					,vFp_v FpSetupTalk_
					,bFp_ps FpTalk_
				);
				//���M�����֐�
				void setup_listen(void);
				//���M�֐� �Ԃ��l�͐����Ƀf�[�^�����������Ȃ������ꍇ��true
				bool listen(idata* mdata_);
				//���M�����֐�
				void setup_talk(void);
				//���M�֐� �Ԃ��l�̓f�[�^�����݂��Ȃ��ꍇ��true
				bool talk(odata* pdata_);
				//���M�\�T�C�Y�֐��̎g�p���錾����
				void talkconfig_useSendable(dsizeFp_v Fp_);
				//���M�����ʒm�֐����g�p����
				void talkconfig_useInformDestruct(vFp_v Fp_);
				//EDS�������̈��Ɋm�ۂ����ꍇ�̃A�N�Z�X�p�ɁADSRPageFile���o�^����
				void talkconfig_useDSRPageFile(int DSRPAG_);
			}
		}
	}
}

#
#endif
