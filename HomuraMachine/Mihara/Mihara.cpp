#ifndef HMR_MACHINE_MIHARA_INC
#define HMR_MACHINE_MIHARA_INC 0
#
#
#define HMLIB_NOLIB
/*
=== HomuraMachine_Mihara ===
v0_00/141111 hmito
	cpp�������o�[�W����
===hmrM2500===
v2_01/131019 hmIto
	DeviceManage�V�X�e���𓱓�
v2_00/130720 hmIto
	�f�o�b�O�p�̊֐��Q���R�����g�������Đ���
v1_01/130202 iwahori
	main�֐�����message::setup_talk��message::setup_listen���ǉ�
	listen�֐��ɓ����Ă����̂Ŏ��M�͏��肭�����Ă��邪�ԐM���Ȃ��C
	�p�P�b�g�̋��؂�������packet_talk�Ƃ��������Ă��Ȃ��̂������H
	fp_finRecvPac,com::vmc_finRecvPac�ɈϏ��H
v1_00/130112 hmIto
	hmLib_v3_03�ɑΉ�
	�S���d�ʐM�ɑΉ�
	�Z���T�[�����𕪗�����include
===hmrMain2400===
v1_09/121101 hmIto
	restart_wdt��main-loop�̈ꂩ���ɏW��
	�p�P�b�g���؂���HMR_COM_PACTRMNID���g�p
	���M�p�P�b�g���t���ɂȂ����ꍇ�́A�������Z�b�g
	�������Z�b�g�R�}���h�Ƃ���Message�Ɉˑ����Ȃ�(Com�ɂ͈ˑ�)�A0xFF���߂��ǉ�
	->�o�O�����������߈ꎞ�I�ɔp�~
	sprite::message2���g�p�����悤�ɕύX
v1_08/121031 hmIto
	devctr�ɔ��X���[�v���̓d�������p�ϐ��Ƃ��̃R�}���h���ǉ�
v1_07/121030 iwahori
	�����x��message::resist���ǉ�
v1_06/121028 hmIto
	�X���[�v�@�\�ƃ��[�~���O�@�\�i�ʐM�f�o�C�X�؂��ւ��j������
		devctr�֐��Q���p���đ���
		���������Adevice_message�R�}���h���p���Đ��䂷��
		timer4_5���p���Ă���
			�X���[�v����10�b�Ɉ��x���荞�܂��āA���Ԍv��
			���[�~���O���̃f�o�C�X�؂��ւ��Ɏg�p
	���M�o�b�t�@�Ɋ��Â����M���荞�݊��荞�ݐ������̗p
		���M�o�b�t�@�������ɂȂ�����disable
		���M�o�b�t�@�Ƀf�[�^���ǉ�������enable
		com::out_empty�֐����g���āA���M�o�b�t�@���󂩂ǂ����m�F
v1_05/121027 hmIto
	uart�̑����M�������ɍs�����邱�Ƃ܂Ŋm�F
		hmrTask�����邳�����Ă����̂Ŕp�~�D
		�o�b�t�@�̐��䎞�ɑ��M���荞�݂𐧌䂷���悤�ɕύX
v1_04/121020 iwahori
	initialize()����SPI slave slect�s���̏��������ǉ�
		RF module�d���I��
v1_03/121020 hmIto
	�f�o�b�O�C�����m�F���āA�R�����g�A
	�E�g���폜
	packet_message�֐����������������̂��ǉ�
v1_02/121014 iwahori
	#include "hmrSpriteFunctions.h"���ǉ�
	p_DIP1��UserConfigBit1�ɕύX
	uart1_fget_interrupt_retry_task()��uart1_fput_interrupt()�̏��Ԃ������ւ�
	���M�����l
	packet_message()��cstring Ans{1,1}��cstring Ans={1,1}
	sprite::ON��sprite::power�ɕύX
v1_01/121013 hmIto
	���炭�����M�������قڊ���
	�������I�[�����K�v�Ȋ֐��̍쐬���c��
v1_00/120921 hmIto
	�o�[�W�����Ǘ��J�n
	�t�@�C���𕡐��ɕ���
*/
#include <homuraLib_v2/old/hmrVMC1.h>
#include <homuraLib_v2/machine/service/delay.hpp>
#include <homuraLib_v2/machine/service/task.hpp>
#include <homuraLib_v2/type.hpp>
#include "Device.hpp"
#include "Service.hpp"
#include "Com.hpp"
#include "Message.hpp"
#include "CO2.hpp"
#include "Battery.hpp"
#include "GPS.hpp"
#include "Drive.hpp"
#include "Inertial.hpp"
#include "Camera.hpp" 
#include "Thermo.hpp"
#include "Battery.hpp"
#include "DeviceManage.hpp"
#include "DeviceManage_forCourier.hpp"
#include "DeviceManageMessage.hpp"

