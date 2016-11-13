#ifndef HMR_MACHINE_IO_INC
#define HMR_MACHINE_IO_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/future.hpp>
#include<homuraLib_v2/old/hmrVMC1.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include"Com.hpp"
#include"System_base.hpp"
#include"Message_base.hpp"
#include"Message.hpp"
#include"Device.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			//=== ��{��` ===
		#define RF_BaudRate 9600
		#define RFDebug_BaudRate 38400
		#define MP_BaudRate 38400


			template<typename io_device_>
			struct cIO :public message_host_interface, public io_device_{
				typedef cIO<io_device_> this_type;
			private:
				//���b�N�t���O
				bool Lock;
			private:
				//���W�X�^�n
				xc32::interrupt_uart<RF0_uart_register_> Uart0;
				xc32::interrupt_uart<RF1_uart_register_> Uart1;
				RF0_power PinPowerUart0;
				RF1_power PinPowerUart1;
			private:
				//ModuleID
				typedef enum{
					null = 0x00,
					mobile_phone = 0x01,
					rf_module = 0x02
				}rf_module_mode;
			private:
				//=== ���[�h����@�\ ===
				typedef enum{ NormalMode, SleepMode, RoamingMode }mode;
				//�f�o�C�X���[�h��ύX����
				void mode_set(mode Mode_);
				//�f�o�C�X���[�h���擾����
				volatile mode mode_get(void);
			private:
				//VMC1
				VMC1* pVMC;
				//�󂯎��p�f�[�^
				idata IData;
				bool IPacketMode = 0;
				odata OData;
				bool OPacketMode = 0;
				//���b�Z�[�W�z���_�[
				cMessage Message;
			public:
				cIO(){}
				bool lock(){
					if(is_lock())return false;
					Lock = true;

					//�ʐM�֘A�@�\�̏�����
					pVMC = com::createVMC1();
					vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					com::initialize();

					devmng::courier::uart::fput_set_interrupt();//streamVMC_set_fput_interrupt_flag();//set_interrupt_fputflag(Stream_VMC);//���荞�ݗ\��
					devmng::interrupt_enable_streamVMC_fget_interrupt();// enable_interrupt_fget(Stream_VMC);
					devmng::interrupt_enable_streamVMC_fput_interrupt();// enable_interrupt_fput(Stream_VMC);


					return false;
				}
				bool is_lock()const{ return Lock; }
				void unlock(){
					Lock = false;

					//���荞�݋֎~
					devmng::courier::uart::fget_disable_interrupt();
					devmng::courier::uart::fput_disable_interrupt();

					//�ʐM�֘A�̏I�[������
					vmc1_finalize(pVMC);
					com::finalize();
					com::releaseVMC1(pVMC);
				}
			public:
				virtual void regist(message::element& rElement_){
					Message.regist(rElement_);
				}
			public:
				void operator()(void){
					//��M�\�ȃf�[�^������ꍇ
					if(!com::in_empty()){
						//��M�f�[�^���擾
						com::in_move_pop(&IData);

						//���g�������Ă���ꍇ
						if(idata_is_construct(&IData)){
							//Packet���܂��J���Ă��Ȃ��ꍇ
							if(!IPacketMode){
								//Packet�������ŊJ��
								IPacketMode = true;

								//��M�p���b�Z�[�W���W���[������
								Message.setup_listen();
							}

							//Packet�I��ID�������ꍇ
							if(IData.ID == HMR_COM_PACTRMNID){
								//Packet�������ŕ���
								IPacketMode = false;

								//���b�Z�[�W�j��
								idata_destruct(&IData);
							} else{
								//���b�Z�[�W����;
								Message.listen(&IData);
							}
						}
					}

					//��������̂őҋ@
					delay_ms(5);

					//���M�҂���Packet���Ȃ��Acom�̑��M�o�b�t�@�������ς��ł��Ȃ��Ƃ�
					if(!com::isWaitSendPacket() && !com::out_full()){
						//Packet���܂��J���Ă��Ȃ��ꍇ
						if(!OPacketMode){
							//Packet�������ŊJ��
							OPacketMode = true;
							//���M�p���b�Z�[�W���W���[������
							Message.setup_talk();
						}

						//���M�p�f�[�^����������
						odata_format(&OData);

						//���M�f�[�^�̎擾�Ɏ��s�����ꍇ
						if(Message.talk(&OData)){
							//���g���쐬����Ă���Δj��
							if(odata_is_construct(&OData))odata_destruct(&OData);
							//Packet�������ŕ���
							OPacketMode = false;
							OData.ID = HMR_COM_PACTRMNID;
						}

						//���M�f�[�^�𑗂�o��
						com::out_move_push(&OData);
					}

					//���M���荞�݂��؂��Ă��āA�����M�\��Ԃ̂Ƃ��ɂ́A���M���荞�݂��I���ɂ���
					if(!devmng::courier::uart::fput_is_interrupt_enable()){
						if(vmc1_can_send(pVMC))devmng::interrupt_enable_streamVMC_fput_interrupt();//uart1_enable_fput_interrupt();
					}
				}
			public://old functions
				   //=== rf module uart select ===
				   //���݃A�N�e�B�u��ID
				rf_module_mode rf_module_uart_ModuleID = rf_module_mode::null;
				//ModuleID�̃��W���[���ŏ���������
				void rf_module_uart_initialize(rf_module_mode ModuleID_, uint16 BaudRate_){
					if(ModuleID_ == rf_module_mode::mobile_phone){
						rf_module_uart_ModuleID = rf_module_mode::mobile_phone;
						Device.MobilePhone_uart.initialize(Device.MobilePhone_uart_register);
						Device.MobilePhone_uart.config(Device.MobilePhone_uart_Baudrate, Device.MobilePhone_uart_flowctrl, courier::uart::CourierTxInterruptFunc, courier::uart::CourierRxInterruptFunc);
						Device.MobilePhone_uart.lock();
						//Device.RF_uart.release();
					} else if(ModuleID_ == rf_module_mode::rf_module){
						rf_module_uart_ModuleID = rf_module_mode::rf_module;
						Device.RF_uart.initialize(Device.RF_uart_register);
						Device.RF_uart.config(Device.RF_uart_Baudrate, Device.RF_uart_flowctrl, courier::uart::CourierTxInterruptFunc, courier::uart::CourierRxInterruptFunc);
						Device.RF_uart.lock();
						//Device.MobilePhone_uart.release();
					}
				}
				//ModuleID�ɓd����ύX����
				void rf_module_uart_power_switch(bool onoff, rf_module_mode ModuleID_){
					if(onoff && ModuleID_ == rf_module_mode::mobile_phone){
						Device.PinMobilePhonePower(1);
						Device.PinRFPower(0);
					} else if(onoff && ModuleID_ == rf_module_mode::rf_module){
						Device.PinMobilePhonePower(0);
						Device.PinRFPower(1);
					} else{
						Device.PinMobilePhonePower(0);
						Device.PinRFPower(0);
					}
				}
				//���݂�ModuleID���擾����
				rf_module_mode rf_module_uart_getModuleID(void){ return rf_module_uart_ModuleID; }
				// main stream ���荞�݂̋��֎~�����������Ă悢����Ԃ��֐�
				bool interrupt_can_enable_streamVMC_fget_interrupt(){
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamVMC_fget_interrupt(){
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}
				bool interrupt_can_enable_streamVMC_fput_interrupt(){
					return !(sleep_isSleep() && !sleep_isRem());
				}
				bool interrupt_can_disable_streamVMC_fput_interrupt(){
					return true;//!( sleep_isSleep() && !sleep_isRem() );
				}

				// main stream �̊��荞�݋��֎~�֐�
				bool interrupt_enable_streamVMC_fget_interrupt(){
					if(interrupt_can_enable_streamVMC_fget_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_enable();
						return true;
					} else{
						return false;
					}
				}
				bool interrupt_disable_streamVMC_fget_interrupt(){
					if(interrupt_can_disable_streamVMC_fget_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_disable();
						return true;
					} else{
						return false;
					}
				}
				bool interrupt_enable_streamVMC_fput_interrupt(){
					if(interrupt_can_enable_streamVMC_fput_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_enable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_enable();
						return true;
					} else{
						return false;
					}
				}
				bool interrupt_disable_streamVMC_fput_interrupt(){
					if(interrupt_can_disable_streamVMC_fput_interrupt()){
						if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_disable();
						else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_disable();
						return true;
					} else{
						return false;
					}
				}

				uint8 fget(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)return Device.MobilePhone_uart.recv_data();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)return Device.RF_uart.recv_data();
					else return 0;
				}
				void fget_set_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_set_flag();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_set_flag();
				}
				void fget_clear_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_clear_flag();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_clear_flag();
				}
				void fget_enable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_enable();
				}
				void fget_disable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.recv_disable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.recv_disable();
				}
				bool fget_is_interrupt_enable(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)return Device.MobilePhone_uart.recv_is_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)return Device.RF_uart.recv_is_enable();
					else return false;
				}

				void fput(uint8 data_){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_data(data_);
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_data(data_);
				}
				if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_set_flag();
				void fput_set_interrupt(){
				else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_set_flag();
				}
				void fput_clear_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_clear_flag();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_clear_flag();
				}
				void fput_enable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_enable();
				}
				void fput_disable_interrupt(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)Device.MobilePhone_uart.send_disable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)Device.RF_uart.send_disable();
				}
				bool fput_is_interrupt_enable(){
					if(rf_module_uart_getModuleID() == rf_module_mode::mobile_phone)return Device.MobilePhone_uart.send_is_enable();
					else if(rf_module_uart_getModuleID() == rf_module_mode::rf_module)return Device.RF_uart.send_is_enable();
					else return false;
				}
			public:
				//���݂̒ʐM�悪�S��d�ʐM�Ή�����Ԃ�
				bool isFullDuplex(void);
			};
		}
	}
}
#
#endif
