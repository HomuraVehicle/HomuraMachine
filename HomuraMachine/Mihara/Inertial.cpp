#ifndef HMR_MACHINE_MIHARA_INERTIAL_CPP_INC
#define HMR_MACHINE_MIHARA_INERTIAL_CPP_INC 200
#
/*===航行情報用モジュール群===
*/
#define HMLIB_NOLIB
#include<XC32/i2c.hpp>
#include<homuraLib_v2/machine/service/exclusive_delay.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Inertial.hpp"
#include"Device.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace inertial {
				namespace{
					axel::inform_task AxelInformTask;
					compass::inform_task CompassInformTask;
					gyro::inform_task GyroInformTask;
					cInertial<cDevice::inertial_device> Inertial;
				}				
				void initialize(void){
					axel::device_initialize();
					compass::device_initialize();
					gyro::device_initialize();	

					service::task::quick_start(AxelInformTask,5);
					service::task::quick_start(CompassInformTask, 5);
					service::task::quick_start(GyroInformTask, 5);
					Inertial.lock();
				}
				void finalize(void){
					Inertial.unlock();
				}
				void work(void){
					Inertial();
				}

				namespace axel {
					namespace {
						struct{
							unsigned Data:1;
							unsigned SendData:1;
							unsigned SendData_i:1;
						}Mode={0,0,1};
					}
					void device_initialize(){}
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str,0)){
						case 0x10://sensor on
							Mode.SendData=true;
							Mode.SendData_i=true;
							return false;
						case 0x11://sensor on
							Mode.SendData=false;
							Mode.SendData_i=true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						if(Mode.SendData_i){
							service::cstring_construct_safe(pStr,1);
							if(Mode.SendData)hmLib::cstring_putc(pStr,0,0x10);
							else hmLib::cstring_putc(pStr,0,0x11);
							Mode.SendData_i=false;
							return false;
						}else if(Mode.Data){
							if(!Inertial.AxelObserver.can_get()){
								service::cstring_construct_safe(pStr,2);
								hmLib::cstring_putc(pStr,0,0xFF);
								hmLib::cstring_putc(pStr,1,0x00);	//Defaultでは0にしておく
								Mode.Data=false;
								return false;
							}


							service::cstring_construct_safe(pStr, 7);
							hmLib::cstring_putc(pStr,0x00,0);
							hmLib::coordinates3D::position RawData = Inertial.AxelObserver.get();
							uint16 Data;
							uint8 LowData, HighData;

							//homura-y軸書き込み
							Data = static_cast<xc::uint16>(RawData.x);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr,3,LowData);
							hmLib::cstring_putc(pStr,4,HighData);

							//homura-x軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-RawData.y);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr,1,LowData);
							hmLib::cstring_putc(pStr,2,HighData);

							//homura-z軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-RawData.z);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr,5,LowData);
							hmLib::cstring_putc(pStr,6,HighData);

							Mode.Data=false;
							return false;
						}
						return true;
					}
					void setup_listen(void){return;}
					void setup_talk(void){
						Mode.Data=Mode.SendData;
					}
					hmr::task::duration data_task::operator()(hmr::task::duration dt){
						Mode.Data = Mode.SendData;
						return dt;
					}
					hmr::task::duration inform_task::operator()(hmr::task::duration dt){
						Mode.SendData_i = true;
						return dt;
					}
				}

				namespace compass {
					namespace {
						struct {
							unsigned Data:1;
							unsigned SendData:1;
							unsigned SendData_i:1;
						}Mode={0, 0, 1};
					}
					void device_initialize(){}
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str,0)){
						case 0x10://sensor on
							Mode.SendData=true;
							Mode.SendData_i=true;
							return false;
						case 0x11://sensor on
							Mode.SendData=false;
							Mode.SendData_i=true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						if(Mode.SendData_i){
							service::cstring_construct_safe(pStr,1);
							if(Mode.SendData)hmLib::cstring_putc(pStr,0,0x10);
							else hmLib::cstring_putc(pStr,0,0x11);
							Mode.SendData_i=false;
							return false;
						}else if(Mode.Data){
							if(!Inertial.CompassObserver.can_get()){
								service::cstring_construct_safe(pStr, 2);
								hmLib::cstring_putc(pStr, 0, 0xFF);
								hmLib::cstring_putc(pStr, 1, 0x00);	//Defaultでは0にしておく
								Mode.Data = false;
								return false;
							}

							service::cstring_construct_safe(pStr, 7);
							hmLib::cstring_putc(pStr,0,0x00);
							hmLib::coordinates3D::position RawData = Inertial.CompassObserver.get();
							uint8 LowData, HighData;
							uint16 Data;

							//homura-x軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-RawData.y);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 1, LowData);
							hmLib::cstring_putc(pStr, 2, HighData);

							//homura-y軸書き込み
							Data = static_cast<xc::uint16>(RawData.x);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 3, LowData);
							hmLib::cstring_putc(pStr, 4, HighData);

							//homura-z軸書き込み（正負反転）
							Data = static_cast<xc::uint16>(-RawData.z);
							LowData = Data & 0x00FF;
							Data >>= 8;
							HighData = Data & 0x00FF;

							hmLib::cstring_putc(pStr, 5, LowData);
							hmLib::cstring_putc(pStr, 6, HighData);

							Mode.Data = false;
							return false;
						}
						return true;
					}
					void setup_listen(void){return;}
					void setup_talk(void){
						Mode.Data=Mode.SendData;
					}
					hmr::task::duration data_task::operator()(hmr::task::duration dt){
						Mode.Data = Mode.SendData;
						return dt;
					}
					hmr::task::duration inform_task::operator()(hmr::task::duration dt){
						Mode.SendData_i = true;
						return dt;
					}
				}

				namespace gyro {
					namespace {
						struct{
							unsigned Data:1;
							unsigned SendData:1;
							unsigned SendData_i:1;
						}Mode={0,0,1};
					}
					void device_initialize(){}
					bool listen(hmLib::cstring Str){
						switch(hmLib::cstring_getc(&Str,0)){
						case 0x10://sensor on
							Mode.SendData=true;
							Mode.SendData_i=true;
							return false;
						case 0x11://sensor on
							Mode.SendData=false;
							Mode.SendData_i=true;
							return false;
						default:
							return true;
						}
					}
					bool talk(hmLib::cstring* pStr){
						if(Mode.SendData_i){
							service::cstring_construct_safe(pStr,1);
							if(Mode.SendData)hmLib::cstring_putc(pStr,0,0x10);
							else hmLib::cstring_putc(pStr,0,0x11);
							Mode.SendData_i=false;
							return false;
						}else if(Mode.Data){
							sint32 X,Y,Z;

							X=0;
							Y=0;
							Z=0;
			
							//===========Gyro
							if(!Inertial.GyroObserver.can_get()){
								service::cstring_construct_safe(pStr,2);
								hmLib::cstring_putc(pStr,0,0xFF);
								hmLib::cstring_putc(pStr,1,0x00);	//Defaultでは0にしておく
								Mode.Data=false;
								return false;
							}

							//hmLib::coordinates3D::angle RawData = Inertial.GyroObserver.get();

							//X = RawData.pitch;
							//Y = RawData.roll;
							//Z = RawData.yaw;

							std::pair<uint16,hmLib::coordinates3D::position> RawData=Inertial.GyroObserver.get();
							uint16 Cnt=RawData.first;
							hmLib::coordinates3D::position RawData3D=RawData.second;
							X=static_cast<sint32>(RawData3D.x);
							Y=static_cast<sint32>(RawData3D.y);
							Z=static_cast<sint32>(RawData3D.z);


							service::cstring_construct_safe(pStr,16);
							hmLib::cstring_putc(pStr,0,0x00);
							hmLib::cstring_putc(pStr,1,0x00);	//Defaultでは0にしておく

							//1秒間10回分のデータ、ということにしておく
							//	こうすれば、正しく角度計算してくれる？
							//hmLib::cstring_putc(pStr, 2,(uint8)(10&0x00FF));
							//hmLib::cstring_putc(pStr, 3,(uint8)((10>>8)&0x00FF));
							hmLib::cstring_putc(pStr, 2,(uint8)Cnt);
							hmLib::cstring_putc(pStr, 3,(uint8)((Cnt>>8)&0x00FF));
							

							hmLib::cstring_putc(pStr, 4,(uint8)(X&0x00FF));
							hmLib::cstring_putc(pStr, 5,(uint8)((X>>8)&0x00FF));
							hmLib::cstring_putc(pStr, 6,(uint8)((X>>16)&0x00FF));
							hmLib::cstring_putc(pStr, 7,(uint8)((X>>24)&0x00FF));

							hmLib::cstring_putc(pStr, 8,(uint8)(Y&0x00FF));
							hmLib::cstring_putc(pStr, 9,(uint8)((Y>>8)&0x00FF));
							hmLib::cstring_putc(pStr,10,(uint8)((Y>>16)&0x00FF));
							hmLib::cstring_putc(pStr,11,(uint8)((Y>>24)&0x00FF));

							hmLib::cstring_putc(pStr,12,(uint8)(Z&0x00FF));
							hmLib::cstring_putc(pStr,13,(uint8)((Z>>8)&0x00FF));
							hmLib::cstring_putc(pStr,14,(uint8)((Z>>16)&0x00FF));
							hmLib::cstring_putc(pStr,15,(uint8)((Z>>24)&0x00FF));

							Mode.Data=false;
							return false;
						}
						return true;
					}
					void setup_listen(void){return;}
					void setup_talk(void){
						Mode.Data=Mode.SendData;
					}
					hmr::task::duration data_task::operator()(hmr::task::duration dt){
						Mode.Data = Mode.SendData;
						return dt;
					}
					hmr::task::duration inform_task::operator()(hmr::task::duration dt){
						Mode.SendData_i = true;
						return dt;
					}
				}
			}
		}
	}
}

#
#endif
