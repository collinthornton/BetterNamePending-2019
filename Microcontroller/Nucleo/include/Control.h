#ifndef CONTROL_H
#define CONTROL_H

#include <mbed.h>
#include <string>

#include "Position.h"
#include "PID.h"
#include "Drive.h"

class Control {
    public:
        Control();
        
        void autonomous(void); //! These will need input variables -- set to void initially for the sake of brevity
        void assisted(void);
        void manual(void);

        Drive drive   = Drive(motorFR, motorBR, motorBL, motorFL);

    private:
        PID speedControl    = PID(1.0, 0, 0);
        PID thetaControl    = PID(0.1, 0, 0);
        PID phiControl      = PID(1.0, 0, 0);

        Position position = Position();

        Motor motorFL = Motor(D3, D12);
        Motor motorFR = Motor(D10, D8);
        Motor motorBR = Motor(D11, D4);
        Motor motorBL = Motor(D13, D2);

};

Control::Control() {}

void Control::autonomous(void) {
    float speed = 0, theta = 0, phi = 0;
    string direction, axis;

    position.findPosition();

    position.location.driveAxis == 0 ? axis = "X" : axis = "Y";
    position.location.driveDir  == 0 ? direction = "FORWARD" : direction = "REVERSE";
    
    float x = position.location.X;
    float y = position.location.Y;
    float heading = position.location.heading;
    float desiredTheta;
    
    if(axis == "Y") {
        speed = speedControl.compute(y, position.HALL_WIDTH-position.ROBOT_WIDTH/2);

        if(direction == "FORWARD") desiredTheta = 0;
        else desiredTheta = 180;

        if(x >= 0) theta = thetaControl.compute(atan(x/speed), desiredTheta);
        else       theta = thetaControl.compute(atan(x/speed), desiredTheta); 
        
        
    }
    else if(axis == "X") {
        speed = speedControl.compute(x, position.HALL_WIDTH-position.ROBOT_WIDTH/2);
        if(direction == "FORWARD") desiredTheta = 90;
        else desiredTheta = -90;
    }
    else {
        speed = 0;
        desiredTheta = 0;
    }

    if(direction == "FORWARD") {

    }
}
void Control::assisted(void) {}
void Control::manual(void) {}

Control control = Control();
#endif