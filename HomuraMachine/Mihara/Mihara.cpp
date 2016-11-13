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

#include <homuraLib_v2/machine/service/delay.hpp>
#include <homuraLib_v2/machine/service/task.hpp>
#include <homuraLib_v2/type.hpp>#include "Device.hpp"
#include "Service.hpp"
#include "Com.hpp"
#include "IO.hpp"
#include "System.hpp"
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
	cSystem System;
	cIO<typename cDevice::io_device> IO;

	//デバイスの初期化
	devmng::initialize();

	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::axel::device_initialize, 0);
	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::compass::device_initialize, 0);
	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::gyro::device_initialize, 0);

	
	//割り込み整理
	devmng::interrupt_enable_timerDevmng_interrupt();

	//モジュール初期化
	devmngmsg::initialize();
	thermo::initialize();
	battery::initialize();
	camera::initialize();
	inertial::initialize();
	cCO2<typename cDevice::co2_device> CO2('C');
	motor::initialize();
	cGPS<typename cDevice::gps_device> GPS;
	gps::initialize(GPS);

	//Lock
	CO2.lock(System, IO);


	//メッセージ登録
//	message::regist('!',debug_setup_listen,debug_listen,debug_setup_talk,debug_talk);
	Message.regist('m',motor::setup_listen,motor::listen,motor::setup_talk,motor::talk);
	Message.regist('c',inertial::compass::setup_listen,inertial::compass::listen,inertial::compass::setup_talk,inertial::compass::talk);
	Message.regist('a',inertial::axel::setup_listen,inertial::axel::listen,inertial::axel::setup_talk,inertial::axel::talk);
	Message.regist('G',inertial::gyro::setup_listen,inertial::gyro::listen,inertial::gyro::setup_talk,inertial::gyro::talk);
	Message.regist('j', camera::setup_listen, camera::listen, camera::setup_talk, camera::talk);
	Message.regist('g',gps::setup_listen,gps::listen,gps::task_setup_talk,gps::talk);
	Message.regist('b',battery::setup_listen,battery::listen,battery::task_setup_talk,battery::talk);
//	Message.regist('C',co2::setup_listen,co2::listen,co2::task_setup_talk,co2::talk);
	Message.regist('t',thermo::setup_listen,thermo::listen,thermo::task_setup_talk,thermo::talk);
	Message.regist('D', devmngmsg::setup_listen, devmngmsg::listen, devmngmsg::setup_talk, devmngmsg::talk);
//	message::regist('T',infrared_setup_listen,infrared_listen,infrared_task_setup_talk,infrared_talk);
//	message::regist('S',h2s_message);
//	message::regist('7',sht75_setup_listen,sht75_listen,sht75_task_setup_talk,sht75_talk);


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

		IO();
	}

	//終端化処理


	//デバイス終端化
	devmng::finalize();
	camera::finalize();
	inertial::finalize();
	thermo::finalize();
	battery::finalize();

	IO.unlock();
	

	return 0;
}

#
#endif
