#ifndef HMR_DEVICEMANAGEMESSAGE_C_INC
#define HMR_DEVICEMANAGEMESSAGE_C_INC 102
#
/*
===hmrDeviceManage===

v1_02/131019 amby
	メッセージ関数製作、その他の機能はインターフェースへ…

v1_01/131013 hmIto
	インターフェースを大幅に変更＆整理
		モードからクロック速度指定を除外
*/

/*===hmrDeviceManage===
'D'
モード取得コマンド
>> 0xA0 
<< 0xA0, DeviceManageMode, ClockModeSpeed  
	DeviceManageMode : 0x00:normal, 0x10:sleep, 0x20:roaming  
	ClockModeSpeed   : 0x00:normal, 0x01:low, 0x02:high  

Device Manage Mode 設定 

スリープ設定コマンド（実行はされない）
>> 0x10, 0x10, secSleepLength[2],secRemLength[2]
	secSleepLength秒で睡眠サイクル。うち、secSleepLength秒だけレム状態。
	secSleepLengthはsint16の正数、つまり1～32767秒≒9時間強が入る
<< 0x10, 0x10, secSleepLength[2],secRemLength[2],Code
	設定値と実行用コードが返ってくる
<< 0x10, 0xF1
	設定値が異常な場合は、0xF1が戻る

スリープ実行コマンド
>> 0x10, 0x11, Code
	実行用コードを送信して、実行を確認
		直前のスリープ設定コマンドで取得した実行用コードのみ有効
<< なし（入眠）


ローミング設定コマンド（実行はされない）
>> 0x10, 0x20, ModuleID[1],secRoamLength[1]
	ModuleIDの通信モジュールを有効にする
		ModuleID 0:RF, 1:MP
	secRoamLength秒待機した後、元のモジュールに戻る(正確にはsecRoamLength秒ごとに探す相手を変える)
	secRoamLengthにはuint8の正数、つまり1～255秒が入る
<< 0x10, 0x20,ModuleID,Code
	設定値と実行用コードが返ってくる
<< 0x10, 0xF2
	設定値が異常な場合は、0xF2が戻る
ローミング実行コマンド
>> 0x10, 0x21,Code
	実行用コードを送信して、実行を確認
		直前のスリープ設定コマンドで取得した実行用コードのみ有効
<< なし（ローミング）

モードリセット（ノーマルモード実行）コマンド
>> 0x10, 0x00
	ノーマルモード
<< 0x10, 0x00


KILL設定コマンド（実行はされない）
>> 0x20
<< 0x20,Code
	設定値と実行用コードが返ってくる
<< 0xF2
	設定値が異常な場合は、0xF1が戻る
KILL実行コマンド
>> 0x21,Code
	実行用コードを送信して、実行を確認
		直前のKILL設定コマンドで取得した実行用コードのみ有効
<< なし（永眠）


クロック設定用コマンド
>> 0x30, Speed
	Speed : 0:normal, 1:low, 2:high 	
<< 0x30, Speed 

initialize , sleep とかのインフォームを考える

*/

#include<stdlib.h>
#define HMLIB_NOLIB
#ifndef HMR_DEVICEMANAGEMESSAGE_INC
#	include"hmrDeviceManageMessage.h"
#endif
#ifndef HMR_UTILITIES_INC
#	include "hmrUtility.h"
#endif

// mode flag
struct{
	unsigned Info_i:1;	     // 情報送信モード受理
	
	unsigned SleepMode_i:1;  // sleep mode 受理
	//unsigned SleepModeFail:1; // sleep mode 受理失敗(入力がおかしかった)
	
	unsigned RoamingMode_i:1;// roaming mode 受理
	//unsigned RoamingModeFail:1; // roaming mode 受理失敗

	unsigned NormalMode_i:1; // normal mode 受理
	unsigned KillCom_i:1;    // kill command 受理 
	
	unsigned ClockMode_i:1;  // clock 変更コマンド受理
	unsigned ClockModeFail:1;
	
	unsigned SleepModeCodeFail:1;   // sleep mode exe 受理したがCode認識失敗
	unsigned RoamingModeCodeFail:1; // roaming mode exe 受理したが失敗
	unsigned KillCodeFail:1;		  // normal mode exe 受理したが失敗
}_devmngmsg_Mode={0,0,0,0,0,0,0,0,0,0};

