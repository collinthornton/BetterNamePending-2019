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

        bool MOTORS_ARMED = true;
    private:
        void findState(void);
        void outputData(void);
        float drivePowerMonitor(void);
        PwmOut speaker = PwmOut(PD_13);        

        string state = "MANUAL";
        bool firstLOS = true, careAboutVoltage = false;

        Control control = Control();
        Timer speakerTime;
};

Robot::Robot() {}

void Robot::init(void) {
    pc.transmit("Activated");
    wifi.transmit("Activated");
    hc05.transmit("Activated");
    //speaker.period(.0002);  

/*   wifi.wifiConfig(true);                                                 // Set to true to load from saved -- set to false to change paramters
    while(wifi.wifiInit() < 0) {                                            // Hang on failure to connect to server
        pc.transmit("UNABLE TO CONNECT TO SERVER\n");
        hc05.transmit("UNABLE TO CONNECT TO SERVER\n");
        speakerTime.start();
        if(speakerTime.read() >= 1 && speakerTime.read() < 2) {}//speaker = 0.5;
        else if(speakerTime.read() > 2) {
            speaker = 0.0;
            speakerTime.reset();
        }
        
    }
*/
    speakerTime.stop();
    speakerTime.reset();
    pc.transmit("CONNECTED TO SERVER!");
    hc05.transmit("CONNECTED TO SERVER!");  

                                
  
  wifiTimer.start();
  return;
}

void Robot::live(void) {
/*
    if(drivePowerMonitor() < 19.2 && careAboutVoltage) {
        MOTORS_ARMED = false;
        pc.transmit("LOW POWER");
        return;
    }

    if(wifi.NETWORK_TIMEOUT || wifi.CONNECTION_TIMEOUT) {
        if(firstLOS) {
            speakerTime.start();
            pc.transmit("LOST CONNECTION TO SERVER!");
            firstLOS = false;
        }
        if(speakerTime.read() >= 1  && speakerTime.read() < 2) {
            speaker = 0.5;
            pc.transmit("Playing tone");
        }
        else if(speakerTime.read() >= 2) {
            speaker = 0.0;
            speakerTime.reset();
        }
        //wifi.wifiInit();
        control.manual(0, 0, 0);
        return;
    }
    if(firstLOS == false) {
        pc.transmit("RECONNECTED TO SERVER!");
        speaker = 0.0;
        speakerTime.stop();
        speakerTime.reset();
        firstLOS = true;
    }
*/
    control.position.positionTimer();
    findState();
    outputData();

    if(MOTORS_ARMED) {
        if(state == "WANDER") control.autonomous();
        else if(state == "MANUAL") {
        // if(wifi.motorUpdated)       { control.manual(wifi.speed, wifi.theta, wifi.phi); wifi.motorUpdated = false; }
            //if(hc05.motorUpdated)  { control.manual(hc05.speed, hc05.theta, hc05.phi); hc05.motorUpdated = false; }        
           // if(pc.motorUpdated)    { control.manual(pc.speed, pc.theta, pc.phi);       pc.motorUpdated   = false; }
        }
    }

    
    return;
}

void Robot::findState(void) {
    pc.processPC();
    if(wifi.driveType == "AUTONOMOUS") state == "WANDER";
    else if(wifi.driveType == "MANUAL") state == "MANUAL";
    
    return;
}

void Robot::outputData(void) {
    if(pc.printSonar)    pc.transmit(control.position.toString().c_str());
    //if(hc05.printSonar) hc05.transmit(control.position.toString().c_str());
    //if(wifi.printSonar) wifi.transmit(control.position.toString().c_str());
    //if(pc.pcUpdated) { wifi.transmit(pc.pcData.c_str()); pc.pcUpdated = false; }
   // if(hc05.newMsg) { wifi.transmit(hc05.msg, false); hc05.msg.clear(); }

    return;
}

float Robot::drivePowerMonitor(void) {
    float R1 = 99.4, R2 = 9.6, input=0;
    AnalogIn vIn(PF_3);

    input = vIn.read() * 3.3;
    float vOriginal = input / (R2/(R1+R2));

    return vOriginal;
}


#endif