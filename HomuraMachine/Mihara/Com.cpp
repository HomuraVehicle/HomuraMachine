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
#include"Service_base.hpp"
#include"Com.hpp"
#include"DeviceManage.hpp"
#define HMR_COM_BufqueSize 20

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace com {
				namespace {
					//**************com�ϐ��Q******************//
					//TRMN���i�߂��ɂ���������߁C�҂��Ȃ���΂����Ȃ�PACTRMNID��
					//	finSendPac��TRMNID�������ł��Ȃ������ꍇ��++
					//	oBuf_move_push��TRMNID���m�F�ł����ꍇ��--
					uint8			FailPac;
					//���M�҂�PAC��
					//	oBuf_move_push��FailPac�p��TRMNID����������Ȃ������ꍇ��++
					//	finSendPac��TRMNID�������ł����ꍇ��--
					uint8			WaitPacNum;			//��M���Ė����M��Packet�̐�
					uint8			HoldPac;				//���M�X��Packet�̐�
					bool			RecvFailDat=0;			//��M����Dat�̕ۑ�����m�ۂ��Ȃ�����/�ł��Ȃ������ꍇ�ɗ����AfinRecvDat�ŗ�����
					bool			RecvMode=0;			//��M��(Strt��M����Trmn��M�܂ł̊ԁj
					hmLib::cstring			RecvDatStr;
					did_t		RecvDatID=0;
					dsize_t		RecvDatCnt=0;			//��M����Dat�̃J�E���^
					vFp_v		FpInfromFinRecvPac=0;	//��M����Pac��S�Ď�M�������ɌĂяo���֐��|�C���^
					bool			SendMode=0;			//���M��(Strt���M����Trmn���M�܂ł̊ԁj
					odata			SendDat;				//���M����Dat
					dsize_t		SendDatCnt=0;			//���M����Dat�̃J�E���^
					vFp_v		FpInfromFinSendPac=0;	//���M����Pac��S�Ď�M�������ɌĂяo���֐��|�C���^
					//**************com_iBuf�֘A***************//
					//��M�o�b�t�@
					hmLib::cqueue iBuf_Que;
					idata iBuf_QueBuf[HMR_COM_BufqueSize];
				}

				//��M�o�b�t�@�̏�����
				void iBuf_initialize(void) {
					hmLib::cqueue_placement_construct(
						&iBuf_Que
						, HMR_COM_BufqueSize
						, sizeof(idata)
						, iBuf_QueBuf
						, 0);
				}
				//��M�o�b�t�@�̏I�[��
				void iBuf_finalize(void) {
					hmLib::cqueue_iterator itr;

					//�c���Ă���S�f�[�^��j��
					for(itr=hmLib::cqueue_begin(&iBuf_Que); itr!=hmLib::cqueue_end(&iBuf_Que); itr=hmLib::cqueue_next(&iBuf_Que, itr)) {
						service::cstring_destruct_safe(&((idata*)(itr))->Str);
					}

					//iQue,oQue��j��
					hmLib::cqueue_destruct(&iBuf_Que);
				}
				//��M�\�����m�F
				bool iBuf_empty(void) { return hmLib::cqueue_empty(&iBuf_Que) != 0; }
				//��M�ς݃f�[�^���폜����
				void iBuf_move_pop(idata* pData_) {
					if(iBuf_empty()) {
						idata_format(pData_);
						return;
					}
					idata_move((idata*)hmLib::cqueue_getptr(&iBuf_Que), pData_);
					hmLib::cqueue_pop(&iBuf_Que);
				}
				//iBuf�Ƀf�[�^��}���\���m�F
				bool iBuf_full(void) { return hmLib::cqueue_full(&iBuf_Que) != 0; }
				//iBuf�̃T�C�Y���擾����
				hmLib::cqueue_size_t iBuf_size(void) { return hmLib::cqueue_size(&iBuf_Que); }
				//iBuf�Ƀf�[�^��}������
				void iBuf_move_push(idata* mData_) {
					if(!idata_is_construct(mData_))return;

					if(iBuf_full()) {
						idata_destruct(mData_);
						return;
					}

					//���ۏ��move
					hmLib::cqueue_push(&iBuf_Que, (void*)(mData_));
					idata_format(mData_);
				}
				//*****************oBuf�֘A**************************//
				//���M�o�b�t�@
				hmLib::cqueue oBuf_Que;
				odata oBuf_QueBuf[HMR_COM_BufqueSize];
				//���M�o�b�t�@�̏�����
				void oBuf_initialize(void) {
					hmLib::cqueue_placement_construct(
						&oBuf_Que
						, HMR_COM_BufqueSize
						, sizeof(odata)
						, oBuf_QueBuf
						, 0);
				}
				//���M�o�b�t�@�̏I�[��
				void oBuf_finalize(void) {
					hmLib::cqueue_iterator itr;

					//�c���Ă���S�f�[�^��j��
					for(itr=hmLib::cqueue_begin(&oBuf_Que); itr!=hmLib::cqueue_end(&oBuf_Que); itr=hmLib::cqueue_next(&oBuf_Que, itr)) {
						service::cstring_destruct_safe(&((odata*)(itr))->Str);
					}

					//oQue��j��
					hmLib::cqueue_destruct(&oBuf_Que);
				}
				//���M�\�����m�F
				bool oBuf_empty(void) { return hmLib::cqueue_empty(&oBuf_Que) != 0; }
				//���M�ς݃f�[�^���폜����
				void oBuf_move_pop(odata* pData_) {
					if(oBuf_empty()) {
						odata_format(pData_);
						return;
					}
					odata_move((odata*)hmLib::cqueue_getptr(&oBuf_Que), pData_);
					hmLib::cqueue_pop(&oBuf_Que);
				}
				//oBuf�̃T�C�Y���擾����
				hmLib::cqueue_size_t oBuf_size(void) { return hmLib::cqueue_size(&oBuf_Que); }
				//���M�\���̊m�F
				bool oBuf_full(void) { return hmLib::cqueue_full(&oBuf_Que)!=0; }
				//���M�o�b�t�@�̖������p�P�b�g�I���^�O
				bool oBuf_isPacEnd(void) { return ((odata*)hmLib::cqueue_getptr(&oBuf_Que))->ID==HMR_COM_PACTRMNID; }
				//���M�p�֐�(��M�σf�[�^�p)�@���M�f�[�^�̐擪�A�h���X(Ptr)�A���M�f�[�^��(Size)�������ɓ����΁A���̑��M�f�[�^�𑗐M�҂��o�b�t�@�ɑ����B
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
				//****************wdt����*********************//
				uint8 xwdt_WdtCnt=0;	//WDT�̃J�E���^�[
				bool xwdt_WdtEn=0;	//WDT�̗L�������t���O
				//com�Ď��pWatchDogTimer�̃��Z�b�g
				void wdt_restart() { xwdt_WdtCnt=0; }
				//com�Ď��pWatchDogTimer���~
				void wdt_enable() { xwdt_WdtEn=true; }
				//com�Ď��pWatchDogTimer���~
				void wdt_disable() { xwdt_WdtEn=false; }
				//com�Ď��pWatchDogTimer�̃^�X�N�֐�
				struct wdt_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						//�L���łȂ��ꍇ�́A����
						if(!xwdt_WdtEn)return dt;

						xwdt_WdtCnt += dt;

						if(xwdt_WdtCnt>wdt_sec)devmng::kill();

						return dt;
					}
				}WdtTask;
				//****************VMC�֘A����*********************//
				hmLib_boolian vmc_can_iniRecvPac(void) { return static_cast<hmLib_boolian>(true); }
				//PacStrt��M�������ɌĂ΂��
				void vmc_iniRecvPac(void) {
					//��M���[�h��ON�ɂ���
					RecvMode=1;

					//WDT�΍�
					wdt_restart();
				}
				//iniRecvDat�����s���ėǂ����̊m�F�ɌĂ΂��
				hmLib_boolian vmc_can_iniRecvDat(void) { return static_cast<hmLib_boolian>(!iBuf_full()); }
				//PacTrmn��M�������ɌĂ΂��@�����̓G���[�̗L��
				void vmc_finRecvPac(unsigned char Err) {
					idata IData;
					//	cstring Str;
					//HMR_COM_PACTRMNID�R�}���h���쐬���APac�����
					//	service::cstring_construct_safe(&Str,1);
					//	cstring_putc(&Str,0,Err);
					//DEBUG	idata_format(&IData);
					//DEBUG	IData.ID=HMR_COM_PACTRMNID;
					//DEBUG	iBuf_move_push(&IData);

					idata_format(&IData);
					IData.ID=HMR_COM_PACTRMNID;
					iBuf_move_push(&IData);

					//��M���[�h��OFF�ɂ���
					RecvMode=0;
					//Pac��M�ʒm�֐����Ăяo��
					if(FpInfromFinRecvPac)FpInfromFinRecvPac();
					//��M�ς݁A���M�҂��̃f�[�^���𑝂₷
					++WaitPacNum;
				}
				//Dat��M�J�n���ɌĂ΂�� �����͎�M����f�[�^�T�C�Y
				unsigned int DebugCnt;
				unsigned char* Debug;
				void vmc_iniRecvDat(did_t ID, dsize_t Size) {
					//ID��Size���������ꍇ�͐�ɔj��
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
				//Dat��M�I�����ɌĂ΂�� �����̓G���[�̗L��
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

					//�G���[��������΃o�b�t�@�Ƀf�[�^�ǉ��A����Δj��
					idata_construct(&Data, RecvDatID, &RecvDatStr);
					iBuf_move_push(&Data);

				}
				//recv�����s���Ă悢���̊m�F�ɌĂ΂��
				hmLib_boolian vmc_can_recv(void) { return static_cast<hmLib_boolian>(true); }
				//Dat�̒��g��M���ɌĂ΂��@��������M�����f�[�^
				void vmc_recv(unsigned char c) {
					if(RecvFailDat==0)hmLib::cstring_putc(&RecvDatStr, RecvDatCnt++, c);
					//DEBUG	if(RecvFailDat==0)cstring_putc(&RecvDatStr,RecvDatCnt++,c);
				}
				hmLib_boolian vmc_can_iniSendPac(void) {
					return static_cast<hmLib_boolian>(devmng::isFullDuplex() || (WaitPacNum>0));
				}
				//RacStrt���M�������ɌĂ΂��
				void vmc_iniSendPac(void) {
					//���M���[�h��ON�ɂ���
					SendMode=1;

					if(WaitPacNum)--WaitPacNum;

					//WDT�΍�
					//	wdt_restart();
				}
				//PacTrmn���M�������ɌĂ΂��@�����̓G���[�̗L��
				void vmc_finSendPac(unsigned char Err) {
					odata Data;

					odata_format(&Data);
					while(1) {
						//HMR_COM_PACTRMNID�R�}���h���o�Ă��Ȃ������ꍇ�AHMR_COM_PACTRMNID�R�}���h����o�Ă���܂ł͖�����Pac�ł��邱�Ƃ�ʒm
						if(oBuf_empty()) {
							++FailPac;
							break;
						}

						//�擪����move&pop
						oBuf_move_pop(&Data);
						//HMR_COM_PACTRMNID�R�}���h==Pac�̏I�[�̍��}���o�Ă����ꍇ
						if(Data.ID==HMR_COM_PACTRMNID) {
							//HMR_COM_PACTRMNID�R�}���h���������APac�����
							odata_destruct(&Data);
							--HoldPac;
							break;
						} else {
							odata_destruct(&Data);
						}
					}
					//���M���[�h��OFF�ɂ���
					SendMode=0;
					//Pac��M�ʒm�֐����Ăяo��
					if(FpInfromFinSendPac)FpInfromFinSendPac();
				}
				//�f�[�^�̗L���̊m�F���\���ǂ���
				hmLib_boolian vmc_can_existSendDat(void) { return static_cast<hmLib_boolian>(!oBuf_empty()); }
				//���M���K�v�ȃf�[�^�̗L���̊m�F
				hmLib_boolian vmc_existSendDat(void) {
					//�o�b�t�@�����f�[�^��PacketTrmn�̏ꍇ�ɂ͑��݂��Ȃ��̂�false
					if(oBuf_isPacEnd())return static_cast<hmLib_boolian>(false);
					//��L�ȊO�̏ꍇ�̓f�[�^�����݂���̂�true
					return static_cast<hmLib_boolian>(true);
				}
				//iniSendDat�����s���ėǂ����̊m�F�ɌĂ΂��
				hmLib_boolian vmc_can_iniSendDat(void) {
					return static_cast<hmLib_boolian>(!oBuf_isPacEnd());
				}
				//Dat���M�m�莞�ɌĂ΂��@�T�C�Y��߂�
				void vmc_iniSendDat(did_t* pID, dsize_t* pSize) {
					if(odata_is_construct(&SendDat)) {
						odata_destruct(&SendDat);
					}
					//���M�f�[�^���p�P�b�g�Ɉڂ�
					oBuf_move_pop(&SendDat);

					if(!odata_is_construct(&SendDat)) {
						*pID=0;
						*pSize=0;
						return;
					}

					//�J�E���^�N���A
					SendDatCnt=0;
					//�����ɏ���߂��Ă��
					*pID=SendDat.ID;
					*pSize=hmLib::cstring_size(&(SendDat.Str));
				}
				//Dat���M�I�����ɌĂ΂��@�����̓G���[�̗L��
				void vmc_finSendDat(unsigned char Err) {
					//Dat���M�I����ʒm����
					if(odata_is_construct(&SendDat)) {
						odata_destruct(&SendDat);
					}
				}
				//send�����s���Ă悢���̊m�F�ɌĂ΂��
				hmLib_boolian vmc_can_send(void) {
					//FpSendable�����݂��Ă��Ȃ����A���݂��邪���M�\Size�ȓ��ł���ꍇ�A��ɐ^
					return static_cast<hmLib_boolian>((SendDat.FpSendable==0)
						|| (SendDatCnt<SendDat.FpSendable()));
				}
				//Dat�̒��g���M���ɌĂ΂��
				unsigned char vmc_send(void) {
					//	int tmpPAG;
					//	char c;
					//	if(SendDat.DSRPAG==0){
					//�f�[�^���M
					return hmLib::cstring_getc(&(SendDat.Str), SendDatCnt++);
					/*	}else{
							tmpPAG=_DSRPAG;
							_DSRPAG=__builtin_edspage(SendDat.DSRPAG);
							//�f�[�^���M
							c=cstring_getc(&(SendDat.Str),SendDatCnt++);
							_DSRPAG=tmpPAG;
							return c;
							}
							*/
				}

				//**************com�֐��Q*************//
				//�������֐�
				void initialize(service_interface& Service) {
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

					wdt_restart();
				}
				//�I�[���֐�
				void finalize() {
					wdt_disable();

					service::cstring_destruct_safe(&(RecvDatStr));
					service::cstring_destruct_safe(&(SendDat.Str));

					iBuf_finalize();
					oBuf_finalize();
				}
				//��M���[�h���m�F
				bool isRecvMode(void) { return RecvMode; }
				//���M���[�h���m�F
				bool isSendMode(void) { return SendMode; }
				//VMC1���쐬����
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
				//VMC1��j������
				void releaseVMC1(VMC1* pVMC1) {
					vmc1_release(pVMC1);
				}
				//****************IO�֐��Q**************************//
				//��M�\�ł��邩�m�F
				bool in_empty(void) { return iBuf_empty(); }
				//��M�o�b�t�@�������ς��ł��邩�̊m�F
				bool in_full(void) { return iBuf_full(); }
				//��M�o�b�t�@�̐擪�f�[�^���o��
				void in_move_pop(idata* pData_) { iBuf_move_pop(pData_); }
				//���M�o�b�t�@�������ς��ł��邩�̊m�F
				bool out_full(void) { return oBuf_full(); }
				//���M�o�b�t�@���󂩂ǂ����̊m�F
				bool out_empty(void) { return oBuf_empty(); }
				//���M�o�b�t�@�Ƀf�[�^�ǉ�
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
