
// ---------------------------------------------------------------------------
//
//
//
// ! -------- EDITS FOR MERCURY ROBOTICS - Collin Thornton --------
//
//  - Modications to allow use of STM32 Nucleo F746ZG
//  - Simplified code by getting rid of unnecessary definitions for other boards
//  - Combined necessary functions from NewPing.h and NewPing.cpp into Ping.h
//
// ! ---------------------------------------------

#ifndef PING_H
#define PING_H

#define SONAR_NUM 1  
#define MAX_DISTANCE 25                                   // Adjust to indicate total number of ultrasonic sensors. 
#define ENABLE_SONAR_INTERRUPT                          // Comment this line to enable single pin sonar, uncomment to increase speed by about 200x

#include <mbed.h>
#include <SerialPC.h>

// Shouldn't need to change these values unless you have a specific need to do so.
#define MAX_SENSOR_DISTANCE 500 // Maximum sensor distance can be as high as 500cm, no reason to wait for ping longer than sound takes to travel this distance and back. Default=500
#define US_ROUNDTRIP_CM 57      // Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space. Default=57
#define ROUNDING_ENABLED false  // Set to "true" to enable distance rounding which also adds 64 bytes to binary size. Default=false

// Probably shouldn't change these values unless you really know what you're doing.
#define NO_ECHO 0               // Value returned if there's no ping echo within the specified MAX_SENSOR_DISTANCE or max_cm_distance. Default=0
#define MAX_SENSOR_DELAY 5800   // Maximum uS it takes for sensor to start the ping. Default=5800
#ifndef ENABLE_SONAR_INTERRUPT
#define PING_OVERHEAD 5         // Ping overhead in microseconds (uS). Default=5
#else
#define PING_OVERHEAD 150       //! Remember to calibrate PING_OVERHEAD for device
#endif
#define PING_TIMER_OVERHEAD 13  // Ping timer overhead in microseconds (uS). Default=13
                                // Conversion from uS to distance (round result to nearest cm or inch).
#define PingConvert(echoTime, conversionFactor) (max(((unsigned int)echoTime + conversionFactor / 2) / conversionFactor, (echoTime ? 1 : 0)))

class Ping
{
  public:
    #ifndef ENABLE_SONAR_INTERRUPT
    Ping(PinName singlePin, unsigned int max_cm_distance = MAX_SENSOR_DISTANCE);
    DigitalInOut pin;
    #else
    Ping(PinName echoPin, PinName triggerPin, unsigned int max_cm_dinstance = MAX_SENSOR_DISTANCE);
    DigitalOut trigger;
    InterruptIn echo;
    #endif

    static Ping* instance[SONAR_NUM];

    unsigned int ping(unsigned int max_cm_distance = 0);
    unsigned int ping_cm(unsigned int max_cm_distance = 0);
    static int* ping_cm_all(unsigned int max_cm_distance = 0);
    static int ping_all(unsigned int max_cm_distance = 0);

    Timer pingTimer;
    static Timer triggerTimer;
    unsigned int _maxEchoTime;
    long _max_time;

  private:
    void set_max_distance(unsigned int max_cm_distance);
    bool ping_trigger();
    static bool ping_trigger_all();

    static void set_state(short state);

    bool triggerSuccess;

    #ifdef ENABLE_SONAR_INTERRUPT
    void ISR(void);

    static void static_ISR0(void);
    static void static_ISR1(void);
    static void static_ISR2(void);
    static void static_ISR3(void);
    static void static_ISR4(void);
    static void static_ISR5(void);
    static void static_ISR6(void);
    static void static_ISR7(void);
    static void static_ISR8(void);

    volatile unsigned int tempTime;
    volatile bool pingUpdatedShared = false;
    bool pingUpdated = false;
    #else
    static void set_input();
    static void set_output();

    unsigned int tempTime;
    #endif
};

#ifndef ENABLE_SONAR_INTERRUPT
Ping::Ping(PinName singlePin, unsigned int max_cm_distance) : pin(singlePin)
{
    set_max_distance(max_cm_distance);
    for(int i=0; i<SONAR_NUM; ++i) {
        if(instance[i] == NULL)
            instance[i] = this;
    }
}
#else
Ping::Ping(PinName triggerPin, PinName echoPin, unsigned int max_cm_distance) : echo(echoPin), trigger(triggerPin) {
    switch(echoPin) {
        case D9:
            echo.fall(&static_ISR0);
            instance[0] = this;
            break;
        default:
            while(1) {            
                pc.printf("Please enter a correct pin name for the sonar echo interrupt\n");
            }     //! Function will enter infinite loop if wrong pin name is given for echoPin
            return;
    }

    set_max_distance(max_cm_distance);
}
#endif

void Ping::set_max_distance(unsigned int max_cm_distance)
{
#if ROUNDING_ENABLED == false
    _maxEchoTime = min(max_cm_distance + 1, (unsigned int)MAX_SENSOR_DISTANCE + 1) * US_ROUNDTRIP_CM;                   // Calculate the maximum distance in uS (no rounding).
#else
    _maxEchoTime = min(max_cm_distance, (unsigned int)MAX_SENSOR_DISTANCE) * US_ROUNDTRIP_CM + (US_ROUNDTRIP_CM / 2);   // Calculate the maximum distance in uS.
#endif
}

