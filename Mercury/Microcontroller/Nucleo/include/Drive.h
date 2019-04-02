#ifndef DRIVE_H
#define DRIVE_H

#include "Motor.h"

//-------------------------------------------------------------------------------------//
//
//  Created by Collin Thornton
//
//  All angles are in degrees from (0 - 360)]
//
//  Public functions:
//      drive(float rho, float theta, float phi)
//      linear(float rho, float theta)
//      rotate(float phi)
//
//  Doc:
//      drive: 
//          -Maps theta (direction on the x-y plane) to motor direction
//          -Uses rho as motor speed proportion
//          -Adjust rotation by formula: speed = speed +- (speed*phi)
//          -In order to keep moving linearly, phi = 1/-1 will
//              cause a swing turn, not a point turn.
//      linear:
//          -Maps theta (direction on the x-y plane) to motor direction 
//          -Uses rho as motor speed propotion
//
//      rotate:
//          -phi = 1 causes point turn CW
//          -phi = -1 cause point turn CCW
//          -rho scales rotation speed
//
//-------------------------------------------------------------------------------------//

class Drive
{
    public:
        Drive(Motor&, Motor&, Motor&, Motor&);

        void drive(float, float, float);
        void linear(float, float);
        void rotate(float, float);

    private:

        Motor *motor[4];
        bool direction[4];
        float speed[4];
};

Drive::Drive(Motor& motorFR, Motor& motorBR, Motor& motorBL, Motor& motorFL) {
    this->motor[0] = &motorFR;
    this->motor[1] = &motorBR;
    this->motor[2] = &motorBL;
    this->motor[3] = &motorFL;
}

//------------------------------------------------------------------------------------------------------------//

