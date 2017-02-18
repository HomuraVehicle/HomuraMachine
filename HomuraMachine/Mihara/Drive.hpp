#ifndef HMR_MACHINE_MIHARA_DRIVE_INC
#define HMR_MACHINE_MIHARA_DRIVE_INC 100
#
/*
===hmrDrive===
モーター、マニュピュレータ系をコントロール

=== hmr::machine::mihara::drive ===
v1_00/141121 hmIto
	cpp化
=== hmrDrive ===
v0_00/130112 iwahori
	ファイルを作成・モーターのメッセージ関数をtalkとlistenに分解
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
			template<typename motor_device_>
			class cMotor :public motor_device_{
				using this_type = cMotor<motor_device_>;
			private:
				pinMotorLA PinMotorLA;
				pinMotorLB PinMotorLB;
				pinMotorRA PinMotorRA;
				pinMotorRB PinMotorRB;
				pinMotorPower PinMotorPower;
			private:
				bool MotorPower;
			private:
				void setMotorPower(bool OnOff_){
					MotorPower = OnOff_;
					if(SystemClient.mode() == systems::mode::drive)PinMotorPower(MotorPower);
				}
				bool getMotoPower()const{ return MotorPower; }
			private:
				//通信断絶時の自動停止用watch dog count
				uint16 wdt_count;
				void increment_wdt_count(){ ++wdt_count; }
				void clear_wdt_count(){ wdt_count = 0; }
				uint16 get_wtd_count()const{ return wdt_count; }
			private:
				struct wdt_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					wdt_task(this_type& Ref_):Ref(Ref_){}
					duration operator()(duration dt){
						Ref_.increment_wdt_count();

						if(Ref.get_wtd_count() >= 2){
							Ref.clear_wdt_count();
							Ref.PinMotorLA(0);
							Ref.PinMotorLB(0);
							Ref.PinMotorRA(0);
							Ref.PinMotorRB(0);
						}
						return dt;
					}
				}WdtTask;
				task::handler WdtTaskHandler;
			private:
				//モード通知受領クラス
				struct system_client : public system_client_interface{
				private:
					this_type& Ref;
					systems::mode::type CurrentMode;
				public:
					system_client(this_type& Ref_):Ref(Ref_){}
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_){
						switch(NewMode_){
						case systems::mode::drive:
							Ref.PinMotorLA(0);
							Ref.PinMotorLB(0);
							Ref.PinMotorRA(0);
							Ref.PinMotorRB(0);
							Ref.PinMotorPower(Ref.MotorPower);
							break;
						default:
							Ref.PinMotorLA(0);
							Ref.PinMotorLB(0);
							Ref.PinMotorRA(0);
							Ref.PinMotorRB(0);
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
				public:
					message_client(this_type& Ref_, com::did_t ID_)
						: message_client_interface(ID_)
						, Ref(Ref_){}
					~message_client(){}
				public:
					void setup_talk(void){ return; }
					bool talk(hmLib::cstring* pStr){ return true; }
					void setup_listen(void){ return; }
					bool listen(hmLib::cstring Str){
						//左車輪制御
						if(cstring_getc(&Str, 1) != 0x00){
							Ref.PinMotorLA(1);
							Ref.PinMotorLB(1);
						} else if(cstring_getc(&Str, 0) == 0x64){
							Ref.PinMotorLA(1);
							Ref.PinMotorLB(0);
						} else if(cstring_getc(&Str, 0) == 0x9C){
							Ref.PinMotorLA(0);
							Ref.PinMotorLB(1);
						} else{
							Ref.PinMotorLA(0);
							Ref.PinMotorLB(0);
						}

						//右車輪制御
						if(cstring_getc(&Str, 3) != 0x00){
							Ref.PinMotorRA(1);
							Ref.PinMotorRB(1);
						} else if(cstring_getc(&Str, 2) == 0x64){
							Ref.PinMotorRA(1);
							Ref.PinMotorRB(0);
						} else if(cstring_getc(&Str, 2) == 0x9C){
							Ref.PinMotorRA(0);
							Ref.PinMotorRB(1);
						} else{
							Ref.PinMotorRA(0);
							Ref.PinMotorRB(0);
						}

						Ref.clear_wdt_count();

						return false;
					}

				}MessageClient;
			public:
				cMotor(unsigned char ID_, system_interface& System_, io_interface& IO_, service_interface& Service_)
					: wdt_count(0)
					, WdtTask(*this)
					, SystemClient(*this)
					, MessageClient(*this, ID_){

					PinMotorLA.lock();
					PinMotorLB.lock();
					PinMotorRA.lock();
					PinMotorRB.lock();
					PinMotorPower.lock();
					PinMotorPower(true);
					MotorPower = true;

					WdtTaskHandler = Service_.task().quick_start(WdtTask, 5);

					System_.regist(SystemClient);
					IO_.regist(MessageClient);
				}
				~cMotor(){
					WdtTaskHandler.stop(WdtTask);

					PinMotorLA.unlock();
					PinMotorLB.unlock();
					PinMotorRA.unlock();
					PinMotorRB.unlock();
				}
				void operator()(){}
			};
		}
	}
}
#
#endif

