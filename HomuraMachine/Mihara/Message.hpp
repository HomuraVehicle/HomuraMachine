#ifndef HMR_MACHINE_MIHARA_MESSAGE_INC
#define HMR_MACHINE_MIHARA_MESSAGE_INC 300
#
/*
IDを読み出し、IDを除いた部分をlisten関数に渡す
talk関数で送信すべきデータを受け取り，IDを付加して返す

=== hmr::machine::mihara::Message ===
v3_00/141111 hmIto
	c++化
===hmrMessage===
v2_04/130212 hmIto
	msg_listen()
		idataのdestructをし忘れていた問題を修正。
		idataの引数を実態コピーからmoveポインタに変更。
	msg_talk()
		trueがFpTalkから戻ってきた際に、万が一Strが確保されている場合destructするよう修正
v2_03/130202 iwahori
	msg_setup_talk()：forループの上限値をHMR_MESSAGE_BUFSIZEから_msg_BufSizeに修正
	msg_setup_listen()：forループの上限値をHMR_MESSAGE_BUFSIZEから_msg_BufSizeに修正
v2_02/130127 iwahori
	msg_setup_talk()内で_msg_Buf[i].FpSetupListen()となっていた部分を
	_msg_Buf[i].FpSetupTalk()に修正
v2_01/130105 hmIto
	hmLib_v3_03に対応
	SetupListen/SetupTalk関数を追加
		ListenとTalk実行前に一度だけ呼ばれる
		送受信フラグの設定用関数
v2_00/121201 hmIto
	hmrCom_v2_00に対応
	メッセージ処理関数を、Talk関数とListen関数に分割
v1_05/121128 hmIto
	hmCLibとhmLibの統合に伴い、hmLib_v3_03に対応するよう変更		
v1_04/121103 hmIto
	NonDeleteFlag機能に対応
		msg_set_nondelete_mode();
v1_03/121013 hmIto
	名前空間Messageをmsgに変更
v1_02/121012 hmIto
	MessageType構造体がSendable関数も管理する形に変更
v1_01/121009 hmIto
	インターフェイスを大きく変更
		msg_rangeは廃止
		work関数の引数をポインタに変更
v1_00/121005 hmIto
	ひとまず、完成
*/
#include<homuraLib_v2/type.hpp>
#include<hmLib/cstring.h>
#include"IO_base.hpp"
#include"Com.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace message {	
				typedef hmr::machine::mihara::com::did_t did_t;
				typedef hmr::machine::mihara::com::dsize_t dsize_t;
				typedef hmr::machine::mihara::com::dsizeFp_v dsizeFp_v;
				typedef hmr::machine::mihara::com::vFp_v vFp_v;
			}
			struct cMessage{
				typedef ::xc::chain<message_client_interface> chain;
				typedef typename chain::iterator iterator;
			private:
				//client chain
				chain Chain;
				//Current Iterator
				iterator TalkItr;
				//FpSendable関数登録用一時変数
				message::dsizeFp_v talkconfig_FpSendable = 0;
				//FpInformDestruct関数登録用一時変数
				vFp_v talkconfig_FpInformDestruct = 0;
				//DSRPageFile登録用一時変数
				int talkconfig_DSRPAG = 0;
			public:
				//メッセージ登録関数
				virtual void regist(message_client_interface& Client_){
					Chain.push_back(Client_);
				}
			public:
				//受信準備関数
				void setup_listen(void){
					for(iterator Itr = Chain.begin(); Itr != Chain.end(); ++Itr){
						Itr->setup_listen();
					}
				}
				//受信関数
				bool listen(idata* mData_){
					for(iterator Itr = Chain.begin(); Itr != Chain.end(); ++Itr){
						if(Itr->id() == mData_->ID){
							Itr->listen(mData_->Str);
							idata_destruct(mData_);
							return false;
						}
						Itr->setup_talk();
					}

					idata_destruct(mData_);
					return true;
				}
				//送信準備関数
				void setup_talk(void){
					for(iterator Itr = Chain.begin(); Itr != Chain.end(); ++Itr){
						Itr->setup_talk();
					}

					TalkItr = Chain.begin();
				}
				//送信関数 返り値はデータが存在しない場合にtrue
				bool talk(odata* pData_){
					hmLib::cstring Str;
					talkconfig_clear();

					//formatをかけておく
					hmLib::cstring_format(&Str);

					//Talkを走査
					while(TalkItr != Chain.end()){
						//talkがこれ以上必要ない時
						if(TalkItr->talk(&Str)){
							//万が一確保されていた時のために、解放
							if(hmLib::cstring_is_construct(&Str))service::cstring_destruct_safe(&Str);
							//次のtalkへ
							++TalkItr;
							continue;
						}

						//odataを構築して終了
						odata_construct(pData_, TalkItr->id(), &Str
							, talkconfig_DSRPAG
							, talkconfig_FpSendable
							, talkconfig_FpInformDestruct
						);

						return false;
					}

					//すでに最後のバッファまで処理済みなら終了
					return true;
				}
			public:
				//送信設定用
				void talkconfig_clear(void){
					//talkconfig関数群の使用に備える
					talkconfig_FpSendable = 0;
					talkconfig_FpInformDestruct = 0;
					talkconfig_DSRPAG = 0;
				}
				//送信可能サイズ関数の使用を宣言する
				void talkconfig_useSendable(message::dsizeFp_v Fp_){
					talkconfig_FpSendable = Fp_;
				}
				//送信完了通知関数を使用する
				void talkconfig_useInformDestruct(vFp_v Fp_){
					talkconfig_FpInformDestruct = Fp_;
				}
				//EDSメモリ領域に確保した場合のアクセス用に、DSRPageFileを登録する
				void talkconfig_useDSRPageFile(int DSRPAG_){
					talkconfig_DSRPAG = DSRPAG_;
				}
			};
		}
	}
}

#
#endif
