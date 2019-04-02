#include "Ping.h"



// This is a section of test code for pinging all ultrasonic sensor simulataneously. It was becoming very messy in the main Ping.h library,
// so was moved here until further development. 

#ifndef ENABLE_SONAR_INTERRUPT
void Ping::set_input() {
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL)
            instance[i]->pin.input();
    }
}
void Ping::set_output() {
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL)
            instance[i]->pin.output();
    }
}
#endif
void Ping::set_state(short state) {
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL) {
            #ifndef ENABLE_SONAR_INTERRUPT
            instance[i]->pin = state;
            #else
            instance[i]->trigger = state;
            #endif
        }
    }
}
bool Ping::ping_trigger_all() {      
    Timer triggerTimer;
    #ifndef ENABLE_SONAR_INTERRUPT
    if(instance[0] == NULL) return false;
    
    bool isReady[SONAR_NUM];
    for(int i=0; i<SONAR_NUM; ++i) {
        instance[i]->triggerSuccess = false;
        isReady[i] = true;
    }

    set_output();
    wait_us(5);
    set_state(0);
    wait_us(4);
    set_state(1);
    wait_us(10);
    set_state(0);
    set_input();
    
    wait_us(5);
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL){
            if(instance[i]->pin)
                isReady[i] = false;
        }
    }

    triggerTimer.start();
    long max_time = instance[0]->_maxEchoTime + MAX_SENSOR_DELAY;
    unsigned short numSuccesses = 0;


    while(triggerTimer.read_us() < max_time) {
        for(int i=0; i<SONAR_NUM; ++ i) {
            if(instance[i]->pin && !instance[i]->triggerSuccess && isReady) {
                 instance[i]->triggerSuccess = true;
                 ++numSuccesses;
            }
        }
        if(numSuccesses == SONAR_NUM) break;
    }

    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL){
            instance[i]->_max_time = instance[i]->_maxEchoTime;
            instance[i]->pingTimer.reset();
            instance[i]->pingTimer.start();
        }
    }

    return true;

    #else
    bool isReady[SONAR_NUM];
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL) {
            if(instance[i]->echo) instance[i]->triggerSuccess = false;
            isReady[i] = true;
        }
    }
    __disable_irq();
    set_state(0);
    wait_us(4);
    set_state(1);
    wait_us(10);
    set_state(0);

    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] != NULL) {
            if(instance[i]->echo) isReady[i] = false;
        }
    }
    for(int i=0; i<SONAR_NUM; ++i) {
        if(isReady && instance[i] != NULL) {
            instance[i]->pingTimer.reset();
            instance[i]->pingTimer.start();
            instance[i]->_max_time = instance[i]->_maxEchoTime;
            instance[i]->triggerSuccess = true;
        }
    }
    triggerTimer.reset();
    triggerTimer.start();

    __enable_irq();
    return true;

    #endif
}
int Ping::ping_all(unsigned int max_cm_distance)
{
    for(int i=0; i<SONAR_NUM; ++i) {
        if(max_cm_distance > 0)
            instance[i]->set_max_distance(max_cm_distance); // Call function to set a new max sensor distance for each device
    }

    ping_trigger_all();                                     // Trigger a ping, if any sensor's triggerSuccess returns false, return the negative instance number of the sensor. 
    for(int i=0; i<SONAR_NUM; ++i) {
        if(!instance[i]->triggerSuccess)
            return i*-1;
        instance[i]->triggerSuccess = false;
    }

    #ifdef ENABLE_SONAR_INTERRUPT                           // If interrupts are enabled, all that needs to be done is trigger the ping
    return 1;
    
    #else                                                   // Otherwise, we have to sit around and wait for a response
    for(int i=0; i<SONAR_NUM; ++i) {
        instance[i]->pin.input();
    }

    while(triggerTimer.read_us() < instance[0]->_max_time) {
        for(int i=0; i<SONAR_NUM; ++i) {
            if(!instance[i]->pin) {
                instance[i]->pingTimer.stop();
            }
        }
    }
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i]->pingTimer.read_us() <= instance[i]->_max_time) {
            instance[i]->tempTime = instance[i]->pingTimer.read_us() - PING_OVERHEAD;   // Calculate ping time, including overhead
            instance[i]->pingTimer.reset();
        } else {
            instance[i]->pingTimer.stop();
            instance[i]->pingTimer.reset();
        }
    }

    return 1;
    #endif

}
int*  Ping::ping_cm_all(unsigned int max_cm_distance) {
    unsigned int echoTime[SONAR_NUM];
    int distance_cm[SONAR_NUM];
    bool pingSuccess[SONAR_NUM];
    for(int i=0; i<SONAR_NUM; ++i) pingSuccess[i] = true;

    #ifndef ENABLE_SONAR_INTERRUPT
    ping_all(max_cm_distance);
    for(int i=0; i<SONAR_NUM; ++i) {
        echoTime = instance[i]->tempTime;
    }                                         // Calls the ping method and returns with the ping echo distance in uS.
    #else

    __disable_irq();
    for(int i=0; i<SONAR_NUM; ++i){
        instance[i]->pingUpdated = instance[i]->pingUpdatedShared;
        if(instance[i]->pingUpdated) {
            echoTime[i] = instance[i]->tempTime - PING_OVERHEAD;
            instance[i]->pingUpdated = false;
            instance[i]->pingUpdatedShared = false;
        }
    }
    __enable_irq();

    ping_all(max_cm_distance); 
    for(int i=0; i<SONAR_NUM; ++i) {
        if(echoTime[i] > instance[i]->_maxEchoTime) pingSuccess[i] = false;
    }
    #endif
#if ROUNDING_ENABLED == false
    for(int i=0; i<SONAR_NUM; ++i) {
        if(pingSuccess) distance_cm[i] = echoTime[i] / US_ROUNDTRIP_CM;
    }
    return distance_cm;                   // Call the ping method and returns the distance in centimeters (no rounding).
#else
    for(int i=0; i<SONAR_NUM; ++i) {
        if(pingSuccess) distance_cm[i] = instance[i]->NewPingConvert(echoTime[i], US_ROUNDTRIP_CM); // Convert uS to cm.
    }
    return distance_cm;
#endif
}