// ロック用のコード
unsigned char _devmngmsg_sleep_Code=0;	//ロック解除用のコード
unsigned char _devmngmsg_roaming_Code=0;	//ロック解除用のコード
unsigned char _devmngmsg_kill_Code=0;	//ロック解除用のコード

uint16 _devmngmsg_sleep_sec_rem;
uint16 _devmngmsg_sleep_sec_nonRem;
unsigned char _devmngmsg_clockMode;
uint16 _devmngmsg_roaming_sec_interval;
//unsigned char _devmngmsg_roaming_ID;

/*
// initialize 関数
void devmngmsg_initialize(){
	_devmngmsg_Mode.Info_i = hmLib_false;
	_devmngmsg_Mode.SleepMode_i = hmLib_false;
	_devmngmsg_Mode.RoamingMode_i = hmLib_false;
	_devmngmsg_Mode.NormalMode_i = hmLib_false;
	_devmngmsg_Mode.KillCom_i = hmLib_false;
	_devmngmsg_Mode.ClockMode_i = hmLib_false;
	_devmngmsg_Mode.ClockModeFail = hmLib_false;
	_devmngmsg_Mode.SleepModeCodeFail = hmLib_false;
	_devmngmsg_Mode.RoamingModeCodeFail = hmLib_false;
	_devmngmsg_Mode.KillCodeFail = hmLib_false;

	//devmng_initialize();
}
// finalize 関数
void devmngmsg_finalize(){
	//devmng_finalize();
}
*/

