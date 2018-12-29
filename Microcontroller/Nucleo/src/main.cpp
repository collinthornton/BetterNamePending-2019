#define MAX_DISTANCE 240
#define ENABLE_SONAR_INTERRUPT              // Comment this line to enable single pin sonar, uncomment to increase speed by about 200x

#include <Includes.h>

Ping sonar[SONAR_NUM] = {
  #ifdef ENABLE_SONAR_INTERRUPT 
  Ping(D2, D3, MAX_DISTANCE)                // Declare ultrasonic sensors here for interrupt use
  #else
  Ping(D2, MAX_DISTANCE)                    // Declare ultrasonic sensors here for single pin use
  #endif
};

Timer dt, sonarTimer;
unsigned int ping[SONAR_NUM];               // Variable to track distance in cm for each ultrasonic sensor

void sonar_timer() {
  for(unsigned int i=0; i<SONAR_NUM; ++i)
    ping[i] = sonar[i].ping_cm(MAX_DISTANCE);
  sonarTimer.reset();
}

Ping * Ping::instance[SONAR_NUM];


int main()
{

  pc.printf("Activated\n");
  dt.start();

  /*for(int i=0; i<SONAR_NUM; ++i) {
    Ping::instance[i] = NULL;
  }*/

  pcData.resize(128);
  wifiData.resize(128);

  wifi.attach(&wifi_ISR);

  wifiConfig(true);      // Set to true to load from saved -- set to false to change paramters
  while(wifiInit() < 0) {
    pc.printf("UNABLE TO CONNECT TO SERVER\n");
    hc05.printf("UNABLE TO CONNECT TO SERVER\n");
  }                         // Hang on failure to connect to server


  //wifiConnectPing();

  sonarTimer.start();

  while (1)
  {

    __disable_irq();
      handleLossOfSignal();
    __enable_irq();


    processPC();
    processHC05();

    if(sonarTimer.read_us() >= 33000) {
      sonar_timer();

    pc.printf("%i ", ping[0]);
    pc.printf("%i\n", dt.read_us());    
    }
  
    dt.reset();
    
  }

}