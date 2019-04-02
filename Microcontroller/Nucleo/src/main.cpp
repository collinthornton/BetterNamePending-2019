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

  sonarTimer.start();
  //wifiTimer.start();
  
  int counter = 0;

  while (1)
  {

   // __disable_irq();                                                        // Disable interrupts to handle a loss of signal event. 
   //   wifi.handleLossOfSignal();
   // __enable_irq();


    processPC();                                                                   
/*
    if(sonarTimer.read_us() >= 15000 && printSonar == true) {               // Ping the ultrasonic sensors if 15 ms as elapsed
      sonar_timer();                                                        // Note that this value is calibrated for the MAX_DISTANCE that has been set
      pc.printf("%2i %2i %2i %2i %2i %2i %2i %2i %2i\n", ping[0], ping[1], ping[2], ping[3], 
        ping[4], ping[5], ping[6], ping[7], dt.read_us());                          // If MAX_DISTANCE is increased, this must also be increased
    }*/   

    position.positionTimer();    
    if(printPosition) pc.printf("%s %2i\n", position.toString().c_str(), dt.read_us());

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