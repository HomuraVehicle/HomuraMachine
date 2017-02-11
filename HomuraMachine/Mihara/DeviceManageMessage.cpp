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
#include<homuraLib_v2/machine/service/lock_code.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"DeviceManageMessage.hpp"
#include"DeviceManage.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			namespace devmngmsg{
				namespace{
					// mode flag
					struct{
						unsigned Info_i : 1;	     // 情報送信モード受理

						unsigned SleepMode_i : 1;  // sleep mode 受理
						//unsigned SleepModeFail:1; // sleep mode 受理失敗(入力がおかしかった)

						unsigned RoamingMode_i : 1;// roaming mode 受理
						//unsigned RoamingModeFail:1; // roaming mode 受理失敗

						unsigned NormalMode_i : 1; // normal mode 受理
						unsigned KillCom_i : 1;    // kill command 受理 

						unsigned ClockMode_i : 1;  // clock 変更コマンド受理
						unsigned ClockModeFail : 1;

						unsigned SleepModeCodeFail : 1;   // sleep mode exe 受理したがCode認識失敗
						unsigned RoamingModeCodeFail : 1; // roaming mode exe 受理したが失敗
						unsigned KillCodeFail : 1;		  // normal mode exe 受理したが失敗
					}_devmngmsg_Mode = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

					// ロック用のコード
					unsigned char _devmngmsg_sleep_Code = 0;	//ロック解除用のコード
					unsigned char _devmngmsg_roaming_Code = 0;	//ロック解除用のコード
					unsigned char _devmngmsg_kill_Code = 0;	//ロック解除用のコード

					uint16 _devmngmsg_sleep_sec_rem;
					uint16 _devmngmsg_sleep_sec_nonRem;
					unsigned char _devmngmsg_clockMode;
					uint16 _devmngmsg_roaming_sec_interval;
					//unsigned char _devmngmsg_roaming_ID;
				}
				bool listen(hmLib::cstring Str){

					//データサイズ確認
					if (hmLib::cstring_size(&Str) == 0)return true;

					//1byte目でモードを分ける
					switch (hmLib::cstring_getc(&Str, 0)){

						// Infomation mode
					case 0xA0:
						_devmngmsg_Mode.Info_i = true;
						return false;

						// 各種待機モード設定
					case 0x10:
						if (hmLib::cstring_size(&Str) == 1)return true;
						switch (hmLib::cstring_getc(&Str, 1)){
						case 0x00:// normal 
							devmng::mode_set(devmng::NormalMode);

							_devmngmsg_Mode.NormalMode_i = true;
							return false;

						case 0x10:// sleep mode
							if (hmLib::cstring_size(&Str) != 6)return true;
							_devmngmsg_sleep_sec_nonRem = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
							_devmngmsg_sleep_sec_rem = ((uint16)hmLib::cstring_getc(&Str, 4) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 5) * 256;
							// sleep mode　設定準備
							devmng::sleep_setInterval(_devmngmsg_sleep_sec_nonRem, _devmngmsg_sleep_sec_rem);
							//devmng::sleep_getInterval(&_devmngmsg_sleep_sec_nonRem, &_devmngmsg_sleep_sec_rem); 
							// code 取得
							_devmngmsg_sleep_Code = service::lockcode();

							_devmngmsg_Mode.SleepMode_i = true;
							return false;

						case 0x11:// sleep execute mode
							if (hmLib::cstring_size(&Str) != 3)return true;
							// code のチェック
							if (hmLib::cstring_getc(&Str, 2) == _devmngmsg_sleep_Code){// 一致
								// inform 関数のセット

								// sleep の実行
								devmng::mode_set(devmng::SleepMode);
							}
							else{
								// 失敗通知
								_devmngmsg_Mode.SleepModeCodeFail = true;
							}
							return false;

						case 0x20:// roaming mode 
							if (hmLib::cstring_size(&Str) != 4)return true;
							_devmngmsg_roaming_sec_interval = ((uint16)hmLib::cstring_getc(&Str, 2) & 0x00FF) + (uint16)hmLib::cstring_getc(&Str, 3) * 256;
							// sleep mode　設定準備
							devmng::roaming_setInterval(_devmngmsg_roaming_sec_interval);
							//devmng::roaming_getInterval(&_devmngmsg_roaming_sec_interval); 
							// code 取得
							_devmngmsg_roaming_Code = service::lockcode();
							_devmngmsg_Mode.RoamingMode_i = true;
							return false;

						case 0x21:// roaming execute mode
							if (hmLib::cstring_size(&Str) != 3)return true;
							// code のチェック
							if (hmLib::cstring_getc(&Str, 2) == _devmngmsg_roaming_Code){// 一致
								// inform 関数のセット

								// roaming の実行
								devmng::mode_set(devmng::RoamingMode);
							}
							else{
								// 失敗通知
								_devmngmsg_Mode.RoamingModeCodeFail = true;
							}
							return false;
						}

					case 0x20:
						_devmngmsg_kill_Code = service::lockcode();
						_devmngmsg_Mode.KillCom_i = true;
						return false;

					case 0x21:
						if (hmLib::cstring_size(&Str) != 2)return true;
						// code のチェック
						if (hmLib::cstring_getc(&Str, 1) == _devmngmsg_kill_Code){// 一致
							// kill の実行
							devmng::kill();
						}
						else{
							// 失敗通知
							_devmngmsg_Mode.KillCodeFail = true;
						}
						return false;

					case 0x30:
						if (hmLib::cstring_size(&Str) != 2)return true;
						// mode のチェック
						_devmngmsg_clockMode = hmLib::cstring_getc(&Str, 1);
						if (_devmngmsg_clockMode == 0){
							devmng::clock_set(devmng::NormalClock);
						}
						else if (_devmngmsg_clockMode == 1){
							devmng::clock_set(devmng::LowClock);
						}
						else if (_devmngmsg_clockMode == 2){
							devmng::clock_set(devmng::HighClock);
						}
						else{
							_devmngmsg_Mode.ClockModeFail = true;
							return false;
						}
						// clock mode 受理
						_devmngmsg_Mode.ClockMode_i = true;
						return false;
					default:
						return true;
					}
				}
				bool talk(hmLib::cstring* pStr){
					devmng::mode mode;
					devmng::clock clockMode;
					//hmLib_uint16 sumadc;
					if (_devmngmsg_Mode.Info_i){
						service::cstring_construct_safe(pStr, 4);
						hmLib::cstring_putc(pStr, 0, 0xA0);

						mode = devmng::mode_get();
						if (mode == devmng::NormalMode){
							hmLib::cstring_putc(pStr, 1, 0x00);
						}
						else if (mode == devmng::SleepMode){
							hmLib::cstring_putc(pStr, 1, 0x10);
						}
						else if (mode == devmng::RoamingMode){
							hmLib::cstring_putc(pStr, 1, 0x20);
						}

						clockMode = devmng::clock_get();
						if (clockMode == devmng::NormalClock){
							hmLib::cstring_putc(pStr, 2, 0x00);
						}
						else if (clockMode == devmng::LowClock){
							hmLib::cstring_putc(pStr, 2, 0x01);
						}
						else if (clockMode == devmng::HighClock){
							hmLib::cstring_putc(pStr, 2, 0x02);
						}

						// RF or MP 
						if (devmng::rf_module_uart_getModuleID() == devmng::rf_module){
							hmLib::cstring_putc(pStr, 3, 0x00);
						}
						else{
							hmLib::cstring_putc(pStr, 3, 0x01);
						}

						_devmngmsg_Mode.Info_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.NormalMode_i){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0x00);

						_devmngmsg_Mode.NormalMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.SleepMode_i){
						service::cstring_construct_safe(pStr, 7);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0x10);
						hmLib::cstring_putc(pStr, 2, (unsigned char)(_devmngmsg_sleep_sec_nonRem & 0x00FF));
						hmLib::cstring_putc(pStr, 3, (unsigned char)((_devmngmsg_sleep_sec_nonRem >> 8) & 0x00FF));
						hmLib::cstring_putc(pStr, 4, (unsigned char)(_devmngmsg_sleep_sec_rem & 0x00FF));
						hmLib::cstring_putc(pStr, 5, (unsigned char)((_devmngmsg_sleep_sec_rem >> 8) & 0x00FF));
						hmLib::cstring_putc(pStr, 6, _devmngmsg_sleep_Code);

						_devmngmsg_Mode.SleepMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.SleepModeCodeFail){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0xF1);

						_devmngmsg_Mode.SleepModeCodeFail = false;
						return false;

					}
					else if (_devmngmsg_Mode.RoamingMode_i){
						service::cstring_construct_safe(pStr, 6);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0x20);
						hmLib::cstring_putc(pStr, 2, (unsigned char)(_devmngmsg_roaming_sec_interval & 0x00FF));
						hmLib::cstring_putc(pStr, 3, (unsigned char)((_devmngmsg_roaming_sec_interval >> 8) & 0x00FF));
						hmLib::cstring_putc(pStr, 4, _devmngmsg_roaming_Code);

						//変更先の通信手段を返す
						if (devmng::rf_module_uart_getModuleID() == devmng::rf_module){
							hmLib::cstring_putc(pStr, 5, 0x01); // 現状がRFなら次はMP
						}
						else{
							hmLib::cstring_putc(pStr, 5, 0x00); // 現状がMPなら次はRF 
						}

						_devmngmsg_Mode.RoamingMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.RoamingModeCodeFail){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x10);
						hmLib::cstring_putc(pStr, 1, 0xF2);

						_devmngmsg_Mode.RoamingModeCodeFail = false;
						return false;

					}
					else if (_devmngmsg_Mode.KillCom_i){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x20);
						hmLib::cstring_putc(pStr, 1, _devmngmsg_kill_Code);

						_devmngmsg_Mode.KillCom_i = false;
						return false;
					}
					else if (_devmngmsg_Mode.KillCodeFail){
						service::cstring_construct_safe(pStr, 1);
						hmLib::cstring_putc(pStr, 1, 0xF2);

						_devmngmsg_Mode.KillCodeFail = false;
						return false;

					}
					else if (_devmngmsg_Mode.ClockMode_i){
						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x30);
						hmLib::cstring_putc(pStr, 1, _devmngmsg_clockMode);

						_devmngmsg_Mode.ClockMode_i = false;
						return false;

					}
					else if (_devmngmsg_Mode.ClockModeFail){
						service::cstring_construct_safe(pStr, 1);
						hmLib::cstring_putc(pStr, 0, 0xF3);

						_devmngmsg_Mode.ClockModeFail = false;
						return false;
					}

					return true;
				}
				void setup_listen(void){ return; }
				void setup_talk(void){
					//_thermo_Mode.Data=_thermo_Mode.SendData;
					/*Debug*///	_thermo_Mode.Data=1;
				}
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						_devmngmsg_Mode.Info_i = true;
						return dt;
					}
				}InformTask;
				// initialize 関数
				void initialize(){
					_devmngmsg_Mode.Info_i = false;
					_devmngmsg_Mode.SleepMode_i = false;
					_devmngmsg_Mode.RoamingMode_i = false;
					_devmngmsg_Mode.NormalMode_i = false;
					_devmngmsg_Mode.KillCom_i = false;
					_devmngmsg_Mode.ClockMode_i = false;
					_devmngmsg_Mode.ClockModeFail = false;
					_devmngmsg_Mode.SleepModeCodeFail = false;
					_devmngmsg_Mode.RoamingModeCodeFail = false;
					_devmngmsg_Mode.KillCodeFail = false;

//					devmng::initialize();

					//タスク登録
					task::quick_start(InformTask, 5);
				}
				// finalize 関数
				void finalize(){}
			}
		}
	}
}

#
#endif
