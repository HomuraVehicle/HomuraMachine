#ifndef HMR_MACHINE_MIHARA_BATTERY_INC
#define HMR_MACHINE_MIHARA_BATTERY_INC 100
#
/*
バッテリー管理用コマンド

=== Battery ===

=== hmrBattery ===
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
			template<typename battery_device_>
			struct cBattery :public battery_device_{
				using this_type = battery_device_;
			private:
				apinData1 ApinData1;
				apinData2 ApinData2;
				apinData3 ApinData3;
			private:
				bool DataMode;
				xc32::future<uint16> FututeData1;
				xc32::future<uint16> FututeData2;
				xc32::future<uint16> FututeData3;
			private:
				void setDataMode(bool OnOff_){ DataMode = OnOff_; }
				bool getDataMode()const{ return DataMode; }
			private:
				//タスク
				struct data_task :public hmr::task::client_interface{
				private:
					this_type& Ref;
				public:
					data_task(this_type& Ref_):Ref(Ref_){}
					duration operator()(duration dt){
						if(Ref.DataMode){
							Ref.FutureData1 = Ref.ApinData1(100);
							Ref.FutureData2 = Ref.ApinData2(100);
							Ref.FutureData3 = Ref.ApinData3(100);
						}
						return dt;
					}
				}DataTask;
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
							break;
						default:
							break;
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrrentMode; }
				}SystemClient;
				systems::element SystemElement;
				//通信受領クラス
				struct message_client : public message_client_interface{
				private:
					this_type& Ref;
				private:
					bool DataMode_i;
					bool SendData_i;
					uint16 SendData1;
					uint16 SendData2;
					uint16 SendData3;
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
						, SendData1(0)
						, SendData2(0)
						, SendData3(0)
						, InformTask(*this){
						//タスク登録
						task::quick_start(InformTask, 5);
					}
					~message_client(){
						task::stop(InformTask);
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
						} else if(SendData_i){
							SendData_i = false;

							service::cstring_construct_safe(pStr, 1 + 2 * 3);
							hmLib::cstring_putc(pStr, 0, 0x00);

							//データ取得
							hmLib::cstring_putc(pStr, 1, (hmLib_uint8)(SendData1 & 0x00FF));		//Low
							hmLib::cstring_putc(pStr, 2, (hmLib_uint8)((SendData1 >> 8) & 0x00FF));	//High

							hmLib::cstring_putc(pStr, 3, (hmLib_uint8)(SendData2 & 0x00FF));		//Low
							hmLib::cstring_putc(pStr, 4, (hmLib_uint8)((SendData2 >> 8) & 0x00FF));	//High

							hmLib::cstring_putc(pStr, 5, (hmLib_uint8)(SendData3 & 0x00FF));		//Low
							hmLib::cstring_putc(pStr, 6, (hmLib_uint8)((SendData3 >> 8) & 0x00FF));	//High

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
						case 0x11:
							DataMode_i = false;
							Ref.setDataMode(false);
							return false;
						default:
							return true;
						}
					}
				public:
					void setSendData(uint16 SendData1_, uint16 SendData2_, uint16 SendData3_){
						SendData_i = true;
						SendData1 = SendData1_;
						SendData2 = SendData2_;
						SendData3 = SendData3_;
					}
				}MessageClient;
				message::element MessageElement;
			public:
				cBattery(unsigned char ID_, system_host& SystemHost_, message_host& MessageHost_)
					: DataMode(false)
					, DataTask(*this)
					, SystemClient(*this)
					, SystemElement(system_client_holder(SystemClient))
					, MessageClient(*this)
					, MessageElement(message_client_holder(ID_, MessageClient)){

					//pin設定
					ApinData1.lock();
					ApinData2.lock();
					ApinData3.lock();

					//タスク登録
					task::quick_start(DataTask, 5);

					//Client登録
					SystemHost_.regist(SystemElement);
					MessageHost_.regist(MessageElement);
				}
				~cBattery(){
					//タスク停止
					task::stop(DataTask);
					
					ApinData1.unlock();
					ApinData2.unlock();
					ApinData3.unlock();
				}
				void operator()(void){
					if(FutureData1.valid() && FutureData1.can_get()
						&& FutureData2.valid() && FutureData2.can_get()
						&& FutureData3.valid() && FutureData3.can_get()){
						MessageClient.setSendData(FutureData1.get(), FutureData2.get(), FutureData3.get());
					}
				}

			};
		}
	}
}
#
#endif
