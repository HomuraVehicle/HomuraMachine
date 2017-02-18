#ifndef HMR_MACHINE_MIHARA_COM_INC
#define HMR_MACHINE_MIHARA_COM_INC 300
#
/*
=== hmr::machine::Com ===
v3_00/141111 hmIto
	cpp化
===hmrCom===
送受信バッファについて取り仕切る
	@コマンドがパケットの区切り
		パケットを受信し終わると、自動的に受信バッファに@コマンドデータがpushされる
		送信時にパケット区切りには@コマンドデータを送信バッファにpushしておくとパケットが閉じられる

v2_05/131102 hmIto
	全二重/半二重に応じて、常にデータを送る/受信済みの数だけデータを送る、を変更
v2_04/131026 hmIto
	com用WatchDogTimer機能を提供
		ini_recvPac/sendPacでrestartされる。
		wdt_sec以上の時間送受信がなく、かつrestartも呼ばれないと、killする。
		機能を有効にするには、wdt_interrupt_functionをtaskに登録する必要がある。
v2_03/130928 hmIto
	iBuf/oBuf_move_pop/pushにおいて、empty/fullであってもつかえてしまう問題を修正
	odata::informDestruct機能の処理をodata::destructに委譲
v2_02/130803 hmIto
	inform_destruct機能を利用している際に，まれにinformされない問題を修正
		エラー発生時にfinSendPacが呼ばれるが，その際にdestructはされるがinformは実行されていなかった．
		同様の問題が，FailPacが存在する条件下におけるmove_pushにおいても存在していた．
v2_00/121201 hmIto
	odata/iQue_dataがcstringを使うよう変更
	odata型を変更
		NonDeleteFlagを廃止
			cstringのdynamic_destructとstatic_destructが統合されたため
		FpSendableを追加
			送信可能文字数を取得する関数
		FpInformDestructを追加
			cstringのdestruct時に元のmoduleに対し通知する関数
			特にstaticバッファの再利用の可否の確認に利用
v1_09/121128 hmIto
	hmCLibとhmLibの統合に伴い、hmLib_v3_03に対応するよう変更	
v1_08/121101 hmIto
	HMR_COM_PACTRMNIDを@コマンドの代わりに使用
	odataからFnSendableを廃止
	odataにNonDeleteFlagを追加
v1_07/121028 hmIto
	bool out_empty(void)追加
v1_06/121021 hmIto
	送受信バッファ操作用関数名を変更(Inをinに、Outをoutに）
	0byteコマンドを送信時に異常終了扱いされる致命的バグを修正
		IDがsizeに含まれていた名残で、Size=0をエラーデータとしていた問題
	hmrVMC1_v2_00に対応
		vmc1_create,vmc1_initialize関数の引数追加に対応
		合わせて、createVMC1にも自身のVMCID指定用引数増加
v1_05/121020 iwahori
	cファイルのIO関数群の名称のhファイルとの相違を変更．(inをInに，outをOutに)
v1_04/121012 hmIto
	iQue/oQueをhmrCom.c内で隠蔽(__を付加)
	インターフェイスをBegin/EndからPtr/Sizeに戻す
	名前空間をComからcomに変更
v1_03/121008 hmIto
	関数形を抜本的に改定
		VMC1関数群をラッピングしていたRF関数群は、廃止
		VMC1との通信に特化するVMC関数群は、ヘッダファイルでは隠ぺい
		利用者はvmc1_create/release関数を使って、VMCを利用
		Bufの送受信制御は、In/Out関数群に集約
			In/Outでは、インターフェイスをID/Begin/Endに統一
			受信中のデータの送信可能サイズを返すsendProgressを廃止
			送信段階で送信データを訪ねるpush_later関数を追加
	lock/unlock関数廃止
		Mutexが使えないため、バッファのロックは各スレッドの優先順位で制御。
		そのため、ロックするスレッドによって処理が異なる。
		以後は、各スレッドが独自に自身の優先度を上げるようなlock/unlock処理を行う。
	121009Test_hmrhmrVMC1.cにて、動作確認済み
v1_02/121005 hmIto
	VMC1の関数群をラッピングするのではなく、VMC1を生成する関数を提供する形に変更
		通信を実際に行う関数は、VMC1を利用する
v1_01/120922 hmIto
	ひとまず動作が安定していることを確認（120922Test_hmrhmrVMC1.cにて動作確認済み）
v1_00/120921 hmIto
	バージョン管理開始

*/
#include<hmLib/cqueue.h>
#include<homuraLib_v2/old/hmrVMC1.h>
#include"Data.hpp"

#define HMR_COM_PACTRMNID 0xF0
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace com{
				//データ型の定義
				typedef vmc1_dsize_t dsize_t;
				typedef vmc1_did_t did_t;
				typedef dsize_t(*dsizeFp_v)(void);
				typedef void(*vFp_v)(void);

				//**************Com関数群*************//
				//初期化関数
				void initialize(service_interface& Service);
				//終端化関数
				void finalize(void);
				//受信モードか確認
				bool isRecvMode(void);
				//送信モードか確認
				bool isSendMode(void);
				//VMC1を作成する
				VMC1* createVMC1(void);
				//VMC1を破棄する
				void releaseVMC1(VMC1* pVMC1);

				//****************IO関数群******************//
				//受信可能であるか確認
				bool in_empty(void);
				//受信バッファに空きがあるか確認
				bool in_full(void);
				//データを受信
				idata in_front(void);
				//データを受信を終了
				void in_move_pop(idata* pData_);
				//送信可能であるかを確認
				bool out_full(void);
				//送信バッファが空かどうかの確認
				bool out_empty(void);
				//データを送信(受信済データ用)
				void out_move_push(odata* mData_);

				//**************ComWDT関連************//
				#define wdt_sec 180
				//com監視用WatchDogTimerのリセット
				void wdt_restart();
				//com監視用WatchDogTimerを停止
				void wdt_enable();
				//com監視用WatchDogTimerを停止
				void wdt_disable();
				//******************ioBuf内部関数群*****************//
				bool isWaitSendPacket(void);

				//*****************debug****************//
				hmLib_boolian vmc_can_send(void);
			}
		}
	}
}

#
#endif
