#ifndef HMR_MACHINE_MIHARA_SERVICE_INC
#define HMR_MACHINE_MIHARA_SERVICE_INC 100
#
#include<homuraLib_v2/machine/service/delay.hpp>
#include<homuraLib_v2/machine/service/exclusive_delay.hpp>
#include<homuraLib_v2/machine/service/chrono.hpp>
#include"Device.hpp"

namespace hmr{
	namespace machine{
		namespace mihara{
			class cService :public cDevice::service_device{
				friend void hmr::machine::service::delay_ms(unsigned int ms_);
				friend void hmr::machine::service::exclusive_delay_ms(unsigned int ms_);
			private:
/*				struct message :public msgset::machine::chrono{
				private:
					cService& ref;
				public:
					message(cService& ref_) :ref(ref_){}
				private://msgset::maachine::chrono
					virtual void recv_time(){
						send_time(service::chrono::now());
					}
					virtual void recv_hds_time(){
						send_hds_time(service::chrono::now_hds());
					}
					virtual void recv_correction(hmr::chrono::unix_time_point TimeCorrection_Unix_, hmr::chrono::msec_time_point TimeCorrection_AddMSec_){
						ref.Clock.correct(TimeCorrection_Unix_, TimeCorrection_AddMSec_);
						send_inform_correction(TimeCorrection_Unix_, TimeCorrection_AddMSec_);
					}
				}Message;*/
			private:
				static xc32::delay_ms_timer<delay_timer_register> delay_timer;
				xc::lock_guard<xc32::delay_ms_timer<delay_timer_register>> delay_timer_Lock;
				static xc32::delay_ms_timer<exclusive_delay_timer_register> exclusive_delay_timer;
				xc::lock_guard<xc32::delay_ms_timer<exclusive_delay_timer_register>> exclusive_delay_timer_Lock;
			private://alarm_function for clock
//				typedef chrono::timer_clock<chrono_timer_register> clock_timer;
//				static clock_timer Clock;
//				xc::lock_guard<clock_timer> Clock_Lock;
			public:
//				static chrono::clock_interface& clock() { return Clock; }
/*			private://task_host
				typedef task::functional_host task_host_type;
				static task_host_type TaskHost;
				struct alarm_function :public clock_timer::alarm_function {
				private:
					cService& ref;
				public:
					alarm_function(cService& ref_) :ref(ref_){}
					void operator()(void) {
						TaskHost(1);
						if (service::chrono::now_sec() == 0)ref.Message.send_alarm(service::chrono::now_unix());
					}
				}ClockAlarm;
			public:
				static task::host_interface& task_host() { return TaskHost; }*/
			public:
				cService()
					:delay_timer_Lock(delay_timer)
					, exclusive_delay_timer_Lock(exclusive_delay_timer)
//					, Clock_Lock(Clock)
//					, ClockAlarm(*this)
//					, Message(*this)
				{
//					Clock.set_sec_alarm(ClockAlarm);
				}
				void operator()(void) {}
			};
		}
	}
}
#
#endif
