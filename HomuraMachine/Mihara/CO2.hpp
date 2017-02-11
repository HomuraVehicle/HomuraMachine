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
#include"IO_base.hpp"
#include"Service_base.hpp"
#include"Device.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename co2_device_>
			struct cCO2 : public co2_device_{
				typedef cCO2<co2_device_> this_type;
			private:
				//ピンフラグ
				apinData ApinData;
				setPumpPower PinPowerPump;
				setSensorPower PinPowerSensor;
			private:
				//モード
				bool DataMode;
				bool PowerPumpMode;
				bool PowerSensorMode;
				xc32::future<uint16> FutureData;
			private:
				void setDataMode(bool OnOff){
					DataMode = OnOff;
				}
				void setPowerSensor(bool OnOff){
					PowerSensorMode = OnOff;
					if(SystemClient.mode() == systems::mode::drive)PinPowerSensor(OnOff);
				}
				void setPowerPump(bool OnOff){
					PowerPumpMode = OnOff;
					if(SystemClient.mode() == systems::mode::drive)PinPowerPump(OnOff);
				}
				bool getDataMode()const{ return DataMode; }
				bool getPowerSensor()const{ return PowerPumpMode; }
				bool getPowerPump()const{ return PowerSensorMode; }
			private:
				//タスク
				struct data_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					data_task(this_type& Ref_) :Ref(Ref_){}
					duration operator()(duration dt){
						if(Ref.DataMode){
							Ref.FutureData = Ref.ApinData(100);
						}
						return dt;
					}
				}DataTask;
				task::handler DataTaskHandler;
			private:
				//モード通知受領クラス
				struct system_client : public system_client_interface{
				private:
					this_type& Ref;
					systems::mode::type CurrentMode;
				public:
					system_client(this_type& Ref_) :Ref(Ref_){}
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_){
						switch(NewMode_){
						case systems::mode::drive:
							Ref.PinPowerPump(PowerPumpMode);
							Ref.PinPowerSensor(PowerSensorMode);
							break;
						default:
							Ref.PinPowerPump(false);
							Ref.PinPowerSensor(false);
							break;
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrrentMode; }
				}SystemClient;
			private:
				//通信受領クラス
				struct message_client : public message_client_interface{
				private:
					this_type& Ref;
				private:
					bool DataMode_i;
					bool PowerPumpMode_i;
					bool PowerSensorMode_i;
					bool SendData_i;
					uint16 SendData;
				private:
					struct inform_task :public hmr::task::client_interface{
					private:
						message_client& Ref;
					public:
						inform_task(message_client& Ref_) :Ref(Ref_){}
						duration operator()(duration dt){
							Ref.DataMode_i = true;
							Ref.PowerPumpMode_i = true;
							Ref.PowerSensorMode_i = true;
							return dt;
						}
					}InformTask;
					task::handler InformTaskHandler;
				public:
					message_client(this_type& Ref_, com::did_t ID_, service_interface& Service_)
						: message_client_interface(ID_)
						, Ref(Ref_)
						, DataMode_i(true)
						, PowerPumpMode_i(true)
						, PowerSensorMode_i(true)
						, SendData_i(false)
						, SendData(0)
						, InformTask(*this){
						//タスク登録
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~message_client(){
						InformTaskHandler.stop();
					}
				public:
					void setup_talk(void){ return; }
					bool talk(hmLib::cstring* pStr){
						if(DataMode_i){
							DataMode_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getDataMode())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);
							return false;
						} else if(PowerSensor_i){
							PowerSensorMode_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getSensorPower())hmLib::cstring_putc(pStr, 0, 0x20);
							else hmLib::cstring_putc(pStr, 0, 0x21);
							return false;
						} else if(PowerPump_i){
							PowerPumpMode_i = false;

							service::cstring_construct_safe(pStr, 1);
							if(Ref.getPumpPower())hmLib::cstring_putc(pStr, 0, 0x30);
							else hmLib::cstring_putc(pStr, 0, 0x31);
							return false;
						} else if(SendData_i){
							SendData_i = false;

							service::cstring_construct_safe(pStr, 3);
							hmLib::cstring_putc(pStr, 0, 0x00);
							// CO2データ取得
							hmLib::cstring_putc(pStr, 1, (unsigned char)(SendData & 0x00FF));
							hmLib::cstring_putc(pStr, 2, (unsigned char)((SendData >> 8) & 0x00FF));
							return false;
						}
						return true;
					}
					void setup_listen(void){ return; }
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10://DataMode ON
							DataMode_i = true;
							Ref.setDataMode(true);
							return false;
						case 0x11://DataMode OFF
							DataMode_i = true;
							Ref.setDataMode(false);
							return false;
						case 0x20://sensor on
							PowerSensorMode_i = true;
							Ref.setSensorPower(true);
							return false;
						case 0x21://sensor on
							PowerSensorMode_i = true;
							Ref.setSensorPower(false);
							return false;
						case 0x30://pump on
							PowerPumpMode_i = true;
							Ref.setPumpPower(true);
							return false;
						case 0x31://pump off
							PowerPumpMode_i = true;
							Ref.setPumpPower(false);
							return false;
						default:
							return true;
						}
					}
				public:
					void setSendData(uint16 SendData_){
						SendData_i = true;
						SendData = SendData_;
					}
				}MessageClient;
			public:
				cCO2(unsigned char ID_, system_interface& System_, io_interface& IO_, service_interface& Service_)
					: DataMode(false)
					, PowerPumpMode(false)
					, PowerSensorMode(false)
					, DataTask(*this)
					, SystemClient(*this)
					, MessageClient(*this, ID_, Service_){

					//pin設定
					ApinData.lock(xc32::sfr::adc::vref_mode::vref_Vref_Gnd, 1);
					PinPowerPump.lock();
					PinPowerSensor.lock();

					//タスク登録
					DataTaskHandler = Service_.task().quick_start(DataTask, 5);
						
					//Client登録
					System_.regist(SystemClient);
					IO_.regist(MessageClient);

				}
				~cCO2(){
					//タスク停止
					DataTaskHandler.stop(DataTask);
				}
				void operator()(void){
					if(!FutureData.valid())return 0xffff;
					if(!FutureData.can_get()return 0xffff;
					MessageClient.setSendData(FutureData.get(););
				}
			};
		}
	}
}
#
#endif
