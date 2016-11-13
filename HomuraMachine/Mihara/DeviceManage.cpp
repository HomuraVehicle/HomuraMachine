#ifndef HMR_DEVICEMANAGE_C_INC
#define HMR_DEVICEMANAGE_C_INC 200
#
#include<XC32/i2c.hpp>
#include<XC32/spi.hpp>
#include<XC32/uart.hpp>
#include<homuraLib_v2/machine/service/delay.hpp>
#include<homuraLib_v2/machine/service/exclusive_delay.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Device.hpp"
#include"DeviceManage.hpp"
#include"DeviceManage_forInertial.hpp"
#include"DeviceManage_forCamera.hpp"
#include"DeviceManage_forCO2.hpp"
#include"DeviceManage_forSDcard.hpp"
#include"DeviceManage_forCourier.hpp"
#include"Com.hpp"

namespace hmr {
	namespace machine {
		namespace service{
			namespace{
				//関数駆動型のタスクホスト
				hmr::task::functional_host<> TaskHost;
			}
			//serviceのタスクとして登録
			hmr::task::host_interface& Task(TaskHost);
		}
		namespace mihara {
			namespace devmng {
				class device :public mihara::cDevice::devmng_device {
				public:
					pinRedLED PinRedLED;
					pinDevicePower PinDevicePower;
					timer_register Task_timer_register;
					xc32::interrupt_timer<timer_register> Task_timer;
					static const uint16 TaskTimerMS=1000;//DevMng専用タイマーの初期化値(milisecond)

					static const uint8 TaskTimerIPL=2;//DevMng専用タイマーの割り込み優先度
					static const uint8 Camera1_uart_tx_IPL=6;
					static const uint8 Camera1_uart_rx_IPL=7;
					static const uint8 Camera2_uart_tx_IPL=6;
					static const uint8 Camera2_uart_rx_IPL=7;
					static const uint8 Camera_timer_IPL=2;
					static const uint8 RF_uart_tx_IPL=3;
					static const uint8 RF_uart_rx_IPL=4;
					static const uint8 MobilePhone_uart_tx_IPL=3;
					static const uint8 MobilePhone_uart_rx_IPL=4;
					static const uint8 Main_IPL=1;
					static const uint8 Lock_level_IPL=5;
					static const uint8 StrongLock_level_IPL=7;

//=================================================================
					xc32::wdt WDT;
//					xc::lock_guard<xc32::wdt> WDTLock;

					//=====DIP pin=====
					pinDip1 Config_DisableWDT;
					pinDip2 Config_DefaultMP;
					pinDip3 Config_DefaultHighClock;
					pinDip4 Config_RFDebugMode;

					//====rf module====
					pinRF_Power PinRFPower;
					pinMobilePhone_Power PinMobilePhonePower;
					rf_uart_register RF_uart_register;
					xc32::interrupt_uart<rf_uart_register> RF_uart;
					static const uint16 RF_uart_Baudrate=9600;
					static const xc32::uart::flowcontrol::mode RF_uart_flowctrl=xc32::uart::flowcontrol::mode::rts_cts_control;
					mobilephone_uart_register MobilePhone_uart_register;
					xc32::interrupt_uart<mobilephone_uart_register> MobilePhone_uart;
					static const uint16 MobilePhone_uart_Baudrate=38400;
					static const xc32::uart::flowcontrol::mode MobilePhone_uart_flowctrl=xc32::uart::flowcontrol::mode::rts_cts_control;

					//=====SD card=====
					pinSDPower PinSDPower;
					pinSD_SPISelect PinSD_SPISelect;
					sdcard_spi_register SDcard_spi_register;
					xc32::synchronous_spi<sdcard_spi_register> SDcard_spi;

					//====ADC====
					pinADC0Power PinADC0Power;
					pinADC1to4Power PinADC1to4Power;

					//====Motor====
					pinMotorLA PinMotorLA;
					pinMotorLB PinMotorLB;
					pinMotorRA PinMotorRA;
					pinMotorRB PinMotorRB;
					pinMotorPower PinMotorPower;

					pin5VDCDC Pin5VDCDC;
					pinExternalI2C_Pw PinExtI2CPower;
//=================================================================
				}Device;

				//++++++++++++++++++ protected宣言 +++++++++++++++++
				//=== 基本関数 ===
				//デバイスの電源管理
				void power(bool onoff);

