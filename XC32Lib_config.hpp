//Pic32X固有の部分を有効にしたくなければ、以下のマクロを有効にする
#define XC32LIB_PIC32MZ

//デバッグモード時は以下のマクロを有効
#ifdef _MSC_VER
#	define XC32LIB_DEBUGMODE
#endif

/*割り込み関数__ISRマクロ，IPLレジスタ用define
#define XC32LIB_I2C1_IPL 5
#define XC32LIB_I2C1_IPL_FOR_ISR IPL5
のように割り込み優先度を5に変えたい場合は2つdefineする．当然数字は1〜7で同じ値にすること
defineしないと7（最上位）にデフォルトでなる．
*/
//I2C1
//#define XC32LIB_CONFIG_I2C1_IPL 1
//#define XC32LIB_CONFIG_I2C1_IPL_FOR_ISR IPL1

//I2C2
//#define XC32LIB_CONFIG_I2C2_IPL 1
//#define XC32LIB_CONFIG_I2C2_IPL_FOR_ISR IPL1

//I2C3
//#define XC32LIB_CONFIG_I2C3_IPL 1
//#define XC32LIB_CONFIG_I2C3_IPL_FOR_ISR IPL1

//I2C4
//#define XC32LIB_CONFIG_I2C4_IPL 1
//#define XC32LIB_CONFIG_I2C4_IPL_FOR_ISR IPL1

//I2C5
//#define XC32LIB_CONFIG_I2C5_IPL 1
//#define XC32LIB_CONFIG_I2C5_IPL_FOR_ISR IPL1

//I2C6
//#define XC32LIB_CONFIG_I2C6_IPL 1
//#define XC32LIB_CONFIG_I2C6_IPL_FOR_ISR IPL1

//UART1 TX->courier stream (rf module)
#define XC32LIB_CONFIG_UART1_TX_IPL 3
#define XC32LIB_CONFIG_UART1_TX_IPL_FOR_ISR IPL3
//UART1 RX->courier stream (rf module)
#define XC32LIB_CONFIG_UART1_RX_IPL 4
#define XC32LIB_CONFIG_UART1_RX_IPL_FOR_ISR IPL4

//UART2 TX
//#define XC32LIB_CONFIG_UART2_TX_IPL 5
//#define XC32LIB_CONFIG_UART2_TX_IPL_FOR_ISR IPL5
//UART2 RX
//#define XC32LIB_CONFIG_UART2_RX_IPL 6
//#define XC32LIB_CONFIG_UART2_RX_IPL_FOR_ISR IPL6

//UART3 TX->courier stream (mobile phone)
#define XC32LIB_CONFIG_UART3_TX_IPL 3
#define XC32LIB_CONFIG_UART3_TX_IPL_FOR_ISR IPL3
//UART3 RX->courier stream (mobile phone)
#define XC32LIB_CONFIG_UART3_RX_IPL 4
#define XC32LIB_CONFIG_UART3_RX_IPL_FOR_ISR IPL4

//UART4 TX
#define XC32LIB_CONFIG_UART4_TX_IPL 5
#define XC32LIB_CONFIG_UART4_TX_IPL_FOR_ISR IPL5
//UART4 RX
#define XC32LIB_CONFIG_UART4_RX_IPL 6
#define XC32LIB_CONFIG_UART4_RX_IPL_FOR_ISR IPL6

//UART5 TX->sprite stream tx
#define XC32LIB_CONFIG_UART5_TX_IPL 6
#define XC32LIB_CONFIG_UART5_TX_IPL_FOR_ISR IPL6
//UART5 RX->sprite stream rx
#define XC32LIB_CONFIG_UART5_RX_IPL 7
#define XC32LIB_CONFIG_UART5_RX_IPL_FOR_ISR IPL7AUTO

//UART6 TX
//#define XC32LIB_CONFIG_UART6_TX_IPL 5
//#define XC32LIB_CONFIG_UART6_TX_IPL_FOR_ISR IPL5
//UART6 RX
//#define XC32LIB_CONFIG_UART6_RX_IPL 6
//#define XC32LIB_CONFIG_UART6_RX_IPL_FOR_ISR IPL6

//TIMER1->debugger timer
#define XC32LIB_CONFIG_TIMER1_IPL 2
#define XC32LIB_CONFIG_TIMER1_IPL_FOR_ISR IPL2

//TIMER2->sprite timer
#define XC32LIB_CONFIG_TIMER2_IPL 2
#define XC32LIB_CONFIG_TIMER2_IPL_FOR_ISR IPL2

//TIMER3->chrono timer
#define XC32LIB_CONFIG_TIMER3_IPL 2
#define XC32LIB_CONFIG_TIMER3_IPL_FOR_ISR IPL2

//TIMER4->delay timer
#define XC32LIB_CONFIG_TIMER4_IPL 2
#define XC32LIB_CONFIG_TIMER4_IPL_FOR_ISR IPL2

//TIMER5->exclusive delay timer
#define XC32LIB_CONFIG_TIMER5_IPL 2
#define XC32LIB_CONFIG_TIMER5_IPL_FOR_ISR IPL2

//TIMER6->system longtimer
#define XC32LIB_CONFIG_TIMER6_IPL 2
#define XC32LIB_CONFIG_TIMER6_IPL_FOR_ISR IPL2

//TIMER7->system longtimer
#define XC32LIB_CONFIG_TIMER7_IPL 2
#define XC32LIB_CONFIG_TIMER7_IPL_FOR_ISR IPL2

//TIMER8->devmng timer
#define XC32LIB_CONFIG_TIMER8_IPL 2
#define XC32LIB_CONFIG_TIMER8_IPL_FOR_ISR IPL2

//TIMER9
//#define XC32LIB_CONFIG_TIMER9_IPL 2
//#define XC32LIB_CONFIG_TIMER9_IPL_FOR_ISR IPL2

//SPI1RX
//#define XC32LIB_CONFIG_SPI1_RX_IPL 2
//#define XC32LIB_CONFIG_SPI1_RX_IPL_FOR_ISR IPL2

//SPI2RX
//#define XC32LIB_CONFIG_SPI2_RX_IPL 2
//#define XC32LIB_CONFIG_SPI2_RX_IPL_FOR_ISR IPL2

//SPI3RX
//#define XC32LIB_CONFIG_SPI3_RX_IPL 2
//#define XC32LIB_CONFIG_SPI3_RX_IPL_FOR_ISR IPL2

//SPI4RX
//#define XC32LIB_CONFIG_SPI4_RX_IPL 2
//#define XC32LIB_CONFIG_SPI4_RX_IPL_FOR_ISR IPL2
