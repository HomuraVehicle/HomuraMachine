#ifndef HMR_MACHINE_MIHARA_COURIER_CPP_INC
#define HMR_MACHINE_MIHARA_COURIER_CPP_INC 100
#
#include<hmLib/type.h>
#include"Courier.hpp"
#include"Device.hpp"
#include"Com.hpp"
#include<homuraLib/hmrVMC1.h>
namespace hmr {
	namespace machine{
		namespace mihara {
			namespace courier {
				namespace {
					mode ModuleID;

					//VMC1
					VMC1* pVMC;

					struct device :public cDevice::courier_device {
					public:
						xc32::interrupt_uart<RF0_uart_register> RF0InterruptUart;
						xc32::interrupt_uart<RF1_uart_register> RF1InterruptUart;

						RF0_power RF0Power;
						RF1_power RF1Power;
					}Device;

					//====== ���荞�݊֐� ======
					struct RF0_recv_interrupt :public xc32::interrupt::function{
						virtual void operator()(void) {
							//�����݃t���O���N���A
							Device.RF0InterruptUart.recv_clear_flag();
							Device.RF0InterruptUart.recv_clear_overflow();

							//�f�[�^�����M���ACom�ɏ���������
							vmc1_recv(pVMC, Device.RF0InterruptUart.recv_data());
						}
					}RF0RecvInterrupt;
					struct RF1_recv_interrupt :public xc32::interrupt::function{
						virtual void operator()(void) {
							//�����݃t���O���N���A
							Device.RF1InterruptUart.recv_clear_flag();
							Device.RF1InterruptUart.recv_clear_overflow();

							//�f�[�^�����M���ACom�ɏ���������
							vmc1_recv(pVMC, Device.RF1InterruptUart.recv_data());
						}
					}RF1RecvInterrupt;
					struct RF0_send_interrupt :public xc32::interrupt::function{
						virtual void operator()(void) {
							//�����݃t���O���N���A
							Device.RF0InterruptUart.send_clear_flag();

							//���M�\�Ȃ��A���M
							Device.RF0InterruptUart.send_data(vmc1_send(pVMC));

							//���M�\�łȂ��Ȃ����ꍇ�́A���荞�݂��؂�
							if (!vmc1_can_send(pVMC)){
								Device.RF0InterruptUart.send_disable();
							}
						}
					}RF0SendInterrupt;
					struct RF1_send_interrupt :public xc32::interrupt::function{
						virtual void operator()(void) {
							//�����݃t���O���N���A
							Device.RF1InterruptUart.send_clear_flag();

							//���M�\�Ȃ��A���M
							Device.RF1InterruptUart.send_data(vmc1_send(pVMC));

							//���M�\�łȂ��Ȃ����ꍇ�́A���荞�݂��؂�
							if (!vmc1_can_send(pVMC)){
								Device.RF1InterruptUart.send_disable();
							}
						}
					}RF1SendInterrupt;
				}

				//������
				void initialize(){
					ModuleID = mode::null;

					Device.RF0Power.lock();
					Device.RF1Power.lock();

					power_switch(false, mode::null);

					pVMC = com::createVMC1();
					vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));

					streamVMC_set_fput_interrupt_flag();//set_interrupt_fputflag(Stream_VMC);//���荞�ݗ\��
					devmng_interrupt_enable_streamVMC_fget_interrupt();// enable_interrupt_fget(Stream_VMC);
					devmng_interrupt_enable_streamVMC_fput_interrupt();// enable_interrupt_fput(Stream_VMC);
				}
				//�I�[��
				void finalize(){
					vmc1_finalize(pVMC);
					com::releaseVMC1(pVMC);

					power_switch(false, mode::null);

					Device.RF0Power.unlock();
					Device.RF1Power.unlock();
				}
				//�^�X�N����
				void work(){
					if (Device.RF0InterruptUart.is_lock() && Device.RF0InterruptUart.send_is_enable() && vmc1_can_send(pVMC)){
						Device.RF0InterruptUart.send_enable();
					}
					if (Device.RF1InterruptUart.is_lock() && Device.RF1InterruptUart.send_is_enable() && vmc1_can_send(pVMC)){
						Device.RF1InterruptUart.send_enable();
					}
				}
				//ModuleID�ɓd�����ύX����
				void power_switch(boolian onoff, mode ModuleID_){
					if (ModuleID_ != ModuleID)return;

					Device.RF0Power(false);
					Device.RF1Power(false);
					Device.RF0InterruptUart.unlock();
					Device.RF1InterruptUart.unlock();

					switch (ModuleID){
					case mode::rf:
						Device.RF0Power(true);
						Device.RF1Power(false);
						Device.RF0InterruptUart.lock(RF0SendInterrupt, RF0RecvInterrupt, 5, 9600, xc32::uart::flowcontrol::no_control);
						Device.RF1InterruptUart.unlock();
						return;
					case mode::mp:
						Device.RF0Power(true);
						Device.RF1Power(false);
						Device.RF0InterruptUart.lock(RF1SendInterrupt, RF1RecvInterrupt, 5, 38400, xc32::uart::flowcontrol::rts_cts_control);
						Device.RF1InterruptUart.unlock();
						return;
					case mode::rf_debug:
						Device.RF0Power(true);
						Device.RF1Power(false);
						Device.RF0InterruptUart.lock(RF0SendInterrupt, RF0RecvInterrupt, 5, 38400, xc32::uart::flowcontrol::no_control);
						Device.RF1InterruptUart.unlock();
						return;
					default:
						return;
					}
				}
				//���݂�ModuleID���擾����
				mode getModuleID(void){ return ModuleID; }
			}
		}
	}
}
#
#endif

