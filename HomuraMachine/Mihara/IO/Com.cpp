#ifndef HMR_MACHINE_MIHARA_COM_CPP_INC
#define HMR_MACHINE_MIHARA_COM_CPP_INC 300
#
#include<cstdlib>
#define HMLIB_NOLIB
#include<hmLib/cqueue.h>
#include<hmLib/cstring.h>
#include<hmLib/type.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"../Service_base.hpp"
#include"Com.hpp"
#include"ComClient.hpp"
#define HMR_COM_BufqueSize 20

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace com {
				namespace {
					//**************com変数群******************//
					//TRMNを絞めずにおわったため，待たなければいけないPACTRMNID数
					//	finSendPacでTRMNIDが処理できなかった場合に++
					//	oBuf_move_pushでTRMNIDが確認できた場合に--
					uint8			FailPac;
					//送信待ちPAC数
					//	oBuf_move_pushでFailPac用にTRMNIDが処理されなかった場合に++
					//	finSendPacでTRMNIDが処理できた場合に--
					uint8			WaitPacNum;			//受信して未送信のPacketの数
					uint8			HoldPac;				//送信街のPacketの数
					bool			RecvFailDat=0;			//受信中のDatの保存先を確保しなかった/できなかった場合に立ち、finRecvDatで落ちる
					bool			RecvMode=0;			//受信中(Strt受信からTrmn受信までの間）
					hmLib::cstring			RecvDatStr;
					did_t		RecvDatID=0;
					dsize_t		RecvDatCnt=0;			//受信中のDatのカウンタ
					vFp_v		FpInfromFinRecvPac=0;	//受信中のPacを全て受信完了時に呼び出す関数ポインタ
					bool			SendMode=0;			//送信中(Strt送信からTrmn送信までの間）
					odata			SendDat;				//送信中のDat
					dsize_t		SendDatCnt=0;			//送信中のDatのカウンタ
					vFp_v		FpInfromFinSendPac=0;	//送信中のPacを全て受信完了時に呼び出す関数ポインタ
					//**************com_iBuf関連***************//
					//受信バッファ
					hmLib::cqueue iBuf_Que;
					idata iBuf_QueBuf[HMR_COM_BufqueSize];

					io::com_client_interface* pComClient;
				}

				//受信バッファの初期化
				void iBuf_initialize(void) {
					hmLib::cqueue_placement_construct(
						&iBuf_Que
						, HMR_COM_BufqueSize
						, sizeof(idata)
						, iBuf_QueBuf
						, 0);
				}
				//受信バッファの終端化
				void iBuf_finalize(void) {
					hmLib::cqueue_iterator itr;

					//残っている全データを破棄
					for(itr=hmLib::cqueue_begin(&iBuf_Que); itr!=hmLib::cqueue_end(&iBuf_Que); itr=hmLib::cqueue_next(&iBuf_Que, itr)) {
						service::cstring_destruct_safe(&((idata*)(itr))->Str);
					}

					//iQue,oQueを破棄
					hmLib::cqueue_destruct(&iBuf_Que);
				}
				//受信可能かを確認
				bool iBuf_empty(void) { return hmLib::cqueue_empty(&iBuf_Que) != 0; }
				//受信済みデータを削除する
				void iBuf_move_pop(idata* pData_) {
					if(iBuf_empty()) {
						idata_format(pData_);
						return;
					}
					idata_move((idata*)hmLib::cqueue_getptr(&iBuf_Que), pData_);
					hmLib::cqueue_pop(&iBuf_Que);
				}
				//iBufにデータを挿入可能か確認
				bool iBuf_full(void) { return hmLib::cqueue_full(&iBuf_Que) != 0; }
				//iBufのサイズを取得する
				hmLib::cqueue_size_t iBuf_size(void) { return hmLib::cqueue_size(&iBuf_Que); }
				//iBufにデータを挿入する
				void iBuf_move_push(idata* mData_) {
					if(!idata_is_construct(mData_))return;

					if(iBuf_full()) {
						idata_destruct(mData_);
						return;
					}

					//実際上のmove
					hmLib::cqueue_push(&iBuf_Que, (void*)(mData_));
					idata_format(mData_);
				}
				//*****************oBuf関連**************************//
				//送信バッファ
				hmLib::cqueue oBuf_Que;
				odata oBuf_QueBuf[HMR_COM_BufqueSize];
				//送信バッファの初期化
				void oBuf_initialize(void) {
					hmLib::cqueue_placement_construct(
						&oBuf_Que
						, HMR_COM_BufqueSize
						, sizeof(odata)
						, oBuf_QueBuf
						, 0);
				}
				//送信バッファの終端化
				void oBuf_finalize(void) {
					hmLib::cqueue_iterator itr;

					//残っている全データを破棄
					for(itr=hmLib::cqueue_begin(&oBuf_Que); itr!=hmLib::cqueue_end(&oBuf_Que); itr=hmLib::cqueue_next(&oBuf_Que, itr)) {
						service::cstring_destruct_safe(&((odata*)(itr))->Str);
					}

					//oQueを破棄
					hmLib::cqueue_destruct(&oBuf_Que);
				}
				//送信可能かを確認
				bool oBuf_empty(void) { return hmLib::cqueue_empty(&oBuf_Que) != 0; }
				//送信済みデータを削除する
				void oBuf_move_pop(odata* pData_) {
					if(oBuf_empty()) {
						odata_format(pData_);
						return;
					}
					odata_move((odata*)hmLib::cqueue_getptr(&oBuf_Que), pData_);
					hmLib::cqueue_pop(&oBuf_Que);
				}
				//oBufのサイズを取得する
				hmLib::cqueue_size_t oBuf_size(void) { return hmLib::cqueue_size(&oBuf_Que); }
				//送信可能かの確認
				bool oBuf_full(void) { return hmLib::cqueue_full(&oBuf_Que)!=0; }
				//送信バッファの末尾がパケット終了タグ
				bool oBuf_isPacEnd(void) { return ((odata*)hmLib::cqueue_getptr(&oBuf_Que))->ID==HMR_COM_PACTRMNID; }
				//送信用関数(受信済データ用)　送信データの先頭アドレス(Ptr)、送信データ長(Size)を引数に入れれば、その送信データを送信待ちバッファに送れる。
				void oBuf_move_push(odata* mData_) {
					if(!odata_is_construct(mData_))return;

					if(FailPac) {
						if(mData_->ID==HMR_COM_PACTRMNID) {
							--FailPac;
						}
						odata_destruct(mData_);
					} else if(oBuf_full()) {
						odata_destruct(mData_);
					} else {
						if(mData_->ID==HMR_COM_PACTRMNID) {
							++HoldPac;
						}
						hmLib::cqueue_push(&oBuf_Que, (void*)mData_);
						odata_format(mData_);
					}
				}
				//****************wdt実装*********************//
				uint8 xwdt_WdtCnt=0;	//WDTのカウンター
				bool xwdt_WdtEn=0;	//WDTの有効無効フラグ
				//com監視用WatchDogTimerのリセット
				void wdt_restart() { xwdt_WdtCnt=0; }
				//com監視用WatchDogTimerを停止
				void wdt_enable() { xwdt_WdtEn=true; }
				//com監視用WatchDogTimerを停止
				void wdt_disable() { xwdt_WdtEn=false; }
				//com監視用WatchDogTimerのタスク関数
				struct wdt_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						//有効でない場合は、無視
						if(!xwdt_WdtEn)return dt;

						xwdt_WdtCnt += dt;

						if(xwdt_WdtCnt>wdt_sec)pComClient->inform_timeout();

						return dt;
					}
				}WdtTask;
				//****************VMC関連実装*********************//
				hmLib_boolian vmc_can_iniRecvPac(void) { return static_cast<hmLib_boolian>(true); }
				//PacStrt受信完了時に呼ばれる
				void vmc_iniRecvPac(void) {
					//受信モードをONにする
					RecvMode=1;

					//WDT対策
					wdt_restart();
				}
				//iniRecvDatを実行して良いかの確認に呼ばれる
				hmLib_boolian vmc_can_iniRecvDat(void) { return static_cast<hmLib_boolian>(!iBuf_full()); }
				//PacTrmn受信完了時に呼ばれる　引数はエラーの有無
				void vmc_finRecvPac(unsigned char Err) {
					idata IData;
					//	cstring Str;
					//HMR_COM_PACTRMNIDコマンドを作成し、Pacを閉じる
					//	service::cstring_construct_safe(&Str,1);
					//	cstring_putc(&Str,0,Err);
					//DEBUG	idata_format(&IData);
					//DEBUG	IData.ID=HMR_COM_PACTRMNID;
					//DEBUG	iBuf_move_push(&IData);

					idata_format(&IData);
					IData.ID=HMR_COM_PACTRMNID;
					iBuf_move_push(&IData);

					//受信モードをOFFにする
					RecvMode=0;
					//Pac受信通知関数を呼び出す
					if(FpInfromFinRecvPac)FpInfromFinRecvPac();
					//受信済み、送信待ちのデータ数を増やす
					++WaitPacNum;
				}
				//Dat受信開始時に呼ばれる 引数は受信するデータサイズ
				unsigned int DebugCnt;
				unsigned char* Debug;
				void vmc_iniRecvDat(did_t ID, dsize_t Size) {
					//IDやSizeがおかし場合は先に破棄
					if(ID==0 || 64<Size) {
						RecvFailDat=1;
						return;
					}

					RecvDatID=ID;
					RecvDatCnt=0;

					if(hmLib::cstring_is_construct(&RecvDatStr)) {
						service::cstring_destruct_safe(&RecvDatStr);
					}

					service::cstring_construct_safe(&RecvDatStr, Size);

					if(!hmLib::cstring_is_construct(&RecvDatStr) && Size>0) {
						RecvFailDat=1;
						return;
					}

					//	RecvDatStr.Begin=(unsigned char*)malloc(Size);
					//	free(RecvDatStr.Begin);
					//	RecvDatStr.Begin=0;

					//DebugCnt=(1+DebugCnt)%32+10;
					//Debug=(unsigned char*)malloc(DebugCnt);
					//free(Debug);
				}
				//Dat受信終了時に呼ばれる 引数はエラーの有無
				void vmc_finRecvDat(unsigned char Err) {
					idata Data;
					idata_format(&Data);

					if(RecvFailDat) {
						RecvFailDat=0;
						return;
					}

					if(Err) {
						if(hmLib::cstring_is_construct(&RecvDatStr))service::cstring_destruct_safe(&RecvDatStr);
						return;
					}

					//エラーが無ければバッファにデータ追加、あれば破棄
					idata_construct(&Data, RecvDatID, &RecvDatStr);
					iBuf_move_push(&Data);

				}
				//recvを実行してよいかの確認に呼ばれる
				hmLib_boolian vmc_can_recv(void) { return static_cast<hmLib_boolian>(true); }
				//Datの中身受信時に呼ばれる　引数が受信したデータ
				void vmc_recv(unsigned char c) {
					if(RecvFailDat==0)hmLib::cstring_putc(&RecvDatStr, RecvDatCnt++, c);
					//DEBUG	if(RecvFailDat==0)cstring_putc(&RecvDatStr,RecvDatCnt++,c);
				}
				hmLib_boolian vmc_can_iniSendPac(void) {
					return static_cast<hmLib_boolian>(pComClient->is_fullduplex() || (WaitPacNum>0));
				}
				//RacStrt送信完了時に呼ばれる
				void vmc_iniSendPac(void) {
					//送信モードをONにする
					SendMode=1;

					if(WaitPacNum)--WaitPacNum;

					//WDT対策
					//	wdt_restart();
				}
				//PacTrmn送信完了時に呼ばれる　引数はエラーの有無
				void vmc_finSendPac(unsigned char Err) {
					odata Data;

					odata_format(&Data);
					while(1) {
						//HMR_COM_PACTRMNIDコマンドが出てこなかった場合、HMR_COM_PACTRMNIDコマンドが一つ出てくるまでは無効のPacであることを通知
						if(oBuf_empty()) {
							++FailPac;
							break;
						}

						//先頭からmove&pop
						oBuf_move_pop(&Data);
						//HMR_COM_PACTRMNIDコマンド==Pacの終端の合図が出てきた場合
						if(Data.ID==HMR_COM_PACTRMNID) {
							//HMR_COM_PACTRMNIDコマンドを処理し、Pacを閉じる
							odata_destruct(&Data);
							--HoldPac;
							break;
						} else {
							odata_destruct(&Data);
						}
					}
					//送信モードをOFFにする
					SendMode=0;
					//Pac受信通知関数を呼び出す
					if(FpInfromFinSendPac)FpInfromFinSendPac();
				}
				//データの有無の確認が可能かどうか
				hmLib_boolian vmc_can_existSendDat(void) { return static_cast<hmLib_boolian>(!oBuf_empty()); }
				//送信が必要なデータの有無の確認
				hmLib_boolian vmc_existSendDat(void) {
					//バッファ末尾データがPacketTrmnの場合には存在しないのでfalse
					if(oBuf_isPacEnd())return static_cast<hmLib_boolian>(false);
					//上記以外の場合はデータが存在するのでtrue
					return static_cast<hmLib_boolian>(true);
				}
				//iniSendDatを実行して良いかの確認に呼ばれる
				hmLib_boolian vmc_can_iniSendDat(void) {
					return static_cast<hmLib_boolian>(!oBuf_isPacEnd());
				}
				//Dat送信確定時に呼ばれる　サイズを戻す
				void vmc_iniSendDat(did_t* pID, dsize_t* pSize) {
					if(odata_is_construct(&SendDat)) {
						odata_destruct(&SendDat);
					}
					//送信データをパケットに移す
					oBuf_move_pop(&SendDat);

					if(!odata_is_construct(&SendDat)) {
						*pID=0;
						*pSize=0;
						return;
					}

					//カウンタクリア
					SendDatCnt=0;
					//引数に情報を戻してやる
					*pID=SendDat.ID;
					*pSize=hmLib::cstring_size(&(SendDat.Str));
				}
				//Dat送信終了時に呼ばれる　引数はエラーの有無
				void vmc_finSendDat(unsigned char Err) {
					//Dat送信終了を通知する
					if(odata_is_construct(&SendDat)) {
						odata_destruct(&SendDat);
					}
				}
				//sendを実行してよいかの確認に呼ばれる
				hmLib_boolian vmc_can_send(void) {
					//FpSendableが存在していないか、存在するが送信可能Size以内である場合、常に真
					return static_cast<hmLib_boolian>((SendDat.FpSendable==0)
						|| (SendDatCnt<SendDat.FpSendable()));
				}
				//Datの中身送信時に呼ばれる
				unsigned char vmc_send(void) {
					//	int tmpPAG;
					//	char c;
					//	if(SendDat.DSRPAG==0){
					//データ送信
					return hmLib::cstring_getc(&(SendDat.Str), SendDatCnt++);
					/*	}else{
							tmpPAG=_DSRPAG;
							_DSRPAG=__builtin_edspage(SendDat.DSRPAG);
							//データ送信
							c=cstring_getc(&(SendDat.Str),SendDatCnt++);
							_DSRPAG=tmpPAG;
							return c;
							}
							*/
				}

				//**************com関数群*************//
				//初期化関数
				void initialize(service_interface& Service, io::com_client_interface& ComClient_) {
					iBuf_initialize();
					oBuf_initialize();

					FailPac=0;
					HoldPac=0;
					WaitPacNum=0;

					RecvMode=0;
					hmLib::cstring_format(&RecvDatStr);
					RecvDatID=0;
					RecvDatCnt=0;

					SendMode=0;
					odata_format(&SendDat);
					SendDatCnt=0;

					Service.task().quick_start(WdtTask, 1, 0);

					pComClient = &ComClient_;

					wdt_restart();
					wdt_disable();
				}
				//終端化関数
				void finalize() {
					wdt_disable();

					service::cstring_destruct_safe(&(RecvDatStr));
					service::cstring_destruct_safe(&(SendDat.Str));

					iBuf_finalize();
					oBuf_finalize();
				}
				//受信モードか確認
				bool isRecvMode(void) { return RecvMode; }
				//送信モードか確認
				bool isSendMode(void) { return SendMode; }
				//VMC1を作成する
				VMC1* createVMC1() {
					return vmc1_create(
						vmc_can_iniRecvPac,
						vmc_iniRecvPac,
						vmc_finRecvPac,
						vmc_can_iniRecvDat,
						vmc_iniRecvDat,
						vmc_finRecvDat,
						vmc_can_recv,
						vmc_recv,
						vmc_can_iniSendPac,
						vmc_iniSendPac,
						vmc_finSendPac,
						vmc_can_existSendDat,
						vmc_existSendDat,
						vmc_can_iniSendDat,
						vmc_iniSendDat,
						vmc_finSendDat,
						vmc_can_send,
						vmc_send
						);
				}
				//VMC1を破棄する
				void releaseVMC1(VMC1* pVMC1) {
					vmc1_release(pVMC1);
				}
				//****************IO関数群**************************//
				//受信可能であるか確認
				bool in_empty(void) { return iBuf_empty(); }
				//受信バッファがいっぱいであるかの確認
				bool in_full(void) { return iBuf_full(); }
				//受信バッファの先頭データ取り出し
				void in_move_pop(idata* pData_) { iBuf_move_pop(pData_); }
				//送信バッファがいっぱいであるかの確認
				bool out_full(void) { return oBuf_full(); }
				//送信バッファが空かどうかの確認
				bool out_empty(void) { return oBuf_empty(); }
				//送信バッファにデータ追加
				void out_move_push(odata* mData_) { oBuf_move_push(mData_); }
				//****************** for Debug ***************************//
				uint8 getFailPacNum(void) { return FailPac; }
				uint8 getHoldPacNum(void) { return HoldPac; }
				bool isWaitSendPacket(void) { return HoldPac!=0; }
			}
		}
	}
}

#
#endif
