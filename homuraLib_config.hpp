#ifndef HMR_HOMURALIBCONFIG_INC
#define HMR_HOMURALIBCONFIG_INC
#
//XC32Lib関係を使いたくない場合は、以下のマクロを有効にする
#ifdef _MSC_VER
#	define HMR_NOXC32LIB
#endif

//例外を無効にしたい場合は、以下のマクロを有効にする
#define HMR_NOEXCEPT
#
#endif
