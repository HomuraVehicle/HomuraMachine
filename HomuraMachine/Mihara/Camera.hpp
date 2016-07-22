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
#include"Device.hpp"
#include"Message.hpp"
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
						typedef hmr::static_buffer_allocator<unsigned char, 4096, /*3*/4, my_type> sprite_allocator;
					public:
						void operator()(xc::bytes& Bytes_, unsigned int Size_){					
							if(sprite_allocator::allocated_full())return;
							Bytes_.assign(Size_, sprite_allocator());
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
				bool can_getResultTakeAndRead(){ return CanGetResultTakeAndRead; }
				sprite_ans_type getResultTakeAndRead(){ CanGetResultTakeAndRead = false; return ResultTakeAndRead; }
				bool can_getResultCommandReset(){ return CanGetResultCommandReset; }
				sprite_ans_type getResultCommandReset(){ CanGetResultCommandReset = false; return ResultCommandReset; }
				bool can_getResultPowerReset(){ return CanGetResultPowerReset; }
				sprite_ans_type getResultPowerReset(){ CanGetResultPowerReset = false; return ResultPowerReset; }
			public:
				cSpriteCamera()
					: SpriteLock(Sprite, true)
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

			namespace camera {
				void setup_listen(void);
				void setup_talk(void);
				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void initialize(void);
				void finalize(void);
				void work(void);
			}
		}
	}
}

#
#endif
