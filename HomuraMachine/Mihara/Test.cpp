//#include <homuraLib_v2/machine/service/task.hpp>
//#include <homuraLib_v2/task.hpp>
#include <XC32/sfr/uart5.hpp>
#include <XC32/pin.hpp>
#include <XC32/sfr/port.hpp>
//#include "TestCamera.hpp"
#include <homuraLib_v2/machine/module/TestSprite.hpp>
typedef xc32::sfr::uart5 uart_register;
hmr::machine::module::cSprite<uart_register,xc32::output_pin<xc32::sfr::portF::pin12>> Sprite;
//hmr::machine::module::cSpriteUart<uart_register> SpriteUart;
/*namespace hmr{
    namespace machine{
        namespace service{
            hmr::task::functional_host<> TaskHost;
            hmr::task::host_interface& Task(TaskHost);
        }
    }
}*/
int main(){
    return 0;
}