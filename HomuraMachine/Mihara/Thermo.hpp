#ifndef HMR_MACHINE_MIHARA_THERMO_INC
#define HMR_MACHINE_MIHARA_THERMO_INC 100
#
/*
バッテリー管理用コマンド
=== hmr::machine::mihara::thermo ===
v1_00/141121 hmIto
	cpp化
=== hmrThermo ===
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v0_01/130112 iwahori
	work関数をsetup_listenとsetup_talkに分離
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
			template<typename thermo_device_>
			struct cThermo :public thermo_device_{
			private:
				using this_type = cThermo<thermo_device_>;
			private:
				//analog pin
				apinData ApinData;
			private:
				//mode of sensor
				bool DataMode;
				xc32::future<uint16> FutureData;
			private:
				void setDataMode(bool OnOff){ DataMode = OnOff; }
				bool getDataMode()const{ return DataMode; }
			private:
				//task client
				struct data_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					data_task(this_type& Ref_):Ref(Ref_){}
					duration operator()(duration dt){
						if(Ref.DataMode){
							Ref.FutureData = Ref.ApinData(100);
						}
						return dt;
					}
				}DataTask;
			private:
				//モード通知受領クラス
				struct system_client :public system_client_interface{
				private:
					this_type& Ref;
					systems::mode::type CurrentMode;
				public:
					system_client(this_type& Ref_):Ref(Ref_){}
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_){
						switch(NewMode_){
						case systems::mode::drive:
							break;
						default:
							break;
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrentMode; }
				}SystemClient;
				systems::element SystemElement;
			private:
				//通信受領クラス
				struct message_client :public message_client_interface{
				private:
					this_type& Ref;
				private:
					bool DataMode_i;
					bool SendData_i;
					uint16 SendData;
				private:
					struct inform_task :public hmr::task::client_interface{
					private:
						message_client& Ref;
					public:
						inform_task(message_client& Ref_):Ref(Ref_){}
						duration operator()(duration dt){
							Ref.DataMode_i = true;
							return dt;
						}
					}InformTask;
				public:
					message_client(this_type& Ref_)
						: Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, SendData(0)
						, InformTask(*this){
						service::task::quick_start(InformTask, 5);
					}
					~message_client(){
						service::task::stop(InformTask);
					}
				public:
					void setup_listen(void)override{ return; }
					void setup_talk(void)override{ return; }
					bool talk(hmLib::cstring* pStr){

						if(DataMode_i){
							DataMode_i = false;
							service::cstring_construct_safe(pStr, 1);
							if(Ref.getDataMode())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);

							return false;
						} else if(SendData_i){
							SendData_i = false;
							service::cstring_construct_safe(pStr, 3);
							hmLib::cstring_putc(pStr, 0, 0x00);

							//温度データ取得
							hmLib::cstring_putc(pStr, 1, (unsigned char)(SendData % 256));
							hmLib::cstring_putc(pStr, 2, (unsigned char)((SendData >> 8) % 256));
							return false;
						}

						return true;
					}
					bool listen(hmLib::cstring Str){

						//データサイズ確認
						if(hmLib::cstring_size(&Str) == 0)return true;

						//1byte目でモードを分ける
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10:
							DataMode_i = true;
							Ref.setDataMode(true);
							return false;
						case 0x11:
							DataMode_i = false;
							Ref.setDataMode(false);
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
				message::element MessageElement;
				public:
					cThermo(unsigned char ID_, system_host& SystemHost_, message_host& MessageHost_)
						: DataMode(false)
						, DataTask(*this)
						, SysemClient(*this)
						, SystemElement(system_client_holder(SystemClient))
						, MessageClient(*this)
						, MessageElement(message_client_holder(ID_, MessageClient)){

						ApinData.lock();
						service::task::quick_start(DataTasd, 5);
						SystemHost_.regist(SystemElement);
						MessageHost_.regist(MessageElement);
					}
					~cThermo(){
						service::task::stop(DataTask);
						ApinData.unlock();
					}
					void operator()(){
						if(FututeData.valid() && FutureData.can_get()){
							MessageClient.setSendData(FutureData.get());
						}
					}
			};

			namespace thermo {
				//batttery監視用関数群
				void initialize();
				void finalize();
				bool listen(hmLib::cstring Str);
				bool talk(hmLib::cstring* pStr);
				void setup_listen(void);
				void setup_talk(void);
				void task_setup_listen(void);
				void task_setup_talk(void);
			}
		}
	}
}
#
#endif
