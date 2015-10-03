#ifndef HMR_MACHINE_MIHARA_THERMO_CPP_INC
#define HMR_MACHINE_MIHARA_THERMO_CPP_INC 100
#
#define HMLIB_NOLIB
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Thermo.hpp"
#include"Device.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace thermo {
				namespace {
					struct {
						unsigned Data:1;				//�f�[�^�𑗐M���ɗ�����
						unsigned SendData:1;			//�f�[�^���M���[�h
						unsigned SendData_i:1;			//�f�[�^���M���[�h��
					}Mode={0, 0, 1};
					class device : public hmr::machine::mihara::cDevice::thermo_device {
					public:
						apinData ApinData;
					}Device;
					xc::future<uint16> FutureData;
					static const uint8 ADCAverageNum=100;
				}
				bool listen(hmLib::cstring Str) {

					//�f�[�^�T�C�Y�m�F
					if(hmLib::cstring_size(&Str)==0)return true;

					//1byte�ڂŃ��[�h�𕪂���
					switch(hmLib::cstring_getc(&Str, 0)) {
					case 0x10:
						Mode.SendData=true;
						Mode.SendData_i=true;
						return false;
					case 0x11:
						Mode.SendData=false;
						Mode.SendData_i=true;
						return false;
					default:
						return true;
					}
				}
				bool talk(hmLib::cstring* pStr) {

					if(Mode.SendData_i) {
						service::cstring_construct_safe(pStr, 1);
						if(Mode.SendData)hmLib::cstring_putc(pStr, 0, 0x10);
						else hmLib::cstring_putc(pStr, 0, 0x11);

						//���M�㎩���I�Ƀt���O�𗎂Ƃ�
						Mode.SendData_i=false;
						return false;
					} else if(Mode.Data) {
						service::cstring_construct_safe(pStr, 3);
						hmLib::cstring_putc(pStr, 0, 0x00);

						//���x�f�[�^�擾
						uint16 data=0xFFFF;
						if(FutureData.valid()){
						if(FutureData.can_get())data=FutureData.get();
						}
						hmLib::cstring_putc(pStr, 1, (unsigned char)(data%256));
						hmLib::cstring_putc(pStr, 2, (unsigned char)((data>>8)%256));

						//���M�㎩���I�Ƀt���O�𗎂Ƃ�
						Mode.Data=false;
						return false;
					}

					return true;
				}
				void setup_listen(void) { return; }
				void setup_talk(void) {return;}
				void task_setup_talk(void) { return; }
				struct data_task :public hmr::task::client_interface{
					duration operator()(duration dt){
							//Future���s
							if(Mode.SendData && (!FutureData.valid()))FutureData=Device.ApinData(ADCAverageNum);
							//Future��invalid�̂Ƃ���can_get����ƃ������A�N�Z�X�ᔽ�ɂȂ�̂Ő�Ƀ`�F�b�N
							if(FutureData.valid()){
								//�擾�\�ɂȂ�����Ƃ�
							if(FutureData.can_get())Mode.Data=1;
						}
						return dt;
					}
				}DataTask;
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						Mode.SendData_i = true;
						return dt;
				}
				}InformTask;
				void initialize(){
					//analog pin�ݒ�
					Device.ApinData.config(xc32::sfr::adc::vref_mode::vref_Vref_Gnd, 1);
					Device.ApinData.lock();

					//task�o�^
					service::task::quick_start(DataTask, 5);
					service::task::quick_start(InformTask, 5);
				}
				void finalize(){
					Device.ApinData.unlock();

					service::task::stop(DataTask);
					service::task::stop(InformTask);
				}
}
		}
	}
}
#
#endif
