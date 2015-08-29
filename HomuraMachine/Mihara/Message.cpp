#ifndef HMR_MACHINE_MIHARA_MESSAGE_CPP_INC
#define HMR_MACHINE_MIHARA_MESSAGE_CPP_INC 300
#
#define HMLIB_NOLIB
#include<cstdlib>
#include<hmLib_v3_06/cqueue.h>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"Message.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace message {
				namespace {
					//message_tのバッファ
					message_t Buf[HMR_MESSAGE_BUFSIZE];
					//登録されているmessage_t数
					unsigned char BufSize=0;
					//送信関数の現在の番号
					unsigned char BufTalkPos=0;
					//FpSendable関数登録用一時変数
					dsizeFp_v talkconfig_FpSendable=0;
					//FpInformDestruct関数登録用一時変数
					vFp_v talkconfig_FpInformDestruct=0;
					//DSRPageFile登録用一時変数
					int talkconfig_DSRPAG=0;
				}
				//送信設定用
				void talkconfig_clear(void){
					//talkconfig関数群の使用に備える
					talkconfig_FpSendable=0;
					talkconfig_FpInformDestruct=0;
					talkconfig_DSRPAG=0;
				}
				//送信可能サイズ関数の使用を宣言する
				void talkconfig_useSendable(dsizeFp_v Fp_){
					talkconfig_FpSendable=Fp_;
				}
				//送信完了通知関数を使用する
				void talkconfig_useInformDestruct(vFp_v Fp_){
					talkconfig_FpInformDestruct=Fp_;
				}
				//EDSメモリ領域に確保した場合のアクセス用に、DSRPageFileを登録する
				void talkconfig_useDSRPageFile(int DSRPAG_){
					talkconfig_DSRPAG=DSRPAG_;
				}
				//初期化処理
				void initialize(void){
					BufSize=0;
				}
				//終端化処理
				void finalize(void){}
				//DatTypeを登録
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
				//受信準備関数
				void setup_listen(void){
					unsigned char i;
					for(i=0;i<BufSize;++i){
						if(Buf[i].FpSetupListen)Buf[i].FpSetupListen();
					}
				}
				//受信関数
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
				//送信準備関数
				void setup_talk(void){
					unsigned char i;

					for(i=0;i<BufSize;++i){
						if(Buf[i].FpSetupTalk)Buf[i].FpSetupTalk();
					}
					BufTalkPos=0;
				}
				//送信関数 返り値は
				bool talk(odata* pData_){
					hmLib::cstring Str;
					talkconfig_clear();

					//formatをかけておく
					hmLib::cstring_format(&Str);
						while(Buf[BufTalkPos].FpTalk==0 || Buf[BufTalkPos].FpTalk(&Str)==true){
						//万が一確保されていた時のために、解放
						if(hmLib::cstring_is_construct(&Str))service::cstring_destruct_safe(&Str);
						//すでに最後のバッファまで処理済みなら終了
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
