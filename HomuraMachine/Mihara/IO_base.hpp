#ifndef HMR_MACHINE_MIHARA_IOBASE_INC
#define HMR_MACHINE_MIHARA_IOBASE_INC 100
#
namespace hmr{
	namespace machine{
		namespace mihara{
			namespace io{
				//ModuleID
				enum module_mode{
					module_null = 0x00,
					module_phone = 0x01,
					module_rf = 0x02
				};
				//module selector
				struct module_selector_interface{
					virtual bool setModuleMode(module_mode Mode_) = 0;
					virtual module_mode getModuleMode()const = 0;
				};
			}
		}
}
#
#endif
