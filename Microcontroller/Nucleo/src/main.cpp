#include <mbed.h>

#define MAX_DISTANCE 240

#include <Ping.h>
#include <WiFly.h>

const unsigned int SONAR_NUM = 1;
Ping sonar[SONAR_NUM] = {
Ping(D2, MAX_DISTANCE)
};

Timer dt;
Timeout sonarTimeout[SONAR_NUM], checkSum;
unsigned int ping[SONAR_NUM];


void sonar0_ISR()
{
  ping[0] = sonar[0].ping_cm();
  sonarTimeout[0].attach_us(&sonar0_ISR, 33000);
}

// --- add other ISR functions here --- //
//  -be sure to update SONAR_NUM and attach appropriate interrupts
//  -example for next ISR below
//
//  void sonar1_ISR() {
//    ping[1] = sonar[1].ping_cm();
//    sonarTimeout[1].attach_us(&sonar1_ISR, 33000);
//  }
//
// ----------------------------------- //

int main()
{

  pc.printf("Activated\n");
  dt.start();

  wifi.attach(&wifi_ISR);
  //wifiConfig();   // Uncomment this line to set the necessary WiFLY paramters
  wifiInit();
  connected.fall(&hostLost);
  connected.rise(&hostGained);
  joined.fall(&networkLost);
  joined.rise(&networkGained);

  wifiConnectPing();

  //sonarTimeout[0].attach_us(&sonar0_ISR, 33000);
  // sonarTimeout[1].attach_us(&sonar1_ISR, 33000);
  
  //while(!wifi.readable()) {}

  while (1)
  {

    char pcIn;
    if(pc.readable()) {
      pcIn = pc.getc();
      if(pcIn == '~') {
        wifiConnectPing();
        pc.printf("\n");
      }
      wifi.putc(pcIn);
      //pc.putc(pcIn);
    }

    //pc.printf("%d - %i\n", ping[0], dt.read_us());

    dt.reset();

  }
}