				//=== モード制御機能 ===
				//モード制御機能初期化
				void mode_initialize(mode Mode_);
				//モード制御機能終端化
				void mode_finalize();
				//クロック変更の通知関数
				void mode_informClockChange(void);

				//=== クロック制御機能 ===
				//クロック制御機能初期化
				void clock_initialize(clock Clock_);
				//クロック制御機能終端化
				void clock_finalize();
				//クロック速度の変更関数
				void clock_restartClock(clock Clock_);
				//クロック速度をデフォルトに変更
				void clock_restartClockDefault();

				//=== スリープモード制御 ===
				//スリープモード制御初期化
				void sleep_initialize(void);
				//スリープモード制御終端化
				void sleep_finalize(void);
				//スリープに入る処理
				void sleep_start();
				//スリープから出る処理
				void sleep_stop(void);
				//スリープ制御処理関数
				void sleep_interrupt_function(sint16 Interval_);
				//スリープ時の割り込み禁止関数
				void sleep_disable_interrupt(void);
				//スリープ時の割り込み復帰関数
				void sleep_enable_interrupt(void);


				//=== ローミングモード制御 ===
				//ローミングモード制御初期化
				void roaming_initialize(void);
				//ローミングモード制御終端化
				void roaming_finalize(void);
				//ローミングに入る処理
				void roaming_start(void);
				//ローミングから出る処理
				void roaming_stop(void);
				//ローミング制御処理関数
				void roaming_interrupt_function(sint16 Interval_);

				//=== RF module uart select ===
				//ModuleIDのモジュールで初期化する
				void rf_module_uart_initialize(rf_module_mode ModuleID_, uint16 BaudRate_);
				//ModuleIDに電源を変更する
				void rf_module_uart_power_switch(bool onoff, rf_module_mode ModuleID_);
				//現在のModuleIDを取得する
				rf_module_mode rf_module_uart_getModuleID(void);

				//=== 割り込み関数制御 ===
				void interrupt_initialize(void);
				void interrupt_finalize(void);

				//++++++++++++++++++ 実装 +++++++++++++++++
				//=== モード制御機能 ===
				//デバイスの現モード
				volatile mode mode_Mode=NormalMode;

				//モード制御機能初期化
				void mode_initialize(mode Mode_) {
					//指定のデフォルトモードで開始
					mode_Mode=Mode_;
				}
				//モード制御機能終端化
				void mode_finalize() {
					mode_Mode=NormalMode;
				}
				//デバイスモードを変更する
				void mode_set(mode Mode_) {
					switch(Mode_) {
					case SleepMode:
						switch(mode_Mode) {
						case RoamingMode:
							roaming_stop();
							break;
						default:
							break;
						}

						//?????
						mode_Mode=SleepMode;
						sleep_start();

						return;
					case RoamingMode:
						switch(mode_Mode) {
						case SleepMode:
							sleep_stop();
							break;
						default:
							break;
						}

						//?????
						mode_Mode=RoamingMode;
						roaming_start();

						return;
					default:
						switch(mode_Mode) {
						case RoamingMode:
							roaming_stop();
							break;
						case SleepMode:
							sleep_stop();
							break;
						default:
							break;
						}

						//?????
						mode_Mode=NormalMode;

						return;
					}

				}
				//デバイスモードを取得する
				volatile mode mode_get(void) { return mode_Mode; }
				//クロック変更の通知関数
				void mode_informClockChange(void) {
					//スリープかつノンレム睡眠中はパスする
					if(sleep_isSleep() && !sleep_isRem())return;

					//クロックモードをただちに反映
					clock_restartClockDefault();
				}
				//=== クロック制御機能 ===
				//デバイスの現クロック
				volatile clock clock_Clock;
				//クロック制御機能初期化
				void clock_initialize(clock Clock_) {
					//指定クロックに変更
					clock_Clock=Clock_;

					//ただちにクロック速度反映
					clock_restartClockDefault();
				}
				//クロック制御機能終端化
				void clock_finalize() {
//					hmr_clock_finalize();
				}
				//クロック速度の変更関数
				void clock_restartClock(clock Clock_) {
					
					//一度クロックを終了
//					hmr_clock_finalize();

					switch(Clock_) {
					case LowClock:
//						hmr_clock_initialize(CLOCK_SLOW);
						break;
					case HighClock:
//						hmr_clock_initialize(CLOCK_FAST);
						break;
					default:
//						hmr_clock_initialize(CLOCK_NORMAL);
						break;
					}

					//クロック変更をほかのモジュールにも通知
					//	hmr_uart1_initialize(hmr_uart1_getModuleID(),);
					//	hmr_uart2_initialize(38400);
					//	hmr_adc_initialize();
					
				}
				//クロック速度をデフォルトに変更
				void clock_restartClockDefault() {
					clock_restartClock(clock_Clock);
				}
				//クロックを変更する
				void clock_set(clock Clock_) {
					//すでに設定済みなら無視
					if(clock_Clock==Clock_)return;

					//変更を反映して、モード制御に通知
					clock_Clock=Clock_;
					mode_informClockChange();
				}
				//クロックを取得する
				volatile clock clock_get(void) { return clock_Clock; }

