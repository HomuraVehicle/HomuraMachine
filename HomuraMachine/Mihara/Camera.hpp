#ifndef HMR_MACHINE_MIHARA_CAMERA_INC
#define HMR_MACHINE_MIHARA_CAMERA_INC 100
#
/*
カメラ関連モジュール
ToDo:talk1,talk2,…(溜まっているデータ数だけ一気にtalk)
=== hmr::machine::mihara::sprite ===
v1_00/141121 hmIto
	cpp化
=== hmrSprite ===
v0_15/130727 iwahori
	各データバッファにtalk用にフラグをつける．連続撮影をminipacketだとそれなりの頻度でメモリリークがおきるバグは変わらず。通常パケットでもかなり待つとメモリリークがおきる
v0_15/130713 iwahori
	連続撮影はできるが、普通のTakeができない．普通のTakeのときはlisten内で_sprite_buf_empty()を課しているのに対して、
AutoTakePicではsetup_talkで!_sprite_buf_full()を課しているあたりが怪しいか？
v0_15/130712 iwahori
	end_TakePicができないバグを修正、end_TakePicの前にspriteitf_clear_ReadPicDatFlagをしていなかったのが原因
v0_14/130706 iwahori
	カメラからデータを受信しながらCTRに送信できるように変更．2回目以降の撮影がうまくいかない．end_TakePicできていない？
	TalkNoの安全装置が_sprite_fnFinReadPicDataに要る
v0_13/130706 hmIto
	PicInfoのデータを送る際に、whileで送信可能となるまで待っていた問題を修正
		あらたに送信可能になったことを通知する変数を用意し、talk_setupで立ち上げるようにした
hmrSprite v0_12/130622 iwahori
	バグを修正。正常に動くようになった
hmrSprite v0_11/130615 iwahori
	ReadPic後のinform関数を使用するように変更．
	2回目以降の写真取得が失敗する．反応が返ってこない
v0_06/130427 iwahori
	新通信形式に対応
v0_06/130406 iwahori
	高画質の2つも取得成功．ミニパケットモードでも写真取得と連続撮影をチェック完了
v0_06/130406 iwahori
	最小サイズの写真取得/連続取得に成功．
	高画質の2つはできていない．写真データのアドレスの指定を間違えているかも
v0_06/130310 iwahori
	SpriteInterfaceに対応完了
	デバッグ作業中，パワーリセット機能の動作確認は完了
	写真データ取得はうまくいってない，原因はまだ不明
	パワーリセット後になぜか通信タイムアウトのエラーをはいてくる
v0_06/130211 hmIto
	
v0_06/130211 iwahori
	_sprite_buf_requestLock()のwhile内で++iされていなかったので追加
	sprite_setup_talk()内の連続撮影モードでカメラサイズをVからのデータを変換していなかったので変更
	_sprite_buf_full()のtrue falseが名前と逆になっていたのでを反転、それに伴いsprite_setup_talk()の分岐も反転
v0_05/130127 iwahori
	_sprite_is_BufAllUsedのreturn値を訂正
v0_04/130127 iwahori
	新通信規約に対応
v0_03/130112 iwahori
	sprite_setup_talk関数内のGetPictureフラグをあげる条件を変更
v0_02/130105 iwahori
	写真取得モード部分を修正
v0_01/121221 iwahori
	全体的に加筆
v0_00/121216 iwahori
	基本関数作成
*/
#include<hmLib/cstring.h>
#include<XCBase/array_queue.hpp>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/module/Sprite.hpp>
#include<homuraLib_v2/static_buffer_allocator.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"System_base.hpp"
#include"IO_base.hpp"
#include"Service_base.hpp"
#include"Device.hpp"
#include<XCBase/future.hpp>

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace camera{
				namespace imagesize = hmr::machine::module::sprite::imagesize;
				typedef module::sprite::picture_info picture_info;
				typedef module::sprite::picture_data picture_data;
				struct picture_bytes{
					camera::picture_data PictureData;
					xc::bytes Bytes;
					vFp_p DeleteFp;
				public:
					picture_bytes(){}
					picture_bytes(camera::picture_data PictureData_, xc::rvalue_reference<xc::bytes> mBytes_, vFp_p DeleteFp_)
						: PictureData(PictureData_)
						, Bytes(mBytes_)
						, DeleteFp(DeleteFp_){
					}
					picture_bytes(xc::rvalue_reference<picture_bytes> mMy_)
						: PictureData(mMy_.ref.PictureData)
						, Bytes(xc::move(mMy_.ref.Bytes))
						, DeleteFp(mMy_.ref.DeleteFp){
					}
					picture_bytes& operator=(xc::rvalue_reference<picture_bytes> mMy_){
						if(this != &(mMy_.ref)){
							PictureData = mMy_.ref.PictureData;
							Bytes = xc::move(mMy_.ref.Bytes);
							DeleteFp = mMy_.ref.DeleteFp;
						}
						return *this;
					}
				private:
					picture_bytes(const picture_bytes&);
					picture_bytes& operator=(const picture_bytes&);
				};
			}
			template<typename sprite_device_, typename identifier>
			class cSpriteCamera : public sprite_device_{
				typedef cSpriteCamera<sprite_device_, identifier> my_type;
			private:
				typedef typename sprite_device_::uart_register uart_register;
				typedef typename sprite_device_::powerCamera powerCamera;
				typedef typename sprite_device_::powerLight powerLight;
				typedef module::cSprite<uart_register, powerCamera> my_sprite;
			public:
				typedef typename my_sprite::status_type sprite_status_type;
				typedef typename my_sprite::error_type sprite_error_type;
				typedef xc::either<void, sprite_error_type > sprite_ans_type;
				typedef typename xc::future<sprite_ans_type>::canceler sprite_canceler;
				//Sprite系
			private:
				my_sprite Sprite;
				xc::unique_lock<my_sprite> SpriteLock;
			public:
				sprite_status_type  status(){ return Sprite.status(); }

				//Light系
			private:
				powerLight PowerLight;
				xc::lock_guard<powerLight> PowerLightLock;
				bool IsAutoLight;
			public:
				void setLight(bool LightOn_){ PowerLight(LightOn_); }
				void setAutoLightMode(bool Enable){
					IsAutoLight = Enable;
					if(!IsAutoLight)PowerLight(false);
				}
				bool isAutoLightMode()const{ return IsAutoLight; }

			private:
				struct take_and_read_sequence : public my_sprite::picture_reader {
					//startすると、
					//	take
					//	read
					//	read
					//	...
					//	operator()
					//の順で撮影シーケンスが実行される
				private:
					struct sprite_bytes_builder{
					private:
						typedef hmr::static_buffer_allocator<unsigned char, 4096, 3, my_type> sprite_allocator;
					public:
						void operator()(xc::bytes& Bytes_, unsigned int Size_){					
							//if(sprite_allocator::allocated_full())return;
							Bytes_.assign(Size_, sprite_allocator());
							if(Bytes_.empty()){
								Bytes_.release();
								return;
							}
						}
						static vFp_p get_delete_fp(const xc::bytes& Bytes_){
							if(!Bytes_)return 0;
							return sprite_allocator::deallocate_buffer;
						}
					};				
				private:
					my_type& Ref;
					sprite_bytes_builder SpriteBytesBuilder;
					sprite_canceler Canceler;
				public:
					take_and_read_sequence(my_type& Ref_)
						: Ref(Ref_){
					}
				public:
					bool start(camera::imagesize::type ImageSize_){
						//すでに撮影中なら却下
						if(is_start())return true;

						if(Ref.IsAutoLight)Ref.PowerLight(true);

						Canceler = Ref.Sprite.async_take_and_readPicture(
							ImageSize_,
							*this,
							SpriteBytesBuilder,
							(Ref.IsMiniPacketMode ? 256 : 4096),
							3, 0
						).then(xc::ref(*this));

						return !Canceler;
					}
					bool is_start()const{ return Canceler; }
					void cancel(){
						if(Canceler)Canceler();
					}
				public://予約関数 for picture_reader
					void take(camera::picture_info PictureInfo_){
						if(Ref.IsAutoLight)Ref.PowerLight(false);
						Ref.PictureInfo = PictureInfo_;
					}
					void read(camera::picture_data PictureData_, xc::rvalue_reference<xc::bytes> Data_){
						vFp_p DeleteFp = sprite_bytes_builder::get_delete_fp(Data_.ref);
						camera::picture_bytes PicBytes(PictureData_, Data_, DeleteFp);
						Ref.PictureDataArray.push(xc::move(PicBytes));
					}
				public://予約関数 for future
					void operator()(sprite_ans_type Ans){
						if(Canceler)Canceler.clear();
						Ref.CanGetResultTakeAndRead = true;
						Ref.ResultTakeAndRead = Ans;
						if(!Ans){
							if(Ref.IsAutoReset)Ref.power_reset();
						}
					}
				}Seq_take_and_read;
				struct command_reset_sequence{
				private:
					my_type& Ref;
					sprite_canceler Canceler;
				public:
					command_reset_sequence(my_type& Ref_)
						: Ref(Ref_){
					}
				public:
					bool start(){
						//すでに撮影中なら却下
						if(is_start())return true;

						Canceler = Ref.Sprite.async_command_reset(
						).then(xc::ref(*this));

						return !Canceler;
					}
					bool is_start()const{ return Canceler; }
					void cancel(){
						if(Canceler)Canceler();
					}
				public://予約関数 for future
					void operator()(sprite_ans_type Ans){
						if(Canceler)Canceler.clear();
						Ref.CanGetResultCommandReset = true;
						Ref.ResultCommandReset = Ans;
						if(!Ans){
							if(Ref.IsAutoReset)Ref.power_reset();
						}
					}
				}Seq_command_reset;
				struct power_reset_sequence{
				private:
					my_type& Ref;
					sprite_canceler Canceler;
				public:
					power_reset_sequence(my_type& Ref_)
						: Ref(Ref_){
					}
				public:
					bool start(){
						//すでに撮影中なら却下
						if(is_start())return true;

						Canceler = Ref.Sprite.async_power_reset(
						).then(xc::ref(*this));

						return !Canceler;
					}
					bool is_start()const{ return Canceler; }
					void cancel(){
						if(Canceler)Canceler();
					}
				public://予約関数 for future
					void operator()(sprite_ans_type Ans){
						if(Canceler)Canceler.clear();
						Ref.CanGetResultPowerReset = true;
						Ref.ResultPowerReset = Ans;
						if(!Ans){
							if(Ref.IsAutoReset)Ref.power_reset();
						}
					}
				}Seq_power_reset;

				//設定関数
			private:
				bool IsMiniPacketMode;
				bool IsAutoReset;
			public:
				void setMiniPacketMode(bool Enable_){ IsMiniPacketMode = Enable_; }
				bool isMiniPacketMode()const{ return IsMiniPacketMode; }
				void setAutoResetMode(bool Enable){ IsAutoReset = Enable; }
				bool isAutoResetMode()const{ return IsAutoReset; }

				//実行関数
			private:
				camera::imagesize::type ReservedImageSize;
				camera::imagesize::type AutoTakeImageSize;
				bool ReservedCommandReset;
			public:
				//手動takePicture auto_takePictureに優先して実行される
				//	現在撮影中なら、その次の撮影がtakePictureで要求したImageSizeでの写真になる
				void takePicture(camera::imagesize::type ImageSize_){ ReservedImageSize = ImageSize_; }
				//引数がimagesize::nullなら、auto_takePicture無効
				void auto_takePicture(camera::imagesize::type ImageSize_){ AutoTakeImageSize = ImageSize_; }
				//auto_takePicture機能が有効か？
				bool is_auto_takePicture()const{ return AutoTakeImageSize != camera::imagesize::null; }
				//カメラのコマンドリセット
				void command_reset(){ ReservedCommandReset = true; }
				//カメラのパワーリセット
				void power_reset(){ Seq_power_reset.start(); }
				//全命令のキャンセル
				void cancel(){
					Seq_take_and_read.cancel();
					Seq_command_reset.cancel();
					Seq_power_reset.cancel();

					PictureInfo = camera::picture_info();
					PictureDataArray.clear();
				}

				//実行結果
			private:
				typedef xc::array_queue<camera::picture_bytes, 3> data_queue;
				camera::picture_info PictureInfo;
				data_queue PictureDataArray;

				bool CanGetResultTakeAndRead;
				sprite_ans_type ResultTakeAndRead;
				bool CanGetResultCommandReset;
				sprite_ans_type ResultCommandReset;
				bool CanGetResultPowerReset;
				sprite_ans_type ResultPowerReset;
			public:
				bool can_getPictureInfo()const{ return PictureInfo.valid(); }
				camera::picture_info getPictureInfo(){ 
					camera::picture_info AnsPictureInfo;
					std::swap(AnsPictureInfo, PictureInfo);
					return AnsPictureInfo;
				}
				bool can_readPictureData()const{ return !(PictureDataArray.empty()); }
				void readPictureData(camera::picture_bytes& PictureData_){
					PictureData_ = xc::move(PictureDataArray.next());
					PictureDataArray.pop();
				}
				bool can_getResultTakeAndRead()const{ return CanGetResultTakeAndRead; }
				sprite_ans_type getResultTakeAndRead(){ CanGetResultTakeAndRead = false; return ResultTakeAndRead; }
				bool can_getResultCommandReset()const{ return CanGetResultCommandReset; }
				sprite_ans_type getResultCommandReset(){ CanGetResultCommandReset = false; return ResultCommandReset; }
				bool can_getResultPowerReset()const{ return CanGetResultPowerReset; }
				sprite_ans_type getResultPowerReset(){ CanGetResultPowerReset = false; return ResultPowerReset; }
			public:
				cSpriteCamera(service_interface& Service_)
					: Sprite()
					, SpriteLock(Sprite, true)
					, PowerLightLock(PowerLight)
					, IsAutoLight(false)
					, Seq_take_and_read(*this)
					, Seq_command_reset(*this)
					, Seq_power_reset(*this)
					, IsMiniPacketMode(false)
					, IsAutoReset(false)
					, ReservedImageSize(camera::imagesize::null)
					, AutoTakeImageSize(camera::imagesize::null)
					, ReservedCommandReset(false)
					, CanGetResultTakeAndRead(false)
					, CanGetResultCommandReset(false)
					, CanGetResultPowerReset(false){

					Sprite.config(Service_.task());
					PowerLight(false);
				}
				bool lock(){
					if(is_lock())return false;
					return SpriteLock.lock();
				}
				bool is_lock()const{ return SpriteLock; }
				void unlock(){
					cancel();
					SpriteLock.unlock();
					PowerLight(false);
				}
				void operator()(void){
					if(Sprite.idle()){
						//command_reset
						if(ReservedCommandReset){
							Seq_command_reset.start();
							ReservedCommandReset=false;
						}
						//バッファが空で写真撮影も終わっていれば、次の撮影へ
						if(PictureDataArray.empty() && !Seq_take_and_read.is_start()){
							if(ReservedImageSize != camera::imagesize::null){
								Seq_take_and_read.start(ReservedImageSize);
								ReservedImageSize = camera::imagesize::null;
							} else if(AutoTakeImageSize != camera::imagesize::null){
								Seq_take_and_read.start(AutoTakeImageSize);
							}
						}
					}
				}
			};

			template<typename camera_device_>
			class cCamera :public camera_device_{
			private:
				class sensor_manager :public system_client_interface{
				private:
					class sprite1_identifer{};
					using my_camera = cSpriteCamera<camera_device_, sprite1_identifer>;
				public:
					using camera_ans_type = typename my_camera::sprite_ans_type;
					using camera_error_type = typename my_camera::sprite_error_type;
					using camera_status_type = typename my_camera::sprite_status_type;
				private:
					my_camera Camera;
					bool CameraPower;
					bool LightPower;
					bool MiniPacketMode;
					bool AutoLight;
					bool AutoReset;
					camera::imagesize::type AutoTakePicImageSize;
				private:
					systems::mode::type CurrentMode;
				public://override function of system_client_interface
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_)override{
						switch(NewMode_){
						case systems::mode::observe:
							if(CameraPower){
								Camera.lock();
							}
							Camera.setLight(LightPower);
							Camera.setAutoLightMode(AutoLight);
							Camera.setMiniPacketMode(MiniPacketMode);
							Camera.setAutoResetMode(AutoReset);
							Camera.auto_takePicture(AutoTakePicImageSize);
							break;
						default:
							Camera.cancel();
							Camera.unlock();
							break;
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrentMode; }
					void setCameraPower(bool OnOff_){
						if((CameraPower != OnOff_) && CurrentMode == systems::mode::observe){
							if(OnOff_)Camera.lock();
							else Camera.unlock();
						}
						CameraPower = OnOff_;
					}
					bool getCameraPower()const{ return CameraPower; }
					void setLightPower(bool OnOff_){
						LightPower = OnOff_;
						if(CurrentMode == systems::mode::observe)Camera.setLight(LightPower);
					}
					bool getLightPower()const{ return LightPower; }
					void setMiniPacketMode(bool OnOff_){
						MiniPacketMode = OnOff_;
						if(CurrentMode == systems::mode::observe)Camera.setMiniPacketMode(MiniPacketMode);
					}
					bool getMiniPacketMode()const{ return MiniPacketMode; }
					void setAutoLight(bool OnOff_){
						AutoLight = OnOff_;
						if(CurrentMode == systems::mode::observe)Camera.setAutoLightMode(AutoLight);
					}
					bool getAutoLight()const{ return AutoLight; }
					void setAutoReset(bool OnOff_){
						AutoReset = OnOff_;
						if(CurrentMode == systems::mode::observe)Camera.setAutoResetMode(AutoReset);
					}
					bool getAutoReset()const{ return AutoReset; }
					void setAutoTakePicMode(camera::imagesize::type PictureSize_){
						AutoTakePicImageSize = PictureSize_;
						if(CurrentMode == systems::mode::observe)Camera.auto_takePicture(AutoTakePicImageSize);
					}
					camera::imagesize::type getAutoTakePicMode()const{ return AutoTakePicImageSize; }
				public://wrapper functions of cSpriteCamera
					camera_status_type  status(){ return Camera.status(); }
					void takePicture(camera::imagesize::type ImageSize_){
						if(CurrentMode == systems::mode::observe && Camera.is_lock())Camera.takePicture(ImageSize_);
					}
					//引数がimagesize::nullなら、auto_takePicture無効
					//void auto_takePicture(camera::imagesize::type ImageSize_){ AutoTakeImageSize = ImageSize_; }
					//auto_takePicture機能が有効か？
					bool is_auto_takePicture()const{ return AutoTakePicImageSize != camera::imagesize::null; }
					//カメラのコマンドリセット
					void command_reset(){ 
						if(CurrentMode == systems::mode::observe && Camera.is_lock())Camera.command_reset();
					}
					//カメラのパワーリセット
					void power_reset(){
						if(CurrentMode == systems::mode::observe && Camera.is_lock())Camera.power_reset();
					}
					//全命令のキャンセル
					void cancel(){ Camera.cancel(); }
					bool can_getPictureInfo()const{ return Camera.can_getPictureInfo(); }
					camera::picture_info getPictureInfo(){ return Camera.getPictureInfo(); }
					bool can_readPictureData()const{ return Camera.can_readPictureData(); }
					void readPictureData(camera::picture_bytes& PictureData_){ Camera.readPictureData(PictureData_); }
					bool can_getResultTakeAndRead()const{ return Camera.can_getResultTakeAndRead(); }
					camera_ans_type getResultTakeAndRead(){ return Camera.getResultTakeAndRead(); }
					bool can_getResultCommandReset()const{ return Camera.can_getResultCommandReset(); }
					camera_ans_type getResultCommandReset(){ return Camera.getResultCommandReset(); }
					bool can_getResultPowerReset()const{ return Camera.can_getResultPowerReset(); }
					camera_ans_type getResultPowerReset(){ return Camera.getResultPowerReset(); }
				public:
					sensor_manager(service_interface& Service_)
						: Camera(Service_)
						, CameraPower(false)
						, LightPower(false)
						, MiniPacketMode(false)
						, AutoLight(false)
						, AutoReset(false)
						, AutoTakePicImageSize(camera::imagesize::null){
						Camera.lock();
						CameraPower = true;
					}
					~sensor_manager(){
						Camera.unlock();
					}
				public:
					void operator()(){
						if(Camera.is_lock())Camera();
					}
				};
				sensor_manager CameraManager;
			private:
				class message_client :public message_client_interface{
				private:
					sensor_manager& Ref;
				private:
					bool MiniPackMode_i;
					bool AutoTakePicMode_i;
					bool AutoResetMode_i;
					bool LightPower_i;
					bool AutoLight_i;

					bool CommandReset_i;
					bool PowerReset_i;

					bool StatusSendMode;
					bool StatusSendMode_i;
					bool SendStatus;
					bool SendPictureInfo;
					bool SendPictureData;
					bool SendErrorResult;
					typename sensor_manager::camera_error_type Error;
				public:
					void setSendPictureInfo(bool OnOff_){ SendPictureInfo = OnOff_; }
					void setSendPictureData(bool OnOff_){ SendPictureData = OnOff_; }
				private:
					class inform_task :public hmr::task::client_interface{
					private:
						message_client& Ref;
					public:
						inform_task(message_client& Ref_):Ref(Ref_){}
						duration operator()(duration dt){
							Ref.SendStatus = true;
							return dt;
						}
					};
					inform_task InformTask;
					task::handler InformTaskHandler;
				public:
					message_client(sensor_manager& Ref_, com::did_t ID_, service_interface& Service_)
						: message_client_interface(ID_)
						, Ref(Ref_)
						, MiniPackMode_i(true)
						, AutoTakePicMode_i(true)
						, AutoResetMode_i(true)
						, LightPower_i(true)
						, AutoLight_i(true)
						, CommandReset_i(false)
						, PowerReset_i(false)
						, StatusSendMode(false)
						, StatusSendMode_i(false)
						, SendStatus(false)
						, SendPictureInfo(false)
						, SendPictureData(false)
						, SendErrorResult(false)
						, InformTask(*this){
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~message_client(){
						InformTaskHandler.stop();
					}
				public://override function of message_client_interface
					void setup_listen(void)override{ return; }
					void setup_talk(void)override{
						//内部情報送信モード時
						if(StatusSendMode)SendStatus = true;
					}
					bool listen(hmLib::cstring Str)override{
						static uint8 c = 0;

						//データサイズ確認
						if(hmLib::cstring_size(&Str) == 0)return true;

						c = hmLib::cstring_getc(&Str, 0);

						//1byte目でモードを分ける
						switch(c){
						case 0x00://写真取得モード
							if(hmLib::cstring_size(&Str) != 2)return true;
							{
								//PictureSize設定							
								camera::imagesize::type ImageSize = camera::imagesize::size_160_120;
								if(hmLib::cstring_getc(&Str, 1) >= 2){
									ImageSize = camera::imagesize::size_640_480;
								} else if(hmLib::cstring_getc(&Str, 1) == 1){
									ImageSize = camera::imagesize::size_320_240;
								}

								Ref.takePicture(ImageSize);
							}
							return false;
						case 0x10://連続撮影モードON
							if(hmLib::cstring_size(&Str) < 2)return true;

							AutoTakePicMode_i = true;
							{
								//PictureSize設定							
								camera::imagesize::type ImageSize = camera::imagesize::size_160_120;
								if(hmLib::cstring_getc(&Str, 1) >= 2){
									ImageSize = camera::imagesize::size_640_480;
								} else if(hmLib::cstring_getc(&Str, 1) == 1){
									ImageSize = camera::imagesize::size_320_240;
								}

								Ref.setAutoTakePicMode(ImageSize);
							}
							return false;
						case 0x11://連続撮影モードOFF
							AutoTakePicMode_i = true;

							Ref.setAutoTakePicMode(camera::imagesize::null);

							return false;
						case 0x30:	//ライトのON要求
							LightPower_i = true;

							//ライトON
							Ref.setLightPower(true);
							return false;
						case 0x31://ライトのOFF要求
							LightPower_i = true;

							//ライトOFF
							Ref.setLightPower(false);
							return false;
						case 0x40:	//自動フラッシュON設定
							AutoLight_i = true;

							//自動フラッシュモードON
							Ref.setAutoLight(true);

							return false;
						case 0x41:	//自動フラッシュOFF設定
							AutoLight_i = true;

							//自動フラッシュモードOFF
							Ref.setAutoLight(false);

							return false;
						case 0x50:	//ミニパケットモードON設定
							MiniPackMode_i = true;

							//ミニパケットモードON予約
							Ref.setMiniPacketMode(true);

							return false;
						case 0x51:	//ミニパケットモードOFF設定
							MiniPackMode_i = true;

							//ミニパケットモードOFF予約
							Ref.setMiniPacketMode(false);

							return false;
						case 0x60:	//内部リセット要求
							CommandReset_i = true;
							//リセットを行う
							Ref.command_reset();
							return false;
						case 0x70:	//強制リセット要求
									//強制リセットをかける
							Ref.power_reset();
							Error = typename sensor_manager::camera_error_type();
							SendErrorResult = false;

							return false;
						case 0x80:	//エラー時自動強制リセットON設定
							AutoResetMode_i = true;
							//AutoReset機能ON
							Ref.setAutoReset(true);
							return false;
						case 0x81:	//エラー時自動強制リセットOFF設定
							AutoResetMode_i = true;
							//AutoReset機能OFF
							Ref.setAutoReset(false);
							return false;
						case 0xB0:	//内部データ送信モードON
							StatusSendMode_i = true;
							StatusSendMode = true;
							return false;
						case 0xB1:	//内部データ送信モードOFF
							StatusSendMode_i = true;
							StatusSendMode = false;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr)override{
						//連続撮影モードのON/OFF設定
						if(AutoTakePicMode_i){
							//フラグをおろす
							AutoTakePicMode_i = false;
							service::cstring_construct_safe(pStr, 1);
							if(Ref.is_auto_takePicture())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);

							return false;
						}
						//ライトのON/OFF
						else if(LightPower_i){
							//フラグをおろす
							LightPower_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getLightPower())hmLib::cstring_putc(pStr, 0, 0x30);
							else hmLib::cstring_putc(pStr, 0, 0x31);
							return false;
						}
						//フラッシュ機能ON/OFFの設定
						else if(AutoLight_i){
							//フラグをおろす
							AutoLight_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getAutoLight())hmLib::cstring_putc(pStr, 0, 0x40);
							else hmLib::cstring_putc(pStr, 0, 0x41);

							return false;
						}
						//ミニパケットモードかどうかの設定
						else if(MiniPackMode_i){
							//フラグをおろす
							MiniPackMode_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getMiniPacketMode()) hmLib::cstring_putc(pStr, 0, 0x50);
							else hmLib::cstring_putc(pStr, 0, 0x51);

							return false;
						}
						//内部リセット要求
						else if(CommandReset_i){
							//フラグをおろす．
							CommandReset_i = false;

							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x60);
							hmLib::cstring_putc(pStr, 1, Ref.status().byte());

							return false;
						}
						//エラー時自動強制リセットON・OFF設定
						else if(AutoResetMode_i){
							//フラグをおろす
							AutoResetMode_i = false;

							service::cstring_construct_safe(pStr, 1);

							if(Ref.getAutoReset())hmLib::cstring_putc(pStr, 0, 0x80);
							else hmLib::cstring_putc(pStr, 0, 0x81);

							return false;
						} else if(StatusSendMode_i){
							StatusSendMode_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(StatusSendMode)hmLib::cstring_putc(pStr, 0, 0xB0);
							else hmLib::cstring_putc(pStr, 0, 0xB1);

							return false;

							//内部情報取得
						} else if(SendStatus){
							//フラグをおろす．
							SendStatus = false;

							service::cstring_construct_safe(pStr, 4);
							hmLib::cstring_putc(pStr, 0, 0xA0);
							hmLib::cstring_putc(pStr, 1, Ref.status().byte());			//現在のステータス
							if(SendErrorResult){
								hmLib::cstring_putc(pStr, 2, Error.byte_category());				//現在エラー状態かどうか
								hmLib::cstring_putc(pStr, 3, Error.byte_pos());		//エラー時のステータス
							} else{
								hmLib::cstring_putc(pStr, 2, 0);				//現在エラー状態かどうか
								hmLib::cstring_putc(pStr, 3, 0);		//エラー時のステータス
							}
							return false;
						}
						//写真取得モードACK返信
						else if(SendPictureInfo){
							//フラグをおろす
							SendPictureInfo = false;

							module::sprite::picture_info PictureInfo = Ref.getPictureInfo();

							//ACK送信
							service::cstring_construct_safe(pStr, 4);
							hmLib::cstring_putc(pStr, 0, (unsigned char)(0x00));
							hmLib::cstring_putc(pStr, 1, (unsigned char)(PictureInfo.ImageSize));
							hmLib::cstring_putc(pStr, 2, (unsigned char)(PictureInfo.PictureSize));
							hmLib::cstring_putc(pStr, 3, (unsigned char)(PictureInfo.PictureSize >> 8));

							return false;
						}
						//写真撮影orデータ要求モード
						else if(SendPictureData){
							//フラグをおろす
							SendPictureData = false;

							//talk待ちキューから吸い出し
							camera::picture_bytes PictureData;
							Ref.readPictureData(PictureData);

							//cstringを作成 3byteのヘッダ分だけ余計に確保してくれているので、その分を考慮して作成
							hmLib::cstring_placement_construct(pStr, 3 + PictureData.PictureData.DataSize, PictureData.Bytes.release(), PictureData.DeleteFp);

							//messageに渡す
							hmLib::cstring_putc(pStr, 0, 0x01);
							hmLib::cstring_putc(pStr, 1, static_cast<unsigned char>(PictureData.PictureData.PicturePos));
							hmLib::cstring_putc(pStr, 2, static_cast<unsigned char>((PictureData.PictureData.PicturePos) >> 8));

							return false;
						}
						return true;
					}
				public:
					void setSendErrorData(typename sensor_manager::camera_error_type Error_){
						Error = Error_;
						SendErrorResult = true;
					}
				};
				message_client MessageClient;
			public:
				cCamera(unsigned char CameraID_, system_interface& System_, io_interface& IO_, service_interface& Service_)
					: CameraManager(Service_)
					, MessageClient(CameraManager, CameraID_, Service_){
					System_.regist(CameraManager);
					IO_.regist(MessageClient);
				}
			public:
				void operator()(){
					CameraManager();
					//PicInfoを待っている場合
					if(CameraManager.can_getPictureInfo())MessageClient.setSendPictureInfo(true);
					if(CameraManager.can_readPictureData())MessageClient.setSendPictureData(true);

					//エラーの有無を確認
					if(CameraManager.can_getResultTakeAndRead()){
						typename sensor_manager::camera_ans_type AnsType = CameraManager.getResultTakeAndRead();
						if(!AnsType){
							MessageClient.setSendErrorData(AnsType.alternate());
						}
					}
					if(CameraManager.can_getResultCommandReset()){
						typename sensor_manager::camera_ans_type AnsType = CameraManager.getResultCommandReset();
						if(!AnsType){
							MessageClient.setSendErrorData(AnsType.alternate());
						}
					}
					if(CameraManager.can_getResultPowerReset()){
						typename sensor_manager::camera_ans_type AnsType = CameraManager.getResultPowerReset();
						if(!AnsType){
							MessageClient.setSendErrorData(AnsType.alternate());
						}
					}
				}
			};
		}
	}
}

#
#endif
