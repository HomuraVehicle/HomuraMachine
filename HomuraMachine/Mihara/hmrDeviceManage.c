#ifndef HMR_DEVICEMANAGE_C_INC
#define HMR_DEVICEMANAGE_C_INC 103
#
#ifndef HMR_DEVICEMANAGE_INC
#	include"hmrDeviceManage.h"
#endif
#ifndef HMR_DEVICE_INC
#	include"hmrDevice.h"
#endif
#ifndef HMR_TASK_INC
#	include"hmrTask.h"
#endif
#ifndef HMR_COM_INC
#	include"hmrCom.h"
#endif
#ifndef HMR_UTILITY_INC
#	include"hmrUtility.h"
#endif


//++++++++++++++++++ protected宣言 +++++++++++++++++
//=== 基本関数 ===
//デバイスの電源管理
void _devmng_power(boolian onoff);

//=== モード制御機能 ===
//モード制御機能初期化
void _devmng_mode_initialize(devmng_mode Mode_);
//モード制御機能終端化
void _devmng_mode_finalize();
//クロック変更の通知関数
void _devmng_mode_informClockChange(void);

//=== クロック制御機能 ===
//クロック制御機能初期化
void _devmng_clock_initialize(devmng_clock Clock_);
//クロック制御機能終端化
void _devmng_clock_finalize();
//クロック速度の変更関数
void _devmng_clock_restartClock(devmng_clock Clock_);
//クロック速度をデフォルトに変更
void _devmng_clock_restartClockDefault();

//=== タスク制御 ===
//タスク制御初期化
void _devmng_task_initialize();
//タスク制御終端化
void _devmng_task_finalize();
//タスク制御処理関数
void _devmng_task_interrupt_function(sint16 Interval_);

//=== スリープモード制御 ===
//スリープモード制御初期化
void _devmng_sleep_initialize(void);
//スリープモード制御終端化
void _devmng_sleep_finalize(void);
//スリープに入る処理
void _devmng_sleep_start();
//スリープから出る処理
void _devmng_sleep_stop(void);
//スリープ制御処理関数
void _devmng_sleep_interrupt_function(sint16 Interval_);
//スリープ時の割り込み禁止関数
void _devmng_sleep_disable_interrupt(void);
//スリープ時の割り込み復帰関数
void _devmng_sleep_enable_interrupt(void);


//=== ローミングモード制御 ===
//ローミングモード制御初期化
void _devmng_roaming_initialize(void);
//ローミングモード制御終端化
void _devmng_roaming_finalize(void);
//ローミングに入る処理
void _devmng_roaming_start(void);
//ローミングから出る処理
void _devmng_roaming_stop(void);
//ローミング制御処理関数
void _devmng_roaming_interrupt_function(sint16 Interval_);

//=== 割り込み関数制御 ===
void _devmng_interrupt_initialize(void);
void _devmng_interrupt_finalize(void);


//++++++++++++++++++ 実装 +++++++++++++++++
//=== モード制御機能 ===
//デバイスの現モード
volatile devmng_mode x_devmng_mode_Mode=NormalMode;
//モード制御機能初期化
void _devmng_mode_initialize(devmng_mode Mode_){
	//指定のデフォルトモードで開始
	x_devmng_mode_Mode=Mode_;
}
//モード制御機能終端化
void _devmng_mode_finalize(){
	x_devmng_mode_Mode=NormalMode;
}
//デバイスモードを変更する
void devmng_mode_set(devmng_mode Mode_){
	switch(Mode_){
	case SleepMode:
		switch(x_devmng_mode_Mode){
		case RoamingMode:
			_devmng_roaming_stop();
			break;
		default:
				break;
		}

		//?????
		x_devmng_mode_Mode=SleepMode;
		_devmng_sleep_start();

		return;
	case RoamingMode:
		switch(x_devmng_mode_Mode){
		case SleepMode:
			_devmng_sleep_stop();
			break;
		default:
			break;
		}

		//?????
		x_devmng_mode_Mode=RoamingMode;
		_devmng_roaming_start();

		return;
	default:
		switch(x_devmng_mode_Mode){
		case RoamingMode:
			_devmng_roaming_stop();
			break;
		case SleepMode:
			_devmng_sleep_stop();
			break;
                    default:
                        break;
		}

		//?????
		x_devmng_mode_Mode=NormalMode;

		return;
	}

}
//デバイスモードを取得する
volatile devmng_mode devmng_mode_get(void){return x_devmng_mode_Mode;}
//クロック変更の通知関数
void _devmng_mode_informClockChange(void){
	//スリープかつノンレム睡眠中はパスする
	if(devmng_sleep_isSleep() && !devmng_sleep_isRem())return;

	//クロックモードをただちに反映
	_devmng_clock_restartClockDefault();
}

