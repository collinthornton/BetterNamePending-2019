#ifndef POSITION_H

#define POSITION_H

//#include <mbed.h>
#include <Ping.h>

#include <cmath>
#include <string>

using namespace std;

class Position {
    public:
        Position();
        string toString(void);
        int findPosition(void);
        void positionTimer(void);

        struct location {
            short driveAxis;                                // 0 = Y, 1 = X
            short driveDir;                                 // 0 = forward, 1 = backward
            short X;
            short Y;
            float heading;                                  // Heading as compared to walls
        };
        location location;

        const short ROBOT_WIDTH = 40, HALL_WIDTH = 61, TUNNEL_WIDTH = 45;
        
        bool is_updated = false;
    
    private:
        int findState(void);
        float findTheta(short, short, short, short);        // Returns -1000 if acos() = nan (should only occur on bridge, tunnel is handled)
        float findTheta(short, short);

        string state         = "UNKNOWN";                   // Possible states: "UNKNOWN", "HALL", "CORNER", "APPROACHING_CORNER"
        string previousState = "UNKNOWN";
        string axis          = "UNKNOWN";                   // Possible states: "UNKOWN", "X", "Y". Y corresponds to front of robot.
        string direction     = "UNKNOWN";                   // FORWARD = forward/right , REVERSE = backward/left    (dependent upon axis)
        string nextAxis      = "UNKNOWN";                   // Possible states: "UNKNOWN", "X", "Y"
        string nextDirection = "UNKNOWN";                   // Possible states: "UNKOWN", "FORWARD", "REVERSE"
        bool   tunnel        = false;
        bool   bridge        = false;

        bool st[8] = {0,0,0,0,0,0,0}, side[4] = {0,0,0,0};
        Timer timer;

        int distance[8] = {0,0,0,0,0,0,0,0};
        const float THRESH = 18;                            // Approximately 7" //! Adjust this value if too sensitive to approaching corners
            /*
                Expected ultrasonic sensor orientation
                    Front: sonar[0] and sonar[1]
                    Right: sonar[2] and sonar[3]
                    Back:  sonar[4] and sonar[5]
                    Left:  sonar[6] and sonar[7]
            */
        short unknownCounter = 0;

        Ping sonar[SONAR_NUM] = {
        #ifdef ENABLE_SONAR_INTERRUPT 
            Ping(D9, A0, MAX_DISTANCE),                // Declare ultrasonic sensors here for interrupt use
            Ping(PE_3, A2, MAX_DISTANCE),
            Ping(PF_2, PE_5, MAX_DISTANCE),
            Ping(PE_10, PD_11, MAX_DISTANCE),
            Ping(PE_2, PE_12, MAX_DISTANCE),
            Ping(PA_0, PE_15, MAX_DISTANCE),
            Ping(PF_7, PD_0, MAX_DISTANCE),
            Ping(PF_9, PD_1, MAX_DISTANCE)
        #else
            Ping(D9, MAX_DISTANCE),                    // Declare ultrasonic sensors here for single pin use
            Ping(A1, MAX_DISTANCE),
            Ping(A5, MAX_DISTANCE),
            Ping(PE_5, MAX_DISTANCE),
            Ping(PE_2, MAX_DISTANCE),
            Ping(PA_0, MAX_DISTANCE),
            Ping(PF_7, MAX_DISTANCE),
            Ping(PF_9, MAX_DISTANCE)
        #endif
        };



};

Position::Position() {
    axis = "Y";
    direction = "FORWARD";
    location.driveAxis = 0;                    // Initialize drive axis as forward

    timer.start();
}

