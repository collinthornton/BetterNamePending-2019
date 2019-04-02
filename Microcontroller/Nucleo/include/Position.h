#ifndef POSITION_H

#define POSITION_H

#include <mbed.h>
#include <Ping.h>

#include <limits>
#include <string>
#include <math.h>

class Position {
    public:
        Position();
        string toString(void);
        int findPosition(void);
        void positionTimer(void);

        struct location {
            short driveAxis;                                // 0 = Y, 1 = X
            short driveDir;                                 // 0 = forward, 1 = backward
            float front;
            float right;
            float back;
            float left;
            float heading;                                  // Heading as compared to walls
        };
        location location;
        
        bool is_updated = false;
    
    private:
        int findState(void);

        string state         = "UNKNOWN";                   // Possible states: "UNKNOWN", "HALL", "CORNER", "APPROACHING_CORNER"
        string previousState = "UNKNOWN";
        string axis          = "UNKNOWN";                   // Possible states: "UNKOWN", "X", "Y". Y corresponds to front of robot.
        string direction     = "UNKNOWN";                   // FORWARD = forward/right , REVERSE = backward/left    (dependent upon axis)
        string nextAxis      = "UNKNOWN";                   // Possible states: "UNKNOWN", "X", "Y"
        string nextDirection = "UNKNOWN";                   // Possible states: "UNKOWN", "FORWARD", "REVERSE"

        bool st[8] = {0,0,0,0,0,0,0}, side[4] = {0,0,0,0};
        Timer time;

        int distance[8];
        const float THRESH = 18;                            // Approximately 7" //! Adjust this value if too sensitive to approaching corners
            /*
                Expected ultrasonic sensor orientation
                    Front: sonar[0] and sonar[1]
                    Right: sonar[2] and sonar[3]
                    Back:  sonar[4] and sonar[5]
                    Left:  sonar[6] and sonar[7]
            */
};

Position::Position() {
    axis = "Y";
    direction = "FORWARD";
    location.driveAxis = 0;                    // Initialize drive axis as forward

    time.start();
}
int Position::findState(void) {

    if(SONAR_NUM < 8) return -2;

    //float maxD = -100, minD = 100;
    //short maxedOutCounter = 0;

    for(int i=0; i < 8; ++i) {
        if(Ping::instance[i] != NULL) {
            distance[i] = sonar[i].ping_cm(MAX_DISTANCE);
        //    if(distance[i] > maxD) maxD = distance[i];
        //    if(distance[i] < minD) minD = distance[i];
        //    if(distance[i] == -1) ++maxedOutCounter;
        }
    }

    //if(minD > THRESH || maxedOutCounter > 5) {
    //    state = "UNKNOWN";
    //    return 0;
    //}
 
    for(int i=0; i<8; ++i) {
        st[i] = 0;
        if(i < 4) side[i] = 0;
    }                                                               // front, right, back, left
    for(int i=0; i<8; ++i) {
        if(distance[i] <= THRESH && distance[i] >= 0) st[i] = 1;
    }

    if(st[0] && st[1]) side[0] = 1;   
    if(st[2] && st[3]) side[1] = 1; 
    if(st[4] && st[5]) side[2] = 1; 
    if(st[6] && st[7]) side[3] = 1; 
    
    if(side[1] && side[3] && !side[0] && !side[2]) {
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

    float front = 0, right = 0, back = 0, left = 0, heading = 0;
    short driveDir = 0, driveAxis = 0;

    if(state == "UNKNOWN") {
        location.front = 0;
        location.right = 0;
        location.back  = 0;
        location.left  = 0;
        location.heading = 0;
        return -2;
    }

    axis == "X" ? location.driveAxis = 0 : location.driveAxis = 1;
    direction == "FORWARD" ? location.driveDir = 0 : location.driveDir = 1;

    if(state == "HALL" || state == "CORNER") {
        axis == "X" ? driveAxis = 0 : driveAxis = 1;
        direction == "FORWARD" ? driveDir = 0 : driveDir = 1;
        front = (distance[0] + distance[1]) / 2;
        back  = (distance[4] + distance[5]) / 2;

        right = (distance[2] + distance[3]) / 2;
        left  = (distance[6] + distance[7]) / 2;

        if(front < 0) front = MAX_DISTANCE;
        if(right < 0) right = MAX_DISTANCE;  
        if(back  < 0) back  = MAX_DISTANCE;                       // MAX_DISTANCE is defined in Ping.h
        if(left  < 0) left  = MAX_DISTANCE;
    }
    else if(state == "TRANSITION") {                //! HERE

    }

    location.driveDir   = driveDir;
    location.driveAxis  = driveAxis;
    location.front      = front;
    location.right      = right;
    location.back       = back;
    location.left       = left;
    location.heading    = heading;

    return 1;
}
string Position::toString(void) {
    string output;
/*
    output.append(state);                           output.append("\n");
    output.append(to_string(location.driveAxis));   output.append("\n");
    output.append(to_string(location.driveDir));    output.append("\n");
    output.append(to_string(location.front));       output.append("\n");
    output.append(to_string(location.right));       output.append("\n");
    output.append(to_string(location.back));        output.append("\n");
    output.append(to_string(location.left));        output.append("\n");
    output.append(to_string(location.heading));     output.append("\n");
*/

    for(int i=0; i<SONAR_NUM; ++i) {
        char distanceStr[4];
        sprintf(distanceStr, "%2i", distance[i]);
        output += distanceStr;
        output += ' ';
    }
    
    output += "State: "     + state         + ' ';
    output += "Direction: " + direction     + ' ';
    output += "Axis: "      + axis          + ' ';
    output += "Next Dir: "  + nextDirection + ' ';
    output += "Next Axis: " + nextAxis      + ' ';

    for(int i=0; i<SONAR_NUM; ++i) {
        output += to_string(st[i]) + ' ';
    }
    
    return output;
}

void Position::positionTimer(void) {
    if(time.read_us() > 15000) {
        findState();
        time.reset();
    }
}

Position position = Position();

#endif