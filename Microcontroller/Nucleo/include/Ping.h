
// ---------------------------------------------------------------------------
//
//
//
// ! -------- EDITS FOR MERCURY ROBOTICS - Collin Thornton --------
//
//  - Modications to allow use of ARM STM32 Nucleo F746ZG
//  - Simplified code by getting rid of unnecessary definitions for other boards
//  - Combined necessary functions from NewPing.h and NewPing.cpp into Ping.h
//
// ! ---------------------------------------------

#ifndef PING_H
#define PING_H

#include <mbed.h>

// Shouldn't need to change these values unless you have a specific need to do so.
#define MAX_SENSOR_DISTANCE 500 // Maximum sensor distance can be as high as 500cm, no reason to wait for ping longer than sound takes to travel this distance and back. Default=500
#define US_ROUNDTRIP_CM 57      // Microseconds (uS) it takes sound to travel round-trip 1cm (2cm total), uses integer to save compiled code space. Default=57
#define US_ROUNDTRIP_IN 146     // Microseconds (uS) it takes sound to travel round-trip 1 inch (2 inches total), uses integer to save compiled code space. Defalult=146
#define ROUNDING_ENABLED false // Set to "true" to enable distance rounding which also adds 64 bytes to binary size. Default=false
#define TIMER_ENABLED true     // Set to "false" to disable the timer ISR (if getting "__vector_7" compile errors set this to false). Default=true

// Probably shouldn't change these values unless you really know what you're doing.
#define NO_ECHO 0               // Value returned if there's no ping echo within the specified MAX_SENSOR_DISTANCE or max_cm_distance. Default=0
#define MAX_SENSOR_DELAY 5800   // Maximum uS it takes for sensor to start the ping. Default=5800
#define ECHO_TIMER_FREQ 24      // Frequency to check for a ping echo (every 24uS is about 0.4cm accuracy). Default=24
#define PING_MEDIAN_DELAY 29000 // Microsecond delay between pings in the ping_median method. Default=29000
#define PING_OVERHEAD 5         // Ping overhead in microseconds (uS). Default=5
#define PING_TIMER_OVERHEAD 13  // Ping timer overhead in microseconds (uS). Default=13
// Conversion from uS to distance (round result to nearest cm or inch).
#define PingConvert(echoTime, conversionFactor) (max(((unsigned int)echoTime + conversionFactor / 2) / conversionFactor, (echoTime ? 1 : 0)))

class Ping
{
  public:
    Ping(PinName singlePin, unsigned int max_cm_distance = MAX_SENSOR_DISTANCE);

    unsigned int ping(unsigned int max_cm_distance = 0);
    unsigned int ping_cm(unsigned int max_cm_distance = 0);

  private:
    void set_max_distance(unsigned int max_cm_distance);

    DigitalInOut pin;
    Timer pingTimer;
    unsigned int _maxEchoTime;
    long _max_time;
    unsigned int tempTime;
    bool ping_trigger();
};

Ping::Ping(PinName singlePin, unsigned int max_cm_distance) : pin(singlePin)
{
    set_max_distance(max_cm_distance);
}

void Ping::set_max_distance(unsigned int max_cm_distance)
{
#if ROUNDING_ENABLED == false
    _maxEchoTime = min(max_cm_distance + 1, (unsigned int)MAX_SENSOR_DISTANCE + 1) * US_ROUNDTRIP_CM; // Calculate the maximum distance in uS (no rounding).
#else
    _maxEchoTime = min(max_cm_distance, (unsigned int)MAX_SENSOR_DISTANCE) * US_ROUNDTRIP_CM + (US_ROUNDTRIP_CM / 2); // Calculate the maximum distance in uS.
#endif
}

unsigned int Ping::ping(unsigned int max_cm_distance)
{

    if (max_cm_distance > 0)
        set_max_distance(max_cm_distance); // Call function to set a new max sensor distance.

    if (!ping_trigger())
        return NO_ECHO; // Trigger a ping, if it returns false, return NO_ECHO to the calling function.

    pin.input();
    while (pin)
    {
        if (pingTimer.read_us() > _max_time)
        {
            pingTimer.stop();
            pingTimer.reset();
            return NO_ECHO; // Stop the loop and return NO_ECHO (false) if we're beyond the set maximum distance
        }
    }

    tempTime = pingTimer.read_us();
    pingTimer.stop();
    pingTimer.reset();
    return (tempTime - PING_OVERHEAD); // Calculate ping time, include overhead.
}

unsigned int Ping::ping_cm(unsigned int max_cm_distance)
{
    unsigned int echoTime = Ping::ping(max_cm_distance); // Calls the ping method and returns with the ping echo distance in uS.
#if ROUNDING_ENABLED == false
    return echoTime / US_ROUNDTRIP_CM; // Call the ping method and returns the distance in centimeters (no rounding).
#else
    return NewPingConvert(echoTime, US_ROUNDTRIP_CM);                                                                 // Convert uS to centimeters.
#endif
}

bool Ping::ping_trigger()
{
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
}

#endif