//=== クロック制御機能 ===
//デバイスの現クロック
volatile devmng_clock x_devmng_clock_Clock;
//クロック制御機能初期化
void _devmng_clock_initialize(devmng_clock Clock_){
	//指定クロックに変更
	x_devmng_clock_Clock=Clock_;

	//ただちにクロック速度反映
	_devmng_clock_restartClockDefault();
}
//クロック制御機能終端化
void _devmng_clock_finalize(){
	hmr_clock_finalize();
}
//クロック速度の変更関数
void _devmng_clock_restartClock(devmng_clock Clock_){
	//一度クロックを終了
	hmr_clock_finalize();

	switch(Clock_){
	case LowClock:
		hmr_clock_initialize(CLOCK_SLOW);
		break;
	case HighClock:
		hmr_clock_initialize(CLOCK_FAST);
		break;
	default:
		hmr_clock_initialize(CLOCK_NORMAL);
		break;
	}

	//クロック変更をほかのモジュールにも通知
//	hmr_uart1_initialize(hmr_uart1_getModuleID(),);
//	hmr_uart2_initialize(38400);
//	hmr_adc_initialize();
}
//クロック速度をデフォルトに変更
void _devmng_clock_restartClockDefault(){
	_devmng_clock_restartClock(x_devmng_clock_Clock);
}
//クロックを変更する
void devmng_clock_set(devmng_clock Clock_){
	//すでに設定済みなら無視
	if(x_devmng_clock_Clock==Clock_)return;

	//変更を反映して、モード制御に通知
	x_devmng_clock_Clock=Clock_;
	_devmng_mode_informClockChange();
}
//クロックを取得する
volatile devmng_clock devmng_clock_get(void){return x_devmng_clock_Clock;}

//=== タスク制御 ===
#define x_devmng_task_BufSize 32
task x_devmng_task_Buf[x_devmng_task_BufSize];
taskmaster x_devmng_TaskMaster;
//タスク制御初期化
void _devmng_task_initialize(){
	//task生成
	task_construct_static(&x_devmng_TaskMaster,x_devmng_task_Buf,x_devmng_task_Buf+x_devmng_task_BufSize);
}
//タスク制御終端化
void _devmng_task_finalize(){
	//task破棄
	task_destruct(&x_devmng_TaskMaster);
}
//タスクを新規登録できる確認する
boolian devmng_task_can_start(){return task_can_start(&x_devmng_TaskMaster);}
//タスクを登録（二重登録は回避）
boolian devmng_task_start(sint16 Interval,task_function TaskFp,task_inform TaskInformStopFp){
	return task_start(&x_devmng_TaskMaster,Interval,TaskFp,TaskInformStopFp);
}
//タスクを登録（二重登録できてしまうので、危険）
boolian devmng_task_quick_start(sint16 Interval,task_function TaskFp,task_inform TaskInformStopFp){
	return task_quick_start(&x_devmng_TaskMaster,Interval,TaskFp,TaskInformStopFp);
}
//タスクを設定変更
boolian devmng_task_restart(task_function TaskFp,sint16 Interval){
	return task_restart(&x_devmng_TaskMaster,TaskFp,Interval);
}
//タスクを削除
boolian devmng_task_stop(task_function TaskFp){
	return task_stop(&x_devmng_TaskMaster,TaskFp);
}
//タスク制御処理関数
void _devmng_task_interrupt_function(sint16 Interval_){
	task_interrupt_function(&x_devmng_TaskMaster,Interval_);
}

