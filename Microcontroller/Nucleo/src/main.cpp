#include "Robot.h"
using namespace std::__cxx11;

Timer dt;                                                      // Various Timer objects

int main()
{

  Robot robot = Robot();

  robot.init();
  dt.start();

  while (1)
  {

    //__disable_irq();                                                        // Disable interrupts to handle a loss of signal event. 
    //  robot.wifi.handleLossOfSignal();
    //__enable_irq();


    robot.live();

    /*if(robot.wifiTimer.read() >= .03) {                                           //! .075 This is where the WiFLY crashes. If .075 is decreased, the board will crash. 
      string out;
      out = to_string(5);

      robot.wifiTimer.reset();
      //robot.wifi.transmit(out);
      //robot.pc.transmit(out);
    }*/
    

    dt.reset();

  }

}