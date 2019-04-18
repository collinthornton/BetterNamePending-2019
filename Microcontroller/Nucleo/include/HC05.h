#ifndef HC05_H
#define HC05_H

#include <string>

//#include "Control.h"

class Bluetooth {

    public:
        Bluetooth(const PinName &tx, const PinName &rx, int);
        void transmit(string&, bool);
        void transmit(char, bool);
        void transmit(const char*, bool);

        void processBT(void);

        bool newMsg = false, motorUpdated = false, printSonar = false;
        string msg;
            
        double speed = 0, theta = 0, phi = 0;

    private:
        void inputBT(void);
        static void bt_ISR(void);
        static Bluetooth * instance;
        bool motorControl = true;

        float speedConstant = .075;
        double velocity;

        Serial hc05;
        char btIn;
};

Bluetooth::Bluetooth(const PinName &tx = PE_8, const PinName &rx = PE_7, int baud = 38400) : hc05(tx, rx, baud) {
    instance = this;
    hc05.attach(&bt_ISR);
    msg.resize(128);
    btIn = ' ';
}

void Bluetooth::transmit(string &out, bool cr = true) {
    hc05.printf("%s", out);
    if(cr)
        hc05.printf("\r\n");
}
void Bluetooth::transmit(const char* out, bool cr = true) {
    hc05.printf("%s", out);
    if(cr)
        hc05.printf("\r\n");
}
void Bluetooth::transmit(char out, bool cr = true) {
    hc05.printf("%c", out);
    if(cr)
        hc05.printf("\r\n");
}
void Bluetooth::inputBT(void) {
    btIn = hc05.getc();
}
void Bluetooth::processBT(void) {
    if(!hc05.readable()) {
        return;
    }
    inputBT();
    msg += btIn;
    
    switch (btIn) {
        case 'p':
            printSonar = !printSonar;
            break;

        case 'w':
            speed += speedConstant;

            speed = min(speed, 1.0);
            speed = max(speed, 0.0);
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 's':
            speed -= speedConstant;
            speed = min(speed, 1.0);
            speed = max(speed, 0.0);
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 'd':
            theta += 15;
            theta = min(theta, 180.0);
            theta = max(theta, -180.0);
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 'a':
            theta -= 15;
            theta = min(theta, 180.0);
            theta = max(theta, -180.0);
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 'q':
            phi -= .1;
            phi = min(phi, 1.0);
            phi = max(phi, -1.0);
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 'e':
            phi += .1;
            phi = min(phi, 1.0);
            phi = max(phi, -1.0); 
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 'f':
            theta = 0;
            phi = 0;
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%F %f %f\n", speed, theta, phi);
            break;

        case 'b':
            theta = 180;
            phi = 0;
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);
            break;

        case 'z':
            theta = 0;
            phi = 0;
            speed = 0;
            newMsg = true;
            motorUpdated = true;
            hc05.printf("%f %f %f\n", speed, theta, phi);

    }
}
void Bluetooth::bt_ISR(void) {
    if(Bluetooth::instance != NULL) {
        instance->processBT();
    }
}



Bluetooth hc05 = Bluetooth();
Bluetooth * Bluetooth::instance;

#endif