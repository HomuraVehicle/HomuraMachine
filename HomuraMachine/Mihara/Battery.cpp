#ifndef HMR_MACHINE_MIHARA_BATTERY_CPP_INC
#define HMR_MACHINE_MIHARA_BATTERY_CPP_INC 100
#
#define HMLIB_NOLIB
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include "Device.hpp"
#include "Battery.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace battery {
				namespace {
					struct {
						unsigned Data:1;				//データを送信時に落ちる
						unsigned SendData:1;			//データ送信モード
						unsigned SendData_i:1;			//データ送信モード受理
					}Mode={0, 0, 1};
					class device : public hmr::machine::mihara::cDevice::battery_device {
					public:
						apinData1 ApinData1;
						apinData2 ApinData2;
						apinData3 ApinData3;
					}Device;
					xc::future<uint16> FutureData1;
					xc::future<uint16> FutureData2;
					xc::future<uint16> FutureData3;
					static const uint8 ADCAverageNum=1;
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
						service::cstring_construct_safe(pStr, 1+2*3);
						hmLib::cstring_putc(pStr, 0, 0x00);

						//データ取得
						uint16 data=0xFFFF;
						if(FutureData1.valid()){
						if(FutureData1.can_get())data=FutureData1.get();
						}
						hmLib::cstring_putc(pStr, 1, (hmLib_uint8)(data&0x00FF));		//Low
						hmLib::cstring_putc(pStr, 2, (hmLib_uint8)((data>>8)&0x00FF));	//High

						data=0xFFFF;
						if(FutureData2.valid()){
						if(FutureData2.can_get())data=FutureData2.get();
						}
						hmLib::cstring_putc(pStr, 3, (hmLib_uint8)(data&0x00FF));		//Low
						hmLib::cstring_putc(pStr, 4, (hmLib_uint8)((data>>8)&0x00FF));	//High

						data=0xFFFF;
						if(FutureData3.valid()){
						if(FutureData3.can_get())data=FutureData3.get();
						}
						hmLib::cstring_putc(pStr, 5, (hmLib_uint8)(data&0x00FF));		//Low
						hmLib::cstring_putc(pStr, 6, (hmLib_uint8)((data>>8)&0x00FF));	//High

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
						if(Mode.SendData && (!FutureData1.valid() || !FutureData2.valid() || !FutureData3.valid())){
							FutureData1=Device.ApinData1(ADCAverageNum);
							FutureData2=Device.ApinData2(ADCAverageNum);
							FutureData3=Device.ApinData3(ADCAverageNum);
						}
						if(FutureData1.valid() && FutureData2.valid() && FutureData3.valid()){
							if(FutureData1.can_get() && FutureData2.can_get() && FutureData3.can_get())Mode.Data=1;
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
					Device.ApinData1.config(xc32::adc::vref_mode::vref_Vref_Gnd, 1);
					Device.ApinData2.config(xc32::adc::vref_mode::vref_Vref_Gnd, 1);
					Device.ApinData3.config(xc32::adc::vref_mode::vref_Vref_Gnd, 1);
					Device.ApinData1.lock();
					Device.ApinData2.lock();
					Device.ApinData3.lock();

					//タスク登録
					service::task::quick_start(InformTask, 5);
					service::task::quick_start(DataTask, 5);
				}
				void finalize(){
					Device.ApinData1.unlock();
					Device.ApinData2.unlock();
					Device.ApinData3.unlock();

					service::task::stop(InformTask);
					service::task::stop(DataTask);
				}
			}
		}
	}
}

#
#endif
