#define MAX_DISTANCE 25
#define SONAR_NUM 1                         // Adjust to indicate total number of ultrasonic sensors. 
#define ENABLE_SONAR_INTERRUPT              // Comment this line to enable single pin sonar, uncomment to increase speed by about 200x
//#define ADJUST_HC05

#include <Includes.h>

Ping sonar[SONAR_NUM] = {
  #ifdef ENABLE_SONAR_INTERRUPT 
  Ping(D8, D9, MAX_DISTANCE)                // Declare ultrasonic sensors here for interrupt use
  #else
  Ping(D8, MAX_DISTANCE)                    // Declare ultrasonic sensors here for single pin use
  #endif
};

Timer dt, sonarTimer, wifiTimer;            // Various Timer objects
int *ping;                                  // Variable to track distance in cm for each ultrasonic sensor

void sonar_timer() {                        // Ping all Sonar objects and reset the sonar timer
  for(unsigned int i=0; i<SONAR_NUM; ++i)
    ping[i] = sonar[i].ping_cm(MAX_DISTANCE);
  sonarTimer.reset();
}

/*void sonar_timer() {
  ping = sonar[0].ping_cm_all(MAX_DISTANCE);
}*/

Ping * Ping::instance[SONAR_NUM];         // This fills declares an array the same size as the number of Sonar objects
                                          // it is used for the interrupt functions
int main()
{

  pc.printf("Activated\n");
  dt.start();

  pcData.resize(128);

  Wifi wifi = Wifi();
  wifi.wifiConfig(false);                        // Set to true to load from saved -- set to false to change paramters
  while(wifi.wifiInit() < 0) {                 // Hang on failure to connect to server
    pc.printf("UNABLE TO CONNECT TO SERVER\n");
    wifi.transmitBT("UNABLE TO CONNECT TO SERVER\n");
  }                                    
  //wifi.wifiConnectPing();

  sonarTimer.start();
  //wifiTimer.start();

  while (1)
  {

    //__disable_irq();                                  // Disable interrupts to handle a loss of signal event. 
    //  wifi.handleLossOfSignal();
    //__enable_irq();


    processPC();                                      
    wifi.processHC05();                                    

    if(sonarTimer.read_us() >= 15000 && printSonar == true) {               // Ping the ultrasonic sensors if 15 ms as elapsed
      sonar_timer();                                                        // Note that this value is calibrated for the MAX_DISTANCE that has been set
      pc.printf("%i %i\n", ping[0], dt.read_us());                          // If MAX_DISTANCE is increased, this must also be increased
    }

    
    if(wifiTimer.read() >= .033) {                                //! .075 This is where the WiFLY crashes. If .075 is decreased, the board will crash. 
      char *outPing, *outTime, *out;
      __itoa(ping[0], outPing, 10);
      __itoa(dt.read_us(), outTime, 10);
      strcat(outPing, outTime);
      out = strcat(out, "\r");
      wifi.transmit(out);              // Print the output of the sonar ping and loop time. 
      wifiTimer.reset();
    }
    

    dt.reset();

  }

}