int Position::findState(void) {

    if(SONAR_NUM < 8) return -2;

    int minD = 100;
    short maxedOutCounter = 0;

    for(int i=0; i < 8; ++i) {
        if(Ping::instance[i] != NULL) {
            distance[i] = sonar[i].ping_cm(MAX_DISTANCE);
            if(distance[i] < minD) minD = distance[i];
            if(distance[i] == -1) ++maxedOutCounter;
        }
    }
 
    for(int i=0; i<SONAR_NUM; ++i) {
        if(i < 4) side[i] = 0;
        if(distance[i] <= THRESH && distance[i] >= 0) st[i] = 1;
        else st[i] = 0;
    }                                                               

    if(st[0] && st[1]) side[0] = 1;   
    if(st[2] && st[3]) side[1] = 1; 
    if(st[4] && st[5]) side[2] = 1; 
    if(st[6] && st[7]) side[3] = 1; 
    

    if(minD > THRESH || maxedOutCounter > 5) {
        state = "UNKNOWN";
    }
    else if(side[1] && side[3] && !side[0] && !side[2]) {
        state       = "HALL";
        axis        = "Y";
        nextAxis    = "X";
        previousState = state;
    }
    else if(side[0] && side[2] && !side[1] && !side[3]) {
        state       = "HALL";
        axis        = "X";
        nextAxis    = "Y";
        previousState = state;
    }
    else if(side[0] && side[1] && !st[4] && !st[5] && !st[6] && !st[7]) {
        state       = "CORNER";
        direction   = nextDirection;
        axis        = nextAxis;
        previousState = state;
    }
    else if(side[1] && side[2] && !st[0] && !st[1] && !st[6] && !st[7]) {
        state       = "CORNER";
        direction   = nextDirection;
        axis        = nextAxis;
        previousState = state;
    }
    else if(side[2] && side[3] && !st[0] && !st[1] && !st[2] && !st[3]) {
        state       = "CORNER";
        direction   = nextDirection;
        axis        = nextAxis;
        previousState = state;
    }
    else if(side[3] && side[0] && !st[2] && !st[3] && !st[4] && !st[5]) {
        state       = "CORNER";
        direction   = nextDirection;
        axis        = nextAxis;
        previousState = state;
    }
    else if(side[3] && (st[2] ^ st[3]) && previousState == "HALL") {                   // Transitions from hall to corner           
        state           = "TRANSITION";
        nextDirection   = "FORWARD";
        nextAxis        = "X";
    }
    else if(side[1] && (st[6] ^ st[7]) && previousState == "HALL") {
        state           = "TRANSITION";
        nextDirection   = "REVERSE";
        nextAxis        = "X";
    } 
    else if(side[0] && (st[4] ^ st[5]) && previousState == "HALL") {
        state           = "TRANSITION";
        nextDirection   = "REVERSE";
        nextAxis        = "Y";
    }
    else if(side[2] && (st[0] ^ st[1]) && previousState == "HALL") {
        state           = "TRANSITION";
        nextDirection   = "FORWARD";
        nextAxis        = "Y";
    }
    else if(((side[1] && st[7] && !st[6]) || (side[3] && st[2] && !st[3])) && previousState == "CORNER") {     // Transitions from corner to hall
        state           = "TRANSITION";
        direction       = "FORWARD";
        axis            = "Y";
    }
    else if(((side[1] && st[6] && !st[7]) || (side[3] && st[3] && !st[2])) && previousState == "CORNER") {
        state           = "TRANSITION";
        direction       = "REVERSE";
        axis            = "Y";
    }
    else if(((side[0] && st[4] && !st[5]) || (side[2] && st[1] && !st[0])) && previousState == "CORNER") {
        state           = "TRANSITION";
        direction       = "FORWARD";
        axis            = "X";
    }
    else if(((side[0] && st[5] && !st[4]) || (side[2] && st[0] && !st[1])) && previousState == "CORNER") {
        state           = "TRANSITION";
        direction       = "REVERSE";
        axis            = "X";
    }
    else {
        state = "UNKNOWN";
    }
    return 1;
}

