#ifndef PC_SERIAL_H
#define PC_SERIAL_H

#include <string>

#include "WiFly.h"
#include "HC05.h"
#include "Control.h"

Serial pc(USBTX, USBRX, 38400);

string pcData;

bool printSonar=true;
bool printWifi=false;

bool printPosition=true;
float speedConstant = .075;


char inputPC() {
    char pcIn = pc.getc();

    if(printWifi) wifi.transmit(pcIn, false);
    
    #ifdef ADJUST_HC05
    hc05.transmit(pcIn);
    pc.transmit(pcIn);
    #endif

    return pcIn;
}
int processPC() {
    if(!pc.readable()) return -1;

    while(pc.readable()) {
        char input = inputPC();
        char *out="0";
        pcData += input;

        switch(input) {
            case '\r':

                hc05.transmit(pcData.c_str());
                pcData.clear();
                return 1;
            case '~':
                if(wifi.wifiConnectPing() < 0) hc05.transmit("Error Pinging: Not Connected To Host\n");
                else hc05.transmit("\nPing Sent\n"); 
                pcData.clear();
                return 1;
            case '@':
                wifi.connectEnabled = !wifi.connectEnabled;
                __itoa(wifi.connectEnabled, out, 10);
                hc05.transmit("\nout\n");
                pcData.clear();
                return 1;
            if(controlMotors) {
                case 'w':
                    speed += speedConstant;

                    speed = min(speed, 1.0);
                    speed = max(speed, 0.0);
            
                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);
                    break;

                case 's':

                    speed -= speedConstant;
    
                    speed = min(speed, 1.0);
                    speed = max(speed, 0.0);

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);

                    break;
                case 'd':
                    theta += 15;

                    theta = min(theta, 180.0);
                    theta = max(theta, -180.0);

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);

                    return 1;
                case 'a':
                    theta -= 15;

                    theta = min(theta, 180.0);
                    theta = max(theta, -180.0);

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);
                    return 1;
                case 'q':
                    phi -= .1;

                    phi = min(phi, 1.0);
                    phi = max(phi, -1.0);

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);
                    return 1;
                case 'e':
                    phi += .1;

                    phi = min(phi, 1.0);
                    phi = max(phi, -1.0); 

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);
                    return 1;
                case 'f':
                    theta = 0;
                    phi = 0;

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%F %f %f\n", speed, theta, phi);
                    return 1;
                case 'b':
                    theta = 180;
                    phi = 0;

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%F %f %f\n", speed, theta, phi);
                    return 1;
                case 'z':
                    theta = 0;
                    phi = 0;
                    speed = 0;

                    control.drive.drive(speed, theta, phi);
                    pc.printf("%f %f %f\n", speed, theta, phi);
            }
            case 'p':
                printSonar = !printSonar;
                return 1;
        }
    }
    return 0;
}

#endif