using namespace hmr::machine::mihara;
//VMC1
VMC1* pVMC;
//courier interrpt function
devmng::courier::uart::tx_interrupt_function devmng::courier::uart::CourierTxInterruptFunc;
void devmng::courier::uart::tx_interrupt_function::operator()(void){
	//���荞�݃t���O���N���A
//	clear_interrupt_fputflag(Stream_VMC);

	//���M�\�Ȃ��A���M
	devmng::courier::uart::fput(vmc1_send(pVMC));

	//���M�\�łȂ��Ȃ����ꍇ�́A���荞�݂��؂�
	if(!vmc1_can_send(pVMC)){
		//uart1_disable_fput_interrupt();
		devmng::interrupt_disable_streamVMC_fput_interrupt();
	}
}
devmng::courier::uart::rx_interrupt_function devmng::courier::uart::CourierRxInterruptFunc;
void devmng::courier::uart::rx_interrupt_function::operator()(void){
	//�����݃t���O���N���A
//	clear_interrupt_fgetflag(Stream_VMC);

	//�f�[�^�����M���ACom�ɏ���������
	vmc1_recv(pVMC,devmng::courier::uart::fget());
}

//====== main �֐� ======
using namespace hmr::machine::mihara;
using namespace hmr::machine::service;
int main(void){
	cDevice Device;
	cService Service;

	//�f�o�C�X�̏�����
	devmng::initialize();

	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::axel::device_initialize, 0);
	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::compass::device_initialize, 0);
	if(devmng::sleep_can_registRemInformFunction())devmng::sleep_registRemInformFunction(inertial::gyro::device_initialize, 0);

	//�ʐM�֘A�@�\�̏�����
	pVMC = com::createVMC1();
	vmc1_initialize(pVMC, (const unsigned char*)("hmr"), (const unsigned char*)("ctr"));
	com::initialize();
	message::initialize();

	//���荞�ݐ���
	devmng::interrupt_enable_timerDevmng_interrupt();

	devmng::courier::uart::fput_set_interrupt();//streamVMC_set_fput_interrupt_flag();//set_interrupt_fputflag(Stream_VMC);//���荞�ݗ\��
	devmng::interrupt_enable_streamVMC_fget_interrupt();// enable_interrupt_fget(Stream_VMC);
	devmng::interrupt_enable_streamVMC_fput_interrupt();// enable_interrupt_fput(Stream_VMC);

	//���W���[��������
	devmngmsg::initialize();
	thermo::initialize();
	battery::initialize();
	camera::initialize();
	inertial::initialize();
	co2::initialize();
	motor::initialize();
	cGPS<cDevice::gps_device> GPS;
	gps::initialize(GPS);

	//���b�Z�[�W�o�^
//	message::regist('!',debug_setup_listen,debug_listen,debug_setup_talk,debug_talk);
	message::regist('m',motor::setup_listen,motor::listen,motor::setup_talk,motor::talk);
	message::regist('c',inertial::compass::setup_listen,inertial::compass::listen,inertial::compass::setup_talk,inertial::compass::talk);
	message::regist('a',inertial::axel::setup_listen,inertial::axel::listen,inertial::axel::setup_talk,inertial::axel::talk);
	message::regist('G',inertial::gyro::setup_listen,inertial::gyro::listen,inertial::gyro::setup_talk,inertial::gyro::talk);
	message::regist('j', camera::setup_listen, camera::listen, camera::setup_talk, camera::talk);
	message::regist('g',gps::setup_listen,gps::listen,gps::task_setup_talk,gps::talk);
	message::regist('b',battery::setup_listen,battery::listen,battery::task_setup_talk,battery::talk);
	message::regist('C',co2::setup_listen,co2::listen,co2::task_setup_talk,co2::talk);
	message::regist('t',thermo::setup_listen,thermo::listen,thermo::task_setup_talk,thermo::talk);
	message::regist('D', devmngmsg::setup_listen, devmngmsg::listen, devmngmsg::setup_talk, devmngmsg::talk);
