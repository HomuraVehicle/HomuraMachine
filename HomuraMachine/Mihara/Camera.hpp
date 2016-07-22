#ifndef HMR_MACHINE_MIHARA_CAMERA_INC
#define HMR_MACHINE_MIHARA_CAMERA_INC 100
#
/*
�J�����֘A���W���[��
ToDo:talk1,talk2,�c(���܂��Ă���f�[�^��������C��talk)
=== hmr::machine::mihara::sprite ===
v1_00/141121 hmIto
	cpp��
=== hmrSprite ===
v0_15/130727 iwahori
	�e�f�[�^�o�b�t�@��talk�p�Ƀt���O������D�A���B�e��minipacket���Ƃ���Ȃ�̕p�x�Ń��������[�N��������o�O�͕ς�炸�B�ʏ�p�P�b�g�ł����Ȃ�҂ƃ��������[�N��������
v0_15/130713 iwahori
	�A���B�e�͂ł��邪�A���ʂ�Take���ł��Ȃ��D���ʂ�Take�̂Ƃ���listen����_sprite_buf_empty()���ۂ��Ă���̂ɑ΂��āA
AutoTakePic�ł�setup_talk��!_sprite_buf_full()���ۂ��Ă��邠���肪���������H
v0_15/130712 iwahori
	end_TakePic���ł��Ȃ��o�O���C���Aend_TakePic�̑O��spriteitf_clear_ReadPicDatFlag�����Ă��Ȃ������̂�����
v0_14/130706 iwahori
	�J��������f�[�^����M���Ȃ���CTR�ɑ��M�ł���悤�ɕύX�D2��ڈȍ~�̎B�e�����܂������Ȃ��Dend_TakePic�ł��Ă��Ȃ��H
	TalkNo�̈��S���u��_sprite_fnFinReadPicData�ɗv��
v0_13/130706 hmIto
	PicInfo�̃f�[�^�𑗂�ۂɁAwhile�ő��M�\�ƂȂ�܂ő҂��Ă��������C��
		���炽�ɑ��M�\�ɂȂ������Ƃ�ʒm����ϐ���p�ӂ��Atalk_setup�ŗ����グ��悤�ɂ���
hmrSprite v0_12/130622 iwahori
	�o�O���C���B����ɓ����悤�ɂȂ���
hmrSprite v0_11/130615 iwahori
	ReadPic���inform�֐����g�p����悤�ɕύX�D
	2��ڈȍ~�̎ʐ^�擾�����s����D�������Ԃ��Ă��Ȃ�
v0_06/130427 iwahori
	�V�ʐM�`���ɑΉ�
v0_06/130406 iwahori
	���掿��2���擾�����D�~�j�p�P�b�g���[�h�ł��ʐ^�擾�ƘA���B�e���`�F�b�N����
v0_06/130406 iwahori
	�ŏ��T�C�Y�̎ʐ^�擾/�A���擾�ɐ����D
	���掿��2�͂ł��Ă��Ȃ��D�ʐ^�f�[�^�̃A�h���X�̎w����ԈႦ�Ă��邩��
v0_06/130310 iwahori
	SpriteInterface�ɑΉ�����
	�f�o�b�O��ƒ��C�p���[���Z�b�g�@�\�̓���m�F�͊���
	�ʐ^�f�[�^�擾�͂��܂������ĂȂ��C�����͂܂��s��
	�p���[���Z�b�g��ɂȂ����ʐM�^�C���A�E�g�̃G���[���͂��Ă���
v0_06/130211 hmIto
	
v0_06/130211 iwahori
	_sprite_buf_requestLock()��while����++i����Ă��Ȃ������̂Œǉ�
	sprite_setup_talk()���̘A���B�e���[�h�ŃJ�����T�C�Y��V����̃f�[�^��ϊ����Ă��Ȃ������̂ŕύX
	_sprite_buf_full()��true false�����O�Ƌt�ɂȂ��Ă����̂ł𔽓]�A����ɔ���sprite_setup_talk()�̕�������]
v0_05/130127 iwahori
	_sprite_is_BufAllUsed��return�l�����
v0_04/130127 iwahori
	�V�ʐM�K��ɑΉ�
v0_03/130112 iwahori
	sprite_setup_talk�֐�����GetPicture�t���O�������������ύX
v0_02/130105 iwahori
	�ʐ^�擾���[�h�������C��
v0_01/121221 iwahori
	�S�̓I�ɉ��M
v0_00/121216 iwahori
	��{�֐��쐬
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
				//Sprite�n
			private:
				my_sprite Sprite;
				xc::unique_lock<my_sprite> SpriteLock;
			public:
				sprite_status_type  status(){ return Sprite.status(); }

				//Light�n
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
					//start����ƁA
					//	take
					//	read
					//	read
					//	...
					//	operator()
					//�̏��ŎB�e�V�[�P���X�����s�����
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
						//���łɎB�e���Ȃ�p��
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
				public://�\��֐� for picture_reader
					void take(camera::picture_info PictureInfo_){
						if(Ref.IsAutoLight)Ref.PowerLight(false);
						Ref.PictureInfo = PictureInfo_;
					}
					void read(camera::picture_data PictureData_, xc::rvalue_reference<xc::bytes> Data_){
						vFp_p DeleteFp = sprite_bytes_builder::get_delete_fp(Data_.ref);
						camera::picture_bytes PicBytes(PictureData_, Data_, DeleteFp);
						Ref.PictureDataArray.push(xc::move(PicBytes));
					}
				public://�\��֐� for future
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
						//���łɎB�e���Ȃ�p��
						if(is_start())return true;

						Canceler = Ref.Sprite.async_command_reset(
						).then(xc::ref(*this));

						return !Canceler;
					}
					bool is_start()const{ return Canceler; }
					void cancel(){
						if(Canceler)Canceler();
					}
				public://�\��֐� for future
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
						//���łɎB�e���Ȃ�p��
						if(is_start())return true;

						Canceler = Ref.Sprite.async_power_reset(
						).then(xc::ref(*this));

						return !Canceler;
					}
					bool is_start()const{ return Canceler; }
					void cancel(){
						if(Canceler)Canceler();
					}
				public://�\��֐� for future
					void operator()(sprite_ans_type Ans){
						if(Canceler)Canceler.clear();
						Ref.CanGetResultPowerReset = true;
						Ref.ResultPowerReset = Ans;
						if(!Ans){
							if(Ref.IsAutoReset)Ref.power_reset();
						}
					}
				}Seq_power_reset;

				//�ݒ�֐�
			private:
				bool IsMiniPacketMode;
				bool IsAutoReset;
			public:
				void setMiniPacketMode(bool Enable_){ IsMiniPacketMode = Enable_; }
				bool isMiniPacketMode()const{ return IsMiniPacketMode; }
				void setAutoResetMode(bool Enable){ IsAutoReset = Enable; }
				bool isAutoResetMode()const{ return IsAutoReset; }

				//���s�֐�
			private:
				camera::imagesize::type ReservedImageSize;
				camera::imagesize::type AutoTakeImageSize;
				bool ReservedCommandReset;
			public:
				//�蓮takePicture auto_takePicture�ɗD�悵�Ď��s�����
				//	���ݎB�e���Ȃ�A���̎��̎B�e��takePicture�ŗv������ImageSize�ł̎ʐ^�ɂȂ�
				void takePicture(camera::imagesize::type ImageSize_){ ReservedImageSize = ImageSize_; }
				//������imagesize::null�Ȃ�Aauto_takePicture����
				void auto_takePicture(camera::imagesize::type ImageSize_){ AutoTakeImageSize = ImageSize_; }
				//auto_takePicture�@�\���L�����H
				bool is_auto_takePicture()const{ return AutoTakeImageSize != camera::imagesize::null; }
				//�J�����̃R�}���h���Z�b�g
				void command_reset(){ ReservedCommandReset = true; }
				//�J�����̃p���[���Z�b�g
				void power_reset(){ Seq_power_reset.start(); }
				//�S���߂̃L�����Z��
				void cancel(){
					Seq_take_and_read.cancel();
					Seq_command_reset.cancel();
					Seq_power_reset.cancel();

					PictureInfo = camera::picture_info();
					PictureDataArray.clear();
				}

				//���s����
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
						//�o�b�t�@����Ŏʐ^�B�e���I����Ă���΁A���̎B�e��
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
