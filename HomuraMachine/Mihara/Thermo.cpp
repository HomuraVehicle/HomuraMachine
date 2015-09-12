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
						unsigned Data:1;				//データを送信時に落ちる
						unsigned SendData:1;			//データ送信モード
						unsigned SendData_i:1;			//データ送信モード受理
					}Mode={0, 0, 1};
					class device : public hmr::machine::mihara::cDevice::thermo_device {
					public:
						apinData ApinData;
					}Device;
					xc::future<uint16> FutureData;
					static const uint8 ADCAverageNum=100;
				}
				bool listen(hmLib::cstring Str) {

					//データサイズ確認
					if(hmLib::cstring_size(&Str)==0)return true;

					//1byte目でモードを分ける
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

						//送信後自動的にフラグを落とす
						Mode.SendData_i=false;
						return false;
					} else if(Mode.Data) {
						service::cstring_construct_safe(pStr, 3);
						hmLib::cstring_putc(pStr, 0, 0x00);

						//温度データ取得
						uint16 data=0xFFFF;
						if(FutureData.valid()){
						if(FutureData.can_get())data=FutureData.get();
						}
						hmLib::cstring_putc(pStr, 1, (unsigned char)(data%256));
						hmLib::cstring_putc(pStr, 2, (unsigned char)((data>>8)%256));

						//送信後自動的にフラグを落とす
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
							//Future発行
							if(Mode.SendData && (!FutureData.valid()))FutureData=Device.ApinData(ADCAverageNum);
							//Futureがinvalidのときにcan_getするとメモリアクセス違反になるので先にチェック
							if(FutureData.valid()){
								//取得可能になったらとる
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
					//analog pin設定
					Device.ApinData.config(xc32::sfr::adc::vref_mode::vref_Vref_Gnd, 1);
					Device.ApinData.lock();

					//task登録
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