//	message::regist('T',infrared_setup_listen,infrared_listen,infrared_task_setup_talk,infrared_talk);
//	message::regist('S',h2s_message);
//	message::regist('7',sht75_setup_listen,sht75_listen,sht75_task_setup_talk,sht75_talk);

	//�^�X�N�o�^�i�ڍs���Ă��Ȃ�functional_task�o�[�W�����̊֐��j
//	devmng_task_quick_start(5,infrared_task_data,0);
//	devmng_task_quick_start(5,infrared_task_inform,0);
//	devmng_task_quick_start(5,sht75_task_data,0);
//	devmng_task_quick_start(5,sht75_task_inform,0);
//	devmng_task_quick_start(10,h2s_task_interrupt,0);

	//�󂯎����p�f�[�^
	idata IData;
	bool IPacketMode = 0;
	odata OData;
	bool OPacketMode = 0;
	hmr::sint8 debug_cnt = 0;

	while(1){
		Device();
		Service();
		camera::work();
		inertial::work();
		//Watch Dog Timer ���Z�b�g
		devmng::clear_wdt();
//		restart_wdt();

		//DeviceManage�̊��荞�݈Ϗ������֐�
		if(devmng::interrupt_yield())continue;

		//���������̂őҋ@
		delay_ms(5);

		//���M�\�ȃf�[�^�������ꍇ
		if(!com::in_empty()){
			//���M�f�[�^���擾
			com::in_move_pop(&IData);

			//���g�������Ă����ꍇ
			if(idata_is_construct(&IData)){
				//Packet���܂��J���Ă��Ȃ��ꍇ
				if(!IPacketMode){
					//Packet�������ŊJ��
					IPacketMode=true;

					//���M�p���b�Z�[�W���W���[������
					message::setup_listen();
				}

				//Packet�I��ID�������ꍇ
				if(IData.ID==HMR_COM_PACTRMNID){
					//Packet�������ŕ���
					IPacketMode=false;

					//���b�Z�[�W�j��
					idata_destruct(&IData);
				}else{
					//���b�Z�[�W����;
					message::listen(&IData);
				}
			}
		}

		//���������̂őҋ@
		delay_ms(5);

		//���M�҂���Packet���Ȃ��Acom�̑��M�o�b�t�@�������ς��ł��Ȃ��Ƃ�
		if(!com::isWaitSendPacket() && !com::out_full()){
			//Packet���܂��J���Ă��Ȃ��ꍇ
			if(!OPacketMode){
				//Packet�������ŊJ��
				OPacketMode=true;
				//���M�p���b�Z�[�W���W���[������
				message::setup_talk();
			}

			//���M�p�f�[�^����������
			odata_format(&OData);

			//���M�f�[�^�̎擾�Ɏ��s�����ꍇ
			if(message::talk(&OData)){
				//���g���쐬�����Ă����Δj��
				if(odata_is_construct(&OData))odata_destruct(&OData);
				//Packet�������ŕ���
				OPacketMode=false;
				OData.ID=HMR_COM_PACTRMNID;
			}

			//���M�f�[�^�𑗂��o��
			com::out_move_push(&OData);		
		}

		//���M���荞�݂��؂����Ă��āA�����M�\���Ԃ̂Ƃ��ɂ́A���M���荞�݂��I���ɂ���
		if(!devmng::courier::uart::fput_is_interrupt_enable()){
			if(vmc1_can_send(pVMC))devmng::interrupt_enable_streamVMC_fput_interrupt();//uart1_enable_fput_interrupt();
		}

	}

	//�I�[������

	//���荞�݋֎~
	devmng::courier::uart::fget_disable_interrupt();
	devmng::courier::uart::fput_disable_interrupt();

	//�ʐM�֘A�̏I�[������
	message::finalize();
	vmc1_finalize(pVMC);
	com::finalize();
	com::releaseVMC1(pVMC);

	//�f�o�C�X�I�[��
	devmng::finalize();
	camera::finalize();
	inertial::finalize();
	thermo::finalize();
	co2::finalize();
	battery::finalize();
	

	return 0;
}

#
#endif
