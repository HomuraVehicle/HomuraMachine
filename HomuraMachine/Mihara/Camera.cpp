#ifndef HMR_MACHINE_MIHARA_CAMERA_CPP_INC
#define HMR_MACHINE_MIHARA_CAMERA_CPP_INC 100
#
#define HMLIB_NOLIB
#include<homuraLib_v2/type.hpp>
#include<hmLib/cqueue.h>
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

					//�J���������ʒm���[�h
					bool StatusMode;
					bool LightPower;
					sprite_error_type Error;
					struct {
						unsigned SendStatusMode : 1;		//�X�e�[�^�X���M�t���O
						unsigned SetStatusMode : 1;			//�X�e�[�^�X���M�t���O�ύX�ʒm

						unsigned SetMiniPacMode : 1;		//�~�j�p�P�b�g���[�h�ύX�ʒm
						unsigned SetAutoTakePic : 1;		//�A���B�e���[�h�ύX�ʒm
						unsigned SetAutoReset : 1;			//�I�[�g���Z�b�g�ݒ��ύX�ʒm
						unsigned SetLightPower : 1;			//���C�g��ON/OFF�ݒ��ύX�ʒm
						unsigned SetAutoLight : 1;			//���C�g����ON/OFF�ݒ��ύX�ʒm

						unsigned SendPictureInfo : 1;		//�ʐ^���񑗐M�t���O
						unsigned SendPictureData : 1;		//�ʐ^�f�[�^���M�t���O

						unsigned AckCommandReset : 1;		//�R�}���h���Z�b�g�󗝒ʒm
						unsigned AckPowerReset:1;			//�p���[���Z�b�g�󗝒ʒm

						unsigned SendErrorResult:1;			//�G���[���M�t���O
					}Mode;
				}
				//=============== Message���̓z�� ==================
				void setup_listen(void) { return; }
				void setup_talk(void) {
					//�������񑗐M���[�h��
					if(StatusMode)Mode.SendStatusMode = true;

					//PicInfo���҂��Ă����ꍇ
					if(Camera.can_getPictureInfo())Mode.SendPictureInfo = true;
					if(Camera.can_readPictureData())Mode.SendPictureData = true;

					//�G���[�̗L�����m�F
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

					//�f�[�^�T�C�Y�m�F
					if(hmLib::cstring_size(&Str)==0)return true;

					c = hmLib::cstring_getc(&Str, 0);

					//1byte�ڂŃ��[�h�𕪂���
					switch(c) {
					case 0x00://�ʐ^�擾���[�h
						if(hmLib::cstring_size(&Str) != 2)return true;
						{
							//PictureSize�ݒ�							
							imagesize::type ImageSize = imagesize::size_160_120;
							if(hmLib::cstring_getc(&Str, 1) >= 2) {
								ImageSize = imagesize::size_640_480;
							} else if(hmLib::cstring_getc(&Str, 1) == 1) {
								ImageSize = imagesize::size_320_240;
							}

							Camera.takePicture(ImageSize);
						}
						return false;
					case 0x10://�A���B�e���[�hON
						if(hmLib::cstring_size(&Str) < 2)return true;

						Mode.SetAutoTakePic = true;
						{
							//PictureSize�ݒ�							
							imagesize::type ImageSize = imagesize::size_160_120;
							if(hmLib::cstring_getc(&Str, 1) >= 2) {
								ImageSize = imagesize::size_640_480;
							} else if(hmLib::cstring_getc(&Str, 1) == 1) {
								ImageSize = imagesize::size_320_240;
							}

							Camera.auto_takePicture(ImageSize);
						}
						return false;
					case 0x11://�A���B�e���[�hOFF
						Mode.SetAutoTakePic = true;
						
						Camera.auto_takePicture(camera::imagesize::null);

						return false;
					case 0x30:	//���C�g��ON�v��
						Mode.SetLightPower = true;

						//���C�gON
						Camera.setLight(true);
						LightPower = true;
						return false;
					case 0x31://���C�g��OFF�v��
						Mode.SetLightPower = true;

						//���C�gOFF
						Camera.setLight(false);
						LightPower = false;
						return false;
					case 0x40:	//�����t���b�V��ON�ݒ�
						Mode.SetAutoLight = true;

						//�����t���b�V�����[�hON
						Camera.setAutoLightMode(true);

						return false;
					case 0x41:	//�����t���b�V��OFF�ݒ�
						Mode.SetAutoLight = true;

						//�����t���b�V�����[�hOFF
						Camera.setAutoLightMode(false);

						return false;
					case 0x50:	//�~�j�p�P�b�g���[�hON�ݒ�
						Mode.SetMiniPacMode=true;

						//�~�j�p�P�b�g���[�hON�\��
						Camera.setMiniPacketMode(true);

						return false;
					case 0x51:	//�~�j�p�P�b�g���[�hOFF�ݒ�
						Mode.SetMiniPacMode=true;

						//�~�j�p�P�b�g���[�hOFF�\��
						Camera.setMiniPacketMode(false);

						return false;
					case 0x60:	//�������Z�b�g�v��
						Mode.AckCommandReset = true;
						//���Z�b�g���s��
						Camera.command_reset();
						return false;
					case 0x70:	//�������Z�b�g�v��
						//�������Z�b�g��������
						Camera.power_reset();
						Error=sprite_error_type();
						Mode.SendErrorResult=false;

						return false;
					case 0x80:	//�G���[�������������Z�b�gON�ݒ�
						Mode.SetAutoReset=true;
						//AutoReset�@�\ON
						Camera.setAutoResetMode(true);
						return false;
					case 0x81:	//�G���[�������������Z�b�gOFF�ݒ�
						Mode.SetAutoReset=true;
						//AutoReset�@�\OFF
						Camera.setAutoResetMode(false);
						return false;
					case 0xB0:	//�����f�[�^���M���[�hON
						Mode.SetStatusMode=true;
						StatusMode=true;
						return false;
					case 0xB1:	//�����f�[�^���M���[�hOFF
						Mode.SetStatusMode = true;
						StatusMode = false;
						return false;
					default:
						return true;
					}
				}
				bool talk(hmLib::cstring* pStr) {
					//�A���B�e���[�h��ON/OFF�ݒ�
					if(Mode.SetAutoTakePic) {
						//�t���O�����낷
						Mode.SetAutoTakePic = false;
						service::cstring_construct_safe(pStr, 1);
						if(Camera.is_auto_takePicture())hmLib::cstring_putc(pStr, 0, 0x10);
						else hmLib::cstring_putc(pStr, 0, 0x11);

						return false;
					}
					//���C�g��ON/OFF
					else if(Mode.SetLightPower) {
						//�t���O�����낷
						Mode.SetLightPower=false;

						service::cstring_construct_safe(pStr, 1);
						if(LightPower)hmLib::cstring_putc(pStr, 0, 0x30);
						else hmLib::cstring_putc(pStr, 0, 0x31);
						return false;
					}
					//�t���b�V���@�\ON/OFF�̐ݒ�
					else if(Mode.SetAutoLight) {
						//�t���O�����낷
						Mode.SetAutoLight = false;

						service::cstring_construct_safe(pStr, 1);
						if(Camera.isAutoLightMode())hmLib::cstring_putc(pStr, 0, 0x40);
						else hmLib::cstring_putc(pStr, 0, 0x41);

						return false;
					}
					//�~�j�p�P�b�g���[�h���ǂ����̐ݒ�
					else if(Mode.SetMiniPacMode) {
						//�t���O�����낷
						Mode.SetMiniPacMode = false;

						service::cstring_construct_safe(pStr, 1);
						if(Camera.isMiniPacketMode()) hmLib::cstring_putc(pStr, 0, 0x50);
						else hmLib::cstring_putc(pStr, 0, 0x51);

						return false;
					}
					//�������Z�b�g�v��
					else if(Mode.AckCommandReset) {
						//�t���O�����낷�D
						Mode.AckCommandReset = false;

						service::cstring_construct_safe(pStr, 2);
						hmLib::cstring_putc(pStr, 0, 0x60);
						hmLib::cstring_putc(pStr, 1, Camera.status().byte());

						return false;
					}
					//�G���[�������������Z�b�gON�EOFF�ݒ�
					else if(Mode.SetAutoReset) {
						//�t���O�����낷
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

						//���������擾
					} else if(Mode.SendStatusMode) {
						//�t���O�����낷�D
						Mode.SendStatusMode = false;

						service::cstring_construct_safe(pStr, 4);
						hmLib::cstring_putc(pStr, 0, 0xA0);
						hmLib::cstring_putc(pStr, 1, Camera.status().byte());			//���݂̃X�e�[�^�X
						if(Mode.SendErrorResult){
							hmLib::cstring_putc(pStr, 2, Error.byte_category());				//���݃G���[���Ԃ��ǂ���
							hmLib::cstring_putc(pStr, 3, Error.byte_pos());		//�G���[���̃X�e�[�^�X
						} else{
							hmLib::cstring_putc(pStr, 2, 0);				//���݃G���[���Ԃ��ǂ���
							hmLib::cstring_putc(pStr, 3, 0);		//�G���[���̃X�e�[�^�X
						}
						return false;
					}
					//�ʐ^�擾���[�hACK�ԐM
					else if(Mode.SendPictureInfo) {
						//�t���O�����낷
						Mode.SendPictureInfo = false;

						module::sprite::picture_info PictureInfo = Camera.getPictureInfo();

						//ACK���M
						service::cstring_construct_safe(pStr, 4);
						hmLib::cstring_putc(pStr, 0, (unsigned char)(0x00));
						hmLib::cstring_putc(pStr, 1, (unsigned char)(PictureInfo.ImageSize));
						hmLib::cstring_putc(pStr, 2, (unsigned char)(PictureInfo.PictureSize));
						hmLib::cstring_putc(pStr, 3, (unsigned char)(PictureInfo.PictureSize >> 8));

						return false;
					}
					//�ʐ^�B�eor�f�[�^�v�����[�h
					else if(Mode.SendPictureData) {
						//�t���O�����낷
						Mode.SendPictureData=false;

						//talk�҂��L���[�����z���o��
						camera::picture_bytes PictureData;
						Camera.readPictureData(PictureData);

						//cstring���쐬 3byte�̃w�b�_�������]�v�Ɋm�ۂ��Ă����Ă����̂ŁA���̕����l�����č쐬
						hmLib::cstring_placement_construct(pStr,3+PictureData.PictureData.DataSize,PictureData.Bytes.release(),PictureData.DeleteFp);

						//message�ɓn��
						hmLib::cstring_putc(pStr, 0, 0x01);
						hmLib::cstring_putc(pStr, 1, static_cast<unsigned char>(PictureData.PictureData.PicturePos));
						hmLib::cstring_putc(pStr, 2, static_cast<unsigned char>((PictureData.PictureData.PicturePos)>>8));

						return false;
					}
					return true;
				}

				//�^�X�N
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						Mode.SendStatusMode = true;
						return dt;
					}
				}InformTask;
				//=============== Sprite���J�֐� ===================
				void initialize(void) {
					Camera.lock();
					//�^�X�N�o�^
					service::task::quick_start(InformTask, 5);
				}
				void finalize(void) {
					Camera.unlock();
					//�^�X�N����
					service::task::stop(InformTask);
				}
				void work(void){Camera();}

			}
		}
	}
}
#
#endif