//=== スリープモード制御 ===
//スリープ/覚醒通知用関数
typedef struct{
	devmng_vFp_v InformSleep;
	devmng_vFp_v InformWakeUp;
}devmng_sleep_InformSleepFn;
#define x_devmng_sleep_InformSleepFnBufSize 16
devmng_sleep_InformSleepFn x_devmng_sleep_InformSleepFnBuf[x_devmng_sleep_InformSleepFnBufSize];
unsigned char x_devmng_sleep_InformSleepFnBufPos=0;
//レム/ノンレム通知用関数
typedef struct{
	devmng_vFp_v InformRem;
	devmng_vFp_v InformNonRem;
}devmng_sleep_InformRemFn;
#define x_devmng_sleep_InformRemFnBufSize 16
devmng_sleep_InformRemFn x_devmng_sleep_InformRemFnBuf[x_devmng_sleep_InformRemFnBufSize];
unsigned char x_devmng_sleep_InformRemFnBufPos=0;
//スリープ中の時間カウント系
sint16 x_devmng_sleep_secNonRem=540;
sint16 x_devmng_sleep_secRem=60;
sint16 x_devmng_sleep_secCnt=0;
//スリープ状態管理用フラグ
volatile boolian x_devmng_sleep_IsSleep=0; 
volatile boolian x_devmng_sleep_IsRem=0;
boolian x_devmng_sleep_RequestEndRem=0;
//スリープモード制御初期化
void _devmng_sleep_initialize(void){
	x_devmng_sleep_InformSleepFnBufPos=0;
	x_devmng_sleep_InformRemFnBufPos=0;

	x_devmng_sleep_secNonRem=540;
	x_devmng_sleep_secRem=60;
	x_devmng_sleep_secCnt=0;
	x_devmng_sleep_IsSleep=0;
	x_devmng_sleep_IsRem=0;
	x_devmng_sleep_RequestEndRem=0;
}
//スリープモード制御終端化
void _devmng_sleep_finalize(void){
	//スリープ中なら停止
	if(x_devmng_sleep_IsSleep)_devmng_sleep_stop();

	x_devmng_sleep_InformSleepFnBufPos=0;
	x_devmng_sleep_InformRemFnBufPos=0;

	x_devmng_sleep_secNonRem=540;
	x_devmng_sleep_secRem=60;
	x_devmng_sleep_secCnt=0;
	x_devmng_sleep_IsSleep=0;
	x_devmng_sleep_IsRem=0;
}
//レムスリープ状態かどうかを取得する
volatile boolian devmng_sleep_isRem(void){return x_devmng_sleep_IsRem && x_devmng_sleep_IsSleep;}
//スリープ状態かどうかを取得する
volatile boolian devmng_sleep_isSleep(void){return x_devmng_sleep_IsSleep;}
//スリープモードのパラメータを取得する
void devmng_sleep_getInterval(sint16* secNonRem,sint16* secRem){
	*secNonRem=x_devmng_sleep_secNonRem;
	*secRem=x_devmng_sleep_secRem;
}
//スリープモードのパラメータを設定する
void devmng_sleep_setInterval(sint16 secNonRem, sint16 secRem){
	if(secNonRem<0)secNonRem=0;
	if(secRem<1)secRem=1;

	x_devmng_sleep_secNonRem=secNonRem;
	x_devmng_sleep_secRem=secRem;
}
//入眠時＆起床時に呼び出される通知関数を登録できるか
boolian devmng_sleep_can_registSleepInformFunction(){return x_devmng_sleep_InformSleepFnBufPos!=x_devmng_sleep_InformSleepFnBufSize;}
//入眠時＆起床時に呼び出される通知関数を登録する
boolian devmng_sleep_registSleepInformFunction(devmng_vFp_v InformSleep,devmng_vFp_v InformWakeUp){
	if(!devmng_sleep_can_registSleepInformFunction())return hmLib_true;
	x_devmng_sleep_InformSleepFnBuf[x_devmng_sleep_InformSleepFnBufPos].InformSleep=InformSleep;
	x_devmng_sleep_InformSleepFnBuf[x_devmng_sleep_InformSleepFnBufPos].InformWakeUp=InformWakeUp;
	++x_devmng_sleep_InformSleepFnBufPos;

	return hmLib_false;
}
//レムスリープに入る/出る際の通知関数を登録できるか
boolian devmng_sleep_can_registRemInformFunction(){return x_devmng_sleep_InformRemFnBufPos!=x_devmng_sleep_InformRemFnBufSize;}
//レムスリープに入る/出る際の通知関数を登録する
boolian devmng_sleep_registRemInformFunction(devmng_vFp_v InformRem,devmng_vFp_v InformNonRem){
	if(!devmng_sleep_can_registRemInformFunction())return hmLib_true;
	x_devmng_sleep_InformRemFnBuf[x_devmng_sleep_InformRemFnBufPos].InformRem=InformRem;
	x_devmng_sleep_InformRemFnBuf[x_devmng_sleep_InformRemFnBufPos].InformNonRem=InformNonRem;
	++x_devmng_sleep_InformRemFnBufPos;

	return hmLib_false;
}
//スリープへ入る処理
void _devmng_sleep_start(){
	devmng_sleep_InformSleepFn* Itr;

	//すでにスリープ中なら終了
	if(x_devmng_sleep_IsSleep)return;

	//カウンターをリセット
	x_devmng_sleep_secCnt=0;
	x_devmng_sleep_IsRem=0;

	//スリープ状態に変更
	x_devmng_sleep_IsSleep=1;

	// 割り込み禁止
	_devmng_sleep_disable_interrupt();

	//Inform関数
	for(Itr=x_devmng_sleep_InformSleepFnBuf;Itr!=x_devmng_sleep_InformSleepFnBuf+x_devmng_sleep_InformSleepFnBufSize;++Itr){
		//if(x_devmng_sleep_InformSleepFnBuf->InformSleep)x_devmng_sleep_InformSleepFnBuf->InformSleep();
		if(Itr->InformSleep)Itr->InformSleep();	
	}

	//電源を落とす
	_devmng_power(0);

	//クロック速度を落とす
	_devmng_clock_restartClock(LowClock);
}
//スリープから出る処理
void _devmng_sleep_stop(void){
	devmng_sleep_InformSleepFn* Itr;

	//すでに非スリープ中なら終了
	if(!x_devmng_sleep_IsSleep)return;

	//クロック速度をデフォルトに戻す
	_devmng_clock_restartClockDefault();

	//電源を戻す すでにレム睡眠なのでカット
	//_devmng_power(1);

	//スリープ状態を解除
	x_devmng_sleep_IsSleep=0;

	//Inform関数
	for(Itr=x_devmng_sleep_InformSleepFnBuf;Itr!=x_devmng_sleep_InformSleepFnBuf+x_devmng_sleep_InformSleepFnBufSize;++Itr){
		//if(x_devmng_sleep_InformSleepFnBuf->InformWakeUp)x_devmng_sleep_InformSleepFnBuf->InformWakeUp();
		if(Itr->InformWakeUp)Itr->InformWakeUp();
	}

	// 割り込み許可
	_devmng_sleep_enable_interrupt();

}
// スリープ時の割り込み禁止
void _devmng_sleep_disable_interrupt(){
	// Main stream の割り込み禁止
	streamVMC_disable_fget_interrupt();
	//streamVMC_disable_fput_interrupt();
	// Camera stream の割り込み禁止
	streamCMR_disable_fget_interrupt();
	//streamCMR_disable_fput_interrupt();
	// カメラ のTimer 割り込み禁止
	timerCMR_disable_interrupt();
}
// スリープ時の割り込み復帰
void _devmng_sleep_enable_interrupt(){
	// Main stream の受信割り込み開始
	streamVMC_enable_fget_interrupt();
	// Camera stream の受信割り込み開始
	streamCMR_enable_fget_interrupt();
	// カメラ のTimer 割り込み開始
	timerCMR_enable_interrupt();
}
//スリープ制御処理割込関数
void _devmng_sleep_interrupt_function(sint16 Interval_){
	devmng_sleep_InformRemFn* Itr;

	//SleepCntを加算
	x_devmng_sleep_secCnt+=Interval_;

	//レム状態のとき 
	if(x_devmng_sleep_IsRem){
		//割り込みで急に電源を落とすのは危険なので、リクエストだけ送っておく(実際の終了は、work関数に委譲)
		if(x_devmng_sleep_secCnt>=x_devmng_sleep_secRem)x_devmng_sleep_RequestEndRem=1;
	}//ノンレム状態のとき
	else{
		if(x_devmng_sleep_secCnt>=x_devmng_sleep_secNonRem){
			//カウンターを初期化
			x_devmng_sleep_secCnt=0;
			
			//クロック速度をデフォルトに戻す
			_devmng_clock_restartClockDefault();

			//電源を入れる
			_devmng_power(1);
			// 安定待ち
			exclusive_delay_ms(300);
			HMR_PIN_RedLED_set(1);
			exclusive_delay_ms(300);
			HMR_PIN_RedLED_set(0);
			exclusive_delay_ms(300);
			HMR_PIN_RedLED_set(1);
			exclusive_delay_ms(300);
			HMR_PIN_RedLED_set(0);
			exclusive_delay_ms(300);
			HMR_PIN_RedLED_set(1);

			//レム状態に移行
			x_devmng_sleep_IsRem=1;

			//Inform関数
			for(Itr=x_devmng_sleep_InformRemFnBuf;Itr!=(x_devmng_sleep_InformRemFnBuf+x_devmng_sleep_InformRemFnBufSize);++Itr){
				//if(x_devmng_sleep_InformRemFnBuf->InformRem)x_devmng_sleep_InformRemFnBuf->InformRem();
				if(Itr->InformRem)Itr->InformRem();
			}

			// 割り込み許可
			_devmng_sleep_enable_interrupt();

		}
	}
}
//スリープ制御処理通常関数
void _devmng_sleep_interrupt_yield(void){
	devmng_sleep_InformRemFn* Itr;

	//レム睡眠終了要求関数
	if(x_devmng_sleep_RequestEndRem){
		//フラグを下げる
		x_devmng_sleep_RequestEndRem=0;
		//カウンターを初期化
		x_devmng_sleep_secCnt=0;

		// 割り込みを禁止
		_devmng_sleep_disable_interrupt();

		//ノンレム状態に移行
		x_devmng_sleep_IsRem=0;

		//Inform関数
		for(Itr=x_devmng_sleep_InformRemFnBuf;Itr!=x_devmng_sleep_InformRemFnBuf+x_devmng_sleep_InformRemFnBufSize;++Itr){
			//if(x_devmng_sleep_InformRemFnBuf->InformNonRem)x_devmng_sleep_InformRemFnBuf->InformNonRem();
			if(Itr->InformNonRem)Itr->InformNonRem();
		}

		//クロック速度を落とす
		_devmng_clock_restartClock(LowClock);

		//電源を落とす
		_devmng_power(0);
	}
}
//=== ローミングモード制御 ===
//ローミングの間隔制御変数
sint16 x_devmng_roaming_secInterval=0;
sint16 x_devmng_roaming_secCnt=0;	
//ローミングの状態変数
volatile boolian x_devmng_roaming_IsRoaming=0;
//ローミングモード制御初期化
void _devmng_roaming_initialize(void){
	x_devmng_roaming_secInterval=0;
	x_devmng_roaming_secCnt=0;
	x_devmng_roaming_IsRoaming=0;
}
//ローミングモード制御終端化
void _devmng_roaming_finalize(void){
	//ローミング中なら、停止
	if(x_devmng_roaming_IsRoaming)_devmng_roaming_stop();

	x_devmng_roaming_secInterval=0;
	x_devmng_roaming_secCnt=0;
	x_devmng_roaming_IsRoaming=0;
}
//ローミングモードのパラメータを取得する
void devmng_roaming_getInterval(sint16* secInterval){
	*secInterval=x_devmng_roaming_secInterval;
}
//ローミングモードのパラメータを設定する
void devmng_roaming_setInterval(sint16 secInterval){
	//ローミング間隔は10秒以上である必要
	if(secInterval<=10)secInterval=10;
	x_devmng_roaming_secInterval=secInterval;
}
//ローミング状態かどうかを取得する
volatile boolian _devmng_roaming_isRoaming(void){return x_devmng_roaming_IsRoaming;}
//ローミング状態に入る
void _devmng_roaming_start(void){
	//すでにローミング中なら無視
	if(x_devmng_roaming_IsRoaming)return;

	//カウンタリセット
	x_devmng_roaming_secCnt=0;

	//ローミング状態へ移行
	x_devmng_roaming_IsRoaming=1;

	//通信デバイス切り替え
	if(hmr_uart1_getModuleID()==UART1_MODULEID_RF){
		HMR_PIN_RedLED_set(1);
		hmr_uart1_initialize(UART1_MODULEID_MP,devmng_MP_BaudRate);
	}else{
		HMR_PIN_RedLED_set(0);
		if(Config_RFDebugMode)hmr_uart1_initialize(UART1_MODULEID_RF,devmng_RFDebug_BaudRate);
		else hmr_uart1_initialize(UART1_MODULEID_RF,devmng_RF_BaudRate);
	}
}
//ローミング状態を解除
void _devmng_roaming_stop(void){
	//すでにローミング中でないなら無視
	if(!x_devmng_roaming_IsRoaming)return;

	//ローミング状態を解除
	x_devmng_roaming_IsRoaming=0;
}
//ローミング制御処理関数
void _devmng_roaming_interrupt_function(sint16 Interval_){
	//カウンターを進める
	x_devmng_roaming_secCnt+=Interval_;
	
	//カウンターが切り替え値を超えた場合は、通信デバイス切り替え
	if(x_devmng_roaming_secCnt>x_devmng_roaming_secInterval){
		if(hmr_uart1_getModuleID()==UART1_MODULEID_RF){
			HMR_PIN_RedLED_set(1);
			hmr_uart1_initialize(UART1_MODULEID_MP,devmng_MP_BaudRate);
		}else{
			HMR_PIN_RedLED_set(0);
			if(Config_RFDebugMode)hmr_uart1_initialize(UART1_MODULEID_RF,devmng_RFDebug_BaudRate);
			else hmr_uart1_initialize(UART1_MODULEID_RF,devmng_RF_BaudRate);
		}
		x_devmng_roaming_secCnt=0;
	}
}

