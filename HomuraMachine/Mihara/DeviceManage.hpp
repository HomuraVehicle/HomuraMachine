#ifndef HMR_MACHINE_MIHARA_DEVICEMANAGE_INC
#define HMR_MACHINE_MIHARA_DEVICEMANAGE_INC 200
#
#include<homuraLib_v2/type.hpp>
/*
Device制御用の機能を提供する
	Kill機能
		自分自身とすべてのの電源を切る
	スリープ機能
		定期的に電源をオン/オフを繰り返す
	ローミング機能
		Roamingモードに入ると、RF(RoamingTime)=>MB(RoamingTime)=>RFのように、使用通信機の遷移を繰り返す。
		RoamingTimeは内部変数で決定する（コマンドで設定可能）
	クロックモード変更機能
		設定クロック速度を変更する
=== hmr::machine::mihara::DeviceManage ===
v2_00/141117 hmito
	C++化
===hmrDeviceManage===
v1_03/131026 amby
	割り込み許可関数軍を追加、このクラスで管理する
v1_02/131019 hmIto
	clockmodeをclockに変更
	isSleep, isRoamingを追加
v1_01/131013 hmIto
	インターフェースを大幅に変更＆整理
		モードからクロック速度指定を除外
*/
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace devmng {
				typedef void(*vFp_v)(void);

				//=== 基本定義 ===
				#define RF_BaudRate 9600
				#define RFDebug_BaudRate 38400
				#define MP_BaudRate 38400

				//=== 基本操作関数 ===
				//デバイス初期化関数
				void initialize(void);
				//デバイス終端化関数
				void finalize(void);
				//デバイス強制終了
				void kill(void);
				//現在の通信先が全二重通信対応かを返す
				bool isFullDuplex(void);
				//clear_wdt
				void clear_wdt(void);
				//ModuleID
				typedef enum  {
					null=0x00,
					mobile_phone=0x01,
					rf_module=0x02
				}rf_module_mode;
				//現在のModuleIDを取得する
				rf_module_mode rf_module_uart_getModuleID(void);


				//=== モード制御機能 ===
				typedef enum{ NormalMode, SleepMode, RoamingMode }mode;
				//デバイスモードを変更する
				void mode_set(mode Mode_);
				//デバイスモードを取得する
				volatile mode mode_get(void);

				//=== クロック制御機能 ===
				typedef enum{ LowClock, NormalClock, HighClock }clock;
				//クロックを変更する
				void clock_set(clock Clock_);
				//クロックを取得する
				volatile clock clock_get(void);

				//=== スリープモード制御 ===
				//スリープモードのパラメータを取得する
				void sleep_getInterval(sint16* secNonRem, sint16* secRem);
				//スリープモードのパラメータを設定する
				void sleep_setInterval(sint16 secNonRem, sint16 secRem);
				//入眠時＆起床時に呼び出される通知関数を登録できるか
				bool sleep_can_registSleepInformFunction();
				//入眠時＆起床時に呼び出される通知関数を登録する
				bool sleep_registSleepInformFunction(vFp_v InformSleep, vFp_v InformWakeUp);
				//レムスリープに入る/出る際の通知関数を登録できるか
				bool sleep_can_registRemInformFunction();
				//レムスリープに入る/出る際の通知関数を登録する
				bool sleep_registRemInformFunction(vFp_v InformRem, vFp_v InformNonRem);
				//スリープ状態かどうかを取得する
				volatile bool sleep_isSleep(void);
				//レムスリープ状態かどうかを取得する
				volatile bool sleep_isRem(void);

				//=== ローミングモード制御 ===
				//ローミングモードのパラメータを取得する
				void roaming_getInterval(sint16* secInterval);
				//ローミングモードのパラメータを設定する
				void roaming_setInterval(sint16 secInterval);
				//ローミング状態かどうかを取得する
				volatile bool roaming_isRoaming(void);

				//=== 割り込み関数 ===
				//タスク、スリープ、およびローミング実行用関数
				void interrupt_function(sint16 Interval_);
				//割り込みのmainループへの委譲処理関数
				bool interrupt_yield(void);

				//=== 割り込み制御用関数
				// 割り込み許可変えてよいかどうかのチェック関数　冗長過ぎたかも・・
				bool interrupt_can_enable_streamVMC_fget_interrupt(void);
				bool interrupt_can_disable_streamVMC_fget_interrupt(void);
				bool interrupt_can_enable_streamVMC_fput_interrupt(void);
				bool interrupt_can_disable_streamVMC_fput_interrupt(void);

/*				bool interrupt_can_enable_streamCMR_fget_interrupt(void);
				bool interrupt_can_disable_streamCMR_fget_interrupt(void);
				bool interrupt_can_enable_streamCMR_fput_interrupt(void);
				bool interrupt_can_disable_streamCMR_fput_interrupt(void);

				bool interrupt_can_enable_timerCMR_interrupt(void);
				bool interrupt_can_disable_timerCMR_interrupt(void);
*/
				bool interrupt_can_enable_timerDevmng_interrupt(void);
				bool interrupt_can_disable_timerDevmng_interrupt(void);

				// main stream 送受信割り込み許可関数
				bool interrupt_enable_streamVMC_fget_interrupt(void);
				bool interrupt_disable_streamVMC_fget_interrupt(void);
				bool interrupt_enable_streamVMC_fput_interrupt(void);
				bool interrupt_disable_streamVMC_fput_interrupt(void);

/*				// camera stream 送受信割り込み許可関数
				bool interrupt_enable_streamCMR_fget_interrupt(void);
				bool interrupt_disable_streamCMR_fget_interrupt(void);
				bool interrupt_enable_streamCMR_fput_interrupt(void);
				bool interrupt_disable_streamCMR_fput_interrupt(void);
				
				// timer CMR　送受信割り込み許可関数
				bool interrupt_enable_timerCMR_interrupt(void);
				bool interrupt_disable_timerCMR_interrupt(void);
				*/

				// timer Device Manager 送受信割り込み許可関数
				bool interrupt_enable_timerDevmng_interrupt(void);
				bool interrupt_disable_timerDevmng_interrupt(void);

			}
		}
	}
}

#
#endif
