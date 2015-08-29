#ifndef HMR_MACHINE_MIHARA_CAMERA_CPP_INC
#define HMR_MACHINE_MIHARA_CAMERA_CPP_INC 100
#
#define HMLIB_NOLIB
#include<homuraLib_v2/type.hpp>
#include<hmLib_v3_06/cqueue.h>
#include<homuraLib_v2/machine/module/Sprite.hpp>
#include<homuraLib_v2/machine/service/delay.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Camera.hpp"
#include"Device.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace camera{
				namespace{
					struct sprite1_identifier{};
					typedef cSpriteCamera<cDevice::sprite_device, sprite1_identifier> my_camera;
					typedef my_camera::sprite_ans_type sprite_ans_type;
					typedef my_camera::sprite_error_type sprite_error_type;

					my_camera Camera;

					//カメラ情報通知モード
					bool StatusMode;
					bool LightPower;
					sprite_error_type Error;
					struct {
						unsigned SendStatusMode : 1;		//ステータス送信フラグ
						unsigned SetStatusMode : 1;			//ステータス送信フラグ変更通知

						unsigned SetMiniPacMode : 1;		//ミニパケットモード変更通知
						unsigned SetAutoTakePic : 1;		//連続撮影モード変更通知
						unsigned SetAutoReset : 1;			//オートリセット設定変更通知
						unsigned SetLightPower : 1;			//ライトのON/OFF設定変更通知
						unsigned SetAutoLight : 1;			//ライト自動ON/OFF設定変更通知

						unsigned SendPictureInfo : 1;		//写真情報送信フラグ
						unsigned SendPictureData : 1;		//写真データ送信フラグ

						unsigned AckCommandReset : 1;		//コマンドリセット受理通知
						unsigned AckPowerReset:1;			//パワーリセット受理通知

						unsigned SendErrorResult:1;			//エラー送信フラグ
					}Mode;
				}
				//=============== Message側の奴隷 ==================
				void setup_listen(void) { return; }
				void setup_talk(void) {
					//内部情報送信モード時
					if(StatusMode)Mode.SendStatusMode = true;

					//PicInfoを待っている場合
					if(Camera.can_getPictureInfo())Mode.SendPictureInfo = true;
					if(Camera.can_readPictureData())Mode.SendPictureData = true;

					//エラーの有無を確認
					if(Camera.can_getResultTakeAndRead()){
						sprite_ans_type AnsType = Camera.getResultTakeAndRead();
						if(!AnsType){
							Error = AnsType.alternate();
							Mode.SendErrorResult = true;
						}
					}
					if(Camera.can_getResultCommandReset()){
						sprite_ans_type AnsType = Camera.getResultCommandReset();
						if(!AnsType){
							Error = AnsType.alternate();
							Mode.SendErrorResult = true;
						}
					}
					if(Camera.can_getResultPowerReset()){
						sprite_ans_type AnsType = Camera.getResultPowerReset();
						if(!AnsType){
							Error = AnsType.alternate();
							Mode.SendErrorResult = true;
						}
					}
				}
				bool listen(hmLib::cstring Str) {
					static uint8 c=0;

					//データサイズ確認
					if(hmLib::cstring_size(&Str)==0)return true;

					c = hmLib::cstring_getc(&Str, 0);

					//1byte目でモードを分ける
					switch(c) {
					case 0x00://写真取得モード
						if(hmLib::cstring_size(&Str) != 2)return true;
						{
							//PictureSize設定							
							imagesize::type ImageSize = imagesize::size_160_120;
							if(hmLib::cstring_getc(&Str, 1) >= 2) {
								ImageSize = imagesize::size_640_480;
							} else if(hmLib::cstring_getc(&Str, 1) == 1) {
								ImageSize = imagesize::size_320_240;
							}

							Camera.takePicture(ImageSize);
						}
						return false;
					case 0x10://連続撮影モードON
						if(hmLib::cstring_size(&Str) < 2)return true;

						Mode.SetAutoTakePic = true;
						{
							//PictureSize設定							
							imagesize::type ImageSize = imagesize::size_160_120;
							if(hmLib::cstring_getc(&Str, 1) >= 2) {
								ImageSize = imagesize::size_640_480;
							} else if(hmLib::cstring_getc(&Str, 1) == 1) {
								ImageSize = imagesize::size_320_240;
							}

							Camera.auto_takePicture(ImageSize);
						}
						return false;
					case 0x11://連続撮影モードOFF
						Mode.SetAutoTakePic = true;
						
						Camera.auto_takePicture(camera::imagesize::null);

						return false;
					case 0x30:	//ライトのON要求
						Mode.SetLightPower = true;

						//ライトON
						Camera.setLight(true);
						LightPower = true;
						return false;
					case 0x31://ライトのOFF要求
						Mode.SetLightPower = true;

						//ライトOFF
						Camera.setLight(false);
						LightPower = false;
						return false;
					case 0x40:	//自動フラッシュON設定
						Mode.SetAutoLight = true;

						//自動フラッシュモードON
						Camera.setAutoLightMode(true);

						return false;
					case 0x41:	//自動フラッシュOFF設定
						Mode.SetAutoLight = true;

						//自動フラッシュモードOFF
						Camera.setAutoLightMode(false);

						return false;
					case 0x50:	//ミニパケットモードON設定
						Mode.SetMiniPacMode=true;

						//ミニパケットモードON予約
						Camera.setMiniPacketMode(true);

						return false;
					case 0x51:	//ミニパケットモードOFF設定
						Mode.SetMiniPacMode=true;

						//ミニパケットモードOFF予約
						Camera.setMiniPacketMode(false);

						return false;
					case 0x60:	//内部リセット要求
						Mode.AckCommandReset = true;
						//リセットを行う
						Camera.command_reset();
						return false;
					case 0x70:	//強制リセット要求
						//強制リセットをかける
						Camera.power_reset();
						Error=sprite_error_type();
						Mode.SendErrorResult=false;

						return false;
					case 0x80:	//エラー時自動強制リセットON設定
						Mode.SetAutoReset=true;
						//AutoReset機能ON
						Camera.setAutoResetMode(true);
						return false;
					case 0x81:	//エラー時自動強制リセットOFF設定
						Mode.SetAutoReset=true;
						//AutoReset機能OFF
						Camera.setAutoResetMode(false);
						return false;
					case 0xB0:	//内部データ送信モードON
						Mode.SetStatusMode=true;
						StatusMode=true;
						return false;
					case 0xB1:	//内部データ送信モードOFF
						Mode.SetStatusMode = true;
						StatusMode = false;
						return false;
					default:
						return true;
					}
				}
				bool talk(hmLib::cstring* pStr) {
					//連続撮影モードのON/OFF設定
					if(Mode.SetAutoTakePic) {
						//フラグをおろす
						Mode.SetAutoTakePic = false;
						service::cstring_construct_safe(pStr, 1);
						if(Camera.is_auto_takePicture())hmLib::cstring_putc(pStr, 0, 0x10);
						else hmLib::cstring_putc(pStr, 0, 0x11);

						return false;
					}
					//ライトのON/OFF
					else if(Mode.SetLightPower) {
						//フラグをおろす
						Mode.SetLightPower=false;

						service::cstring_construct_safe(pStr, 1);
						if(LightPower)hmLib::cstring_putc(pStr, 0, 0x30);
						else hmLib::cstring_putc(pStr, 0, 0x31);
						return false;
					}
					//フラッシュ機能ON/OFFの設定
					else if(Mode.SetAutoLight) {
						//フラグをおろす
						Mode.SetAutoLight = false;

						service::cstring_construct_safe(pStr, 1);
						if(Camera.isAutoLightMode())hmLib::cstring_putc(pStr, 0, 0x40);
						else hmLib::cstring_putc(pStr, 0, 0x41);

						return false;
					}
					//ミニパケットモードかどうかの設定
					else if(Mode.SetMiniPacMode) {
						//フラグをおろす
						Mode.SetMiniPacMode = false;

						service::cstring_construct_safe(pStr, 1);
						if(Camera.isMiniPacketMode()) hmLib::cstring_putc(pStr, 0, 0x50);
						else hmLib::cstring_putc(pStr, 0, 0x51);

						return false;
					}
					//内部リセット要求
					else if(Mode.AckCommandReset) {
						//フラグをおろす．
						Mode.AckCommandReset = false;

						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x60);
						hmLib::cstring_putc(pStr, 1, Camera.status().byte());

						return false;
					}
					//エラー時自動強制リセットON・OFF設定
					else if(Mode.SetAutoReset) {
						//フラグをおろす
						Mode.SetAutoReset=false;

						service::cstring_construct_safe(pStr, 1);

						if(Camera.isAutoResetMode())hmLib::cstring_putc(pStr, 0, 0x80);
						else hmLib::cstring_putc(pStr, 0, 0x81);

						return false;
					} else if(Mode.SetStatusMode) {
						Mode.SetStatusMode = false;

						service::cstring_construct_safe(pStr, 1);
						if(StatusMode)hmLib::cstring_putc(pStr, 0, 0xB0);
						else hmLib::cstring_putc(pStr, 0, 0xB1);

						return false;

						//内部情報取得
					} else if(Mode.SendStatusMode) {
						//フラグをおろす．
						Mode.SendStatusMode = false;

						service::cstring_construct_safe(pStr, 4);
						hmLib::cstring_putc(pStr, 0, 0xA0);
						hmLib::cstring_putc(pStr, 1, Camera.status().byte());			//現在のステータス
						if(Mode.SendErrorResult){
							hmLib::cstring_putc(pStr, 2, Error.byte_category());				//現在エラー状態かどうか
							hmLib::cstring_putc(pStr, 3, Error.byte_pos());		//エラー時のステータス
						} else{
							hmLib::cstring_putc(pStr, 2, 0);				//現在エラー状態かどうか
							hmLib::cstring_putc(pStr, 3, 0);		//エラー時のステータス
						}
						return false;
					}
					//写真取得モードACK返信
					else if(Mode.SendPictureInfo) {
						//フラグをおろす
						Mode.SendPictureInfo = false;

						module::sprite::picture_info PictureInfo = Camera.getPictureInfo();

						//ACK送信
						service::cstring_construct_safe(pStr, 4);
						hmLib::cstring_putc(pStr, 0, (unsigned char)(0x00));
						hmLib::cstring_putc(pStr, 1, (unsigned char)(PictureInfo.ImageSize));
						hmLib::cstring_putc(pStr, 2, (unsigned char)(PictureInfo.PictureSize));
						hmLib::cstring_putc(pStr, 3, (unsigned char)(PictureInfo.PictureSize >> 8));

						return false;
					}
					//写真撮影orデータ要求モード
					else if(Mode.SendPictureData) {
						//フラグをおろす
						Mode.SendPictureData=false;

						//talk待ちキューから吸い出し
						camera::picture_bytes PictureData;
						Camera.readPictureData(PictureData);

						//cstringを作成 3byteのヘッダ分だけ余計に確保してくれているので、その分を考慮して作成
						hmLib::cstring_placement_construct(pStr,3+PictureData.PictureData.DataSize,PictureData.Bytes.release(),PictureData.DeleteFp);

						//messageに渡す
						hmLib::cstring_putc(pStr, 0, 0x01);
						hmLib::cstring_putc(pStr, 1, static_cast<unsigned char>(PictureData.PictureData.PicturePos));
						hmLib::cstring_putc(pStr, 2, static_cast<unsigned char>((PictureData.PictureData.PicturePos)>>8));

						return false;
					}
					return true;
				}

				//タスク
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						Mode.SendStatusMode = true;
						return dt;
					}
				}InformTask;
				//=============== Sprite公開関数 ===================
				void initialize(void) {
					Camera.lock();
					//タスク登録
					service::task::quick_start(InformTask, 5);
				}
				void finalize(void) {
					Camera.unlock();
					//タスク除去
					service::task::stop(InformTask);
				}
				void work(void){Camera();}

			}
		}
	}
}
#
#endif
