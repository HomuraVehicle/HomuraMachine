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
#include"Message_base.hpp"
#include"Device.hpp"
namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename co2_device_>
			struct cCO2 : public co2_device_{
				typedef cCO2<co2_device_> this_type;
			private:
				//ロックフラグ
				bool Lock;
			private:
				//ピンフラグ
				apinData ApinData;
				powerPump PinPowerPump;
				powerSensor PinPowerSensor;
			private:
				//モード
				bool SendDataMode;
				bool SendDataMode_i;
				bool PowerPumpMode;
				bool PowerPumpMode_i;
				bool PowerSensorMode;
				bool PowerSensorMode_i;
				xc32::future<uint16> FutureData;
			public:
				//タスク
				struct data_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					data_task(this_type& Ref_) :Ref(Ref_){}
					duration operator()(duration dt){
						if(SendDataMode){
							Ref.requestData();
						}
						return dt;
					}
				}DataTask;
				struct inform_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					inform_task(this_type& Ref_) :Ref(Ref_){}
					duration operator()(duration dt){
						Ref.SendDataMode_i = true;
						Ref.PowerPumpMode_i = true;
						Ref.PowerSensorMode_i = true;
						return dt;
					}
				}InformTask;
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
						default:
							Ref.PinPowerPump(false);
							Ref.PinPowerSensor(false);
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrrentMode; }
				}SystemClient;
				systems::element SystemElement;
			private:
				//通信受領クラス
				struct message_client : public message_client_interface{
				private:
					this_type& Ref;
				public:
					message_client(this_type& Ref_) :Ref(Ref_){}
				public:
					void setup_talk(void){ return; }
					bool talk(hmLib::cstring* pStr){
						if(Ref.SendDataMode_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.SendDataMode)hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);
							Ref.SendDataMode_i = false;
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
						} else if(Ref.FutureData.can_get()){
							service::cstring_construct_safe(pStr, 3);
							hmLib::cstring_putc(pStr, 0, 0x00);
							// CO2データ取得
							uint16 Data = Ref.FutureData.get();
							hmLib::cstring_putc(pStr, 1, (unsigned char)(Data & 0x00FF));
							hmLib::cstring_putc(pStr, 2, (unsigned char)((Data >> 8) & 0x00FF));
							return false;
						}
						return true;
					}
					void setup_listen(void){ return; }
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10://SendDataMode ON
							Ref.SendDataMode = true;
							Ref.SendDataMode_i = true;
							return false;
						case 0x11://SendDataMode OFF
							Ref.SendDataMode = false;
							Ref.SendDataMode_i = true;
							return false;
						case 0x20://sensor on
							Ref.Mode.PowerSensor = true;
							Ref.Mode.PowerSensor_i = true;
							Ref.powerSensor(true);
							return false;
						case 0x21://sensor on
							Ref.Mode.PowerSensor = false;
							Ref.Mode.PowerSensor_i = true;
							Ref.powerSensor(false);
							return false;
						case 0x30://pump on
							Ref.Mode.PowerPump = true;
							Ref.Mode.PowerPump_i = true;
							Ref.powerPump(true);
							return false;
						case 0x31://pump off
							Ref.Mode.PowerPump = false;
							Ref.Mode.PowerPump_i = true;
							Ref.powerPump(false);
							return false;
						default:
							return true;
						}
					}
				}MessageClient;
				message::element MessageElement;
			public:
				cCO2(unsigned char ID_)
					: Lock(false)
					, SendDataMode(false)
					, SendDataMode_i(false)
					, PowerPumpMode(false)
					, PowerPumpMode_i(false)
					, PowerSensorMode(false)
					, PowerSensorMode_i(false)
					, DataTask(*this)
					, InformTask(*this)
					, SystemClient(*this)
					, SystemElement(system_client_holder(SystemClient))
					, MessageClient(*this)
					, MessageElement(message_client_holder(ID_,MessageClient)){
				}
				bool lock(system_host_interface& SystemHost_, message_host_interface& MessageHost_){
					if(is_lock())return false;
					Lock = true;

					//pin設定
					ApinData.lock(xc32::sfr::adc::vref_mode::vref_Vref_Gnd, 1);
					PinPowerPump.lock();
					PinPowerSensor.lock();

					//タスク登録
					service::task::quick_start(InformTask, 5);
					service::task::quick_start(DataTask, 5);

					//Client登録
					SystemHost_.regist(SystemElement);
					MessageHost_.regist(MessageElement);

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
				bool requestData(){
					if(!FutureData.valid())return true;
					FutureData = ApinData(100);
					return false;
				}
				void powerSensor(bool OnOff){
					if(SystemClient.mode()==systems::mode::drive)PinPowerSensor(OnOff);
				}
				void powerPump(bool OnOff){
					if(SystemClient.mode() == systems::mode::drive)PinPowerPump(OnOff);
				}
			};
		}
	}
}
#
#endif
