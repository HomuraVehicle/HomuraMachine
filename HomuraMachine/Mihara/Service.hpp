#ifndef HMR_MACHINE_MIHARA_SERVICE_INC
#define HMR_MACHINE_MIHARA_SERVICE_INC 100
#
#include<XCBase/lock.hpp>
#include<homuraLib_v2/task.hpp>
#include"Service_base.hpp"
#include"System_base.hpp"
#include"Device.hpp"
namespace hmr{
	namespace machine{
		namespace mihara{
			template<typename service_device_>
			class cService :public service_device_, public service_interface{
			private:
				typedef service_device_ my_device;
				typedef cService<service_device_> my_type;
			private:
				//Power pins
				typename service_device_::pinADC0Power ADC0Power;
				typename service_device_::pinADC1to4Power ADC1to4Power;
				typename service_device_::pin5VDCDC DCDCPower;
			private:
				struct delay : public hmr::delay_interface{
				private:
					/*!delay関数用タイマー*/
					xc32::delay_ms_timer<typename my_device::delay_timer_register> delay_timer;
					/*!delay関数用タイマーロック*/
					xc::lock_guard<xc32::delay_ms_timer<typename my_device::delay_timer_register>> delay_timer_Lock;
					/*!exclusive_delay関数用タイマー*/
					xc32::delay_ms_timer<typename my_device::exclusive_delay_timer_register> exclusive_delay_timer;
					/*!exclusive_delay関数用タイマーロック*/
					xc::lock_guard<xc32::delay_ms_timer<typename my_device::exclusive_delay_timer_register>> exclusive_delay_timer_Lock;
				public:
					delay()
						: delay_timer()
						, delay_timer_Lock(delay_timer)
						, exclusive_delay_timer()
						, exclusive_delay_timer_Lock(exclusive_delay_timer){
					}
				public:
					/*!delay関数*/
					void delay_ms(duration ms_){ delay_timer(ms_); }
					/*!割り込み排除delay関数*/
					void exclusive_delay_ms(duration ms_){
						xc32::interrupt::lock_guard Lock(xc32::interrupt::Mutex);
						exclusive_delay_timer(ms_);
					}
				};
				delay Delay;
			private:
				typedef xc32::interrupt_timer<typename my_device::task_timer_register> task_timer;
				/*!タスク駆動用タイマー*/
				task_timer TaskTimer;
				xc::unique_lock<task_timer> TaskTimerLock;
			private:
				bool TaskSleep;
				/*!タスクホスト Sleep時は停止する。*/
				hmr::task::functional_host<> TaskHost;
				/*!システム用タスク Sleep時も駆動する。*/
				hmr::task::functional_host<> SystemTaskHost;
			public:
				virtual hmr::delay_interface& delay(){ return Delay; }
				virtual hmr::task::host_interface& task(){ return TaskHost; }
			public:
				hmr::task::host_interface& system_task(){ return SystemTaskHost; }
			private:
				/*!@brief タスクタイマー用割り込み関数処理関数*/
				struct task_interrupt_function : public xc32::sfr::interrupt::function{
				private:
					my_type& Ref;
				public:
					task_interrupt_function(my_type& Ref_):Ref(Ref_){}
					//!xc32::sfr::interrupt::function用の関数
					void operator()(void){
						if(!Ref.TaskSleep)Ref.TaskHost(1);
						Ref.SystemTaskHost(1);
					}
				}TaskInterrupt;
			private:
				typename my_device::async_adc ADC;
				xc32::adc::block_setting ADCBlockSetting;
				xc32::adc::converter_setting ADCConverterSetting;

				typename my_device::shared_i2c I2C;
				typename my_device::shared_spi SPI;
			private:
				struct system_client : public system_client_interface{
				private:
					my_type& Ref;
				public:
					system_client(my_type& Ref_):Ref(Ref_){}
				public://system_clien_interface
					void operator()(systems::mode::type NewMode_, systems::mode::type PreMode_){
						if(NewMode_ == systems::mode::sleep)Ref.TaskSleep = true;
						else Ref.TaskSleep = false;
					}
				}SystemClient;
			public:
				cService()
					: TaskTimerLock(TaskTimer, true)
					, TaskInterrupt(*this)
					, SystemClient(*this){

					ADC0Power.lock();
					ADC0Power(1);
					ADC1to4Power.lock();
					ADC1to4Power(1);
					DCDCPower.lock();
					DCDCPower(1);
					
					//1000ms = 1秒おきに駆動するようセット
					TaskTimer.config(1000, TaskInterrupt, service_device_::task_timer_ipl());
					TaskTimerLock.lock();
					TaskTimer.start();

					ADCBlockSetting.ClockDiv = 2;
					ADCBlockSetting.VrefMode = xc32::adc::vref_mode::vref_Vref_Gnd;
					ADCConverterSetting.ClockDiv = 1;
					ADCConverterSetting.ResolutionMode = xc32::sfr::adc::resolution_mode::resolution_12bits;
					ADCConverterSetting.SamplingTime = 5;


					ADC.set_block_setting(ADCBlockSetting);

					#ifdef BLCK_ANALOG_0
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_0>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_1
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_1>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_2
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_2>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_3
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_3>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_4
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_4>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_5
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_5>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_6
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_6>>(ADCConverterSetting);
					#endif

					#ifdef BLCK_ANALOG_7
					ADC.template set_converter_setting<xc::constexpr_no<BLCK_ANALOG_7>>(ADCConverterSetting);
					#endif

				}
				~cService(){
					TaskTimer.stop();
					ADC0Power.unlock();
					ADC1to4Power.unlock();
					DCDCPower.unlock();
				}
				system_client_interface& getSystemClient(){return SystemClient;}
				void operator()(void){}
			};
		}
	}
}
#
#endif
