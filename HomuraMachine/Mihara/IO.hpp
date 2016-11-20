#ifndef HMR_MACHINE_IO_INC
#define HMR_MACHINE_IO_INC 100
#
#include<hmLib/cstring.h>
#include<XCBase/future.hpp>
#include<homuraLib_v2/old/hmrVMC1.h>
#include<homuraLib_v2/type.hpp>
#include<homuraLib_v2/machine/service/safe_cstring.hpp>
#include<homuraLib_v2/machine/service/task.hpp>
#include<homuraLib_v2/gate.hpp>
#include"Com.hpp"
#include"System_base.hpp"
#include"Message_base.hpp"
#include"Message.hpp"
#include"Device.hpp"
#include"IO_base.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			template<typename io_device_>
			struct cIO :public message_host_interface, public io::module_selector_interface, public io_device_{
				typedef cIO<io_device_> this_type;
			private:
				struct tx_interrupt{
				private:
					this_type& Ref;
				public:
					void operator()(void){
						//���M�\�Ȃ�A���M
						Ref.Uart.putc(vmc1_send(pVMC);

						//���M�\�łȂ��Ȃ����ꍇ�́A���荞�݂�؂�
						if(!vmc1_can_send(pVMC)){
							Ref.Uart.disable_tx_interrupt();
						}
					}
				};
				struct rx_interrput{
				private:
					this_type& Ref;
				public:
					void operator()(void){
						//�f�[�^����M���ACom�ɏ���������
						vmc1_recv(pVMC, Ref.Uart.getc());
					}
				};
			private:
				struct cDualUart: public io_device_{
					typedef cUartSwitcher<io_device_> this_type;
				public:
				private:
					tx_interrupt TxInterrupt;
					rx_interrupt RxInterrupt;
				private:
					//���W�X�^�n
					xc32::interrupt_uart<RF0_uart_register> RFUart;
					xc32::interrupt_uart<RF1_uart_register> PhoneUart;
					RF0_power PinPowerRFUart;
					RF1_power PinPowerPhoneUart;
					//���݂̃��[�h
					io::module_mode Mode;
				public:
					cDualUart(const tx_interrupt& TxInterrupt_, const rx_interrupt& RxInterrupt_)
						: TxInterrupt(TxInterrupt_)
						, RxInterrupt(RxInterrupt_)
						, Mode(module_null){
						PinPowerRFUart.lock();
						PinPowerPhoneUart.lock();
					}
					~cDualUart(){
						setMode(module_null);
						PinPowerRFUart.unlock();
						PinPowerPhoneUart.unlock();
					}
					void setMode(io::module_mode ModuleMode_){
						//�������[�h�Ȃ�A����
						if(ModuleMode_ == Mode)return;

						Mode = ModuleMode_;
						switch(Mode){
						case io::module_phone:
							RFUart.unlock();
							PhoneUart.lock(38400, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PinPowerRFUartlock(false);
							PinPowerPhoneUartlock(true);
						case io::module_rf:
							RFUart.lock(9600, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PhoneUart.unlock();
							PinPowerRFUartlock(true);
							PinPowerPhoneUartlock(false);
						default:
							RFUart.unlock();
							PhoneUart.unlock();
							PinPowerRFUartlock(false);
							PinPowerPhoneUartlock(false);
						}
					}
					io::module_mode getMode()const{ return Mode; }
					operator bool()const{ return Mode == io::module_null; }
				public:
					//���M�������1byte�擾����
					unsigned char getc(){
						switch(Mode){
						case io::module_rf:
							return RFUart.recv_data();
						case io::module_phone:
							return PhoneUart.recv_data();
						default:
							return 0;
						}
					}
					//��M�������1byte�^����
					void putc(unsigned char c){
						switch(Mode){
						case io::module_rf:
							RFUart.send_data(c);
							return;
						case io::module_phone:
							PhoneUart.send_data(c);
							return;
						default:
							return;
						}
					}
					//tx interrupt enable
					void enable_tx_interrupt(){
						switch(Mode){
						case io::module_rf:
							RFUart.send_enable();
							return;
						case io::module_phone:
							PhoneUart.send_enable();
							return;
						default:
							return;
						}
					}
					//tx interrupt disable
					void disable_tx_interrupt(){
						switch(Mode){
						case io::module_rf:
							RFUart.send_disable();
							return;
						case io::module_phone:
							PhoneUart.send_disable();
							return;
						default:
							return;
						}
					}
				};
			private:
				//Uart
				cDualUart Uart;
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
				cIO(){
					//�ʐM�֘A�@�\�̏�����
					pVMC = com::createVMC1();
					vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					com::initialize();
				}
				~cIO{
					//�ʐM�֘A�̏I�[������
					vmc1_finalize(pVMC);
					com::finalize();
					com::releaseVMC1(pVMC);	
				}
			public:
				//message_host_interface
				virtual void regist(message::element& rElement_){
					Message.regist(rElement_);
				}
			public:
				//module_selector_interface
				virtual bool setModuleMode(io::module_mode Mode_){
					Uart.setMode(Mode_);
					return false;
				}
				virtual io::module_mode getModuleMode()const{
					return Uart.getMode();
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
						if(vmc1_can_send(pVMC))Uart.enable_tx_interrupt();
					}
				}
			};
		}
	}
}
#
#endif
