#ifndef HMR_MACHINE_MIHARA_CO2_CPP_INC
#define HMR_MACHINE_MIHARA_CO2_CPP_INC 100
#
#define HMLIB_NOLIB
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"CO2.hpp"
#include"Device.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace co2 {
				cCO2<cDevice::co2_device> CO2;

				namespace{
					struct{
						unsigned Data : 1;
						unsigned SendData : 1;
						unsigned SendData_i : 1;
						unsigned PowerPump : 1;
						unsigned PowerPump_i : 1;
						unsigned PowerSensor : 1;
						unsigned PowerSensor_i : 1;
					}Mode = {0, 0, 1, 0, 1, 0, 1};
					xc32::future<uint16> FutureData;
					static const uint8 ADCAverageNum = 100;

					struct data_task :public hmr::task::client_interface{
						duration operator()(duration dt){
							if(Mode.SendData && (!FutureData.valid()))FutureData = Device.ApinData(ADCAverageNum);
							if(FutureData.valid()){
								if(FutureData.can_get())Mode.Data = 1;
							}
							return dt;
						}
					}DataTask;
					struct inform_task :public hmr::task::client_interface{
						duration operator()(duration dt){
							Mode.SendData_i = true;
							Mode.PowerPump_i = true;
							Mode.PowerSensor_i = true;
							return dt;
						}
					}InformTask;
				}

				bool listen(hmLib::cstring Str){
					switch(hmLib::cstring_getc(&Str,0)){
					case 0x10://sensor on
						Mode.SendData=true;
						Mode.SendData_i=true;
						return false;
					case 0x11://sensor on
						Mode.SendData=false;
						Mode.SendData_i=true;
						return false;
					case 0x20://sensor on
						Mode.PowerSensor=true;
						Mode.PowerSensor_i=true;
						Device.PowerSensor(true);
						return false;
					case 0x21://sensor on
						Mode.PowerSensor=false;
						Mode.PowerSensor_i=true;
						Device.PowerSensor(false);
						return false;
					case 0x30://pump on
						Mode.PowerPump=true;
						Mode.PowerPump_i=true;
						Device.PowerPump(true);
						return false;
					case 0x31://pump off
						Mode.PowerPump=false;
						Mode.PowerPump_i=true;
						Device.PowerPump(false);
						return false;
					default:
						return true;
					}
				}
				bool talk(hmLib::cstring* pStr){
					uint16 sumadc=0;
					if(Mode.SendData_i){
						service::cstring_construct_safe(pStr,1);
						if(Mode.SendData)hmLib::cstring_putc(pStr,0,0x10);
						else hmLib::cstring_putc(pStr,0,0x11);
						Mode.SendData_i=false;
						return false;
					}else if(Mode.PowerSensor_i){
						service::cstring_construct_safe(pStr,1);
						if(Mode.PowerSensor)hmLib::cstring_putc(pStr,0,0x20);
						else hmLib::cstring_putc(pStr,0,0x21);
						Mode.PowerSensor_i=false;
						return false;
					}else if(Mode.PowerPump_i){
						service::cstring_construct_safe(pStr,1);
						if(Mode.PowerPump)hmLib::cstring_putc(pStr,0,0x30);
						else hmLib::cstring_putc(pStr,0,0x31);
						Mode.PowerPump_i=false;
						return false;
					}else if(Mode.Data){
						service::cstring_construct_safe(pStr,3);
						hmLib::cstring_putc(pStr,0,0x00);
						// CO2データ取得
						sumadc = 0xFFFF;
						if(FutureData.valid()){
							if(FutureData.can_get())sumadc=FutureData.get();
						}
						hmLib::cstring_putc(pStr,1,(unsigned char)(sumadc&0x00FF));
						hmLib::cstring_putc(pStr,2,(unsigned char)((sumadc>>8)&0x00FF));
						Mode.Data=false;
						return false;
					}
					return true;
				}
				void setup_listen(void){return;}
				void setup_talk(void){
					Mode.Data=Mode.SendData;
				}
				void task_setup_talk(void){return;}
				void initialize(){
					//タスク登録
					service::task::quick_start(InformTask, 5);
					service::task::quick_start(DataTask, 5);
				}
				void finalize(){
					//タスク登録
					service::task::stop(InformTask);
					service::task::stop(DataTask);
				}
			}
		}
	}
}

#
#endif
