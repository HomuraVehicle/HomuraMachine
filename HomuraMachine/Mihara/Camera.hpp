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
					//������imagesize::null�Ȃ�Aauto_takePicture����
					//void auto_takePicture(camera::imagesize::type ImageSize_){ AutoTakeImageSize = ImageSize_; }
					//auto_takePicture�@�\���L�����H
					bool is_auto_takePicture()const{ return AutoTakePicImageSize != camera::imagesize::null; }
					//�J�����̃R�}���h���Z�b�g
					void command_reset(){ 
						if(CurrentMode == systems::mode::observe && Camera.is_lock())Camera.command_reset();
					}
					//�J�����̃p���[���Z�b�g
					void power_reset(){
						if(CurrentMode == systems::mode::observe && Camera.is_lock())Camera.power_reset();
					}
					//�S���߂̃L�����Z��
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
						//������񑗐M���[�h��
						if(StatusSendMode)SendStatus = true;
					}
					bool listen(hmLib::cstring Str)override{
						static uint8 c = 0;

						//�f�[�^�T�C�Y�m�F
						if(hmLib::cstring_size(&Str) == 0)return true;

						c = hmLib::cstring_getc(&Str, 0);

						//1byte�ڂŃ��[�h�𕪂���
						switch(c){
						case 0x00://�ʐ^�擾���[�h
							if(hmLib::cstring_size(&Str) != 2)return true;
							{
								//PictureSize�ݒ�							
								camera::imagesize::type ImageSize = camera::imagesize::size_160_120;
								if(hmLib::cstring_getc(&Str, 1) >= 2){
									ImageSize = camera::imagesize::size_640_480;
								} else if(hmLib::cstring_getc(&Str, 1) == 1){
									ImageSize = camera::imagesize::size_320_240;
								}

								Ref.takePicture(ImageSize);
							}
							return false;
						case 0x10://�A���B�e���[�hON
							if(hmLib::cstring_size(&Str) < 2)return true;

							AutoTakePicMode_i = true;
							{
								//PictureSize�ݒ�							
								camera::imagesize::type ImageSize = camera::imagesize::size_160_120;
								if(hmLib::cstring_getc(&Str, 1) >= 2){
									ImageSize = camera::imagesize::size_640_480;
								} else if(hmLib::cstring_getc(&Str, 1) == 1){
									ImageSize = camera::imagesize::size_320_240;
								}

								Ref.setAutoTakePicMode(ImageSize);
							}
							return false;
						case 0x11://�A���B�e���[�hOFF
							AutoTakePicMode_i = true;

							Ref.setAutoTakePicMode(camera::imagesize::null);

							return false;
						case 0x30:	//���C�g��ON�v��
							LightPower_i = true;

							//���C�gON
							Ref.setLightPower(true);
							return false;
						case 0x31://���C�g��OFF�v��
							LightPower_i = true;

							//���C�gOFF
							Ref.setLightPower(false);
							return false;
						case 0x40:	//�����t���b�V��ON�ݒ�
							AutoLight_i = true;

							//�����t���b�V�����[�hON
							Ref.setAutoLight(true);

							return false;
						case 0x41:	//�����t���b�V��OFF�ݒ�
							AutoLight_i = true;

							//�����t���b�V�����[�hOFF
							Ref.setAutoLight(false);

							return false;
						case 0x50:	//�~�j�p�P�b�g���[�hON�ݒ�
							MiniPackMode_i = true;

							//�~�j�p�P�b�g���[�hON�\��
							Ref.setMiniPacketMode(true);

							return false;
						case 0x51:	//�~�j�p�P�b�g���[�hOFF�ݒ�
							MiniPackMode_i = true;

							//�~�j�p�P�b�g���[�hOFF�\��
							Ref.setMiniPacketMode(false);

							return false;
						case 0x60:	//�������Z�b�g�v��
							CommandReset_i = true;
							//���Z�b�g���s��
							Ref.command_reset();
							return false;
						case 0x70:	//�������Z�b�g�v��
									//�������Z�b�g��������
							Ref.power_reset();
							Error = typename sensor_manager::camera_error_type();
							SendErrorResult = false;

							return false;
						case 0x80:	//�G���[�������������Z�b�gON�ݒ�
							AutoResetMode_i = true;
							//AutoReset�@�\ON
							Ref.setAutoReset(true);
							return false;
						case 0x81:	//�G���[�������������Z�b�gOFF�ݒ�
							AutoResetMode_i = true;
							//AutoReset�@�\OFF
							Ref.setAutoReset(false);
							return false;
						case 0xB0:	//�����f�[�^���M���[�hON
							StatusSendMode_i = true;
							StatusSendMode = true;
							return false;
						case 0xB1:	//�����f�[�^���M���[�hOFF
							StatusSendMode_i = true;
							StatusSendMode = false;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr)override{
						//�A���B�e���[�h��ON/OFF�ݒ�
						if(AutoTakePicMode_i){
							//�t���O�����낷
							AutoTakePicMode_i = false;
							service::cstring_construct_safe(pStr, 1);
							if(Ref.is_auto_takePicture())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);

							return false;
						}
						//���C�g��ON/OFF
						else if(LightPower_i){
							//�t���O�����낷
							LightPower_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getLightPower())hmLib::cstring_putc(pStr, 0, 0x30);
							else hmLib::cstring_putc(pStr, 0, 0x31);
							return false;
						}
						//�t���b�V���@�\ON/OFF�̐ݒ�
						else if(AutoLight_i){
							//�t���O�����낷
							AutoLight_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getAutoLight())hmLib::cstring_putc(pStr, 0, 0x40);
							else hmLib::cstring_putc(pStr, 0, 0x41);

							return false;
						}
						//�~�j�p�P�b�g���[�h���ǂ����̐ݒ�
						else if(MiniPackMode_i){
							//�t���O�����낷
							MiniPackMode_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getMiniPacketMode()) hmLib::cstring_putc(pStr, 0, 0x50);
							else hmLib::cstring_putc(pStr, 0, 0x51);

							return false;
						}
						//�������Z�b�g�v��
						else if(CommandReset_i){
							//�t���O�����낷�D
							CommandReset_i = false;

							service::cstring_construct_safe(pStr, 2);
							hmLib::cstring_putc(pStr, 0, 0x60);
							hmLib::cstring_putc(pStr, 1, Ref.status().byte());

							return false;
						}
						//�G���[�������������Z�b�gON�EOFF�ݒ�
						else if(AutoResetMode_i){
							//�t���O�����낷
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

							//�������擾
						} else if(SendStatus){
							//�t���O�����낷�D
							SendStatus = false;

							service::cstring_construct_safe(pStr, 4);
							hmLib::cstring_putc(pStr, 0, 0xA0);
							hmLib::cstring_putc(pStr, 1, Ref.status().byte());			//���݂̃X�e�[�^�X
							if(SendErrorResult){
								hmLib::cstring_putc(pStr, 2, Error.byte_category());				//���݃G���[��Ԃ��ǂ���
								hmLib::cstring_putc(pStr, 3, Error.byte_pos());		//�G���[���̃X�e�[�^�X
							} else{
								hmLib::cstring_putc(pStr, 2, 0);				//���݃G���[��Ԃ��ǂ���
								hmLib::cstring_putc(pStr, 3, 0);		//�G���[���̃X�e�[�^�X
							}
							return false;
						}
						//�ʐ^�擾���[�hACK�ԐM
						else if(SendPictureInfo){
							//�t���O�����낷
							SendPictureInfo = false;

							module::sprite::picture_info PictureInfo = Ref.getPictureInfo();

							//ACK���M
							service::cstring_construct_safe(pStr, 4);
							hmLib::cstring_putc(pStr, 0, (unsigned char)(0x00));
							hmLib::cstring_putc(pStr, 1, (unsigned char)(PictureInfo.ImageSize));
							hmLib::cstring_putc(pStr, 2, (unsigned char)(PictureInfo.PictureSize));
							hmLib::cstring_putc(pStr, 3, (unsigned char)(PictureInfo.PictureSize >> 8));

							return false;
						}
						//�ʐ^�B�eor�f�[�^�v�����[�h
						else if(SendPictureData){
							//�t���O�����낷
							SendPictureData = false;

							//talk�҂��L���[����z���o��
							camera::picture_bytes PictureData;
							Ref.readPictureData(PictureData);

							//cstring���쐬 3byte�̃w�b�_�������]�v�Ɋm�ۂ��Ă���Ă���̂ŁA���̕����l�����č쐬
							hmLib::cstring_placement_construct(pStr, 3 + PictureData.PictureData.DataSize, PictureData.Bytes.release(), PictureData.DeleteFp);

							//message�ɓn��
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
					//PicInfo��҂��Ă���ꍇ
					if(CameraManager.can_getPictureInfo())MessageClient.setSendPictureInfo(true);
					if(CameraManager.can_readPictureData())MessageClient.setSendPictureData(true);

					//�G���[�̗L�����m�F
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