				//=== スリープモード制御 ===
				//スリープ/覚醒通知用関数
				typedef struct {
					vFp_v InformSleep;
					vFp_v InformWakeUp;
				}sleep_InformSleepFn;
				const unsigned int sleep_InformSleepFnBufSize=16;
				sleep_InformSleepFn sleep_InformSleepFnBuf[sleep_InformSleepFnBufSize];
				unsigned char sleep_InformSleepFnBufPos=0;
				//レム/ノンレム通知用関数
				typedef struct {
					vFp_v InformRem;
					vFp_v InformNonRem;
				}sleep_InformRemFn;
				const unsigned int sleep_InformRemFnBufSize=16;
				sleep_InformRemFn sleep_InformRemFnBuf[sleep_InformRemFnBufSize];
				unsigned char sleep_InformRemFnBufPos=0;
				//スリープ中の時間カウント系
				sint16 sleep_secNonRem=540;
				sint16 sleep_secRem=60;
				sint16 sleep_secCnt=0;
				//スリープ状態管理用フラグ
				volatile bool sleep_IsSleep=0;
				volatile bool sleep_IsRem=0;
				bool sleep_RequestEndRem=0;
				//スリープモード制御初期化
				void sleep_initialize(void) {
					sleep_InformSleepFnBufPos=0;
					sleep_InformRemFnBufPos=0;

					sleep_secNonRem=540;
					sleep_secRem=60;
					sleep_secCnt=0;
					sleep_IsSleep=0;
					sleep_IsRem=0;
					sleep_RequestEndRem=0;
				}
				//スリープモード制御終端化
				void sleep_finalize(void) {
					//スリープ中なら停止
					if(sleep_IsSleep)sleep_stop();

					sleep_InformSleepFnBufPos=0;
					sleep_InformRemFnBufPos=0;

					sleep_secNonRem=540;
					sleep_secRem=60;
					sleep_secCnt=0;
					sleep_IsSleep=0;
					sleep_IsRem=0;
				}
				//レムスリープ状態かどうかを取得する
				volatile bool sleep_isRem(void) { return sleep_IsRem && sleep_IsSleep; }
				//スリープ状態かどうかを取得する
				volatile bool sleep_isSleep(void) { return sleep_IsSleep; }
				//スリープモードのパラメータを取得する
				void sleep_getInterval(sint16* secNonRem, sint16* secRem) {
					*secNonRem=sleep_secNonRem;
					*secRem=sleep_secRem;
				}
				//スリープモードのパラメータを設定する
				void sleep_setInterval(sint16 secNonRem, sint16 secRem) {
					if(secNonRem<0)secNonRem=0;
					if(secRem<1)secRem=1;

					sleep_secNonRem=secNonRem;
					sleep_secRem=secRem;
				}
				//入眠時＆起床時に呼び出される通知関数を登録できるか
				bool sleep_can_registSleepInformFunction() { return sleep_InformSleepFnBufPos!=sleep_InformSleepFnBufSize; }
				//入眠時＆起床時に呼び出される通知関数を登録する
				bool sleep_registSleepInformFunction(vFp_v InformSleep, vFp_v InformWakeUp) {
					if(!sleep_can_registSleepInformFunction())return true;
					sleep_InformSleepFnBuf[sleep_InformSleepFnBufPos].InformSleep=InformSleep;
					sleep_InformSleepFnBuf[sleep_InformSleepFnBufPos].InformWakeUp=InformWakeUp;
					++sleep_InformSleepFnBufPos;

					return false;
				}
				//レムスリープに入る/出る際の通知関数を登録できるか
				bool sleep_can_registRemInformFunction() { return sleep_InformRemFnBufPos!=sleep_InformRemFnBufSize; }
				//レムスリープに入る/出る際の通知関数を登録する
				bool sleep_registRemInformFunction(vFp_v InformRem, vFp_v InformNonRem) {
					if(!sleep_can_registRemInformFunction())return true;
					sleep_InformRemFnBuf[sleep_InformRemFnBufPos].InformRem=InformRem;
					sleep_InformRemFnBuf[sleep_InformRemFnBufPos].InformNonRem=InformNonRem;
					++sleep_InformRemFnBufPos;

					return false;
				}
				//スリープへ入る処理
				void sleep_start() {
					sleep_InformSleepFn* Itr;

					//すでにスリープ中なら終了
					if(sleep_IsSleep)return;

					//カウンターをリセット
					sleep_secCnt=0;
					sleep_IsRem=0;

					//スリープ状態に変更
					sleep_IsSleep=1;

					// 割り込み禁止
					sleep_disable_interrupt();

					//Inform関数
					for(Itr=sleep_InformSleepFnBuf; Itr!=sleep_InformSleepFnBuf+sleep_InformSleepFnBufSize; ++Itr) {
						//if(sleep_InformSleepFnBuf->InformSleep)sleep_InformSleepFnBuf->InformSleep();
						if(Itr->InformSleep)Itr->InformSleep();
					}

					//電源を落とす
					power(0);

					//クロック速度を落とす
					clock_restartClock(LowClock);
				}
				//スリープから出る処理
				void sleep_stop(void) {
					sleep_InformSleepFn* Itr;

					//すでに非スリープ中なら終了
					if(!sleep_IsSleep)return;

					//クロック速度をデフォルトに戻す
					clock_restartClockDefault();

					//電源を戻す すでにレム睡眠なのでカット
					//power(1);

					//スリープ状態を解除
					sleep_IsSleep=0;

					//Inform関数
					for(Itr=sleep_InformSleepFnBuf; Itr!=sleep_InformSleepFnBuf+sleep_InformSleepFnBufSize; ++Itr) {
						//if(sleep_InformSleepFnBuf->InformWakeUp)sleep_InformSleepFnBuf->InformWakeUp();
						if(Itr->InformWakeUp)Itr->InformWakeUp();
					}

					// 割り込み許可
					sleep_enable_interrupt();

				}
				// スリープ時の割り込み禁止
				void sleep_disable_interrupt() {
					// Main stream の割り込み禁止
					if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
					else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_disable();
					//streamVMC_disable_fput_interrupt();
					// Camera stream の割り込み禁止
//					Device.Camera1_uart.recv_disable();
//					Device.Camera2_uart.recv_disable();
					//streamCMR_disable_fput_interrupt();
					// カメラ のTimer 割り込み禁止
//					Device.Camera_timer.clear_count();
//					Device.Camera_timer.stop();
				}
				// スリープ時の割り込み復帰
				void sleep_enable_interrupt() {
					// Main stream の受信割り込み開始
					if(rf_module_uart_getModuleID()==rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
					else if(rf_module_uart_getModuleID()==rf_module_mode::rf_module)Device.RF_uart.recv_enable();
					// Camera stream の受信割り込み開始
//					Device.Camera1_uart.recv_enable();
//					Device.Camera2_uart.recv_enable();
					// カメラ のTimer 割り込み開始
//					Device.Camera_timer.clear_count();
//					Device.Camera_timer.start();
				}
				//スリープ制御処理割込関数
				void sleep_interrupt_function(sint16 Interval_) {
					sleep_InformRemFn* Itr;

					//SleepCntを加算
					sleep_secCnt+=Interval_;

					//レム状態のとき 
					if(sleep_IsRem) {
						//割り込みで急に電源を落とすのは危険なので、リクエストだけ送っておく(実際の終了は、work関数に委譲)
						if(sleep_secCnt>=sleep_secRem)sleep_RequestEndRem=1;
					}//ノンレム状態のとき
					else {
						if(sleep_secCnt>=sleep_secNonRem) {
							//カウンターを初期化
							sleep_secCnt=0;

							//クロック速度をデフォルトに戻す
							clock_restartClockDefault();

							//電源を入れる
							power(1);
							// 安定待ち
							service::exclusive_delay_ms(300);
							Device.PinRedLED(1);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(0);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(1);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(0);
							service::exclusive_delay_ms(300);
							Device.PinRedLED(1);

							//レム状態に移行
							sleep_IsRem=1;

							//Inform関数
							for(Itr=sleep_InformRemFnBuf; Itr!=(sleep_InformRemFnBuf+sleep_InformRemFnBufSize); ++Itr) {
								//if(sleep_InformRemFnBuf->InformRem)sleep_InformRemFnBuf->InformRem();
								if(Itr->InformRem)Itr->InformRem();
							}

							// 割り込み許可
							sleep_enable_interrupt();

						}
					}
				}
				//スリープ制御処理通常関数
				void sleep_interrupt_yield(void) {
					sleep_InformRemFn* Itr;

					//レム睡眠終了要求関数
					if(sleep_RequestEndRem) {
						//フラグを下げる
						sleep_RequestEndRem=0;
						//カウンターを初期化
						sleep_secCnt=0;

						// 割り込みを禁止
						sleep_disable_interrupt();

						//ノンレム状態に移行
						sleep_IsRem=0;

						//Inform関数
						for(Itr=sleep_InformRemFnBuf; Itr!=sleep_InformRemFnBuf+sleep_InformRemFnBufSize; ++Itr) {
							//if(sleep_InformRemFnBuf->InformNonRem)sleep_InformRemFnBuf->InformNonRem();
							if(Itr->InformNonRem)Itr->InformNonRem();
						}

						//クロック速度を落とす
						clock_restartClock(LowClock);

						//電源を落とす
						power(0);
					}
				}
				//=== ローミングモード制御 ===
				//ローミングの間隔制御変数
				sint16 roaming_secInterval=0;
				sint16 roaming_secCnt=0;
				//ローミングの状態変数
				volatile bool roaming_IsRoaming=0;
				//ローミングモード制御初期化
				void roaming_initialize(void) {
					roaming_secInterval=0;
					roaming_secCnt=0;
					roaming_IsRoaming=0;
				}
				//ローミングモード制御終端化
				void roaming_finalize(void) {
					//ローミング中なら、停止
					if(roaming_IsRoaming)roaming_stop();

					roaming_secInterval=0;
					roaming_secCnt=0;
					roaming_IsRoaming=0;
				}
				//ローミングモードのパラメータを取得する
				void roaming_getInterval(sint16* secInterval) {
					*secInterval=roaming_secInterval;
				}
				//ローミングモードのパラメータを設定する
				void roaming_setInterval(sint16 secInterval) {
					//ローミング間隔は10秒以上である必要
					if(secInterval<=10)secInterval=10;
					roaming_secInterval=secInterval;
				}
				//ローミング状態かどうかを取得する
				volatile bool roaming_isRoaming(void) { return roaming_IsRoaming; }
				//ローミング状態に入る
				void roaming_start(void) {
					//すでにローミング中なら無視
					if(roaming_IsRoaming)return;

					//カウンタリセット
					roaming_secCnt=0;

					//ローミング状態へ移行
					roaming_IsRoaming=1;

					//通信デバイス切り替え
					if(rf_module_uart_getModuleID()==rf_module_mode::rf_module) {
						Device.PinRedLED(1);
						rf_module_uart_initialize(rf_module_mode::mobile_phone, MP_BaudRate);
					} else {
						Device.PinRedLED(0);
						if(Device.Config_RFDebugMode())rf_module_uart_initialize(rf_module_mode::rf_module, RFDebug_BaudRate);
						else rf_module_uart_initialize(rf_module_mode::rf_module, RF_BaudRate);
					}
				}
				//ローミング状態を解除
				void roaming_stop(void) {
					//すでにローミング中でないなら無視
					if(!roaming_IsRoaming)return;

					//ローミング状態を解除
					roaming_IsRoaming=0;
				}
				//ローミング制御処理関数
				void roaming_interrupt_function(sint16 Interval_) {
					//カウンターを進める
					roaming_secCnt+=Interval_;

					//カウンターが切り替え値を超えた場合は、通信デバイス切り替え
					if(roaming_secCnt>roaming_secInterval) {
						if(rf_module_uart_getModuleID()==rf_module_mode::rf_module) {
							Device.PinRedLED(1);
							rf_module_uart_initialize(rf_module_mode::mobile_phone, MP_BaudRate);
						} else {
							Device.PinRedLED(0);
							if(Device.Config_RFDebugMode())rf_module_uart_initialize(rf_module_mode::rf_module, RFDebug_BaudRate);
							else rf_module_uart_initialize(rf_module_mode::rf_module, RF_BaudRate);
						}
						roaming_secCnt=0;
					}
				}

				//=== 基本操作関数 ===
				//デバイス初期化関数
				void initialize(void) {
					Device.PinRedLED.lock();
					Device.Pin5VDCDC.lock();
					Device.PinADC0Power.lock();
					Device.PinADC1to4Power.lock();
//					Device.PinCamera0Power.lock();
//					Device.PinCamera1Power.lock();
//					Device.PinCameraLED.lock();
					Device.PinCO2PumpPower.lock();
					Device.PinCO2SensorPower.lock();
					Device.PinDevicePower.lock();
					Device.PinExtI2CPower.lock();
//					Device.PinHeadLightPower.lock();
					//Device.PinInertialPower.lock();
					Device.PinMobilePhonePower.lock();
					Device.PinMotorPower.lock();
					Device.PinMotorLA.lock();
					Device.PinMotorLB.lock();
					Device.PinMotorRA.lock();
					Device.PinMotorRB.lock();
					Device.PinRFPower.lock();
					Device.PinSDPower.lock();
					Device.Config_DefaultHighClock.lock();
					Device.Config_DefaultMP.lock();
					Device.Config_DisableWDT.lock();
					Device.Config_RFDebugMode.lock();

					//wdt初期化
					Device.WDT.lock();


					//Pic自体のパワーを入れておく
					Device.PinDevicePower(1);
					Device.Pin5VDCDC(1);

					//デバイスの電源を落とす
					power(0);
					//モードシステム初期化
					mode_initialize(NormalMode);

					//クロック初期化
					if(Device.Config_DefaultHighClock())clock_initialize(HighClock);
					else clock_initialize(NormalClock);

					//割り込み初期化->各モジュール初期化時に設定
//					xc32::interrupt::initialize();
					xc32::sfr::interrupt::cpu_priority(Device.Main_IPL);
//					hmr_interrupt_initalize();

					//通信系モジュール初期化
					if(Device.Config_DefaultMP()) {
						rf_module_uart_initialize(rf_module_mode::mobile_phone,Device.MobilePhone_uart_Baudrate);
						com::wdt_restart();
						com::wdt_enable();
					} else {
						if(Device.Config_RFDebugMode())rf_module_uart_initialize(rf_module_mode::rf_module,Device.RF_uart_Baudrate);
						else rf_module_uart_initialize(rf_module_mode::rf_module,Device.RF_uart_Baudrate);
						com::wdt_disable();
					}

					//=== Coamera
//					Device.Camera1_uart.initialize(Device.Camera1_uart_register);
//					Device.Camera1_uart.config(Device.Camera1_uart_Baudrate, Device.Camera1_uart_flowctrl, camera::uart_camera1::Camera1TxInterruptFunc, camera::uart_camera1::Camera1RxInterruptFunc);
//					Device.Camera1_uart.lock();
		
/*					Device.Camera2_uart.initialize(Device.Camera2_uart_register);
					Device.Camera2_uart.config(Device.Camera2_uart_Baudrate, Device.Camera2_uart_flowctrl, camera::uart_camera2::Camera2TxInterruptFunc, camera::uart_camera2::Camera2RxInterruptFunc);
					Device.Camera2_uart.lock();

					//=== SD card spi
					Device.SDcard_spi.initialize(Device.SDcard_spi_register);
					Device.SDcard_spi.config(true, );
					Device.SDcard_spi.lock();
*/
					//=== Inertial i2c
/*					Device.Axcel_i2c.config(Device.Axcel_i2c_clock, 0);
					Device.Axcel_i2c.lock();
					
					if(!Device.Compass_i2c.is_lock()){
						Device.Compass_i2c.config(Device.Compass_i2c_clock, 0);
						Device.Compass_i2c.lock();
					}

					if(!Device.Gyro_i2c.is_lock()) {
						Device.Gyro_i2c.config(Device.Gyro_i2c_clock, 0);
						Device.Gyro_i2c.lock();
					}
*/					
//					hmr_adc_initialize(); -> analog_pinから設定できるので各モジュールが行う

					//デバイスの電源を入れる
					service::delay_ms(500);
					power(1);

					//devmngモジュール初期化
					sleep_initialize();
					roaming_initialize();
					interrupt_initialize();

					//WatchDogTimer開始
					if(Device.Config_DisableWDT())Device.WDT.disable();
					else Device.WDT.enable();

					Device.PinRedLED(0);
//					Device.PinCameraLED(0);
					service::delay_ms(500);
					Device.PinRedLED(1);
//					Device.PinCameraLED(1);
					service::delay_ms(500);
					Device.PinRedLED(0);
//					Device.PinCameraLED(0);
					service::delay_ms(500);
					Device.PinRedLED(1);
//					Device.PinCameraLED(1);
					service::delay_ms(500);
					Device.PinRedLED(1);
//					Device.PinCameraLED(0);

				}
				//デバイス終端化関数
				void finalize(void) {
					//WatchDogTimer終了
					Device.WDT.disable();
					Device.WDT.unlock();

					//devmngモジュール終端化
					sleep_finalize();
					roaming_finalize();
					interrupt_finalize();

					//通信系モジュール終端化
					Device.RF_uart.recv_disable();
					Device.RF_uart.send_disable();
					Device.RF_uart.release();

					Device.MobilePhone_uart.recv_disable();
					Device.MobilePhone_uart.send_disable();
					Device.MobilePhone_uart.release();

//					Device.Camera1_uart.recv_disable();
//					Device.Camera1_uart.send_disable();
//					Device.Camera1_uart.release();

//					Device.Camera2_uart.recv_disable();
//					Device.Camera2_uart.send_disable();
//					Device.Camera2_uart.release();

//					Device.Axcel_i2c.unlock();
//					Device.Compass_i2c.unlock();
//					Device.Gyro_i2c.unlock();
//					hmr_spi2_finalize();
//					hmr_adc_finalize();

					//デバイスの電源を落とす
					power(0);

					//割り込み終了
//					hmr_interrupt_finalize();

					//クロック終了
					clock_finalize();

					//モード管理終了
					mode_finalize();

					power(0);
					Device.PinDevicePower(0);
					Device.WDT.disable();
					Device.WDT.unlock();
					//デバイス自体を終了
//					hmr_device_finalize();
				}
				//デバイス強制終了
				void kill(void) {
					//自身の電源を切る
					Device.PinDevicePower(0);
				}
				//デバイスの電源管理関数
				void power(bool onoff_) {
					bool onoff= (onoff_!=0);
					//電源オフの場合は、モジュール側も切る
					if(!onoff) {
//						Device.PinCamera0Power(0);
//						Device.PinCamera1Power(0);
//						Device.PinHeadLightPower(0);
						Device.PinCO2PumpPower(0);
						Device.PinCO2SensorPower(0);
					}

					rf_module_uart_power_switch(onoff, rf_module_uart_getModuleID());
					//Device.PinInertialPower(onoff);
					Device.PinADC0Power(onoff);
					Device.PinADC1to4Power(onoff);
					Device.PinMotorPower(onoff);
				}
				//現在の通信先が全二重通信対応かを返す
				bool isFullDuplex(void) {
					return rf_module_uart_getModuleID()==rf_module_mode::mobile_phone || Device.Config_RFDebugMode();
				}
				//clear wdt
				void clear_wdt(void) { Device.WDT.clear(); }

				//=== 割り込み関数 ===
				//DevMng用タイマー割り込み関数
				class:public xc32::interrupt::function {
				private:
					virtual void operator()(void) {
						static bool flag = false;
						Device.Task_timer.clear_count();

						Device.PinRedLED(flag);
						flag = (!flag);

						//デバイスマネージの定期実行処理（引数を秒間隔で使用）
						interrupt_function(1);
					}
				}DevMngInterruptFunction;
				//タスク、スリープ、およびローミング実行用関数
				void interrupt_function(sint16 Interval_) {
					//モードによって割り込み処理を分岐
					switch(mode_get()) {
					case SleepMode:
						sleep_interrupt_function(Interval_);
						break;
					case RoamingMode:
						roaming_interrupt_function(Interval_);
						break;
					default:
						service::TaskHost(Interval_);
						break;
					}
				}
				//割り込みのmainループへの委譲処理関数
				bool interrupt_yield(void) {
					sleep_interrupt_yield();

					//スリープを除外
					if(sleep_isSleep() && !sleep_isRem()) {
						//__asm__("PWRSAV #IDLE_MODE");
						//Idle();
						//Sleep();
						return true;
					}

					return false;
				}
				void interrupt_initialize() {
					Device.Task_timer.initialize(Device.Task_timer_register);
					Device.Task_timer.config(Device.TaskTimerMS, DevMngInterruptFunction);
					Device.Task_timer.lock();
				}
				void interrupt_finalize() {
					Device.Task_timer.stop();
					Device.Task_timer.clear_count();
					Device.Task_timer.release();
				}

				// camera stream 割り込みの許可禁止をいじくってよいかを返す関数
				bool interrupt_can_enable_streamCMR_fget_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamCMR_fget_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				bool interrupt_can_enable_streamCMR_fput_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamCMR_fput_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}

