#ifndef HMR_MACHINE_MIHARA_INC
#define HMR_MACHINE_MIHARA_INC 0
#
#
#define HMLIB_NOLIB
/*
=== HomuraMachine_Mihara ===
v0_00/141111 hmito
	cpp化準備バージョン
===hmrM2500===
v2_01/131019 hmIto
	DeviceManageシステムを導入
v2_00/130720 hmIto
	デバッグ用の関数群やコメントを消して整理
v1_01/130202 iwahori
	main関数内にmessage::setup_talkとmessage::setup_listenを追加
	listen関数に入れているので受信は上手くいっているが返信がない，
	パケットの区切りを作るpacket_talkとかを作っていないのが原因？
	fp_finRecvPac,com::vmc_finRecvPacに委譲？
v1_00/130112 hmIto
	hmLib_v3_03に対応
	全二重通信に対応
	センサー部分を分離してinclude
===hmrMain2400===
v1_09/121101 hmIto
	restart_wdtをmain-loopの一か所に集約
	パケット区切りにHMR_COM_PACTRMNIDを使用
	受信パケットがフルになった場合は、強制リセット
	強制リセットコマンドとしてMessageに依存しない(Comには依存)、0xFF命令を追加
	->バグがあったため一時的に廃止
	sprite::message2を使用するように変更
v1_08/121031 hmIto
	devctrに半スリープ時の電源制御用変数とそのコマンドを追加
v1_07/121030 iwahori
	加速度のmessage::resistを追加
v1_06/121028 hmIto
	スリープ機能とローミング機能（通信デバイス切り替え）を実装
		devctr関数群を用いて操作
		いずれも、device_messageコマンドを用いて制御する
		timer4_5を用いている
			スリープ時に10秒に一度割り込ませて、時間計測
			ローミング時のデバイス切り替えに使用
	送信バッファに基づく送信割り込み割り込み制御を採用
		送信バッファがからになったらdisable
		送信バッファにデータを追加したらenable
		com::out_empty関数を使って、送信バッファが空かどうか確認
v1_05/121027 hmIto
	uartの送受信が正常に行われることまで確認
		hmrTaskがわるさをしていたので廃止．
		バッファの制御時に送信割り込みを制御するように変更
v1_04/121020 iwahori
	initialize()内にSPI slave slectピンの初期化を追加
		RF module電源オン
v1_03/121020 hmIto
	デバッグ修正を確認して、コメントア
	ウトを削除
	packet_message関数が未完成だったのを追加
v1_02/121014 iwahori
	#include "hmrSpriteFunctions.h"を追加
	p_DIP1をUserConfigBit1に変更
	uart1_fget_interrupt_retry_task()とuart1_fput_interrupt()の順番を入れ替え
	送信も同様
	packet_message()内cstring Ans{1,1}→cstring Ans={1,1}
	sprite::ONをsprite::powerに変更
v1_01/121013 hmIto
	恐らく送受信部分がほぼ完成
	初期化終端化が必要な関数の作成が残る
v1_00/120921 hmIto
	バージョン管理開始
	ファイルを複数に分割
*/
#include <homuraLib_v2/old/hmrVMC1.h>
#include <homuraLib_v2/machine/service/delay.hpp>
#include <homuraLib_v2/machine/service/task.hpp>
#include <homuraLib_v2/type.hpp>
#include "Device.hpp"
#include "Service.hpp"
#include "Com.hpp"
#include "Message.hpp"
#include "CO2.hpp"
#include "Battery.hpp"
#include "GPS.hpp"
#include "Drive.hpp"
#include "Inertial.hpp"
#include "Camera.hpp" 
#include "Thermo.hpp"
#include "Battery.hpp"
#include "DeviceManage.hpp"
#include "DeviceManage_forCourier.hpp"
#include "DeviceManageMessage.hpp"

using namespace hmr::machine::mihara;
//VMC1
VMC1* pVMC;
//courier interrpt function
devmng::courier::uart::tx_interrupt_function devmng::courier::uart::CourierTxInterruptFunc;
void devmng::courier::uart::tx_interrupt_function::operator()(void){
	//割り込みフラグをクリア
//	clear_interrupt_fputflag(Stream_VMC);

	//送信可能なら、送信
	devmng::courier::uart::fput(vmc1_send(pVMC));

	//送信可能でなくなった場合は、割り込みを切る
	if(!vmc1_can_send(pVMC)){
		//uart1_disable_fput_interrupt();
		devmng::interrupt_disable_streamVMC_fput_interrupt();
	}
}
devmng::courier::uart::rx_interrupt_function devmng::courier::uart::CourierRxInterruptFunc;
void devmng::courier::uart::rx_interrupt_function::operator()(void){
	//割込みフラグをクリア
//	clear_interrupt_fgetflag(Stream_VMC);

	//データを受信し、Comに処理させる
	vmc1_recv(pVMC,devmng::courier::uart::fget());
}

