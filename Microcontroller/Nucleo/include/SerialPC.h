#ifndef PC_SERIAL_H
#define PC_SERIAL_H

#include <string>

class PC {
    public:
        PC(const PinName &tx, const PinName &rx, int baud);

        char inputPC(void);
        int  processPC(void);
        void transmit(string input, bool cr);        
        void transmit(const char* input, bool cr);
        void transmit(char input, bool cr);

        string pcData;
        bool pcUpdated = false, printSonar = false, printWifi = false, printPosition = false, wifiConnectEnabled = false, motorUpdated = false;
        double speed = 0, theta = 0, phi = 0;
        float speedConstant = 0.075;
    
        Serial pc;
};

PC::PC(const PinName &tx = USBTX, const PinName &rx = USBRX, int baud = 38400) : pc(tx, rx, baud) { pcData.resize(128); }



char PC::inputPC() {
    char pcIn = pc.getc();
    return pcIn;
}
int PC::processPC() {
    if(!pc.readable()) return -1;

    while(pc.readable()) {
        char input = inputPC();
        pcData += input;
        pcUpdated = true;

        switch(input) {
            case '\r':
                hc05.transmit(pcData.c_str());
                pcData.clear();
                break;

            case '@':
                wifiConnectEnabled = !wifiConnectEnabled;
                hc05.transmit(pcData);
                pcData.clear();
                break;

            case 'w':
                speed += speedConstant;
                speed = min(speed, 1.0);
                speed = max(speed, 0.0);
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 's':
                speed -= speedConstant;
                speed = min(speed, 1.0);
                speed = max(speed, 0.0);
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'd':
                theta += 15;
                theta = min(theta, 180.0);
                theta = max(theta, -180.0);
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'a':
                theta -= 15;
                theta = min(theta, 180.0);
                theta = max(theta, -180.0);
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'q':
                phi -= .1;
                phi = min(phi, 1.0);
                phi = max(phi, -1.0);
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'e':
                phi += .1;
                phi = min(phi, 1.0);
                phi = max(phi, -1.0); 
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'f':
                theta = 0;
                phi = 0;
                pc.printf("%F %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'b':
                theta = 180;
                phi = 0;
                pc.printf("%F %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'z':
                theta = 0;
                phi = 0;
                speed = 0;
                pc.printf("%f %f %f\n", speed, theta, phi);
                motorUpdated = true;
                break;

            case 'p':
                printSonar = !printSonar;
                break;
        }
    }
    return 0;
}

void PC::transmit(string input, bool cr = true) {
    pc.printf("%s", input.c_str());
    if(cr)
        pc.printf("\r\n");
}
void PC::transmit(const char* input, bool cr = true) {
    pc.printf("%s", input);
    if(cr)
        pc.printf("\r\n");
}
void PC::transmit(char input, bool cr = true) {
    pc.printf("%c", input);
    if(cr)
        pc.printf("\r\n");
}

//----------------------------------------------------//

//PC pc = PC();

//----------------------------------------------------//

#endif