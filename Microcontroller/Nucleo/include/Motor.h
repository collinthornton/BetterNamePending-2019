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

        float maxSpeed = 0.4;
    private:
        PwmOut motor;
        DigitalOut direction;

};

Motor::Motor(PinName enable, PinName dir) : motor(enable), direction(dir) {
    motor.period(.00005f);                                     // PWM frequency @ 20Khz -- .00005f
}
void Motor::drive(float mag, int dir) {
    mag = max(mag, (float)0.0);
    mag = min(mag, maxSpeed);

    direction = dir;
    motor = mag;
}

#endif