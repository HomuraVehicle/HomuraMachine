#ifndef HMR_MACHINE_MIHARA_INERTIAL_INC
#define HMR_MACHINE_MIHARA_INERTIAL_INC 200
#
/*
慣性航法モジュール制御用
=== hmr::machine::mihara::inertial ===
v2_00/141122 hmito
	cpp化
=== hmrInertial ===
v0_03/131026 amby
	この場合は電源ON,OFFするたびにI２Cでデバイスを初期化する必要があるので、
	そのための関数device_initialize を追加した。
v0_02/131019 iwahori
	タスクシステム用にtask_setup_talkとtask_interruptを作成
v1_00/130907 iwahori
	kk08移行に伴い，使用するI2Cが変わったためi2c1,i2c2を変更．initializeは完了，talkはまだ
v1_00/130622 hmIto
	ほむらの設置位置に対応するように軸を変更
v0_01/130105 iwahori
	work関数をsetup_lisenとsetup_talkに分離
	Gyroのtalk関数を実装
	KK07でのセンサー変更に伴いinitialze関数とアドレスを変更
v0_00/121208 hmIto
	基本関数作成
*/
#include<hmLib/cstring.h>
#include<homuraLib_v2/type.hpp>
#include<XCBase/future.hpp>
#include<homuraLib_v2/machine/module/GyroL3G4200D.hpp>
#include<homuraLib_v2/machine/module/AcceleCompassLSM303DLH.hpp>
#include<XC32/i2c.hpp>
#include<hmLib/coordinates.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"System_base.hpp"
#include"IO_base.hpp"
#include"Service_base.hpp"
#include"Device.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename inertial_device_>
			class cInertial :public inertial_device_{
			private:
				class sensor_manager :public system_client_interface{
				private:
					class sensor{
				private:
					typedef module::cAcceleCompassLSM303DLH<typename inertial_device_::AcceleCompass_i2c, typename inertial_device_::AccelCompass_shaerd_i2c_identifer> cAcceleCompass;
					typedef module::cGyroL3G4200D<typename inertial_device_::Gyro_i2c, typename inertial_device_::Gyro_shared_i2c_identifer> cGyro;
				private:
					struct cPositionObserver{
					private:
						hmLib::coordinates3D::position Data;
						bool IsData;
					public:
						cPositionObserver():IsData(false){}
						void operator()(hmLib::coordinates3D::position Data_){
							Data = Data_;
							IsData = true;
						}
						bool can_get()const{ return IsData; }
						hmLib::coordinates3D::position get(){
							IsData = false;

							//割り込み禁止
							xc32::interrupt::lock_guard Lock(xc32::interrupt::Mutex);
							return Data;
						}
					};
					struct cAngleObserver{
					private:
						hmLib::coordinates3D::angle Data;
						uint16 Cnt;
					public:
						cAngleObserver():Cnt(0){}
						void operator()(hmLib::coordinates3D::angle Data_){
							Data &= Data_;
							++Cnt;
						}
						bool can_get()const{ return Cnt>0; }
						hmLib::coordinates3D::angle get(){
							//割り込み禁止
							xc32::interrupt::lock_guard Lock(xc32::interrupt::Mutex);
							hmLib::coordinates3D::angle AnsData(0, 0, 0);
							std::swap(Data, AnsData);
							return AnsData;
						}
					};
					struct cGyroRawDataObserver{
					public:
						typedef std::pair<uint16, hmLib::coordinates3D::position> DataType;
					private:
						hmLib::coordinates3D::position RawData3D;
						uint16 Cnt;
					public:
						cGyroRawDataObserver():RawData3D(0, 0, 0), Cnt(0){}
						void operator()(module::gyroL3G4200D::raw_data raw_data_){
							++Cnt;
							RawData3D += hmLib::coordinates3D::position(raw_data_.x, raw_data_.y, raw_data_.z);
						}
						bool can_get()const{ return Cnt>0; }
						std::pair<uint16, hmLib::coordinates3D::position> get(){
							std::pair<uint16, hmLib::coordinates3D::position> tmp = std::make_pair(Cnt, RawData3D);
							Cnt = 0;
							RawData3D = hmLib::coordinates3D::position(0, 0, 0);
							return tmp;
						}
					};
				public:
					cPositionObserver AxelObserver;
					cPositionObserver CompassObserver;
					//cAngleObserver GyroObserver;
					cGyroRawDataObserver GyroObserver;
				private:
					cAcceleCompass AcceleCompass;
					cGyro Gyro;
					hmr::delay_interface* pDelay;
					typename inertial_device_::powerInertial PowerInertial;
					xc::lock_guard<typename inertial_device_::powerInertial> PowerInertialLock;
				private:
					bool GyroDataMode;
					bool AxelCompassDataMode;
				public:
					sensor(service_interface& Service_)
						: AcceleCompass()
						, Gyro()
						, pDelay(&Service_.delay())
						, PowerInertialLock(PowerInertial)
						, GyroDataMode(false)
						, AxelCompassDataMode(false){
						PowerInertial(false);
					}
				public:
					bool lock(){
						if(is_lock())return false;

						PowerInertial(true);
						AcceleCompass.lock(xc32::ref(AxelObserver), xc32::ref(CompassObserver), *pDelay);
						Gyro.lock(xc32::ref(GyroObserver), *pDelay);

						return false;
					}
					void unlock(){
						if(!is_lock())return;

						AcceleCompass.unlock();
						Gyro.unlock();
						PowerInertial(false);
					}
					bool is_lock(){ return (AcceleCompass.is_lock() || Gyro.is_lock()); }
					bool getGyroDataMode()const{ return GyroDataMode; }
					void setGyroDataMode(bool OnOff_){ GyroDataMode = OnOff_; }
					bool getAxelCompassDataMode()const{ return AxelCompassDataMode; }
					void setAxelCompassDataMode(bool OnOff_){ AxelCompassDataMode = OnOff_; }
				public:
					void operator()(void){
						if(AxelCompassDataMode)AcceleCompass();
						if(GyroDataMode)Gyro();
					}
				};
				private:
					sensor Sensor;
					bool SensorPower;
					bool AxelDataMode;
					bool CompassDataMode;
					bool GyroDataMode;
				private:
					systems::mode::type CurrentMode;
				public://override funcition of system_client_interface
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_)override{
						switch(NewMode_){
						case systems::mode::observe:
							if(SensorPower){
								if(!Sensor.is_lock())Sensor.lock();
							}else Sensor.unlock();
							break;
						default:
							Sensor.unlock();
							break;
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrentMode; }
					void setSensorPower(bool OnOff_){
						SensorPower = OnOff_;
						if(CurrentMode == systems::mode::observe){
							if(SensorPower){
								if(!Sensor.is_lock())Sensor.lock();
							} else Sensor.unlock();
						}
					}
					bool getSensorPower()const{ return SensorPower; }
					void setAxelDataMode(bool OnOff_){ 
						AxelDataMode = OnOff_;
						Sensor.setAxelCompassDataMode((AxelDataMode || CompassDataMode));
					}
					bool getAxelDataMode()const{ return AxelDataMode; }
					void setCompassDataMode(bool OnOff_){ 
						CompassDataMode = OnOff_; 
						Sensor.setAxelCompassDataMode((AxelDataMode || CompassDataMode));
					}
					bool getCompassDataMode()const{ return CompassDataMode; }
					void setGyroDataMode(bool OnOff_){ 
						GyroDataMode = OnOff_; 
						Sensor.setGyroDataMode(GyroDataMode);
					}
					bool getGyroDataMode()const{ return GyroDataMode; }
					bool can_get_axel_data()const{ return Sensor.AxelObserver.can_get(); }
					bool can_get_compass_data()const{ return Sensor.CompassObserver.can_get(); }
					bool can_get_gyro_data()const{ return Sensor.GyroObserver.can_get(); }
					hmLib::coordinates3D::position get_axel_data(){
						if(Sensor.AxelObserver.can_get())return Sensor.AxelObserver.get();
						else return hmLib::coordinates3D::position();
					}
					hmLib::coordinates3D::position get_compass_data(){
						if(Sensor.CompassObserver.can_get())return Sensor.CompassObserver.get();
						else return hmLib::coordinates3D::position();
					}
					std::pair<uint16, hmLib::coordinates3D::position> get_gyro_data(){
						if(Sensor.GyroObserver.can_get())return Sensor.GyroObserver.get();
						else return std::make_pair(0, hmLib::coordinates3D::position());
					}
				public:
					sensor_manager(service_interface& Service_)
						: Sensor(Service_)
						, SensorPower(true)
						, AxelDataMode(false)
						, CompassDataMode(false)
						, GyroDataMode(false){
						Sensor.lock();
					}
					~sensor_manager(){
						Sensor.unlock();
					}
				public:
					void operator()(){
						if(CurrentMode == systems::mode::observe && SensorPower){
							Sensor();
						}
					}
				};
				sensor_manager SensorManager;
			private:
				class axel_message_client :public message_client_interface{
				private:
					sensor_manager& Ref;
				private:
					bool DataMode_i;
					bool SendData_i;
					hmLib::coordinates3D::position SendData;
				private:
					class inform_task :public hmr::task::client_interface{
					private:
						axel_message_client& Ref;
					public:
						inform_task(axel_message_client& Ref_):Ref(Ref_){}
						duration operator()(duration dt){
							Ref.DataMode_i = true;
							return dt;
						}
					};
					inform_task InformTask;
					task::handler InformTaskHandler;
				public:
					axel_message_client(sensor_manager& Ref_, com::did_t ID_, service_interface& Service_)
						: message_client_interface(ID_)
						, Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, InformTask(*this){
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~axel_message_client(){
						InformTaskHandler.stop();
					}
				public://override function of messge_client_interface
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10:
							Ref.setAxelDataMode(true);
							DataMode_i = true;
							return false;
						case 0x11:
							Ref.setAxelDataMode(false);
							DataMode_i = true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						if(DataMode_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.getAxelDataMode())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);
							DataMode_i = false;
							return false;
						} else if(SendData_i){
							service::cstring_construct_safe(pStr, 7);
							hmLib::cstring_putc(pStr, 0x00, 0);
							uint16 Data;
							uint8 LowData, HighData;

							//homura-y軸書き込み
							Data = static_cast<xc::uint16>(SendData.x);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 3, LowData);
							hmLib::cstring_putc(pStr, 4, HighData);

							//homura-x軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-SendData.y);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 1, LowData);
							hmLib::cstring_putc(pStr, 2, HighData);

							//homura-z軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-SendData.z);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 5, LowData);
							hmLib::cstring_putc(pStr, 6, HighData);

							SendData_i = false;
							return false;
						}
						return true;
					}
					void setup_listen(void){ return; }
					void setup_talk(void){
						if(Ref.getAxelDataMode() && Ref.can_get_axel_data()){
							SendData_i = true;
							SendData = Ref.get_axel_data();
						}
					}
				};
				axel_message_client AxelMessageClient;
			private:
				class compass_message_client :public message_client_interface{
				private:
					sensor_manager& Ref;
				private:
					bool DataMode_i;
					bool SendData_i;
					hmLib::coordinates3D::position SendData;
				private:
					class inform_task :public hmr::task::client_interface{
					private:
						compass_message_client& Ref;
					public:
						inform_task(compass_message_client& Ref_):Ref(Ref_){}
						duration operator()(duration dt){
							Ref.DataMode_i = true;
							return dt;
						}
					};
					inform_task InformTask;
					task::handler InformTaskHandler;
				public:
					compass_message_client(sensor_manager& Ref_, com::did_t ID_ , service_interface& Service_)
						: message_client_interface(ID_)
						, Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, InformTask(*this){
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~compass_message_client(){
						InformTaskHandler.stop();
					}
				public://override function of messge_client_interface
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10:
							Ref.setCompassDataMode(true);
							DataMode_i = true;
							return false;
						case 0x11:
							Ref.setCompassDataMode(false);
							DataMode_i = true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						if(DataMode_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.getCompassDataMode())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);
							DataMode_i = false;
							return false;
						} else if(SendData_i){
							service::cstring_construct_safe(pStr, 7);
							hmLib::cstring_putc(pStr, 0, 0x00);
							uint8 LowData, HighData;
							uint16 Data;

							//homura-x軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-SendData.y);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 1, LowData);
							hmLib::cstring_putc(pStr, 2, HighData);

							//homura-y軸書き込み
							Data = static_cast<xc::uint16>(SendData.x);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 3, LowData);
							hmLib::cstring_putc(pStr, 4, HighData);

							//homura-z軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-SendData.z);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 5, LowData);
							hmLib::cstring_putc(pStr, 6, HighData);

							SendData_i = false;
							return false;
						}
						return true;
					}
					void setup_listen(void){ return; }
					void setup_talk(void){
						if(Ref.getCompassDataMode() && Ref.can_get_compass_data()){
							SendData_i = true;
							SendData = Ref.get_compass_data();
						}
					}
				};
				compass_message_client CompassMessageClient;
			private:
				class gyro_message_client :public message_client_interface{
				private:
					sensor_manager& Ref;
				private:
					bool DataMode_i;
					bool SendData_i;
					std::pair<uint16, hmLib::coordinates3D::position> SendData;
				private:
					class inform_task :public hmr::task::client_interface{
					private:
						gyro_message_client& Ref;
					public:
						inform_task(gyro_message_client& Ref_):Ref(Ref_){}
						duration operator()(duration dt){
							Ref.DataMode_i = true;
							return dt;
						}
					};
					inform_task InformTask;
					task::handler InformTaskHandler;
				public:
					gyro_message_client(sensor_manager& Ref_, com::did_t ID_, service_interface& Service_)
						: message_client_interface(ID_)
						, Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, InformTask(*this){
						InformTaskHandler = Service_.task().quick_start(InformTask, 5);
					}
					~gyro_message_client(){
						InformTaskHandler.stop();
					}
				public://override function of messge_client_interface
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str, 0)){
						case 0x10:
							Ref.setGyroDataMode(true);
							DataMode_i = true;
							return false;
						case 0x11:
							Ref.setGyroDataMode(false);
							DataMode_i = true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						if(DataMode_i){
							service::cstring_construct_safe(pStr, 1);
							if(Ref.getGyroDataMode())hmLib::cstring_putc(pStr, 0, 0x10);
							else hmLib::cstring_putc(pStr, 0, 0x11);
							DataMode_i = false;
							return false;
						} else if(SendData_i){
							sint32 X, Y, Z;

							X = 0;
							Y = 0;
							Z = 0;

							//hmLib::coordinates3D::angle RawData = Inertial.GyroObserver.get();

							//X = RawData.pitch;
							//Y = RawData.roll;
							//Z = RawData.yaw;

							uint16 Cnt = SendData.first;
							hmLib::coordinates3D::position RawData3D = SendData.second;
							X = static_cast<sint32>(RawData3D.x);
							Y = static_cast<sint32>(RawData3D.y);
							Z = static_cast<sint32>(RawData3D.z);


							service::cstring_construct_safe(pStr, 16);
							hmLib::cstring_putc(pStr, 0, 0x00);
							hmLib::cstring_putc(pStr, 1, 0x00);	//Defaultでは0にしておく

																//1秒間10回分のデータ、ということにしておく
																//	こうすれば、正しく角度計算してくれる？
																//hmLib::cstring_putc(pStr, 2,(uint8)(10&0x00FF));
																//hmLib::cstring_putc(pStr, 3,(uint8)((10>>8)&0x00FF));
							hmLib::cstring_putc(pStr, 2, (uint8)Cnt);
							hmLib::cstring_putc(pStr, 3, (uint8)((Cnt >> 8) & 0x00FF));


							hmLib::cstring_putc(pStr, 4, (uint8)(X & 0x00FF));
							hmLib::cstring_putc(pStr, 5, (uint8)((X >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 6, (uint8)((X >> 16) & 0x00FF));
							hmLib::cstring_putc(pStr, 7, (uint8)((X >> 24) & 0x00FF));

							hmLib::cstring_putc(pStr, 8, (uint8)(Y & 0x00FF));
							hmLib::cstring_putc(pStr, 9, (uint8)((Y >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 10, (uint8)((Y >> 16) & 0x00FF));
							hmLib::cstring_putc(pStr, 11, (uint8)((Y >> 24) & 0x00FF));

							hmLib::cstring_putc(pStr, 12, (uint8)(Z & 0x00FF));
							hmLib::cstring_putc(pStr, 13, (uint8)((Z >> 8) & 0x00FF));
							hmLib::cstring_putc(pStr, 14, (uint8)((Z >> 16) & 0x00FF));
							hmLib::cstring_putc(pStr, 15, (uint8)((Z >> 24) & 0x00FF));

							SendData_i = false;
							return false;
						}
						return true;
					}
					void setup_listen(void){ return; }
					void setup_talk(void){
						if(Ref.getGyroDataMode() && Ref.can_get_gyro_data()){
							SendData_i = true;
							SendData = Ref.get_gyro_data();
						}
					}
				};
				gyro_message_client GyroMessageClient;
			public:
				cInertial(unsigned char AxelID_, unsigned char CompassID_, unsigned char GyroID_, system_interface& System_, io_interface& IO_, service_interface& Service_)
					: SensorManager(Service_)
					, AxelMessageClient(SensorManager, AxelID_, Service_)
					, CompassMessageClient(SensorManager, CompassID_, Service_)
					, GyroMessageClient(SensorManager, GyroID_, Service_){

					System_.regist(SensorManager);
					IO_.regist(AxelMessageClient);
					IO_.regist(CompassMessageClient);
					IO_.regist(GyroMessageClient);
				}
			public:
				void operator()(){
					SensorManager();
				}
			};
		}
	}
}

#
#endif
