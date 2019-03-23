#include <Includes.h>


Timer dt, wifiTimer;                                    // Various Timer objects

int main()
{

  pc.printf("Activated\n");
  dt.start();

  pcData.resize(128);

  wifi.wifiConfig(false);                               // Set to true to load from saved -- set to false to change paramters
  while(wifi.wifiInit() < 0) {                          // Hang on failure to connect to server
    pc.printf("UNABLE TO CONNECT TO SERVER\n");
    hc05.transmit("UNABLE TO CONNECT TO SERVER\n");
  }                                    
  //wifi.wifiConnectPing();

  sonarTimer.start();
  wifiTimer.start();

  while (1)
  {

    //__disable_irq();                                  // Disable interrupts to handle a loss of signal event. 
    //  wifi.handleLossOfSignal();
    //__enable_irq();


    processPC();                                                                   

    /*if(sonarTimer.read_us() >= 15000 && printSonar == true) {               // Ping the ultrasonic sensors if 15 ms as elapsed
      sonar_timer();                                                        // Note that this value is calibrated for the MAX_DISTANCE that has been set
      pc.printf("%i %i\n", ping[0], dt.read_us());                          // If MAX_DISTANCE is increased, this must also be increased
    }*/   
    
    if(wifiTimer.read() >= .03) {                                //! .075 This is where the WiFLY crashes. If .075 is decreased, the board will crash. 
      char *outPing, *outTime, *out = "0";

      __itoa(dt.read_us(), out, 10);
      //__itoa(dt.read_us(), outTime, 10);
      //out = strcat(outPing, outTime);
      out = strcat(out, "\r\n");

      //wifi.transmit(out);                               // Print the output of the sonar ping and loop time. 
      wifiTimer.reset();
      wifi.transmit(out);
    }
    

    dt.reset();

  }

}