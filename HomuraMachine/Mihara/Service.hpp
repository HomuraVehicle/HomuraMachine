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
					//1000ms = 1秒おきに駆動するようセット
					TaskTimer.config(1000, TaskInterrupt, service_device_::task_timer_ipl());
					TaskTimerLock.lock();
					TaskTimer.start();
				}
				~cService(){
					TaskTimer.stop();
				}
				system_client_interface& getSystemClient(){return SystemClient;}
				void operator()(void){}
			};
		}
	}
}
#
#endif