//=== 基本操作関数 ===
//デバイス初期化関数
void devmng_initialize(void){
	//デバイス初期化
	hmr_device_initialize();

	//Pic自体のパワーを入れておく
	hmr_device_power(1);

	//デバイスの電源を落とす
	_devmng_power(0);

	//モードシステム初期化
	_devmng_mode_initialize(NormalMode);

	//クロック初期化
	if(Config_DefaultHighClock)_devmng_clock_initialize(HighClock);
	else _devmng_clock_initialize(NormalClock);

	//割り込み初期化
	hmr_interrupt_initalize();

	///通信系モジュール初期化
	if(Config_DefaultMobilePhone){
		hmr_uart1_initialize(UART1_MODULEID_MP,devmng_MP_BaudRate);
		com_wdt_restart();
		com_wdt_enable();
	}else{
		if(Config_RFDebugMode)hmr_uart1_initialize(UART1_MODULEID_RF,devmng_RFDebug_BaudRate);
		else hmr_uart1_initialize(UART1_MODULEID_RF,devmng_RF_BaudRate);
		com_wdt_disable();
	}

	hmr_uart2_initialize(38400);
	hmr_spi2_initialize();
	hmr_i2c_initialize();
	hmr_adc_initialize();

	//デバイスの電源を入れる
	delay_ms(500);
	_devmng_power(1);
	
	//devmngモジュール初期化
	_devmng_sleep_initialize();
	_devmng_roaming_initialize();
	_devmng_task_initialize();
	_devmng_interrupt_initialize();

	//WatchDogTimer開始
	if(Config_DisableWatchDogTimer)disable_wdt();
	else enable_wdt();
}
//デバイス終端化関数
void devmng_finalize(void){
	//WatchDogTimer終了
	disable_wdt();

	//devmngモジュール終端化
	_devmng_task_finalize();
	_devmng_sleep_finalize();
	_devmng_roaming_finalize();
	_devmng_interrupt_finalize();

	//通信系モジュール終端化
	hmr_uart1_finalize();
	hmr_uart2_finalize();
	hmr_spi2_finalize();
	hmr_i2c_finalize();
	hmr_adc_finalize();

	//デバイスの電源を落とす
	_devmng_power(0);

	//割り込み終了
	hmr_interrupt_finalize();

	//クロック終了
	_devmng_clock_finalize();

	//モード管理終了
	_devmng_mode_finalize();

	//デバイス自体を終了
	hmr_device_finalize();
}
//デバイス強制終了
void devmng_kill(void){
	//自身の電源を切る
	hmr_device_power(0);
}
//デバイスの電源管理関数
void _devmng_power(boolian onoff){
	//電源オフの場合は、モジュール側も切る
	if(!onoff){
		sprite_power(0);
		sprite_powerLight(0);
		co2_powerPump(0);
		co2_powerSensor(0);
		h2s_powerSensor(0);
		spi_selectGPS(0);
		spi_selectDrive(0);
	}

	hmr_uart1_power_switch(onoff,hmr_uart1_getModuleID());
	subPIC_power(onoff);
	i2c_power(onoff);
	adc_power(onoff);
}
//現在の通信先が全二重通信対応かを返す
boolian devmng_isFullDuplex(void){
	return hmr_uart1_getModuleID()==UART1_MODULEID_MP || Config_RFDebugMode;
}