hmLib_boolian devmngmsg_listen(cstring Str){

	//データサイズ確認
	if(cstring_size(&Str)==0)return hmLib_true;

	//1byte目でモードを分ける
	switch(cstring_getc(&Str,0)){

	// Infomation mode
	case 0xA0:
		_devmngmsg_Mode.Info_i = hmLib_true;
		return hmLib_false;

    // 各種待機モード設定
	case 0x10:
		if(cstring_size(&Str)==1)return hmLib_true;
		switch(cstring_getc(&Str,1)){
		case 0x00:// normal 
			devmng_mode_set(NormalMode);

			_devmngmsg_Mode.NormalMode_i = hmLib_true;
			return hmLib_false;

		case 0x10:// sleep mode
			if(cstring_size(&Str)!=6)return hmLib_true;
			_devmngmsg_sleep_sec_nonRem = ((uint16)cstring_getc(&Str,2) & 0x00FF) + (uint16)cstring_getc(&Str,3) * 256;
			_devmngmsg_sleep_sec_rem = ((uint16)cstring_getc(&Str,4) &0x00FF) + (uint16)cstring_getc(&Str,5) * 256;
			// sleep mode　設定準備
			devmng_sleep_setInterval(_devmngmsg_sleep_sec_nonRem, _devmngmsg_sleep_sec_rem); 
			//devmng_sleep_getInterval(&_devmngmsg_sleep_sec_nonRem, &_devmngmsg_sleep_sec_rem); 
			// code 取得
			_devmngmsg_sleep_Code = lockcode();

			_devmngmsg_Mode.SleepMode_i = hmLib_true;
			return hmLib_false;

		case 0x11:// sleep execute mode
			if(cstring_size(&Str)!=3)return hmLib_true;
			// code のチェック
			if(cstring_getc(&Str,2) ==  _devmngmsg_sleep_Code){// 一致
				// inform 関数のセット

				// sleep の実行
				devmng_mode_set(SleepMode);
			}else{
				// 失敗通知
				_devmngmsg_Mode.SleepModeCodeFail = hmLib_true;
			}
			return hmLib_false;
		
		case 0x20:// roaming mode 
			if(cstring_size(&Str)!=4)return hmLib_true;
			_devmngmsg_roaming_sec_interval =  ((uint16)cstring_getc(&Str,2) & 0x00FF) + (uint16)cstring_getc(&Str,3) * 256;
			// sleep mode　設定準備
			devmng_roaming_setInterval(_devmngmsg_roaming_sec_interval); 
			//devmng_roaming_getInterval(&_devmngmsg_roaming_sec_interval); 
			// code 取得
			_devmngmsg_roaming_Code = lockcode();
			_devmngmsg_Mode.RoamingMode_i = hmLib_true;
			return hmLib_false;

		case 0x21:// roaming execute mode
			if(cstring_size(&Str)!=3)return hmLib_true;
			// code のチェック
			if(cstring_getc(&Str,2) ==  _devmngmsg_roaming_Code){// 一致
				// inform 関数のセット

				// roaming の実行
				devmng_mode_set(RoamingMode);
			}else{
				// 失敗通知
				_devmngmsg_Mode.RoamingModeCodeFail = hmLib_true;
			}
			return hmLib_false;
	}

	case 0x20:
		_devmngmsg_kill_Code =  lockcode();
		_devmngmsg_Mode.KillCom_i = hmLib_true;
		return hmLib_false;

	case 0x21:
		if(cstring_size(&Str)!=2)return hmLib_true;
		// code のチェック
		if(cstring_getc(&Str,1) ==  _devmngmsg_kill_Code){// 一致
			// kill の実行
			devmng_kill();
		}else{
			// 失敗通知
			_devmngmsg_Mode.KillCodeFail = hmLib_true;
		}
		return hmLib_false;

	case 0x30:
		if(cstring_size(&Str)!=2)return hmLib_true;
		// mode のチェック
		_devmngmsg_clockMode = cstring_getc(&Str,1); 
		if(_devmngmsg_clockMode == 0){
			devmng_clock_set(NormalClock);
		}else if(_devmngmsg_clockMode == 1){
			devmng_clock_set(LowClock);
		}else if(_devmngmsg_clockMode == 2){
			devmng_clock_set(HighClock);
		}else{
			_devmngmsg_Mode.ClockModeFail = hmLib_true;
			return hmLib_false;
		}
		// clock mode 受理
		_devmngmsg_Mode.ClockMode_i = hmLib_true;
		return hmLib_false;
	default:
		return hmLib_true;
	}
}
hmLib_boolian devmngmsg_talk(cstring* pStr){
	devmng_mode mode;
	devmng_clock clockMode;
	//hmLib_uint16 sumadc;
	if(_devmngmsg_Mode.Info_i){
		cstring_construct_dynamic(pStr,4);
		cstring_putc(pStr,0,0xA0);

		mode = devmng_mode_get();
		if(mode == NormalMode){
			cstring_putc(pStr,1,0x00);
		}else if(mode == SleepMode){
			cstring_putc(pStr,1,0x10);
		}else if(mode == RoamingMode){
			cstring_putc(pStr,1,0x20);
		}

		clockMode = devmng_clock_get();
		if(clockMode == NormalClock){
			cstring_putc(pStr,2,0x00);
		}else if(clockMode == LowClock){
			cstring_putc(pStr,2,0x01);
		}else if(clockMode == HighClock){
			cstring_putc(pStr,2,0x02);
		}

		// RF or MP 
		if(hmr_uart1_getModuleID()==UART1_MODULEID_RF){
			cstring_putc(pStr,3,0x00);
		}else{
			cstring_putc(pStr,3,0x01);
		}

		_devmngmsg_Mode.Info_i=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.NormalMode_i){
		cstring_construct_dynamic(pStr,2);
		cstring_putc(pStr,0,0x10);
		cstring_putc(pStr,1,0x00);

		_devmngmsg_Mode.NormalMode_i=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.SleepMode_i){
		cstring_construct_dynamic(pStr,7);
		cstring_putc(pStr,0,0x10);
		cstring_putc(pStr,1,0x10);
		cstring_putc(pStr,2,(unsigned char)(_devmngmsg_sleep_sec_nonRem&0x00FF));
		cstring_putc(pStr,3,(unsigned char)((_devmngmsg_sleep_sec_nonRem>>8)&0x00FF));
		cstring_putc(pStr,4,(unsigned char)(_devmngmsg_sleep_sec_rem&0x00FF));
		cstring_putc(pStr,5,(unsigned char)((_devmngmsg_sleep_sec_rem>>8)&0x00FF));
		cstring_putc(pStr,6,_devmngmsg_sleep_Code);

		_devmngmsg_Mode.SleepMode_i=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.SleepModeCodeFail){
		cstring_construct_dynamic(pStr,2);
		cstring_putc(pStr,0,0x10);
		cstring_putc(pStr,1,0xF1);
		
		_devmngmsg_Mode.SleepModeCodeFail=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.RoamingMode_i){
		cstring_construct_dynamic(pStr,6);
		cstring_putc(pStr,0,0x10);
		cstring_putc(pStr,1,0x20);
		cstring_putc(pStr,2,(unsigned char)(_devmngmsg_roaming_sec_interval&0x00FF));
		cstring_putc(pStr,3,(unsigned char)((_devmngmsg_roaming_sec_interval>>8)&0x00FF));
		cstring_putc(pStr,4,_devmngmsg_roaming_Code);

		//変更先の通信手段を返す
		if(hmr_uart1_getModuleID()==UART1_MODULEID_RF){
			cstring_putc(pStr,5,0x01); // 現状がRFなら次はMP
		}else{
			cstring_putc(pStr,5,0x00); // 現状がMPなら次はRF 
		}

		_devmngmsg_Mode.RoamingMode_i=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.RoamingModeCodeFail){
		cstring_construct_dynamic(pStr,2);
		cstring_putc(pStr,0,0x10);
		cstring_putc(pStr,1,0xF2);
		
		_devmngmsg_Mode.RoamingModeCodeFail=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.KillCom_i){
		cstring_construct_dynamic(pStr,2);
		cstring_putc(pStr,0,0x20);
		cstring_putc(pStr,1,_devmngmsg_kill_Code);

		_devmngmsg_Mode.KillCom_i=hmLib_false;
		return hmLib_false;
	}else if(_devmngmsg_Mode.KillCodeFail){
		cstring_construct_dynamic(pStr,1);
		cstring_putc(pStr,1,0xF2);
		
		_devmngmsg_Mode.KillCodeFail=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.ClockMode_i){
		cstring_construct_dynamic(pStr,2);
		cstring_putc(pStr,0,0x30);
		cstring_putc(pStr,1,_devmngmsg_clockMode);

		_devmngmsg_Mode.ClockMode_i=hmLib_false;
		return hmLib_false;

	}else if(_devmngmsg_Mode.ClockModeFail){
		cstring_construct_dynamic(pStr,1);
		cstring_putc(pStr,0,0xF3);

		_devmngmsg_Mode.ClockModeFail=hmLib_false;
		return hmLib_false;
	}

	return hmLib_true;
}
void devmngmsg_setup_listen(void){return;}
void devmngmsg_setup_talk(void){
	//_thermo_Mode.Data=_thermo_Mode.SendData;
    /*Debug*///	_thermo_Mode.Data=1;
}