unsigned int Ping::ping(unsigned int max_cm_distance)
{

    if (max_cm_distance > 0)
        set_max_distance(max_cm_distance);                  // Call function to set a new max sensor distance.

    if (!ping_trigger())
        return NO_ECHO;                                     // Trigger a ping, if it returns false, return NO_ECHO to the calling function.

    #ifdef ENABLE_SONAR_INTERRUPT                           // If interrupts are enabled, all that needs to be done is trigger the ping
    return 1;
    
    #else                                                   // Otherwise, we have to sit around and wait for a response
    pin.input();
    while (pin)
    {
        if (pingTimer.read_us() > _max_time)
        {
            pingTimer.stop();
            pingTimer.reset();
            return NO_ECHO;                                 // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance
        }
    }

    tempTime = pingTimer.read_us();
    pingTimer.stop();
    pingTimer.reset();
    return (tempTime - PING_OVERHEAD);                      // Calculate ping time, include overhead.
    #endif

}

unsigned int Ping::ping_cm(unsigned int max_cm_distance)
{
    unsigned int echoTime;
    #ifndef ENABLE_SONAR_INTERRUPT
    echoTime = ping(max_cm_distance);                       // Calls the ping method and returns with the ping echo distance in uS.
    #else
    __disable_irq();
    pingUpdated = pingUpdatedShared;
    if(pingUpdated) {
        echoTime = tempTime - PING_OVERHEAD;
        pingUpdated = false;
        pingUpdatedShared = false;
    }
    __enable_irq();
    ping(max_cm_distance); 
    if(echoTime > _maxEchoTime) return NO_ECHO;
    #endif
#if ROUNDING_ENABLED == false
    return echoTime / US_ROUNDTRIP_CM;                      // Call the ping method and returns the distance in centimeters (no rounding).
#else
    return NewPingConvert(echoTime, US_ROUNDTRIP_CM);       // Convert uS to centimeters.
#endif
}

bool Ping::ping_trigger()
{
    #ifndef ENABLE_SONAR_INTERRUPT
    pin.output();
    wait_us(5);
    pin = 0;
    wait_us(4);
    pin = 1;
    wait_us(10);
    pin = 0;

    pin.input();
    wait_us(5);
    if (pin)
        return false;
    pingTimer.reset();
    pingTimer.start();
    _max_time = _maxEchoTime + MAX_SENSOR_DELAY;
    while (!pin)
        if (pingTimer.read_us() > _max_time)
            return false;

    pingTimer.reset();
    _max_time = _maxEchoTime;
    return true;

    #else

    __disable_irq();
    trigger = 0;
    wait_us(4);
    trigger = 1;
    wait_us(10);
    trigger = 0;

    if(echo)
        return false;
    
    pingTimer.reset();
    pingTimer.start();
    _max_time = _maxEchoTime;
    __enable_irq();
    return true;
    #endif
}
#ifdef ENABLE_SONAR_INTERRUPT
void Ping::static_ISR0(void) {
    if(Ping::instance[0] != NULL)
        Ping::instance[0]->ISR();
}
/*
void Ping::static_ISR1(void) {
    if(Ping::instance[1] != NULL)
        Ping::instance[1]->ISR();
}
void Ping::static_ISR2(void) {
    if(instance[2] != NULL)
        instance[2]->ISR();
}
void Ping::static_ISR3(void) {
    if(instance[3] != NULL)
        instance[3]->ISR();
}
void Ping::static_ISR4(void) {
    if(instance[4] != NULL)
        instance[4]->ISR();
}
void Ping::static_ISR5(void) {
    if(instance[5] != NULL)
        instance[5]->ISR();
}
void Ping::static_ISR6(void) {
    if(instance[6] != NULL)
        instance[6]->ISR();
}
void Ping::static_ISR7(void) {
    if(instance[7] != NULL)
        instance[7]->ISR();
}
void Ping::static_ISR8(void) {
    if(instance[8] != NULL)
        instance[8]->ISR();
}
*/
void Ping::ISR(void) {
    pingTimer.stop();
    pingUpdatedShared = true;
    tempTime = pingTimer.read_us() - PING_OVERHEAD;
    pingTimer.reset();
}
#endif

//-----------------------------------------------------------------------//

Ping sonar[SONAR_NUM] = {
  #ifdef ENABLE_SONAR_INTERRUPT 
  Ping(D8, D9, MAX_DISTANCE)                // Declare ultrasonic sensors here for interrupt use
  #else
  Ping(D8, MAX_DISTANCE)                    // Declare ultrasonic sensors here for single pin use
  #endif
};

Ping * Ping::instance[SONAR_NUM];         // This fills declares an array the same size as the number of Sonar objects
                                          // it is used for the interrupt functions
Timer sonarTimer;
int *ping;                                  // Variable to track distance in cm for each ultrasonic sensor

void sonar_timer() {                        // Ping all Sonar objects and reset the sonar timer
  for(unsigned int i=0; i<SONAR_NUM; ++i)
    ping[i] = sonar[i].ping_cm(MAX_DISTANCE);
  sonarTimer.reset();
}

/*void sonar_timer() {
  ping = sonar[0].ping_cm_all(MAX_DISTANCE);
}*/

//---------------------------------------------------------------------//

#endif
