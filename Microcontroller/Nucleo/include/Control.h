#ifndef CONTROL_H
#define CONTROL_H

//#include <mbed.h>
//#include <string>

#include "Position.h"
#include "PID.h"
#include "Drive.h"

class Control {
    public:
        Control();
        
        void autonomous(void); //! These will need input variables -- set to void initially for the sake of brevity
        void assisted(void);
        void manual(float rho, float theta, float phi);

        Position position = Position();

    private:
        PID speedControl    = PID(1.0, 0, 0, 0, 1);
        PID thetaControl    = PID(0.1, 0, 0, -180, 180);
        PID phiControl      = PID(1.0, 0, 0, -1, 1);
        
        Drive drive   = Drive(motorFR, motorBR, motorBL, motorFL);

        Motor motorFL = Motor(D3, D12);
        Motor motorFR = Motor(PB_11, D8);
        Motor motorBR = Motor(D11, D4);
        Motor motorBL = Motor(D13, D2);

};

Control::Control() {}

void Control::autonomous(void) {
    float speed = 0, theta = 0, phi = 0;
    string direction, axis;

    position.location.driveAxis == 0 ? axis = "X" : axis = "Y";
    position.location.driveDir  == 0 ? direction = "FORWARD" : direction = "REVERSE";
    
    float x = position.location.X;
    float y = position.location.Y;
    float heading = position.location.heading;
    
    if(axis == "Y") {
        speed = speedControl.compute(y, position.HALL_WIDTH-position.ROBOT_WIDTH/2);

        if(direction == "FORWARD") theta = 0;
        else theta = 180;

        theta += thetaControl.compute(theta + x, theta);     
    }
    else if(axis == "X") {
        speed = speedControl.compute(x, position.HALL_WIDTH-position.ROBOT_WIDTH/2);
        if(direction == "FORWARD") theta = 90;
        else theta = -90;

        theta += thetaControl.compute(theta + y, theta);
    }
    else {
        speed = 0;
        theta = 0;
    }

    phi += phiControl.compute(heading, 0);

    drive.drive(speed, theta, phi);
}

void Control::assisted(void) {}
void Control::manual(float rho, float theta, float phi) {
    string direction, axis;

    drive.drive(rho, theta, phi);
}

#endif