void Drive::drive(float rho, float theta = 0, float phi = 0) {                  // (rho, theta) = linear vector -- phi = rotation about z-axis
    theta = max(theta, (float)-180.0);
    theta = min(theta, (float)180.0);
    rho = max(rho, (float)0.0);
    rho = min(rho, (float)1.0);
    phi = max(phi, (float)-1.0);
    phi = min(phi, (float)1.0);
                                                                                    // Directional Quadrants
    if(theta >= -45 && theta <= 45) {                                               // All motors forward if theta is from -45deg - 45deg   
        for(int i=0; i<4; ++i) {
            direction[i] = 0;
        }
    }
    else if(theta > 45 && theta <= 135) {                                           // Motors 1,3 forward and 0,2 reverse from 45deg - 135deg
        for(int i=0; i<4; ++i) {                                                    // (Strafe rights)
            i % 2 == 0 ? direction[i]=1 : direction[i]=0;
        }
    }
    else if(theta >= -135 && theta < -45) {                                         // Motors 0,2 forward and 1,3 revers from -45deg - -135deg
        for(int i=0; i<4; ++i) {
            i % 2 == 0 ? direction[i]=0 : direction[i]=1;
        }
    }
    else if((theta >= -180 && theta < -135) || (theta <= 180 && theta > 135)) {     // All motors reverse from -135deg - -180 deg or 135deg - 180deg
        for(int i=0; i<4; ++i) {
            direction[i] = 1;
        }
    }
                                                                                // Speed control quadrants (note that speed is a scalar)
    if(theta >= 0 && theta <= 90) {                                                 // Motors 0,2 (1 to 0 to 1)
        for(int i=0; i<4; ++i) {                                                    // and 1,3 constant from 0deg to 90deg
            i % 2 == 0 ? speed[i]=rho*(abs(theta-45)/45) : speed[i]=rho;
        }
    }
    else if(theta > 90 && theta <= 180) {                                           // Motors 0,2 constant
        for(int i=0; i<4; ++i) {                                                    // and 1,3 (1 to 0 to 1) from 90deg to 180deg
            i % 2 == 0 ? speed[i]=rho : speed[i]=rho*(abs(theta-135)/45);
        }
    }
    else if(theta >= -90 && theta < 0) {                                            // Motors 0,2 constant
        for(int i=0; i<4; ++i) {                                                    // and 1,3 (1 to 0 to 1) from 0deg to -90deg
            i % 2 == 0 ? speed[i]=rho : speed[i]=rho*(abs(theta+45)/45);
        }
    }
    else if(theta >= -180 && theta < -90) {                                         // Motors 0,2 (1 to 0 to 1)
        for(int i=0; i<4; ++i) {                                                    // and 1,3 constant from-90deg to -180deg
            i % 2 == 0 ? speed[i]=rho*(abs(theta+135)/45) : speed[i]=rho;
        }
    }
                                                                            // Note that rotation control will be very sensitive
                                                                                  // Right motors decrease by speed*phi
    speed[0] -= 2*speed[0]*phi;                                                   // Left motors increased by speed*phi
    speed[1] -= 2*speed[1]*phi;
    speed[2] += 2*speed[2]*phi;
    speed[3] += 2*speed[3]*phi;

    for(int i=0; i<4; ++i) {
        if(speed[i] < 0) {
            speed[i] = -speed[i];
            direction[i] = -direction[i];
        }

        speed[i] = min(speed[i], (float)1.0);
        motor[i]->drive(speed[i], direction[i]);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------//

void Drive::linear(float rho, float theta=0) {                                  // (rho, theta) = linear vector
    theta = max(theta, (float)-180.0);
    theta = min(theta, (float)180.0);
    rho = max(rho, (float)0.0);
    rho = min(rho, (float)1.0);                      
                                                                                // Directional Quadrants
    if(theta >= -45 && theta <= 45) {                                               // All motors forward if theta is from -45deg - 45deg   
        for(int i=0; i<4; ++i) {
            direction[i] = 0;
        }
    }
    else if(theta > 45 && theta <= 135) {                                           // Motors 1,3 forward and 0,2 reverse from 45deg - 135deg
        for(int i=0; i<4; ++i) {                                                    // (Strafe rights)
            i % 2 == 0 ? direction[i]=1 : direction[i]=0;
        }
    }
    else if(theta >= -135 && theta < -45) {                                         // Motors 0,2 forward and 1,3 revers from -45deg - -135deg
        for(int i=0; i<4; ++i) {
            i % 2 == 0 ? direction[i]=0 : direction[i]=1;
        }
    }
    else if((theta >= -180 && theta < -135) || (theta <= 180 && theta > 135)) {     // All motors reverse from -135deg - -180 deg or 135deg - 180deg
        for(int i=0; i<4; ++i) {
            direction[i] = 1;
        }
    }
                                                                                // Speed control quadrants (note that speed is a scalar)
    if(theta >= 0 && theta <= 90) {                                                 // Motors 0,2 (1 to 0 to 1)
        for(int i=0; i<4; ++i) {                                                    // and 1,3 constant from 0deg to 90deg
            i % 2 == 0 ? speed[i]=rho*(abs(theta-45)/45) : speed[i]=rho;
        }
    }
    else if(theta > 90 && theta <= 180) {                                           // Motors 0,2 constant
        for(int i=0; i<4; ++i) {                                                    // and 1,3 (1 to 0 to 1) from 90deg to 180deg
            i % 2 == 0 ? speed[i]=rho : speed[i]=rho*(abs(theta-135)/45);
        }
    }
    else if(theta >= -90 && theta < 0) {                                            // Motors 0,2 constant
        for(int i=0; i<4; ++i) {                                                    // and 1,3 (1 to 0 to 1) from 0deg to -90deg
            i % 2 == 0 ? speed[i]=rho : speed[i]=rho*(abs(theta+45)/45);
        }
    }
    else if(theta >= -180 && theta < -90) {                                         // Motors 0,2 (1 to 0 to 1)
        for(int i=0; i<4; ++i) {                                                    // and 1,3 constant from-90deg to -180deg
            i % 2 == 0 ? speed[i]=rho*(abs(theta+135)/45) : speed[i]=rho;
        }
    }
    
    for(int i=0; i<4; ++i) {
        motor[i]->drive(speed[i], direction[i]);
    }
}

//----------------------------------------------------------------------------------------//

void Drive::rotate(float rho, float phi) {
    rho = max(rho, (float)0.0);
    rho = min(rho, (float)1.0);
    phi = max(phi, (float)-1.0);
    phi = min(phi, (float)1.0);

    if(phi > 0) {
        direction[0] = 1;
        direction[1] = 1;
        direction[2] = 0;
        direction[3] = 0;
    }
    else {
        direction[0] = 0;
        direction[1] = 0;
        direction[2] = 1;
        direction[3] = 1;
    }

    for(int i=0; i<4; ++i)
        speed[i] = rho*abs(phi);
}



Motor motorFL(D3, D12);
Motor motorFR(D11, D4);
Motor motorBR(D10, D8);
Motor motorBL(D13, D2);
Drive drive(motorFR, motorBR, motorBL, motorFL);

#endif