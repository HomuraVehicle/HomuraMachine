#ifndef HMR_MACHINE_LOCALALLOCATOR_INC
#define HMR_MACHINE_LOCALALLOCATOR_INC 100
#
#include<XC32Lib/interrupt_safe_allocator.hpp>
namespace xc {
	typedef xc32::interrupt_safe_allocator<void> local_allocator;
}
#
#endif
