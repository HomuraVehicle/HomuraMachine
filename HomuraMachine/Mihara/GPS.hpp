#ifndef HMR_MACHINE_MIHARA_GPS_INC
#define HMR_MACHINE_MIHARA_GPS_INC 100
#
/*
GPS関連モジュール
=== GPS ===
v1_00/141115 hmIto
	CPP化
===hmrGPS===
v0_03/131108 hmIto
	タスクにのってGPSが実際に切り替えられまくる問題を修正
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v0_01/130105 iwahori
	work関数をsetup_lisenとsetup_talkに分離
v0_00/121208 hmIto
	基本関数作成
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include<homuraLib_v2/machine/module/GPS52D.hpp>
#include<homuraLib_v2/machine/module/GPSSwitcher.hpp>
#include<HomuraMachine/Mihara/Device.hpp>
#include"System_base.hpp"
#include"Message_base.hpp"
#include"Device.hpp"
#include<XCBase/future.hpp>

namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename gps_device_>
			class cGPS:public gps_device_{
			private:
				using this_type = cGPS<gps_device_>;
			private:
				class gps_manager:public system_client_interface{
				private:
					using gps1 = module::cGPS52D<typename gps_device_::gps1_uart_register,typename gps_device_::gps1_power>;
					using gps2 = module::cGPS52D<typename gps_device_::gps2_uart_register,typename gps_device_::gps2_power>;
					using gps_switcher = module::cGPSSwitcher<gps1,gps2>;
					gps1 GPS1;
					gps2 GPS2;
					gps_switcher GPSSwitcher;
				public:
					using gps_ch =  gps_switcher::gps_ch;
				private:
					bool GPSPower;
					systems::mode::type CurrentMode;
				public:
					systems::mode::type mode()const{ return CurrentMode; }
					bool getGPSPower()const{ return GPSPower; }
					void setGPSPower(bool OnOff_){
						if((GPSPower != OnOff_) && CurrentMode == systems::mode::drive){
							if(OnOff_)GPSSwitcher.lock();
							else GPSSwitcher.unlock();
						}
						GPSPower = OnOff_;
					}
				public://override function of system_client_interface
					void operator()(systems::mode::type NewMode_,systems::mode::type PreMode_)override{
						switch(NewMode_){
						case systems::mode::drive:
							if(GPSPower)GPSSwitcher.lock();
							else GPSSwitcher.unlock();
							break;
						default:
							GPSSwitcher.unlock();
							break;
						}
						CurrentMode = NewMode_;
					}
				public://wrapper function of GPSSwithcer
					void setChannel(gps_ch GPSCh_){ GPSSwitcher.setCh(GPSCh_); }
					gps_ch getChannel()const{ return GPSSwitcher.getCh(); }
					unsigned char getChannelNo()const{ return GPSSwitcher.getChNo(); }
					bool swapChannel(){ GPSSwitcher.swapCh(); }
					bool roamChannel(){ GPSSwitcher.roamCh(); }
					bool can_read(){ return GPSSwitcher.can_read(); }
					gps::data read(){ return GPSSwitcher.read(); }
					gps::ch_data_pair read_withCh(){ return GPSSwitcher.read_withCh(); }
				public:
					gps_manager():GPS1(),GPS2(),GPSSwitcher(GPS1,GPS2),GPSPower(false){}
				};
				gps_manager GPSManager;
				systems::element SystemElement;
			private:
				class message_client:public message_client_interface{
				private:
					this_type& Ref;
				private:
					bool DataMode_i;
					bool Swap_i;
					bool PowerGPS_i;

					bool SendData;
					gps::data Data;
				private:
					class inform_task:public hmr::task::client_interface{
					private:
						message_client& Ref;
					public:
						inform_task(message_client& Ref_):Ref(Ref_){}
						duration operator()(duration dt){
							Ref.DataMode_i = true;
							return dt;
						}
					};
					inform_task InformTask;
				public:
					message_client(this_type& Ref_)
						: Ref(Ref_)
						, DataMode_i(true)
						, Swap_i(false)
						, PowerGPS_i(false)
						, SendData(false){
						
						service::task::quick_start(InformTask, 5);
					}
					~message_client(){
						service::task::stop(InformTask);
					}
				public:
					bool listen(hmLib::cstring Str) {
						//データサイズ確認
						if(hmLib::cstring_size(&Str)==0)return true;

						//1byte目でモードを分ける
						switch(hmLib::cstring_getc(&Str,0)) {
						case 0x10:
							Ref.DataMode=true;
							DataMode_i=true;
							return false;
						case 0x11:
							Ref.DataMode=false;
							DataMode_i=true;
							return false;
						case 0x20:
							//データサイズ確認
							if(hmLib::cstring_size(&Str)!=2)return true;
							if(hmLib::cstring_getc(&Str,1)!=Ref.GPSManager.getChannelNo()-1) {
								Swap_i=true;
							}
							return false;
						case 0x30:
							Ref.GPSManager.setGPSPower(true);
							PowerGPS_i=true;
							return false;
						case 0x31:
							Ref.GPSManager.setGPSPower(false);
							PowerGPS_i=true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr) {
						if(DataMode_i) {
							service::cstring_construct_safe(pStr,1);
							if(Ref.DataMode)hmLib::cstring_putc(pStr,0,0x10);
							else hmLib::cstring_putc(pStr,0,0x11);
							//送信後自動的にフラグを落とす
							DataMode_i=false;
							return false;
						}
						else if(Swap_i) {
							service::cstring_construct_safe(pStr,1+1);
							hmLib::cstring_putc(pStr,0,0x20);

							Ref.GPSManager.swapChannel();
							hmLib::cstring_putc(pStr,1,Ref.GPSManager.getChannelNo()-1);

							//送信後自動的にフラグを落とす
							Swap_i=false;
							return false;
						}
						else if(PowerGPS_i) {
							service::cstring_construct_safe(pStr,1);

							if(Ref.GPSManager.getGPSPower())hmLib::cstring_putc(pStr,0,0x30);
							else hmLib::cstring_putc(pStr,0,0x31);

							//送信後自動的にフラグを落とす
							PowerGPS_i=false;
							return false;
						}
						else if(SendData) {
							service::cstring_construct_safe(pStr,1+20);
							hmLib::cstring_putc(pStr,0,0x00);

							unsigned char Ch=Ref.GPSManager.getChannelNo()-1;

							hmLib::cstring_putc(pStr,1,static_cast<unsigned char>(Data.getHour()));
							hmLib::cstring_putc(pStr,2,static_cast<unsigned char>(Data.getMin()));
							hmLib::cstring_putc(pStr,3,static_cast<unsigned char>(Data.getSec()));
							hmLib::cstring_putc(pStr,4,static_cast<unsigned char>(Data.getMSec()/10));
							hmLib::cstring_putc(pStr,5,static_cast<unsigned char>((Data.getMSec()%10)*10));

							double Val = Data.getLatitude();
							Val = (Val > 0 ? Val : -Val);

							hmLib::cstring_putc(pStr,6,static_cast<unsigned char>(static_cast<unsigned int>(Val) % 90));
							hmLib::cstring_putc(pStr,7,static_cast<unsigned char>(static_cast<unsigned int>(Val*60)%60));
							hmLib::cstring_putc(pStr,8,static_cast<unsigned char>(static_cast<unsigned int>(Val*6000) % 100));
							hmLib::cstring_putc(pStr,9,static_cast<unsigned char>(static_cast<unsigned int>(Val*600000) % 100));
							hmLib::cstring_putc(pStr,10,static_cast<unsigned char>(Data.getLatitude()>0? 'N':'S'));

							Val = Data.getLongitude();
							Val = (Val > 0 ? Val : -Val);

							hmLib::cstring_putc(pStr,11,static_cast<unsigned char>(static_cast<unsigned int>(Val)%180));
							hmLib::cstring_putc(pStr,12,static_cast<unsigned char>(static_cast<unsigned int>(Val * 60) % 60));
							hmLib::cstring_putc(pStr,13,static_cast<unsigned char>(static_cast<unsigned int>(Val * 6000) % 100));
							hmLib::cstring_putc(pStr,14,static_cast<unsigned char>(static_cast<unsigned int>(Val * 600000) % 100));
							hmLib::cstring_putc(pStr,15,static_cast<unsigned char>(Data.getLatitude()>0 ? 'E' : 'W'));

							hmLib::cstring_putc(pStr,16,static_cast<unsigned char>(Data.is_valid()));

							Val = Data.getAltitude();
							Val = (Val > 0 ? Val : -Val);

							hmLib::cstring_putc(pStr,17,static_cast<unsigned char>(static_cast<unsigned int>(Val/1000)%256));
							hmLib::cstring_putc(pStr,18,static_cast<unsigned char>(static_cast<unsigned int>(Val/10) % 100));
							hmLib::cstring_putc(pStr,19,static_cast<unsigned char>(static_cast<unsigned int>(Val*10) % 100));

							hmLib::cstring_putc(pStr,20,Ch);

							SendData = false;

							return false;
						}

						return true;
					}
					void setup_listen(void) { return; }
					void setup_talk(void) { return; }
				public:
					void setSendData(const gps::data& data_){
						Data = data_;
						SendData = true;
					}
				};
				message_client MessageClient;
				message::element MessageElement;
			private:
				bool DataMode;
			private:
				class data_task:public hmr::task::client_interface{
					this_type& Ref;
				public:
					data_task(this_type& Ref_):Ref(Ref_){}
					duration operator()(duration dt){
						if(DataMode && Ref.GPSManager.can_read()){
							Ref.MessageClient.setSendData(Ref.GPSManager.read());
						}
					}
				};
				data_task DataTask;
			public:
				cGPS(unsigned char ID_,system_host& SystemHost_,message_host& MessageHost_)
					:GPSManager()
					,SystemElement(system_client_holder(GPSManager))
					,MessageClient(*this)
					,MessageElement(message_client_holder(ID_, MessageClient))
					,DataMode(false)
					,DataTask(*this){
					SystemHost_.regist(SystemElement);
					MessageHost_.regist(MessageElement);

					service::task::quick_start(DataTask,3);
				}
				~cGPS(){
					service::task::stop(DataTask);
				}
			};
		}
	}
}

#
#endif