//====== main 関数 ======
using namespace hmr::machine::mihara;
using namespace hmr::machine::service;
int main(void){
	cDevice Device;
	cService Service;

	//デバイスの初期化
	devmng::initialize();

	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::axel::device_initialize, 0);
	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::compass::device_initialize, 0);
	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::gyro::device_initialize, 0);

	//通信関連機能の初期化
	pVMC = com::createVMC1();
	vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
	com::initialize();
	message::initialize();

	//割り込み整理
	devmng::interrupt_enable_timerDevmng_interrupt();

	devmng::courier::uart::fput_set_interrupt();//streamVMC_set_fput_interrupt_flag();//set_interrupt_fputflag(Stream_VMC);//割り込み予約
	devmng::interrupt_enable_streamVMC_fget_interrupt();// enable_interrupt_fget(Stream_VMC);
	devmng::interrupt_enable_streamVMC_fput_interrupt();// enable_interrupt_fput(Stream_VMC);

	//モジュール初期化
	devmngmsg::initialize();
	thermo::initialize();
	battery::initialize();
	camera::initialize();
	inertial::initialize();
	co2::initialize();
	motor::initialize();
	cGPS<cDevice::gps_device> GPS;
	gps::initialize(GPS);

	//メッセージ登録
//	message::regist('!',debug_setup_listen,debug_listen,debug_setup_talk,debug_talk);
	message::regist('m',motor::setup_listen,motor::listen,motor::setup_talk,motor::talk);
	message::regist('c',inertial::compass::setup_listen,inertial::compass::listen,inertial::compass::setup_talk,inertial::compass::talk);
	message::regist('a',inertial::axel::setup_listen,inertial::axel::listen,inertial::axel::setup_talk,inertial::axel::talk);
	message::regist('G',inertial::gyro::setup_listen,inertial::gyro::listen,inertial::gyro::setup_talk,inertial::gyro::talk);
	message::regist('j', camera::setup_listen, camera::listen, camera::setup_talk, camera::talk);
	message::regist('g',gps::setup_listen,gps::listen,gps::task_setup_talk,gps::talk);
	message::regist('b',battery::setup_listen,battery::listen,battery::task_setup_talk,battery::talk);
	message::regist('C',co2::setup_listen,co2::listen,co2::task_setup_talk,co2::talk);
	message::regist('t',thermo::setup_listen,thermo::listen,thermo::task_setup_talk,thermo::talk);
	message::regist('D', devmngmsg::setup_listen, devmngmsg::listen, devmngmsg::setup_talk, devmngmsg::talk);
//	message::regist('T',infrared_setup_listen,infrared_listen,infrared_task_setup_talk,infrared_talk);
//	message::regist('S',h2s_message);
//	message::regist('7',sht75_setup_listen,sht75_listen,sht75_task_setup_talk,sht75_talk);

	//タスク登録（移行していないfunctional_taskバージョンの関数）
//	devmng_task_quick_start(5,infrared_task_data,0);
//	devmng_task_quick_start(5,infrared_task_inform,0);
//	devmng_task_quick_start(5,sht75_task_data,0);
//	devmng_task_quick_start(5,sht75_task_inform,0);
//	devmng_task_quick_start(10,h2s_task_interrupt,0);

	//受け取り用データ
	idata IData;
	bool IPacketMode = 0;
	odata OData;
	bool OPacketMode = 0;
	hmr::sint8 debug_cnt = 0;

	while(1){
		Device();
		Service();
		camera::work();
		inertial::work();
		//Watch Dog Timer リセット
		devmng::clear_wdt();
//		restart_wdt();

		//DeviceManageの割り込み委譲処理関数
		if(devmng::interrupt_yield())continue;

		//早すぎるので待機
		delay_ms(5);

		//受信可能なデータがある場合
		if(!com::in_empty()){
			//受信データを取得
			com::in_move_pop(&IData);

			//中身が入っている場合
			if(idata_is_construct(&IData)){
				//Packetをまだ開いていない場合
				if(!IPacketMode){
					//Packetをここで開く
					IPacketMode=true;

					//受信用メッセージモジュール準備
					message::setup_listen();
				}

				//Packet終了IDだった場合
				if(IData.ID==HMR_COM_PACTRMNID){
					//Packetをここで閉じる
					IPacketMode=false;

					//メッセージ破棄
					idata_destruct(&IData);
				}else{
					//メッセージ処理;
					message::listen(&IData);
				}
			}
		}

		//早すぎるので待機
		delay_ms(5);

		//送信待ちのPacketがなく、comの送信バッファがいっぱいでもないとき
		if(!com::isWaitSendPacket() && !com::out_full()){
			//Packetをまだ開いていない場合
			if(!OPacketMode){
				//Packetをここで開く
				OPacketMode=true;
				//送信用メッセージモジュール準備
				message::setup_talk();
			}

			//送信用データを準備する
			odata_format(&OData);

			//送信データの取得に失敗した場合
			if(message::talk(&OData)){
				//中身が作成されていれば破棄
				if(odata_is_construct(&OData))odata_destruct(&OData);
				//Packetをここで閉じる
				OPacketMode=false;
				OData.ID=HMR_COM_PACTRMNID;
			}

			//送信データを送り出す
			com::out_move_push(&OData);		
		}

		//送信割り込みが切られていて、かつ送信可能状態のときには、送信割り込みをオンにする
		if(!devmng::courier::uart::fput_is_interrupt_enable()){
			if(vmc1_can_send(pVMC))devmng::interrupt_enable_streamVMC_fput_interrupt();//uart1_enable_fput_interrupt();
		}

	}

	//終端化処理

	//割り込み禁止
	devmng::courier::uart::fget_disable_interrupt();
	devmng::courier::uart::fput_disable_interrupt();

	//通信関連の終端化処理
	message::finalize();
	vmc1_finalize(pVMC);
	com::finalize();
	com::releaseVMC1(pVMC);

	//デバイス終端化
	devmng::finalize();
	camera::finalize();
	inertial::finalize();
	thermo::finalize();
	co2::finalize();
	battery::finalize();
	

	return 0;
}

#
#endif
