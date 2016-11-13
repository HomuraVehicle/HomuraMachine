#ifndef HMR_MACHINE_MIHARA_COURIER_INC
#define HMR_MACHINE_MIHARA_COURIER_INC 100
#
#include<hmLib/type.h>
namespace hmr {
	namespace machine{
		namespace mihara {
			namespace courier {
				enum mode {null,rf,mp,rf_debug};
				//rf : RF:9600
				//mp : MP:38400
				//rf_debuf : RF:38400

				//������
				void initialize();
				//�I�[��
				void finalize();
				//�^�X�N����
				void work();
				//ModuleID�ɓd�����ύX����
				void power_switch(boolian onoff, mode ModuleID_);
				//���݂�ModuleID���擾����
				mode getModuleID(void);
			}
		}
	}
}
#
#endif