//=== 割り込み関数 ===
//タスク、スリープ、およびローミング実行用関数
void devmng_interrupt_function(sint16 Interval_){
	//モードによって割り込み処理を分岐
	switch(devmng_mode_get()){
	case SleepMode:
		_devmng_sleep_interrupt_function(Interval_);
		break;
	case RoamingMode:
		_devmng_roaming_interrupt_function(Interval_);
		break;
	default:
		_devmng_task_interrupt_function(Interval_);
		break;
	}
}
//割り込みのmainループへの委譲処理関数
boolian devmng_interrupt_yield(void){
	_devmng_sleep_interrupt_yield();

	//スリープを除外
	if(devmng_sleep_isSleep() && !devmng_sleep_isRem()){
		//__asm__("PWRSAV #IDLE_MODE");
		Idle();
		//Sleep();
		return hmLib_true;
	}

	return hmLib_false;
}

//=== 割り込み制御用関数
// ini fin 関数
void _devmng_interrupt_initialize(){}
void _devmng_interrupt_finalize(){}

// main stream 割り込みの許可禁止をいじくってよいかを返す関数
boolian devmng_interrupt_can_enable_streamVMC_fget_interrupt(){
	return !( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_disable_streamVMC_fget_interrupt(){
	return hmLib_true;//!( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_enable_streamVMC_fput_interrupt(){
	return !( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_disable_streamVMC_fput_interrupt(){
	return hmLib_true;//!( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}

// camera stream 割り込みの許可禁止をいじくってよいかを返す関数
boolian devmng_interrupt_can_enable_streamCMR_fget_interrupt(){
	return !( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_disable_streamCMR_fget_interrupt(){
	return hmLib_true;//!( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_enable_streamCMR_fput_interrupt(){
	return !( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_disable_streamCMR_fput_interrupt(){
	return hmLib_true;//!( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}

// camera timer  割り込みの許可禁止をいじくってよいかを返す関数
boolian devmng_interrupt_can_enable_timerCMR_interrupt(){
	return !( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
boolian devmng_interrupt_can_disable_timerCMR_interrupt(){
	return hmLib_true;//!( devmng_sleep_isSleep() && !devmng_sleep_isRem() );
}
// DevMng timer の割り込みの許可禁止をいじくってよいかを返す関数
boolian devmng_interrupt_can_enable_timerDevmng_interrupt(){
	return hmLib_true;
}
boolian devmng_interrupt_can_disable_timerDevmng_interrupt(){
	return hmLib_true;
}

// main stream の割り込み許可禁止関数
boolian devmng_interrupt_enable_streamVMC_fget_interrupt(){
	if(devmng_interrupt_can_enable_streamVMC_fget_interrupt()){
		streamVMC_enable_fget_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_disable_streamVMC_fget_interrupt(){
	if(devmng_interrupt_can_disable_streamVMC_fget_interrupt()){
		streamVMC_disable_fget_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_enable_streamVMC_fput_interrupt(){
	if(devmng_interrupt_can_enable_streamVMC_fput_interrupt()){
		streamVMC_enable_fput_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_disable_streamVMC_fput_interrupt(){
	if(devmng_interrupt_can_disable_streamVMC_fput_interrupt()){
		streamVMC_disable_fput_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}

// camera stream の割り込み許可禁止関数
boolian devmng_interrupt_enable_streamCMR_fget_interrupt(){
	if(devmng_interrupt_can_enable_streamCMR_fget_interrupt()){
		streamCMR_enable_fget_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_disable_streamCMR_fget_interrupt(){
	if(devmng_interrupt_can_disable_streamCMR_fget_interrupt()){
		streamCMR_disable_fget_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_enable_streamCMR_fput_interrupt(){
	if(devmng_interrupt_can_enable_streamCMR_fput_interrupt()){
		streamCMR_enable_fput_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_disable_streamCMR_fput_interrupt(){
	if(devmng_interrupt_can_disable_streamCMR_fput_interrupt()){
		streamCMR_disable_fput_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}

// timer camera の割り込み許可禁止関数
boolian devmng_interrupt_enable_timerCMR_interrupt(){
	if(devmng_interrupt_can_enable_timerCMR_interrupt()){
		timerCMR_enable_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_disable_timerCMR_interrupt(){
	if(devmng_interrupt_can_disable_timerCMR_interrupt()){
		timerCMR_disable_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}

// timer Device manager の割り込み許可禁止関数
boolian devmng_interrupt_enable_timerDevmng_interrupt(){
	if(devmng_interrupt_can_enable_timerDevmng_interrupt()){
		timerDevmng_enable_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}
boolian devmng_interrupt_disable_timerDevmng_interrupt(){
	if(devmng_interrupt_can_disable_timerDevmng_interrupt()){
		timerDevmng_disable_interrupt();
		return hmLib_true;
	}else{
		return hmLib_false;
	}
}

#
#endif