int Position::findPosition(void) {
    findState();

    short front = 0, right = 0, back = 0, left = 0, X = 0, Y = 0;
    short driveDir = 0, driveAxis = 0;
    float theta = 0.0;

    if(state == "UNKNOWN") {
        ++unknownCounter;
        if(unknownCounter >= 5) {
            location.X = 0;
            location.Y = 0;
            location.heading = 0;
        }
        return -2;
    }
    unknownCounter = 0;
    
    if(state == "HALL") {
        if(axis == "Y") theta = findTheta(distance[2], distance[3], distance[6], distance[7]);
        else            theta = findTheta(distance[0], distance[1], distance[4], distance[5]);
    }
    else if(state == "CORNER") {
        if     (direction == "FORWARD" && side[1])  theta = findTheta(distance[2], distance[3], distance[4], distance[5]);
        else if(direction == "FORWARD" && side[3])  theta = findTheta(distance[6], distance[7], distance[4], distance[5]);
        else if(direction == "REVERSE" && side[1])  theta = findTheta(distance[2], distance[3], distance[0], distance[1]);
        else if(direction == "REVERSE" && side[3])  theta = findTheta(distance[6], distance[7], distance[0], distance[1]);
    }
    else if(state == "TRANSITION") {
        if(previousState == "HALL") {
            if(axis == "Y") {
                if(side[1]) theta = findTheta(distance[2], distance[3]);
                else        theta = findTheta(distance[6], distance[7]);
            }
            else {
                if(side[0]) theta = findTheta(distance[0], distance[1]);
                else        theta = findTheta(distance[4], distance[5]);
            }
        }
        else if(previousState == "CORNER") {
            if     (direction == "FORWARD" && side[1])  theta = findTheta(distance[2], distance[3], distance[4], distance[5]);
            else if(direction == "FORWARD" && side[3])  theta = findTheta(distance[6], distance[7], distance[4], distance[5]);
            else if(direction == "REVERSE" && side[1])  theta = findTheta(distance[2], distance[3], distance[0], distance[1]);
            else if(direction == "REVERSE" && side[3])  theta = findTheta(distance[6], distance[7], distance[0], distance[1]);            
        }
    }

    short distanceX[SONAR_NUM];

    for(int i=0; i<SONAR_NUM; ++i) {                                  // Find the x-component of the sonar measurement
        distanceX[i] = cos(theta) * distance[i];
    }

    front = (distanceX[0] + distanceX[1]) / 2;                        // Define the side distance measurements as the average of both distance measurements for that side
    back  = (distanceX[4] + distanceX[5]) / 2;                        //  and override as needed below. 
    right = (distanceX[2] + distanceX[3]) / 2;
    left  = (distanceX[6] + distanceX[7]) / 2;
    
    if(state == "TRANSITION") {
        if(previousState == "HALL") {
            if(axis == "Y" && direction == "FORWARD") {
                if(!st[2])      right  = distanceX[3];
                else if(!st[7]) left   = distanceX[6];
            }
            else if(axis == "Y" && direction == "REVERSE") {
                if(!st[3])      right  = distanceX[2];
                else if(!st[6]) left   = distanceX[7];
            }
            else if(axis == "X" && direction == "FORWARD") {
                if(!st[1])      front  = distanceX[0];
                else if(!st[4]) back   = distanceX[5];
            }
            else if(axis == "X" && direction == "REVERSE") {
                if(!st[0])      front  = distanceX[1];
                else if(!st[5]) back   = distanceX[4];
            }
        }
        else if(previousState == "CORNER") {
            if(axis == "Y" && direction == "FORWARD") {
                if(st[7])      left    = distanceX[7];
                else if(st[2]) right   = distanceX[2];
            }
            else if(axis == "Y" && direction == "REVERSE") {
                if(st[6])      left    = distanceX[6];
                else if(st[3]) right   = distanceX[3];
            }
            else if(axis == "X" && direction == "FORWARD") {
                if(st[1])      front   = distanceX[1];
                else if(st[4]) back    = distanceX[4];
            }
            else if(axis == "X" && direction == "REVERSE") {
                if(st[0])      front   = distanceX[0];
                else if(st[5]) back    = distanceX[5];
            }
        }
    }

    if(front < 0) front = MAX_DISTANCE;
    if(right < 0) right = MAX_DISTANCE;  
    if(back  < 0) back  = MAX_DISTANCE;                                         // MAX_DISTANCE is defined in Ping.h
    if(left  < 0) left  = MAX_DISTANCE;

    if(axis == "Y") {
        X = (left-right) / 2;
        if(direction == "FORWARD")  Y = front;
        else                        Y = back;
    }
    else {
        Y = (back-front) / 2;
        if(direction == "FORWARD")  X = right;
        else                        X = left;
    }

    axis == "X" ? location.driveAxis = 0 : location.driveAxis = 1;                  // axis = "X", driveAxis = 0, else driveAxis = 1;
    direction == "FORWARD" ? location.driveDir = 0 : location.driveDir = 1;         // direction = "FOR..", driveDir = 0, else driveDir = 0;

    location.driveDir   = driveDir;
    location.driveAxis  = driveAxis;
    location.X          = X;
    location.Y          = Y;
    location.heading    = theta;

    is_updated = true;

    return 1;
}
float Position::findTheta(short distance0, short distance1, short distance2, short distance3) {  
    float theta;

    int part1 = atan((distance0-distance1)/ROBOT_WIDTH);
    int part2 = atan((distance2-distance3)/ROBOT_WIDTH);

    //int part1 = acos((HALL_WIDTH-ROBOT_WIDTH)/(distance0+distance1));
    //int part2 = acos((HALL_WIDTH-ROBOT_WIDTH)/(distance2+distance3));
    
    theta = (part1 + part2) / 2;
    return theta;                                                                                                                        
}
float Position::findTheta(short distance0, short distance1) {
    float theta;

    theta = atan((distance0-distance1)/ROBOT_WIDTH);
    return theta;
}
string Position::toString(void) {
    string output;

    //output += to_string(distance[0]);
    for(int i=0; i<SONAR_NUM; ++i) {
        char buff[10];
        sprintf(buff, "%3i", distance[i]);
        output += buff;
        output += ' ';
    }
/*    
    output += "State: "     + state         + ' ';
    output += "Direction: " + direction     + ' ';
    output += "Axis: "      + axis          + ' ';
    output += "Next Dir: "  + nextDirection + ' ';
    output += "Next Axis: " + nextAxis      + ' ';

    for(int i=0; i<SONAR_NUM; ++i) {
        output += to_string(st[i]) + ' ';
    }

    output += ' ';
    output += to_string(location.driveAxis) + ' ';
    output += to_string(location.driveDir)  + ' ';
    output += to_string(location.X)         + ' ';
    output += to_string(location.Y)         + ' ';
    output += to_string(location.heading)   + ' ';
*/
    return output;
}

void Position::positionTimer(void) {
    if(timer.read() > .025) {
        findPosition();
        timer.reset();
    }
}

#endif