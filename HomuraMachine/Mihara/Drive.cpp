#ifndef HMR_MACHINE_MIHARA_DRIVE_CPP_INC
#define HMR_MACHINE_MIHARA_DRIVE_CPP_INC 100
#
#define HMLIB_NOLIB
#include<stddef.h>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Drive.hpp"
#include"Device.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			namespace motor {
				using namespace hmLib;
				namespace {
					class device : public hmr::machine::mihara::cDevice::motor_device {
					public:
						pinMotorLA PowerLA;
						pinMotorLB PowerLB;
						pinMotorRA PowerRA;
						pinMotorRB PowerRB;
					}Device;
				}
				//*********************** Motor
				//sint8 LBuffer=0;    //-100:100
				//sint8 RBuffer=0;    //-100:100
				unsigned char wdt_count=0;
				bool listen(cstring Str){
					//データサイズ確認
					if(cstring_size(&Str)!=4)return true;

					//左車輪制御
					if(cstring_getc(&Str,1)!=0x00){
						Device.PowerLA(1);
						Device.PowerLB(1);
					}else if(cstring_getc(&Str,0)==0x64){
						Device.PowerLA(1);
						Device.PowerLB(0);
					}else if(cstring_getc(&Str,0)==0x9C){
						Device.PowerLA(0);
						Device.PowerLB(1);
					}else{
						Device.PowerLA(0);
						Device.PowerLB(0);
					}

					//右車輪制御
					if(cstring_getc(&Str,3)!=0x00){
						Device.PowerRA(1);
						Device.PowerRB(1);
					}else if(cstring_getc(&Str,2)==0x64){
						Device.PowerRA(1);
						Device.PowerRB(0);
					}else if(cstring_getc(&Str,2)==0x9C){
						Device.PowerRA(0);
						Device.PowerRB(1);	
					}else{
						Device.PowerRA(0);
						Device.PowerRB(0);		
					}

					wdt_count=0;
		
					return false;
				}
				bool talk(cstring* pStr){
					return true;
				}
				void setup_listen(void){return;}
				void setup_talk(void){return;}
				struct wdt_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						++wdt_count;

						if(wdt_count >= 2){
							wdt_count = 0;
							Device.PowerLA(0);
							Device.PowerLB(0);
							Device.PowerRA(0);
							Device.PowerRB(0);
						}
						return dt;
					}
				}WdtTask;
				void initialize(){
					hmr::machine::service::task::quick_start(WdtTask, 1);
				}
			}
		}
	}
}

#
#endif
