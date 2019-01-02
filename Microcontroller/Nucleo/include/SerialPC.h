#ifndef PC_SERIAL_H
#define PC_SERIAL_H

#include <mbed.h>
#include <string>
#include <WiFly.h>

#ifndef ADJUST_HC05
Serial pc(USBTX, USBRX, 38400);
#endif

string pcData;


char inputPC() {
    char pcIn;

    pcIn = pc.getc();
    #ifdef ADJUST_HC05
    hc05.putc(pcIn);
    pc.putc(pcIn);
    #endif

    return pcIn;
}
int processPC() {
    if(!pc.readable()) return -1;

    while(pc.readable()) {
        char input = inputPC();
        pcData += input;

        switch(input) {
            case '\r':
                hc05.printf("%s", pcData.c_str());
                pcData.clear();
                return 1;
            case '~':
                if(wifiConnectPing() < 0) hc05.printf("Error Pinging: Not Connected To Host\n");
                else hc05.printf("\nPing Sent\n"); 
                pcData.clear();
                return 1;
            case '@':
                connect = !connect;
                hc05.printf("\n%d\n", connect);
                pcData.clear();
                return 1;
        }
    }
    return 0;
}

#endif