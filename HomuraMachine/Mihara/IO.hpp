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
				struct cDualUart: public io_device_{
				private:
					//���W�X�^�n
					xc32::interrupt_uart<RF0_uart_register> RFUart;
					xc32::interrupt_uart<RF1_uart_register> PhoneUart;
					RF0_power PinPowerRFUart;
					RF1_power PinPowerPhoneUart;
					//���݂̃��[�h
					io::module_mode Mode;
					//���M���荞�ݏ��
					bool IsTxInterruptEnable;
					//������ϊ����W���[��VMC1
					VMC1* pVMC;
				private:
					//����M�p�^�X�N VMC�Ɗ��荞�݃��W�X�^���Ȃ�
					struct tx_interrupt{
					private:
						cDualUart& Ref;
					public:
						tx_interrupt(cDualUart& Ref_) :Ref(Ref_){}
						void operator()(void){
							//���M�\�Ȃ�A���M
							Ref.putc(vmc1_send(Ref.pVMC);

							//���M�\�łȂ��Ȃ����ꍇ�́A���荞�݂�؂�
							if(!vmc1_can_send(Ref.pVMC)){
								Ref.disable_tx_interrupt();
							}
						}
					};
					struct rx_interrput{
					private:
						cDualUart& Ref;
					public:
						rx_interrput(cDualUart& Ref_) :Ref(Ref_){}
						void operator()(void){
							//�f�[�^����M���ACom�ɏ���������
							vmc1_recv(Ref.pVMC, Ref.getc());
						}
					};
				private:
					tx_interrupt TxInterrupt;
					rx_interrupt RxInterrupt;
				private:
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
							IsTxInterruptEnable = true;
							return;
						case io::module_phone:
							PhoneUart.send_enable();
							IsTxInterruptEnable = true;
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
							IsTxInterruptEnable = false;
							return;
						case io::module_phone:
							PhoneUart.send_disable();
							IsTxInterruptEnable = false;
							return;
						default:
							return;
						}
					}
					//interrupt�ĊJ�����݂�
					void check_tx_interrupt(){
						//���M���荞�݂��؂��Ă��āA�����M�\��Ԃ̂Ƃ��ɂ́A���M���荞�݂��I���ɂ���
						if(Mode != io::module_null && !IsTxInterruptEnable && vmc1_can_send(pVMC)){
							Uart.enable_tx_interrupt();
						}
					}
				public:
					cDualUart()
						: TxInterrupt(*this)
						, RxInterrupt(*this)
						, Mode(module_null){
						PinPowerRFUart.lock();
						PinPowerPhoneUart.lock();

						//�ʐM�֘A�@�\�̏�����
						pVMC = com::createVMC1();
						vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
					}
					~cDualUart(){
						//VMC���
						vmc1_finalize(pVMC);
						com::releaseVMC1(pVMC);

						setMode(module_null);
						PinPowerRFUart.unlock();
						PinPowerPhoneUart.unlock();
					}
				public:
					//���W���[���I��
					void setMode(io::module_mode ModuleMode_){
						//�������[�h�Ȃ�A����
						if(ModuleMode_ == Mode)return;

						Mode = ModuleMode_;

						IsTxInterruptEnable = false;
						switch(Mode){
						case io::module_phone:
							RFUart.unlock();
							PhoneUart.lock(38400, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PinPowerRFUart(false);
							PinPowerPhoneUart(true);
							check_tx_interrupt();
						case io::module_rf:
							PhoneUart.unlock();
							RFUart.lock(9600, xc32::uart::flowcontrol::rts_cts_control, SendInterrupt, RecvInterrupt);
							PinPowerRFUart(true);
							PinPowerPhoneUart(false);
							check_tx_interrupt();
						default:
							RFUart.unlock();
							PhoneUart.unlock();
							PinPowerRFUart(false);
							PinPowerPhoneUart(false);
						}
					}
					//�����W���[���擾
					io::module_mode getMode()const{ return Mode; }
					//�ǂ����g���ĂȂ���΁Afalse��Ԃ�
					operator bool()const{ return Mode != io::module_null; }
					//Uart�쓮�֐�
					void operator()(void){
						check_tx_interrupt();
					}
				};
			private:
				//Uart
				cDualUart Uart;
				//�󂯎��p�f�[�^
				idata IData;
				bool IPacketMode;
				odata OData;
				bool OPacketMode;
				//���b�Z�[�W�z���_�[
				cMessage Message;
			public:
				cIO()
					: Uart()
					, IData()
					, IPacketMode(false)
					, OData()
					, OPacketMode(false)
					, Message(){
					com::initialize();
				}
				~cIO{
					//�ʐM�֘A�̏I�[������
					com::finalize();
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

					//Uart�쓮
					Uart();
				}
			};
		}
	}
}
#
#endif
