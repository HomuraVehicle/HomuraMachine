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
#include"Com.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace message {
				typedef com::did_t did_t;
				typedef com::dsize_t dsize_t;
				typedef com::dsizeFp_v dsizeFp_v;
				typedef com::vFp_v vFp_v;
				//==============設定用=================//
				#define HMR_MESSAGE_BUFSIZE 32
				//=====================================//

				typedef bool(*bFp_s)(hmLib::cstring);				//Listen用関数
				typedef bool(*bFp_ps)(hmLib::cstring*);	//Talk用関数
				//message_t構造体
				typedef struct{
					did_t	ID;				//メッセージ識別用ID
					vFp_v	FpSetupListen;	//メッセージ受信準備用関数
					bFp_s	FpListen;		//メッセージ受信用関数 戻り値はメッセージ処理の可否
					vFp_v	FpSetupTalk;	//メッセージ送信準備用関数
					bFp_ps	FpTalk;			//メッセージ送信用関数 戻り値はメッセージの有無
				}message_t;
				//初期化処理
				void initialize(void);
				//終端化処理
				void finalize(void);
				//DatTypeを登録
				bool regist(
					did_t ID_
					,vFp_v FpSetupListen_
					,bFp_s FpListen_
					,vFp_v FpSetupTalk_
					,bFp_ps FpTalk_
				);
				//受信準備関数
				void setup_listen(void);
				//受信関数 返り値は正常にデータが処理されなかった場合にtrue
				bool listen(idata* mdata_);
				//送信準備関数
				void setup_talk(void);
				//送信関数 返り値はデータが存在しない場合にtrue
				bool talk(odata* pdata_);
				//送信可能サイズ関数の使用を宣言する
				void talkconfig_useSendable(dsizeFp_v Fp_);
				//送信完了通知関数を使用する
				void talkconfig_useInformDestruct(vFp_v Fp_);
				//EDSメモリ領域に確保した場合のアクセス用に、DSRPageFileを登録する
				void talkconfig_useDSRPageFile(int DSRPAG_);
			}
		}
	}
}

#
#endif
