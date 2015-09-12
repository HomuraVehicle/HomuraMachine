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
#include<homuraLib_v2/machine/module/GyroL3G4200D.hpp>
#include<homuraLib_v2/machine/module/AcceleCompassLSM303DLH.hpp>
#include<homuraLib_v2/task.hpp>
#include<XC32/i2c.hpp>
#include<hmLib/coordinates.hpp>
namespace hmr {
	namespace machine {
		namespace mihara {
			template<typename inertial_device_>
			class cInertial :public inertial_device_{
			private:
				typedef module::cAcceleCompassLSM303DLH<typename inertial_device_::AcceleCompass_i2c,typename inertial_device_::AccelCompass_shaerd_i2c_identifer> cAcceleCompass;
				//typedef module::cGyroL3G4200D<typename inertial_device_::Gyro_i2c> cGyro;
				typedef module::cGyroL3G4200D_I2C<typename inertial_device_::Gyro_i2c,typename inertial_device_::Gyro_shared_i2c_identifer> cGyro_I2C;
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
						,IsLock(false){
					}
				public:
					void config(const observer& Observer_){
						if(is_lock())return;
						Observer=Observer_;
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
					bool is_lock()const{return IsLock;}
					void unlock(){
						if (!is_lock())return;

						lock_guard Lock(I2C);
						if (!Lock.owns_lock())return;

						//power OFF!
						I2C.module_config(false);

						IsLock = false;
					}
				public:
					void operator()(void){
						if (!is_lock())return;

						lock_guard Lock(I2C);
						if (!Lock.owns_lock())return;

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
					cPositionObserver() :IsData(false){}
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
					typedef std::pair<uint16,hmLib::coordinates3D::position> DataType;
				private:
					hmLib::coordinates3D::position RawData3D;
					uint16 Cnt;
				public:
					cGyroRawDataObserver():RawData3D(0,0,0),Cnt(0){}
					void operator()(module::gyroL3G4200D::raw_data raw_data_){
						++Cnt;
						RawData3D+=hmLib::coordinates3D::position(raw_data_.x,raw_data_.y,raw_data_.z);
					}
					bool can_get()const{return Cnt>0;}
					std::pair<uint16,hmLib::coordinates3D::position> get(){
						std::pair<uint16,hmLib::coordinates3D::position> tmp=std::make_pair(Cnt,RawData3D);
						Cnt=0;
						RawData3D=hmLib::coordinates3D::position(0,0,0);
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
			public:
				cInertial()
					: AcceleCompass()
					, Gyro()
					, PowerInertialLock(PowerInertial){
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
				bool is_lock(){return (AcceleCompass.is_lock() || Gyro.is_lock());}
			public:
				void operator()(void){
					AcceleCompass();
					Gyro();
				}
			};
			namespace inertial {
				void initialize();
				void finalize();
				void work();
				namespace axel {
					void device_initialize(void);// PWR ON にしたときに必要な初期化関数
					void device_finalize(void);// PWR OFF にする前に必要な初期化関数
					bool listen(hmLib::cstring Str);
					bool talk(hmLib::cstring* pStr);
					void setup_listen(void);
					void setup_talk(void);
					struct interrupt_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
					struct data_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
					struct inform_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
				}
				namespace compass {
					void device_initialize(void);
					void device_finalize(void);
					bool listen(hmLib::cstring Str);
					bool talk(hmLib::cstring* pStr);
					void setup_listen(void);
					void setup_talk(void);
					struct data_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
					struct inform_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
				}
				namespace gyro {
					void device_initialize(void); // gyro はデフォルトでパワーダウンモードなので、起動する関数
					void device_finalize(void);
					bool listen(hmLib::cstring Str);
					bool talk(hmLib::cstring* pStr);
					void setup_listen(void);
					void setup_talk(void);
					struct data_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
					struct inform_task :public hmr::task::client_interface{
						duration operator()(duration dt);
					};
				}
			}
		}
	}
}

#
#endif
