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
const char SSID[] = "ASUS-2.4";
const char PHRASE[] = "She*8llie";
const char COM_REMOTE[] = "0";
const char HOST_IP[] = "206.189.66.241";
const char HOST_PORT[] = "80";
//const char GET_PING[] = "GET /mercury2018/ping.php";  //! Apparently sends terminator char after string -- HTML doesn't understand

InterruptIn joined(D6), connected(D7);
DigitalOut connect(D5);
Timeout connectionTimeout;
Serial wifi(D1, D0, 9600);
Serial pc(USBTX, USBRX, 9600);

char wifiData[128];
char wifiIn;
unsigned short dataIndex = 0;

void wifiConfig() {
    wait(delay);
    wifi.printf("$$$");
    wait(delay);
    wifi.printf("\r");
    wait(delay);

    wifi.printf("set com remote %s\r", COM_REMOTE);
    wait(delay);
    wifi.printf("set ip dhcp 1\r");
    wait(delay);
    wifi.printf("set ip host %s\r", HOST_IP);
    wifi.printf("set ip remote 80\r");
    wait(delay);
    wifi.printf("set wlan ssid %s\r", SSID);
    wait(1);
    wifi.printf("set wlan phrase %s\r", PHRASE);
    wait(1);
    wifi.printf("set wlan join 1\r");
    wait(delay);
    wifi.printf("set opt jointmr 10000\r");
    wait(delay);
    wifi.printf("set sys iofunc 0x70\r");
    wait(delay);

    wifi.printf("save\r");
    wait(delay);
    wifi.printf("reboot\r");
    wait(5);

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

        while(init.read() < 5)
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
void wifiConnectPing() {

    if(!connected) wifiInit();

    wifi.printf("GET /mercury2018/ping.php\r\n");

    return;
}


void wifi_ISR(void) {
    if(wifi.readable())
        wifiIn = wifi.getc();
    pc.putc(wifiIn);

   // wifiData[dataIndex++] = wifiIn;
   // if(wifiIn == ' ') {
   //     dataIndex = 0;
   //     pc.printf("%s", wifiData);
    //}
}
void connectionTimeOut(void) {
    pc.printf("\n\n------| Connection Timeout Triggered |------\n\n");
    while(!connected) {
        //wifiInit();
        connect = 1;
    }
    pc.printf("\n\n------| Connection Timeout Released |------\n\n");
}
void hostLost(void) {
    connectionTimeout.attach(connectionTimeOut, 1.5);
}
void hostGained(void) {
    connectionTimeout.detach();
}

#endif


