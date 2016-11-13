#ifndef HMR_MACHINE_MIHARA_CO2_INC
#define HMR_MACHINE_MIHARA_CO2_INC 100
#
/*
CO2モジュール制御用
=== hmr::machine::mihara::CO2 ===
v1_00/141111 hmIto
	C++化
=== hmrCO2 ===
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v0_01/130105 iwahori
	work関数をsetup_lisenとsetup_talkに分離
v0_00/121208 hmIto
	基本関数作成
*/
#include<hmLib/cstring.h>
#include<XCBase/future.hpp>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"System_base.hpp"
#include"Device.hpp"
#include"Message_base.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename co2_device_>
			struct cCO2 : public co2_device_{
				typedef cCO2<co2_device_> this_type;
			private:
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
			private:
				//モード通知受領クラス
				struct system_client : public system_client_interface{
				private:
					this_type& Ref;
				public:
					void operator()(system::mode::type NewMode_, system::mode::type PreMode_){

					}
				};
			private:
				//通信受領クラス
				struct message_client : public message_client_interface{
				private:
					this_type& Ref;
				public:
					void setup_talk(void){
						Ref.Mode.Data = Ref.Mode.SendData;
					}
					bool talk(hmLib::cstring* pStr){
						uint16 sumadc = 0;
						if(MRef.ode.SendData_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.Mode.SendData)hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);
							Ref.Mode.SendData_i = false;
							return false;
						} else if(Ref.Mode.PowerSensor_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.Mode.PowerSensor)hmLib::cstring_putc(pStr, 0, 0x20);
							else hmLib::cstring_putc(pStr, 0, 0x21);
							Ref.Mode.PowerSensor_i = false;
							return false;
						} else if(Ref.Mode.PowerPump_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.Mode.PowerPump)hmLib::cstring_putc(pStr, 0, 0x30);
							else hmLib::cstring_putc(pStr, 0, 0x31);
							Ref.Mode.PowerPump_i = false;
							return false;
						} else if(Ref.Mode.Data){
							service::cstring_construct_safe(pStr, 3);
							hmLib::cstring_putc(pStr, 0, 0x00);
							// CO2データ取得
							sumadc = 0xFFFF;
							if(FutureData.valid()){
								if(FutureData.can_get())sumadc = FutureData.get();
							}
							hmLib::cstring_putc(pStr, 1, (unsigned char)(sumadc & 0x00FF));
							hmLib::cstring_putc(pStr, 2, (unsigned char)((sumadc >> 8) & 0x00FF));
							Ref.Mode.Data = false;
							return false;
						}
						return true;
					}
					void setup_listen(void){ return; }
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10://sensor on
							Ref.Mode.SendData = true;
							Ref.Mode.SendData_i = true;
							return false;
						case 0x11://sensor on
							Ref.Mode.SendData = false;
							Ref.Mode.SendData_i = true;
							return false;
						case 0x20://sensor on
							Ref.Mode.PowerSensor = true;
							Ref.Mode.PowerSensor_i = true;
							Ref.PinPowerSensor(true);
							return false;
						case 0x21://sensor on
							Ref.Mode.PowerSensor = false;
							Ref.Mode.PowerSensor_i = true;
							Ref.PinPowerSensor(false);
							return false;
						case 0x30://pump on
							Ref.Mode.PowerPump = true;
							Ref.Mode.PowerPump_i = true;
							Ref.PinPowerPump(true);
							return false;
						case 0x31://pump off
							Ref.Mode.PowerPump = false;
							Ref.Mode.PowerPump_i = true;
							Ref.PinPowerPump(false);
							return false;
						default:
							return true;
						}
					}
				};
				message_client MessageClient;
				message::element MessageElement;
			private:
				//ロックフラグ
				bool Lock;
			private:
				apinData ApinData;
				powerPump PinPowerPump;
				powerSensor PinPowerSensor;
			public:
				cCO2(unsigned char ID_):MessageElement(message_client_holder(ID_,MessageClient)){}
				bool lock(){
					if(is_lock())return false;
					Lock = true;

					//pin設定
					ApinData.lock(xc32::sfr::adc::vref_mode::vref_Vref_Gnd, 1);
					PinPowerPump.lock();
					PinPowerSensor.lock();

					//タスク登録
					service::task::quick_start(InformTask, 5);
					service::task::quick_start(DataTask, 5);

					return false;
				}
				bool is_lock()const{ return Lock; }
				void unlock(){
					Lock = false;

					//pin設定
					ApinData.unlock();
					PinPowerPump.unlock();
					PinPowerSensor.unlock();

					//タスク停止
					service::task::stop(InformTask);
					service::task::stop(DataTask);
				}
			public:
				void regist_message(message_host_interface& Host_){
					Host_.regist(MessageElement);
				}
			};
		}
	}
}
#
#endif
