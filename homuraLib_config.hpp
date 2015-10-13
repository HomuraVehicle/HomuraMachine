#ifndef HMR_HOMURALIBCONFIG_INC
#define HMR_HOMURALIBCONFIG_INC
#
//XC32関係を使いたくない場合は、以下のマクロを有効にする
#if defined(_MSC_VER) || defined(__clang__ )
#	define HMR_NOXC32
#endif

//例外を無効にしたい場合は、以下のマクロを有効にする
#define HMR_NOEXCEPT
#
#endif
