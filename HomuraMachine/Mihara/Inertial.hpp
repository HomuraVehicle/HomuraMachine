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
//#include<homuraLib_v2/task.hpp>
#include<XC32/i2c.hpp>
#include<hmLib/coordinates.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"System_base.hpp"
#include"Message_base.hpp"
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
					//typedef module::cGyroL3G4200D<typename inertial_device_::Gyro_i2c> cGyro;
					typedef module::cGyroL3G4200D_I2C<typename inertial_device_::Gyro_i2c, typename inertial_device_::Gyro_shared_i2c_identifer> cGyro_I2C;
					class cGyro{
					private:
						typedef xc::function<void(module::gyroL3G4200D::raw_data)> observer;
						typedef xc::lock_guard<cGyro_I2C> lock_guard;
					private:
						cGyro_I2C I2C;
						bool IsLock;
						observer Observer;
					public:
						cGyro()
							:I2C()
							, IsLock(false){
						}
					public:
						void config(const observer& Observer_){
							if(is_lock())return;
							Observer = Observer_;
						}
						bool lock(const observer& Observer){
							config(Observer);
							return lock();
						}
						bool lock(){
							if(is_lock())return false;

							lock_guard Lock(I2C);
							if(!Lock.owns_lock())return true;

							I2C.module_config(false);

							machine::service::exclusive_delay_ms(500);

							//power ON!
							I2C.module_config(true, module::gyroL3G4200D::sampling_rate::_100Hz, module::gyroL3G4200D::fullscale::_250dps);

							//buffer 設定
							I2C.buffer_config(module::gyroL3G4200D::buffer_mode::stream, 31);

							IsLock = true;

							return false;
						}
						bool is_lock()const{ return IsLock; }
						void unlock(){
							if(!is_lock())return;

							lock_guard Lock(I2C);
							if(!Lock.owns_lock())return;

							//power OFF!
							I2C.module_config(false);

							IsLock = false;
						}
					public:
						void operator()(void){
							if(!is_lock())return;

							lock_guard Lock(I2C);
							if(!Lock.owns_lock())return;

							if(Observer){
								I2C.read_raw_all(Observer);
							}
						}
					};
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

					cAcceleCompass AcceleCompass;
					cGyro Gyro;
					typename inertial_device_::powerInertial PowerInertial;
					xc::lock_guard<typename inertial_device_::powerInertial> PowerInertialLock;
				private:
					bool GyroDataMode;
					bool AxelCompassDataMode;
				public:
					sensor()
						: AcceleCompass()
						, Gyro()
						, PowerInertialLock(PowerInertial)
						, GyroDataMode(false)
						, AxelCompassDataMode(false){
						PowerInertial(false);
					}
				public:
					bool lock(){
						if(is_lock())return false;

						PowerInertial(true);
						AcceleCompass.lock(xc32::ref(AxelObserver), xc32::ref(CompassObserver));
						Gyro.lock(xc32::ref(GyroObserver));

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
					bool AxelDataMdode:
					bool CompassDataMode;
					bool GyroDataMode;
				private:
					systems::mode::type CurrentMode;
				public://override funcition of system_client_interface
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_)override{
						switch(NewMode_){
						case systems::mode::drive:
							Sensor.PowerInertial(SensorPower);
							break;
						default:
							Sensor.PowerInertial(false);
							break;
						}
						CurrentMode = NewMode_;
					}
				public:
					systems::mode::type mode()const{ return CurrentMode; }
					void setSensorPower(bool OnOff_){
						SensorPower = OnOff_;
						if(CurrentMode = systems::mode::drive)Sensor.InertialPower(SensorPower);
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
						else hmLib::coordinates3D::position();
					}
					hmLib::coordinates3D::position get_compass_data(){
						if(Sensor.CompassObserver.can_get())return Sensor.CompassObserver.get();
						else hmLib::coordinates3D::position();
					}
					std::pair<uint16, hmLib::coordinates3D::position> get_gyro_data(){
						if(Sensor.GyroObserver.can_get())return Sensor.GyroObserver.get();
						else return std::make_pair(0, hmLib::coordinates3D::position());
					}
				public:
					sensor_manager()
						: SensorPower(true)
						, AxelDataMdode(false)
						, CompassDataMode(false)
						, GyroDataMode(false){
						Sensor.lock();
					}
					~sensor_manager(){
						Sensor.unlock();
					}
				public:
					void operator()(){
						if(CurrentMode == systems::mode::drive && SensorPower){
							Sensor();
						}
					}
				};
				senstor_manager SensorManager;
				systems::element SystemElement;
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
				public:
					axel_message_client(sensor_manager& Ref_)
						: Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, InformTask(*this){
						task::quick_start(InformTask, 5);
					}
					~axel_message_client(){
						task::stop(InformTask);
					}
				public:
					void setSendData(const hmLib::coordinates3D::position& SendData_){
						SendData = SendData_;
						SendData_i = true;
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
					void setup_talk(void){ return; }
				};
				axel_message_client AxelMessageClient;
				message::element AxelMessageElement;
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
				public:
					compass_message_client(sensor_manager& Ref_)
						: Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, InformTask(*this){
						task::quick_start(InformTask, 5);
					}
					~compass_message_client(){
						task::stop(InformTask);
					}
				public:
					void setSendData(const hmLib::coordinates3D::position& SendData_){
						SendData = SendData_;
						SendData_i = true;
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
					void setup_talk(void){ return; }
				};
				compass_message_client CompassMessageClient;
				message::element CompassMessageElement;
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
				public:
					gyro_message_client(sensor_manager& Ref_)
						: Ref(Ref_)
						, DataMode_i(true)
						, SendData_i(false)
						, InformTask(*this){
						task::quick_start(InformTask, 5);
					}
					~gyro_message_client(){
						task::stop(InformTask);
					}
				public:
					void setSendData(const std::pair<uint16, hmLib::coordinates3D::position>& SendData_){
						SendData = SendData_;
						SendData_i = true;
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
					void setup_talk(void){ return; }
				};
				compass_message_client GyroMessageClient;
				message::element GyroMessageElement;
				cInertial(unsigned char AxelID_, unsigned char CompassID_, unsigned char GyroID_, system_host& SystemHost_, message_host& MessageHost_)
					: SensorManager()
					, SystemElemet(system_client_holder(SensorManager))
					, AxelMessageClient(SensorManager)
					, AxelMessageElement(message_client_holder(AxelID_, AxelMessageClient))
					, CompassMessageClient(SensorManager)
					, CompassMessageElement(message_client_holder(CompassID_, CompassMessageClient))
					, GyroMessageClient(SensorManager)
					, GyroMessageElement(message_client_holder(GyroID_, GyroMessageClient)){

					SystemHost_.regist(SystemElement);
					MessageHost_.regist(AxelMessageElement);
					MessageHost_.regist(CompassMessageElement);
					MessageHost_.regist(GyroMessageElement);
				}
			public:
				void operator()(){
					SensorManager();
					if(SensorManager.getAxelDataMode() && SensorManager.can_get_axel_data()){
						AxelMessageClient.setSendData(SensorManager.get_axel_data());
					}
					if(SensorManager.getCompassDataMode() && SensorManager.can_get_compass_data()){
						CompassMessageClient.setSendData(SensorManager.get_compass_data());
					}
					if(SensorManager.getGyroDataMode() && SensorManager.can_get_gyro_data()){
						GyroMessageClient.setSendData(SensorManager.get_gyro_data());
					}
				}
			};
		}
	}
}

#
#endif
