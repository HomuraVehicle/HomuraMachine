#ifndef HMR_MACHINE_MIHARA_MESSAGE_CPP_INC
#define HMR_MACHINE_MIHARA_MESSAGE_CPP_INC 300
#
#define HMLIB_NOLIB
#include<cstdlib>
#include<hmLib/cqueue.h>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"Message.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace message {
				namespace {
					//message_t�̃o�b�t�@
					message_t Buf[HMR_MESSAGE_BUFSIZE];
					//�o�^�����Ă���message_t��
					unsigned char BufSize=0;
					//���M�֐��̌��݂̔ԍ�
					unsigned char BufTalkPos=0;
					//FpSendable�֐��o�^�p�ꎞ�ϐ�
					dsizeFp_v talkconfig_FpSendable=0;
					//FpInformDestruct�֐��o�^�p�ꎞ�ϐ�
					vFp_v talkconfig_FpInformDestruct=0;
					//DSRPageFile�o�^�p�ꎞ�ϐ�
					int talkconfig_DSRPAG=0;
				}
				//���M�ݒ��p
				void talkconfig_clear(void){
					//talkconfig�֐��Q�̎g�p�ɔ�����
					talkconfig_FpSendable=0;
					talkconfig_FpInformDestruct=0;
					talkconfig_DSRPAG=0;
				}
				//���M�\�T�C�Y�֐��̎g�p���錾����
				void talkconfig_useSendable(dsizeFp_v Fp_){
					talkconfig_FpSendable=Fp_;
				}
				//���M�����ʒm�֐����g�p����
				void talkconfig_useInformDestruct(vFp_v Fp_){
					talkconfig_FpInformDestruct=Fp_;
				}
				//EDS�������̈��Ɋm�ۂ����ꍇ�̃A�N�Z�X�p�ɁADSRPageFile���o�^����
				void talkconfig_useDSRPageFile(int DSRPAG_){
					talkconfig_DSRPAG=DSRPAG_;
				}
				//����������
				void initialize(void){
					BufSize=0;
				}
				//�I�[������
				void finalize(void){}
				//DatType���o�^
				bool regist(
					did_t ID_
					,vFp_v FpSetupListen_
					,bFp_s FpListen_
					,vFp_v FpSetupTalk_
					,bFp_ps FpTalk_){
					if(BufSize==HMR_MESSAGE_BUFSIZE)return 1;
					if(ID_==0)return 1;

					Buf[BufSize].ID=ID_;
					Buf[BufSize].FpSetupListen=FpSetupListen_;
					Buf[BufSize].FpListen=FpListen_;
					Buf[BufSize].FpSetupTalk=FpSetupTalk_;
					Buf[BufSize].FpTalk=FpTalk_;

					++BufSize;
					return 0;
				}
				//���M�����֐�
				void setup_listen(void){
					unsigned char i;
					for(i=0;i<BufSize;++i){
						if(Buf[i].FpSetupListen)Buf[i].FpSetupListen();
					}
				}
				//���M�֐�
				bool listen(idata* mData_){
					unsigned char i;
				//	cstring str;
	
					for(i=0;i<HMR_MESSAGE_BUFSIZE;++i){
						if(Buf[i].ID==mData_->ID)break;
					}
	
					if(i>=HMR_MESSAGE_BUFSIZE){
						idata_destruct(mData_);
						return true;
					}

					if(Buf[i].FpListen)Buf[i].FpListen(mData_->Str);
					idata_destruct(mData_);
	
					return false;
				}
				//���M�����֐�
				void setup_talk(void){
					unsigned char i;

					for(i=0;i<BufSize;++i){
						if(Buf[i].FpSetupTalk)Buf[i].FpSetupTalk();
					}
					BufTalkPos=0;
				}
				//���M�֐� �Ԃ��l��
				bool talk(odata* pData_){
					hmLib::cstring Str;
					talkconfig_clear();

					//format�������Ă���
					hmLib::cstring_format(&Str);
						while(Buf[BufTalkPos].FpTalk==0 || Buf[BufTalkPos].FpTalk(&Str)==true){
						//�������m�ۂ����Ă������̂��߂ɁA����
						if(hmLib::cstring_is_construct(&Str))service::cstring_destruct_safe(&Str);
						//���łɍŌ��̃o�b�t�@�܂ŏ����ς݂Ȃ��I��
						if(++BufTalkPos>=BufSize)return true;
					}

					odata_construct(pData_,Buf[BufTalkPos].ID,&Str
						,talkconfig_DSRPAG
						,talkconfig_FpSendable
						,talkconfig_FpInformDestruct
					);

					return false;
				}
			}
		}
	}
}

#
#endif
