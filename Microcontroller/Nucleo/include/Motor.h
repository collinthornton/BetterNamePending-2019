#ifndef MOTOR_H
#define MOTOR_H

#include <mbed.h>

//----------------------------------------------------------------------------------//
//  
//  This is a generic motor driver class, currently intended for                    
//  brushed DC motor controllers.
//  
//  Constructors:
//      Motor(enablePing, directionPin1, directionPin2)
//
//  Public Functions:
//      void drive(power, direction)
//      -args
//          power = percentage power (float between 0 and 1)
//          direction = direction of rotation (0 = reverse, 1 = forward)
//
//----------------------------------------------------------------------------------//

class Motor {
    public:
        Motor(PinName, PinName);
        
        void drive(float, int);

    private:
        PwmOut motor;
        DigitalOut direction;

        float maxMag = 0.4;

};

Motor::Motor(PinName enable, PinName dir) : motor(enable), direction(dir) {
    motor.period(.00005f);                                     // PWM frequency @ 20Khz -- .00005f
}
void Motor::drive(float mag, int dir) {
    mag < 0 ? mag = 0 : mag = mag;
    mag > 1 ? mag = 1 : mag = mag;

    if(mag > maxMag) mag = maxMag;

    direction = dir;
    motor = mag;
}

#endif