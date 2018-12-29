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

const float delay = .35;
const char SSID[] = "";
const char PHRASE[] = "";
const char COM_REMOTE[] = "0";
const char HOST_IP[] = "206.189.66.241";
const char HOST_PORT[] = "80";                        // Port 5555 for distribution server
const char GET_PING[] = "GET /mercury2018/ping.php";  //! Apparently sends terminator char after string -- HTML doesn't understand
const char FILE_NAME[] = "BetterNamePending";

InterruptIn joined(D6), connected(D7);
DigitalOut connect(D5);
Timeout connectionTimeout;

Serial wifi(D1, D0, 9600);
Serial hc05(PE_8, PE_7, 9600);

string wifiData;
char wifiIn;

volatile bool networkIsTimedOutShared;
volatile bool hostIsTimedOutShared;

bool networkIsTimedOut;
bool hostIsTimedOut;

void wifiConfig(bool loadFile = true) {
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

    wifi.printf("save %s\r", FILE_NAME);
    wait(delay);
    wifi.printf("reboot\r");

    return;
}
int wifiDisconnectHost() {
    connect = 0;
    if(!connected) return 1;
    else return -1;
}
int wifiInit(short numTries = 0) {

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
int wifiConnectPing() {
    if(!connected) return -1;
    wifi.printf("GET /mercury2018/ping.php?test=success\r\n");

    return 0;
}
void processHC05() {

    if(!hc05.readable()) return;
    char btIn;

    btIn = hc05.getc();
    wifi.putc(btIn);
}
void inputWifi() {
    wifiIn = wifi.getc();
    hc05.putc(wifiIn);
}
int processWifi() {
    if(!wifi.readable()) {
        return -1;
    } 

    inputWifi();
    //wifiData += wifiIn;

    return 0;
}


void wifi_ISR(void) {
    processWifi();
}


void handleLossOfSignal() {
    if(!joined) {
        hc05.printf("------| Network Timed Out |------\n");
        while(!joined) { 
            wifiConfig();
            wait(5);
        }
        hc05.printf("------| Timeout Released |------\n");
    }
    if(!connected) {
        hc05.printf("------| Connection Timed Out |------\n");
        while(!connected) { wifiInit(); }
        hc05.printf("------| Timeout Released |------\n");
    }

    return;
}

#endif


