#ifndef HC05_H
#define HC05_H

#include <string>

#include "Drive.h"


bool direction;
double speed = 0, theta = 0, phi = 0;
bool controlMotors = false;


class Bluetooth {

    public:
        Bluetooth(PinName, PinName, int);
        void transmit(string&);
        void transmit(char);
        void transmit(const char*);

        void processBT(void);

        bool newMsg = false;
        string msg;

    private:
        void inputBT(void);
        static void bt_ISR(void);
        static Bluetooth * instance;
        bool motorControl = true;

        Serial hc05;
        char btIn;
};

Bluetooth::Bluetooth(PinName tx = PE_8, PinName rx = PE_7, int baud = 38400) : hc05(tx, rx, baud) {
    instance = this;
    hc05.attach(&bt_ISR);
    msg.resize(128);
    btIn = ' ';
}

void Bluetooth::transmit(string &out) {
    hc05.printf("%s", out);
}
void Bluetooth::transmit(const char* out) {
    hc05.printf("%s", out);
}
void Bluetooth::transmit(char out) {
    hc05.printf("%c", out);
}
void Bluetooth::inputBT(void) {
    btIn = hc05.getc();
}
void Bluetooth::processBT(void) {
    if(!hc05.readable()) {
        return;
    }
    inputBT();
    switch (btIn) {
        if(controlMotors) {
            case 'w':
                speed += .1;

                speed = min(speed, 1.0);
                speed = max(speed, 0.0);
                
                drive.drive(speed, theta, phi);
                hc05.printf("%f %f %f\n", speed, theta, phi);
                break;
            case 's':
                speed -= .1;

                speed = min(speed, 1.0);
                speed = max(speed, 0.0);

                drive.drive(speed, theta, phi);
                hc05.printf("%f %f %f\n", speed, theta, phi);
                break;
            case 'd':
                theta += 15;

                theta = min(theta, 180.0);
                theta = max(theta, -180.0);

                drive.drive(speed, theta, phi);
                hc05.printf("%f %f %f\n", speed, theta, phi);

                break;
            case 'a':
                theta -= 15;

                theta = min(theta, 180.0);
                theta = max(theta, -180.0);

                drive.drive(speed, theta, phi);
                hc05.printf("%f %f %f\n", speed, theta, phi);
                break;
            case 'q':
                phi -= .1;

                phi = min(phi, 1.0);
                phi = max(phi, -1.0);

                drive.drive(speed, theta, phi);
                hc05.printf("%f %f %f\n", speed, theta, phi);
                break;
            case 'e':
                phi += .1;

                phi = min(phi, 1.0);
                phi = max(phi, -1.0); 

                drive.drive(speed, theta, phi);
                hc05.printf("%f %f %f\n", speed, theta, phi);
        }
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