				// camera timer  割り込みの許可禁止をいじくってよいかを返す関数
				bool interrupt_can_enable_timerCMR_interrupt() {
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_timerCMR_interrupt() {
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				// DevMng timer の割り込みの許可禁止をいじくってよいかを返す関数
				bool interrupt_can_enable_timerDevmng_interrupt() {
					return true;
				}
				bool interrupt_can_disable_timerDevmng_interrupt() {
					return true;
				}

				// camera stream の割り込み許可禁止関数
				bool interrupt_enable_streamCMR_fget_interrupt() {
					if(interrupt_can_enable_streamCMR_fget_interrupt()) {
//						Device.Camera1_uart.recv_enable();
//						Device.Camera2_uart.recv_enable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_streamCMR_fget_interrupt() {
					if(interrupt_can_disable_streamCMR_fget_interrupt()) {
//						Device.Camera1_uart.recv_disable();
//						Device.Camera2_uart.recv_disable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_enable_streamCMR_fput_interrupt() {
					if(interrupt_can_enable_streamCMR_fput_interrupt()) {
//						Device.Camera1_uart.send_enable();
//						Device.Camera2_uart.send_enable();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_streamCMR_fput_interrupt() {
					if(interrupt_can_disable_streamCMR_fput_interrupt()) {
//						Device.Camera1_uart.send_disable();
//						Device.Camera2_uart.send_disable();
						return true;
					} else {
						return false;
					}
				}

				// timer camera の割り込み許可禁止関数
				bool interrupt_enable_timerCMR_interrupt() {
					if(interrupt_can_enable_timerCMR_interrupt()) {
//						Device.Camera_timer.clear_count();
//						Device.Camera_timer.start();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_timerCMR_interrupt() {
					if(interrupt_can_disable_timerCMR_interrupt()) {
//						Device.Camera_timer.clear_count();
//						Device.Camera_timer.stop();
						return true;
					} else {
						return false;
					}
				}

				// timer Device manager の割り込み許可禁止関数
				bool interrupt_enable_timerDevmng_interrupt() {
					if(interrupt_can_enable_timerDevmng_interrupt()) {
						Device.Task_timer.clear_count();
						Device.Task_timer.start();
//						timerDevmng_enable_interrupt();
						return true;
					} else {
						return false;
					}
				}
				bool interrupt_disable_timerDevmng_interrupt() {
					if(interrupt_can_disable_timerDevmng_interrupt()) {
						Device.Task_timer.stop();
						Device.Task_timer.clear_count();
//						timerDevmng_disable_interrupt();
						return true;
					} else {
						return false;
					}
				}

//=================================SD card===============================
				namespace sdcard{
					void Power(bool val_){Device.PinSDPower(val_);}
					namespace spi{
						void write(uint8 data_){Device.SDcard_spi.write(data_);}
						uint8 read(uint8 data_){return Device.SDcard_spi.read(data_);}
						bool in_data(){return Device.SDcard_spi.in_data();}
						void select(bool val_){Device.PinSD_SPISelect(val_);}
					}
				}	
			}
		}
	}
}
#
#endif
