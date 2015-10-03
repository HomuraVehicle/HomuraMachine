#ifndef HMR_MACHINE_MIHARA_DATA_CPP_INC
#define HMR_MACHINE_MIHARA_DATA_CPP_INC 200
#define HMLIB_NOLIB
#
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include"Data.hpp"

namespace hmr {
	namespace machine {
		namespace mihara {
			void idata_format(idata* Dat){
				Dat->ID = 0;
				hmLib::cstring_format(&(Dat->Str));
				return;
			}
			void odata_format(odata* Dat){
				Dat->ID = 0;
				Dat->DSRPAG=0;
				Dat->FpSendable = 0;
				Dat->FpInformDestruct = 0;
				hmLib::cstring_format(&(Dat->Str));
				return;
			}

			bool idata_is_construct(idata* Dat){
				return Dat->ID != 0;
			}
			bool odata_is_construct(odata* Dat){
				return Dat->ID != 0;
			}

			void idata_construct(idata* Dat, data_did_t ID, hmLib::cstring* mStr){
				Dat->ID = ID;
				hmLib::cstring_move( mStr, &(Dat->Str) );
				return;
			}
			void odata_construct(odata* Dat, data_did_t ID, hmLib::cstring* mStr, int DSRPAG,data_dsizeFp_v FpSendable, data_vFp_v FpInformDestruct){
				Dat->ID = ID;
				Dat->FpSendable = FpSendable;
				Dat->FpInformDestruct = FpInformDestruct;
				hmLib::cstring_move( mStr, &(Dat->Str) );
				Dat->DSRPAG=DSRPAG;
				return;
			}

			void idata_destruct(idata* Dat){
				if(hmLib::cstring_is_construct(&(Dat->Str))){
					service::cstring_destruct_safe(&(Dat->Str));
				}
				Dat->ID = 0;
			}
			void odata_destruct(odata* Dat){
				if(hmLib::cstring_is_construct(&(Dat->Str))){
					service::cstring_destruct_safe(&(Dat->Str));
				}
				Dat->ID = 0;
				Dat->FpSendable = 0;
				if(Dat->FpInformDestruct)Dat->FpInformDestruct();
				Dat->FpInformDestruct = 0;
			}

			void idata_move(idata* From, idata* To){
				To->ID=From->ID;
				hmLib::cstring_move(&(From->Str),&(To->Str));
				idata_format(From);
			}
			void odata_move(odata* From, odata* To){
				To->ID=From->ID;
				hmLib::cstring_move(&(From->Str),&(To->Str));
				To->FpSendable = From->FpSendable;
				To->FpInformDestruct = From->FpInformDestruct;
				odata_format(From);
			}
		}
	}
}

#
#endif
