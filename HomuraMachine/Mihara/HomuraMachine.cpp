#ifndef HMR_MACHINE_INC
#define HMR_MACHINE_INC 0
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

#include <homuraLib_v2/type.hpp>
#include "Device.hpp"
#include "System.hpp"
#include "Service.hpp"
#include "IO.hpp"
#include "CO2.hpp"
#include "Battery.hpp"
#include "GPS.hpp"
#include "Drive.hpp"
#include "Inertial.hpp"
#include "Camera.hpp" 
#include "Thermo.hpp"
#include "Battery.hpp"
//#include "DeviceManage.hpp"
//#include "DeviceManage_forCourier.hpp"
//#include "DeviceManageMessage.hpp"

//====== main 関数 ======
using namespace hmr::machine::mihara;
int main(void){
	cDevice Device;

	cService<typename cDevice::service_device> Service;
	cIO<typename cDevice::io_device> IO(Service);
	cSystem<typename cDevice::system_device> System('D',IO, Service, IO);
	System.regist(Service.getSystemClient());
	System.regist(IO.getSystemClient());
	IO.regis_system_agent(System.getIOAgent());

	//モジュール初期化
	cThermo<typename cDevice::thermo_device> Thermo('t', System, IO, Service);
	cBattery<typename cDevice::battery_device> Battery('b', System, IO, Service);
	cCamera<typename cDevice::sprite_device> Camera('j', System, IO, Service);
	cInertial<typename cDevice::inertial_device> Inertial('a', 'c', 'G', System, IO, Service);
	cCO2<typename cDevice::co2_device> CO2('C', System, IO, Service);
	cMotor<typename cDevice::motor_device> Motor('m', System, IO, Service);
	cGPS<typename cDevice::gps_device> GPS('g', System, IO, Service);

    while(1){
		Service();
		System();

		IO();

		CO2();
		Camera();
		Inertial();
		Thermo();
		Battery();
		Motor();

		//早すぎるので待機
		Service.delay().delay_ms(5);
	}

	//終端化処理

	return 0;
}

#
#endif
