#ifndef DEVICE_INCLUDE_INC
#define DEVICE_INCLUDE_INC 100
#ifdef _MSC_VER
#	define HMR_TEST
#endif
#ifndef HMR_TEST
//#include<HomuraMachine/device/p32mz2048ecm100_defines.h>
#include<homuraLib_v2/machine/device/p32mz2048ecm100_defines.h>
#include <proc/p32mz2048ecm100.h>
#include <Cpp/c/sys/attribs.h>
#include<xc.h>
#include<cp0defs.h>
//#include <peripheral/int.h>
#else
#include<homuraLib_v2/machine/device/p32mz2048ecm100_defines.h>
#include<homuraLib_v2/machine/device/p32mz2048ecm100_forVC.h>
#endif

#endif
