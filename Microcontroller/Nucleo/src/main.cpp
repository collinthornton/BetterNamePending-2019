#include <Includes.h>

using namespace std::__cxx11;

Timer dt, wifiTimer;                                                      // Various Timer objects

int main()
{

  pc.printf("Activated\n");
  dt.start();

  pcData.resize(128);

  //wifi.wifiConfig(false);                                                 // Set to true to load from saved -- set to false to change paramters
  //while(wifi.wifiInit() < 0) {                                            // Hang on failure to connect to server
  //  pc.printf("UNABLE TO CONNECT TO SERVER\n");
  //  hc05.transmit("UNABLE TO CONNECT TO SERVER\n");
  //}                                    
  //wifi.wifiConnectPing();

  //wifiTimer.start();
  
  //int counter = 0;

  while (1)
  {

   // __disable_irq();                                                        // Disable interrupts to handle a loss of signal event. 
   //   wifi.handleLossOfSignal();
   // __enable_irq();


    processPC();                                                                   

    //position.positionTimer();    
    //if(printPosition) {
      //pc.printf("%s %2i\n", position.toString().c_str(), dt.read_us());
    //  hc05.transmit(position.toString().c_str());
   // }

    /*if(wifiTimer.read() >= .03) {                                           //! .075 This is where the WiFLY crashes. If .075 is decreased, the board will crash. 
      string out;
      out = to_string(counter++);

      wifiTimer.reset();
      wifi.transmit(out);
      pc.printf("%s\r", out.c_str());
    }*/
    

    dt.reset();

  }

}