sint16 devmngmsg_task_inform(sint16 interval){
	_devmngmsg_Mode.Info_i = hmLib_true;
	return interval;
}

/*
//=== クロックモード制御機能 ===
//クロックを指定モードで再スタートする
void _devmngmsg_clockmode_restartClock(devmngmsg_clockmode Mode_);
//クロックをクロックモードで再スタートする
void _devmngmsg_clockmode_restartClockDefault();
//クロックモードを設定する
void devmngmsg_clockmode_set(devmngmsg_clockmode ClockMode_);
//クロックモードを取得する
devmngmsg_clockmode devmngmsg_clockmode_get(void);


//=== クロックモード制御機能 ===
//デフォルトクロック
devmngmsg_clockmode x_devmngmsg_clockmode_Mode;
//クロックを指定モードで再スタートする
void _devmngmsg_clockmode_restartClock(devmngmsg_clockmode Mode_){
	//クロックモード設定処理
	hmr_clock_finalize();

	switch(Mode_){
	case LowClockMode:
		hmr_clock_initialize(CLOCK_SLOW);
		break;
	case HighClockMode:
		hmr_clock_initialize(CLOCK_FAST);
		break;
	default:
		hmr_clock_initialize(CLOCK_NORMAL);
		break;
	}

	//クロック設定後のため
	hmr_uart1_initialize(hmr_uart1_getModuleID());
	hmr_uart2_initialize();
	hmr_adc_initialize();
}
//クロックをクロックモードで再スタートする
void _devmngmsg_clockmode_restartClockDefault(){
	_devmngmsg_clockmode_restartClock(x_devmngmsg_clockmode_Mode);
}
//クロックモードを設定する
void devmngmsg_clockmode_set(devmngmsg_clockmode ClockMode_){
	//同じクロックモードを指定されたら無視
	if(x_devmngmsg_clockmode_Mode==ClockMode_)return;

	//クロックモードを変更
	x_devmngmsg_clockmode_Mode=ClockMode_;

	//クロックモード変更をモード制御部に通知
	_devmngmsg_mode_informClockMode();
}
//クロックモードを取得する
devmngmsg_clockmode devmngmsg_clockmode_get(void){return x_devmngmsg_clockmode_Mode;}

//=== スリープモード制御 ===
//入眠時＆起床時に呼び出される関数ポインタ
devmngmsg_vFp_v x_devmngmsg_sleep_InformSleep;
devmngmsg_vFp_v x_devmngmsg_sleep_InformWakeup;
//レム＆ノンレムの切り替え時に呼び出される関数ポインタ
devmngmsg_vFp_v x_devmngmsg_sleep_InformRem;
devmngmsg_vFp_v x_devmngmsg_sleep_InformNonRem;
//睡眠時のインターバル制御変数
sint16 x_devmngmsg_sleep_secNonRem=540;
sint16 x_devmngmsg_sleep_secRem=60;
sint16 x_devmngmsg_sleep_secCnt=0;
boolian x_devmngmsg_sleep_IsRem;
//スリープモードのパラメータを取得する
void devmngmsg_sleep_getInterval(sint16& secNonRem,sint16& secRem){
	secNonRem=x_devmngmsg_sleep_secNonRem;
	secRem=x_devmngmsg_sleep_secRem;
}
//スリープモードのパラメータを変更する
void devmngmsg_sleep_setInterval(sint16 secNonRem, sint16 secRem){
	if(secNonRem<0)secNonRem=0;
	if(secRem<1)secRem=1;

	x_devmngmsg_sleep_secNonRem=secNonRem;
	x_devmngmsg_sleep_secRem=secRem;
}
//スリープモードのパラメータを取得する
void devmngmsg_sleep_registSleepInformFunction(devmngmsg_vFp_v InformSleep,devmngmsg_vFp_v InformWakeUp){
	x_devmngmsg_sleep_InformSleep=InformSleep;
	x_devmngmsg_sleep_InformWakeup=InformWakeUp;
}
//レムスリープに入る/出る際の通知関数を登録する
void devmngmsg_sleep_registRemInformFunction(devmngmsg_vFp_v InformRem,devmngmsg_vFp_v InformNonRem){
	x_devmngmsg_sleep_InformRem=InformRem;
	x_devmngmsg_sleep_InformNonRem=InformNonRem;
}
//sleep状態にほむらを変える関数
void _devmngmsg_sleep_start(){	
	//スリープ開始を通知する
	if(x_devmngmsg_sleep_InformSleep)x_devmngmsg_sleep_InformSleep();

	// モジュール電源落とす
	_devmngmsg_module_power_off();

	// クロックスピード落とす
	_devmngmsg_clockmode_restartClock(LowClockMode);
}
//sleep状態のほむらを起こす関数
void _devmngmsg_sleep_stop(void){
	// クロックスピードNORMALに
	_devmngmsg_clockmode_restartClockDefault();

	// モジュール電源復帰
	_devmngmsg_module_power_on();

	//スリープ終了を通知する
	if(x_devmngmsg_sleep_InformWakeup)x_devmngmsg_sleep_InformWakeup();

	//割り込み許可
	uart1_enable_fget_interrupt();
}
//レム状態かの取得関数
boolian _devmngmsg_sleep_isRem(void){return x_devmngmsg_sleep_IsRem;}
//スリープ時のタスク関数
void _devmngmsg_sleep_interrupt_function(sint16 Interval_){
	x_devmngmsg_sleep_secCnt+=Interval_;

	if(x_devmngmsg_sleep_IsRem){
		if(x_devmngmsg_sleep_secCnt>=x_devmngmsg_sleep_secRem){
			x_devmngmsg_sleep_IsRem=0;
			x_devmngmsg_sleep_secCnt=0;
			if(x_devmngmsg_sleep_InformNonRem)x_devmngmsg_sleep_InformNonRem();
		}
	}else{
		if(x_devmngmsg_sleep_secCnt>=x_devmngmsg_sleep_secNonRem){
			x_devmngmsg_sleep_IsRem=1;
			x_devmngmsg_sleep_secCnt=0;
			if(x_devmngmsg_sleep_InformRem)x_devmngmsg_sleep_InformRem();
		}
	}
}

//==================== Roaming ===================
unsigned char x_devmngmsg_roaming_Code=0;	//ロック解除用のコード
boolian x_devmngmsg_roaming_IsRoaming=0;	//ローミング中であることを示すフラグ
sint16 x_devmngmsg_roaming_secInterval=0;		//Roaming切り替えの長さ
sint16 x_devmngmsg_roaming_secCnt=0;		//Roaming状態のカウンタ
sint16 _devmngmsg_roaming_task(sint16 Sec_){
	//カウンタを進める
	++_devmngmsg_roaming_Cnt;

	//ローミング切換え時
	if(_devmngmsg_roaming_Cnt>HMR_DEVCTR_ROAMING_LENGTH){
		if(uart1_getModuleID()==UART1_MODULEID_RF){
			HMR_PIN_RedLED_set(1);
			uart1_initialize(UART1_MODULEID_MP);
		}else{
			HMR_PIN_RedLED_set(0);
			uart1_initialize(UART1_MODULEID_RF);
		}
		_devmngmsg_roaming_Cnt=0;
	}
}
//ローミング実行用関数
//	wdtを無効にして、別の通信モジュールでの通信を試みる
void _devmngmsg_roaming_start(void){
	x_devmngmsg_roaming_IsRoaming=0;
	x_devmngmsg_roaming_secCnt=0;
	disable_wdt();

	if(hmr_uart1_getModuleID()==UART1_MODULEID_RF){
		HMR_PIN_RedLED_set(1);
		hmr_uart1_initialize(UART1_MODULEID_MP);
	}else{
		HMR_PIN_RedLED_set(0);
		hmr_uart1_initialize(UART1_MODULEID_RF);
	}

	//timer4_5の割り込み有効化
	timer4_5_finalize();
	timer4_5_initialize(10000);		//10秒ごとの割り込み
	timer4_5_enable_interrupt();	//割り込み有効化
}
//ローミング終了用関数
void _devmngmsg_roaming_stop(void){
	timer4_5_disable_interrupt();

	restart_wdt();
	//Configに合わせて、WDTセット
	if(Config_DisableWatchDogTimer)enable_wdt();

	//timer4_5の割り込み有効化
	timer4_5_finalize();
	timer4_5_initialize(1000);		//1秒ごとの割り込み
	timer4_5_enable_interrupt();	//割り込み有効化
}
//ローミングモードのパラメータを取得する
void devmngmsg_roaming_getInterval(sint16& secInterval){
	secInterval=x_devmngmsg_roaming_secInterval;
}
//ローミングモードのパラメータを設定する
void devmngmsg_roaming_setInterval(sint16 secInterval){
	if(secInterval<=10)secInterval=10;
	x_devmngmsg_roaming_secInterval=secInterval;
}

//=== 基本操作関数 ===
//デバイス初期化関数
void devmngmsg_initialize(void){
	//電源を一度切る
	_devmngmsg_power(0);

	//デバイス初期化
	hmr_device_initialize();

	//クロック初期化
	if(Config_DefaultHighClock)hmr_clock_smart_initialize(CLOCK_HIGH);
	else hmr_clock_smart_initialize(CLOCK_NORMAL);	

	//割り込み設定初期化
	hmr_interrupt_initalize();

	//通信デバイスの初期化
	if(Config_DefaultMobilePhone)hmr_uart1_smart_initialize(UART1_MODULEID_MP);
	else hmr_uart1_smart_initialize(UART1_MODULEID_RF);

	rs_fputc('O',Stream_VMC);
	rs_fputc('K',Stream_VMC);
	rs_fputc((unsigned char)(RCON),Stream_VMC);
	rs_fputc((unsigned char)(RCON>>8),Stream_VMC);
	rs_fputc(0x0d,Stream_VMC);
	rs_fputc(0x0a,Stream_VMC);

	hmr_uart2_initialize();
	hmr_spi2_initialize();
	hmr_i2c_initialize();
	hmr_adc_initialize();

	rs_fputc('C',Stream_VMC);
	rs_fputc('m',Stream_VMC);
	rs_fputc(0x0d,Stream_VMC);
	rs_fputc(0x0a,Stream_VMC);

	delay_ms(500);
	_devmngmsg_power(1);
	
	rs_fputc('P',Stream_VMC);
	rs_fputc('w',Stream_VMC);
	rs_fputc(0x0d,Stream_VMC);
	rs_fputc(0x0a,Stream_VMC);

	//WatchDogTimer
	if(Config_DisableWatchDogTimer)disable_wdt();
	else enable_wdt();

	//taskシステムを起動する
	task_initialize();
}
//デバイス終端化関数
void devmngmsg_finalize(void){
	//taskシステムを終了する
	task_finalize();

	//uart1_fget/fput停止
	uart1_disable_fget_interrupt();
	uart1_disable_fput_interrupt();
	uart2_disable_fget_interrupt();
	uart2_disable_fput_interrupt();

	//通信デバイスの終端化
	hmr_uart1_finalize();
	hmr_uart2_finalize();
	hmr_spi2_finalize();
	hmr_i2c_finalize();
	hmr_adc_finalize();

	//devmngmsgパワー
	_devmngmsg_power(0);

	//デバイスの終端化処理
	hmr_clock_finalize();
	hmr_interrupt_finalize();
	hmr_device_finalize();
}
//デバイス強制終了
void devmngmsg_kill(void){
	// 終端化
	devmngmsg_finalize();
	// 自分自身も切る
	_devmngmsg_pic_power_off();
}
void _devmngmsg_module_power_off(){
	//spriteカメラ停止
	sprite_finalize();	//timer1,uart2_fget/fputを停止
	sprite_power(0);

	//全電源off
//	spi_selectGPS(0);
//	spi_selectDrive(0);
//	rf_power(0);
//	mp_power(0);
//	subPIC_power(0);
//	i2c_power(0);
//	adc_power(0);
//	h2s_powerSensor(0);
	co2_powerPump(0);
	co2_powerSensor(0);
//	servo_power(0);
}
void _devmngmsg_module_power_on(){
	//　ここは保留中

	//spriteカメラ起動
	sprite_power(1);


	//全電源off
//	spi_selectGPS(0);
//	spi_selectDrive(0);
//	rf_power(0);
//	mp_power(0);
//	subPIC_power(0);
//	i2c_power(0);
//	adc_power(0);
//	h2s_powerSensor(0);
	co2_powerPump(0);
	co2_powerSensor(0);
//	servo_power(0);
}
//通信系のパワー一括制御関数
void _devmngmsg_power(boolian onoff){
	spi_selectGPS(0);
	spi_selectDrive(0);
	subPIC_power(onoff);
	hmr_uart1_power_switch(onoff,hmr_uart1_getModuleID());
	i2c_power(onoff);
	adc_power(onoff);
    HMR_PIN_ADCPW_CH7_set(onoff);
    HMR_PIN_ADCPW_CH8_set(onoff);
}


//=== モード制御機能 ===
devmngmsg_mode x_devmngmsg_mode_Mode;
//デバイスモードを変更する
void devmngmsg_mode_set(devmngmsg_mode Mode_){
	switch(Mode_){
	case SleepMode:
		switch(_devmngmsg_Mode){
		case RoamingMode:
			_devmngmsg_roaming_stop();
			break;
		}

		//モード変更
		_devmngmsg_Mode=SleepMode;
		_devmngmsg_sleep_start();

		return;
	case RoamingMode:
		switch(_devmngmsg_Mode){
		case SleepMode:
			_devmngmsg_sleep_stop();
			break;
		}

		//モード変更
		_devmngmsg_Mode=RoamingMode;
		_devmngmsg_roaming_start();

		return;
	default:
		switch(_devmngmsg_Mode){
		case RoamingMode:
			_devmngmsg_roaming_stop();
			break;
		case SleepMode:
			_devmngmsg_sleep_stop();
			break;
		}

		//モード変更
		_devmngmsg_Mode=NormalMode;

		return;
	}

}
//デバイスモードを取得する
devmngmsg_mode devmngmsg_mode_get(void){return x_devmngmsg_mode_Mode;}
//クロックモード変更に対応する
void _devmngmsg_mode_informClockMode(void){
	//スリープ中のみ無視
	if(x_devmngmsg_mode_Mode==SleepMode && !_devmngmsg_sleep_isRem())return;

	_devmngmsg_clockmode_restartClockDefault();
}

//==================== 割り込み関数 ====================
/*
//スリープ、およびローミング実行用関数
void timer4_5_interrupt(void){
	//フラグクリアと時刻のリセット
	timer4_5_clear_interrupt_flag();
	timer4_5_reset();

	//スリープモードの割り込み制御
	if(devmng_is_sleep()){

	}//ローミングモードの割り込み制御
	else if(devmng_is_roaming()){
		_devmng_sleep_isRem();
	}//通常のタスク処理の場合
	else{
		task_interrupt_function();
	}
}
*/


#
#endif
