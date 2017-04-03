#ifndef HMR_MACHINE_MIHARACPP_INC
#define HMR_MACHINE_MIHARACPP_INC 100
#
#define HMLIB_NOLIB
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"GPS.hpp"
#include"Device.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			namespace gps {
				namespace {
					cGPS<cDevice::gps_device>* pGPS;
					bool IsPowerBeforSleep = false;
				}
				namespace {
					//***************** (*pGPS) ************************************
					unsigned char GPSCh=0;
					struct {
						unsigned Data:1;
						unsigned SendData:1;			//データ送信モード
						unsigned SendData_i:1;			//データ送信モード受理
						unsigned Swapi:1;			//(*pGPS)交換要求受理
						unsigned PowerGPS:1;
						unsigned Poweri:1;
					}Mode={0, 0, 1, 0};
				}
				bool listen(hmLib::cstring Str) {
					//データサイズ確認
					if(hmLib::cstring_size(&Str)==0)return true;

					//1byte目でモードを分ける
					switch(hmLib::cstring_getc(&Str, 0)) {
					case 0x10:
						Mode.SendData=true;
						Mode.SendData_i=true;
						return false;
					case 0x11:
						Mode.SendData=false;
						Mode.SendData_i=true;
						return false;
					case 0x20:
						//データサイズ確認
						if(hmLib::cstring_size(&Str)!=2)return true;
						if(hmLib::cstring_getc(&Str, 1)!=GPSCh) {
							Mode.Swapi=true;
						}
						return false;
					case 0x30:
						Mode.PowerGPS=true;
						Mode.Poweri=true;
						return false;
					case 0x31:
						Mode.PowerGPS=false;
						Mode.Poweri=true;
						return false;
					default:
						return true;
					}
				}
				bool talk(hmLib::cstring* pStr) {
					if(Mode.SendData_i) {
						service::cstring_construct_safe(pStr, 1);
						if(Mode.SendData)hmLib::cstring_putc(pStr, 0, 0x10);
						else hmLib::cstring_putc(pStr, 0, 0x11);
						//送信後自動的にフラグを落とす
						Mode.SendData_i=false;
						return false;
					} else if(Mode.Swapi) {
						service::cstring_construct_safe(pStr, 1+1);
						hmLib::cstring_putc(pStr, 0, 0x20);

						(*pGPS).GPSSwitcher.swapCh();
						GPSCh = (*pGPS).GPSSwitcher.getChNo() - 1;
						hmLib::cstring_putc(pStr, 1, GPSCh);

						//送信後自動的にフラグを落とす
						Mode.Swapi=false;
						return false;
					} else if(Mode.Poweri) {
						service::cstring_construct_safe(pStr, 1);

						if(Mode.PowerGPS) {
							hmLib::cstring_putc(pStr, 0, 0x30);
							(*pGPS).GPSSwitcher.lock();
//							(*pGPS).GPSSwitcher.power(true);
						} else {
							hmLib::cstring_putc(pStr, 0, 0x31);
							(*pGPS).GPSSwitcher.unlock();
//							(*pGPS).GPSSwitcher.power(false);
						}

						//送信後自動的にフラグを落とす
						Mode.Poweri=false;
						return false;
					} else if(Mode.Data) {
						service::cstring_construct_safe(pStr, 1+20);
						hmLib::cstring_putc(pStr, 0, 0x00);

						hmr::gps::data Data;
						unsigned char Ch=0xFF;
						if ((*pGPS).GPSSwitcher.can_read()) {
							Ch = (*pGPS).GPSSwitcher.getChNo()-1;
							Data = (*pGPS).GPSSwitcher.read();
						}

						hmLib::cstring_putc(pStr, 1, static_cast<unsigned char>(Data.getHour()));
						hmLib::cstring_putc(pStr, 2, static_cast<unsigned char>(Data.getMin()));
						hmLib::cstring_putc(pStr, 3, static_cast<unsigned char>(Data.getSec()));
						hmLib::cstring_putc(pStr, 4, static_cast<unsigned char>(Data.getMSec()/10));
						hmLib::cstring_putc(pStr, 5, static_cast<unsigned char>((Data.getMSec()%10)*10));

						double Val = Data.getLatitude();
						Val = (Val > 0 ? Val : -Val);

						hmLib::cstring_putc(pStr, 6, static_cast<unsigned char>(static_cast<unsigned int>(Val) % 90));
						hmLib::cstring_putc(pStr, 7, static_cast<unsigned char>(static_cast<unsigned int>(Val*60)%60));
						hmLib::cstring_putc(pStr, 8, static_cast<unsigned char>(static_cast<unsigned int>(Val*6000) % 100));
						hmLib::cstring_putc(pStr, 9, static_cast<unsigned char>(static_cast<unsigned int>(Val*600000) % 100));
						hmLib::cstring_putc(pStr, 10, static_cast<unsigned char>(Data.getLatitude()>0? 'N':'S'));

						Val = Data.getLongitude();
						Val = (Val > 0 ? Val : -Val);

						hmLib::cstring_putc(pStr, 11, static_cast<unsigned char>(static_cast<unsigned int>(Val)%180));
						hmLib::cstring_putc(pStr, 12, static_cast<unsigned char>(static_cast<unsigned int>(Val * 60) % 60));
						hmLib::cstring_putc(pStr, 13, static_cast<unsigned char>(static_cast<unsigned int>(Val * 6000) % 100));
						hmLib::cstring_putc(pStr, 14, static_cast<unsigned char>(static_cast<unsigned int>(Val * 600000) % 100));
						hmLib::cstring_putc(pStr, 15, static_cast<unsigned char>(Data.getLatitude()>0 ? 'E' : 'W'));

						hmLib::cstring_putc(pStr, 16, static_cast<unsigned char>(Data.is_valid()));

						Val = Data.getAltitude();
						Val = (Val > 0 ? Val : -Val);

						hmLib::cstring_putc(pStr, 17, static_cast<unsigned char>(static_cast<unsigned int>(Val/1000)%256));
						hmLib::cstring_putc(pStr, 18, static_cast<unsigned char>(static_cast<unsigned int>(Val/10) % 100));
						hmLib::cstring_putc(pStr, 19, static_cast<unsigned char>(static_cast<unsigned int>(Val*10) % 100));

						hmLib::cstring_putc(pStr, 20, Ch);

						Mode.Data = false;

						return false;
					}

					return true;
				}
				void setup_listen(void) { return; }
				void setup_talk(void) {
					Mode.Data=Mode.SendData;
				}
				void task_setup_talk(void) { return; }
				struct data_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						Mode.Data = Mode.SendData;
						return dt;
					}
				}DataTask;
				struct inform_task :public hmr::task::client_interface{
					duration operator()(duration dt){
						Mode.SendData_i = true;
						return dt;
					}
				}InformTask;
				void informSleep(){
					IsPowerBeforSleep = (*pGPS).GPSSwitcher.is_lock();
					(*pGPS).GPSSwitcher.unlock();
//					IsPowerBeforSleep = (*pGPS).GPSSwitcher.is_power();
//					(*pGPS).GPSSwitcher.power(false);
				}
				void informAwake(){
					if(IsPowerBeforSleep)(*pGPS).GPSSwitcher.lock();
					else (*pGPS).GPSSwitcher.unlock();
//					(*pGPS).GPSSwitcher.power(IsPowerBeforSleep);
				}
				void initialize(cGPS<cDevice::gps_device>& rGPS_){
					pGPS = &rGPS_; 

					//タスク登録
					task::quick_start(InformTask, 5);
					task::quick_start(DataTask, 3);
				}
			}
		}
	}
}
#
#endif
