// -------------------------------------------------------------- //
//
// Developed by Collin Thornton
//
// This is a library for the WiFLY module
// Developed for the Spring 2019 Mercury Robotics Competition
//
// -------------------------------------------------------------- //


#ifndef WIFLY_H
#define WIFLY_H

#include <mbed.h>

class Wifi {

    public:
        Wifi(PinName, PinName, PinName, int);
        void wifiConfig(bool);
        int wifiInit(short);
        int wifiDisconnectHost(void);
        int wifiConnectPing(void);
        void processHC05(void);

        void transmit(char* input);
        void transmitBT(char* input);

    private:
        void inputWifi(void);
        int processWifi(void);
        void handleLossOfSignal(void);
        int processWifi(void);

        static void wifi_ISR(void);
        static Wifi* instance;

        const float delay = .35;
        const char* SSID;
        const char* PHRASE;
        const char* COM_REMOTE;
        const char* HOST_IP;
        const char* HOST_PORT;
        const char* GET_PING; 
        const char* FILE_NAME;
        const char* BAUD;
        const char* COMM_MATCH;
        const char* WIFI_RATE;

        InterruptIn joined, connected;      // D6, D7
        DigitalOut connect;                 // D5
        Timer losTimer;

        Serial wifi;
        Serial hc05;

        string wifiData;
        char wifiIn;

        bool firstDetection = true;
};

Serial wifi(D1, D0, 38400);

Wifi::Wifi(PinName joinPin = D6, PinName connectedPin = D7, PinName connectPin = D5, int  baud = 38400) : 
        joined(joinPin), connected(connectedPin), connect(connectPin), wifi(D1, D0, baud), hc05(PE_8, PE_7, baud) {

    instance = this;
    wifi.attach(&wifi_ISR);

    SSID = "Warring$Turtles$2.0";
    PHRASE = "12345678";
    COM_REMOTE = "0";
    HOST_IP = "206.189.66.241";
    HOST_PORT = "5555";                        // Port 5555 for distribution server
    GET_PING = "GET /mercury2018/ping.php";    //! Apparently sends terminator char after string -- HTML doesn't understand
    FILE_NAME = "BetterNamePending";
    BAUD = "38400";
    COMM_MATCH =  "13";                        // ASCII code for \r
    WIFI_RATE = "14";                          // 12 = default

    wifiData.resize(128);
}

void Wifi::wifiConfig(bool loadFile = true) {
    wait(delay);
    wifi.printf("$$$");
    wait(delay);
    wifi.printf("\r");
    wait(delay);

    if(loadFile) {
        wifi.printf("load %s\r", FILE_NAME);
        wait(1);
        wifi.printf("exit\r");
        return;
    }

    wifi.printf("set com remote %s\r", COM_REMOTE);
    wait(delay);
    wifi.printf("set ip dhcp 1\r");
    wait(delay);
    wifi.printf("set ip host %s\r", HOST_IP);
    wait(delay);
    wifi.printf("set ip remote %s\r", HOST_PORT);
    wait(delay);
    wifi.printf("set wlan ssid %s\r", SSID);
    wait(1);
    wifi.printf("set wlan phrase %s\r", PHRASE);
    wait(1);
    wifi.printf("set wlan join 1\r");
    wait(delay);
    wifi.printf("set wlan channel 0\r");
    wait(delay);
    wifi.printf("set opt jointmr 10000\r");
    wait(delay);
    wifi.printf("set sys iofunc 0x70\r");
    wait(delay);
    wifi.printf("set uart baud %s\r", BAUD);
    wait(delay);
    wifi.printf("set wlan rate %s\r", WIFI_RATE);
    wait(delay);
    wifi.printf("set comm match %s\r", COMM_MATCH);
    wait(delay);
    wifi.printf("open\r");

//  wifi.printf("save\r");
//  wait(delay);
//  wifi.printf("save %s\r", FILE_NAME);
//  wait(delay);
//  wifi.printf("reboot\r");

    return;
}
int Wifi::wifiDisconnectHost(void) {
    connect = 0;
    if(!connected) return 1;
    else return -1;
}
int Wifi::wifiInit(short numTries = 0) {

    Timer init;

    connect = 0;
    init.start();

    if(numTries >= 5) {
        wifiDisconnectHost();
        return -1;
    }

    while(init.read() < 5) 
        if(joined) break;

    init.stop();
    if(!joined) {

        wait(delay);
        wifi.printf("$$$");
        wait(delay);
        wifi.printf("\r");
        wait(delay);

        wifi.printf("set wlan ssid %s\r", SSID);
        wait(1);
        wifi.printf("set wlan phrase %s\r", PHRASE);
        wait(1);

        wifi.printf("exit\r");

        init.reset();
        init.start();
        while(init.read() < 5)
            if(joined) break;

        init.stop();
        if(!joined) wifiInit(++numTries);

    } else {
        init.reset();
        init.start();

        connect = 1;

        while(init.read() < 3)
            if(connected) break;
        
        init.stop();
        if(!connected) {

            connect = 0;

            wait(delay);
            wifi.printf("$$$");
            wait(delay);
            wifi.printf("\r");
            wait(delay);

            wifi.printf("set com remote %s\r", COM_REMOTE);
            wait(delay);
            wifi.printf("set ip host %s\r", HOST_IP);
            wait(delay);
            wifi.printf("set ip remote %s\r", HOST_PORT);
            wait(delay);

            wifi.printf("exit\r");
            wait(delay);

            connect = 1;

            init.reset();
            init.start();

            while(init.read() < 5)
                if(connected) break;

            if(!connected) wifiInit(++numTries);
        }
    }
    return 1;
}
int Wifi::wifiConnectPing(void) {
    if(!connected) return -1;
    wifi.printf("GET /mercury2018/ping.php?test=success\r\n");

    return 0;
}
void Wifi::transmit(char* input) {
    wifi.printf("%s", input);
}
void Wifi::transmitBT(char* input) {
    hc05.printf("%s", input);
}
void Wifi::processHC05(void) {

    if(!hc05.readable()) return;
    char btIn;

    btIn = hc05.getc();
    wifi.putc(btIn);
    #ifdef ADJUST_HC05
    pc.putc(btIn);
    #endif
}
void Wifi::inputWifi(void) {
    wifiIn = wifi.getc();
    hc05.putc(wifiIn);
}
int Wifi::processWifi(void) {
    if(!wifi.readable()) {
        return -1;
    } 

    inputWifi();
    //wifiData += wifiIn;

    return 0;
}


void Wifi::wifi_ISR(void) {
    if(Wifi::instance != NULL)
        instance->processWifi();
}


void Wifi::handleLossOfSignal(void) {
    if((!joined || !connected) && firstDetection) {
        losTimer.reset();
        losTimer.start();
        firstDetection = false;
    } else if((!joined || !connected) && !firstDetection) {
        if(losTimer.read() >= 1.5) {
            losTimer.stop();
            if(!joined) {
                hc05.printf("------| Network Timed Out |------\n");
                while(!joined) { 
                    wifiConfig();
                }
                hc05.printf("------| Timeout Released |------\n");
            }
            if(!connected) {
                hc05.printf("------| Connection Timed Out |------\n");
                while(!connected) { 
                    wifiInit();
                }
                hc05.printf("------| Timeout Released |------\n");
            }
            firstDetection = true;
        } else if(joined && connected) {
            losTimer.stop();
            losTimer.reset();
        }
    }

    return;
}

#endif


