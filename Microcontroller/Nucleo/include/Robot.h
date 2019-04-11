#ifndef ROBOT_H
#define ROBOT_H

#include "Control.h"
#include "WiFly.h"
#include "SerialPC.h"

class Robot {
    public:
        Robot();
        void init(void);
        void live(void);

        PC pc = PC();
        Wifi wifi = Wifi();

        Timer wifiTimer;
    private:
        void findState(void);
        void outputData(void);

        string state;

        Control control = Control();
};

Robot::Robot() {}

void Robot::init(void) {
    pc.transmit("Activated");
    wifi.transmit("Activated");
    hc05.transmit("Activated");

  // wifi.wifiConfig(false);                                                 // Set to true to load from saved -- set to false to change paramters
    while(wifi.wifiInit() < 0) {                                            // Hang on failure to connect to server
        pc.transmit("UNABLE TO CONNECT TO SERVER\n");
        hc05.transmit("UNABLE TO CONNECT TO SERVER\n");
    }                                    
  
  wifiTimer.start();
  return;
}

void Robot::live(void) {
    findState();
    outputData();

    if(state == "WANDER") control.autonomous();
    else if(state == "MANUAL") {
        if(wifi.motorUpdated)       { control.manual(wifi.speed, wifi.theta, wifi.phi); wifi.motorUpdated = false; }
        else if(pc.motorUpdated)    { control.manual(pc.speed, pc.theta, pc.phi);       pc.motorUpdated   = false; }
        else if(hc05.motorUpdated)  { control.manual(hc05.speed, hc05.theta, hc05.phi); hc05.motorUpdated = false; }
    }
    
    return;
}

void Robot::findState(void) {
    if(wifi.driveType == "AUTONOMOUS") state == "WANDER";
    else if(wifi.driveType == "MANUAL") state == "MANUAL";
    
    return;
}

void Robot::outputData(void) {
    if(pc.printSonar)   pc.transmit(control.position.toString().c_str());
    if(hc05.printSonar) hc05.transmit(control.position.toString().c_str());
    if(wifi.printSonar) wifi.transmit(control.position.toString().c_str());

    return;
}


#endif