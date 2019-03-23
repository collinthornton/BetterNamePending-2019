#ifndef POSITION_H

#define POSITION_H
#include <limits>
#include <string>
#include <math.h>
#include "Ping.h"

class Position {
    public:
        Position(Ping*);
        int find(void);

        struct location {
            float front;
            float right;
            float back;
            float left;
            float heading;                           // Heading as compared to walls
        };
        location location;
         
        string state = "UNKNOWN";                   // Possible states: "UNKNOWN", "HALL", "CORNER", "APPROACHING_CORNER"
        string direction = "NONE";                  // Possible states: "NONE", "X", "Y". Y corresponds to front of robot.
        bool is_updated = false;
    
    private:
        Ping* sonar;
        float distance[8];
        const float THRESH = 18;                    // Approximately 7" //! Adjust this value if too sensitive to approaching corners
            /*
                Expected ultrasonic sensor orientation
                    Front: sonar[0] and sonar[1]
                    Right: sonar[2] and sonar[3]
                    Back:  sonar[4] and sonar[5]
                    Left:  sonar[6] and sonar[7]
            */
};

Position::Position(Ping sonar[8]) {
    this->sonar = sonar;
}
int Position::find(void) {

    if(SONAR_NUM < 8) return -2;

    float max = -100, min = 100;
    for(int i=0; i < 8; ++i) {
        if(Ping::instance[i] != NULL)
            distance[i] = sonar[i].ping_cm();
            if(distance[i] > max) max = distance[i];
            if(distance[i] < min) min = distance[i];
    }

    if(min > THRESH) {
        state = "UNKNOWN";
        direction = "NONE";
        return 0;
    }
 
    bool st[8] = {0,0,0,0,0,0,0,0};
    bool side[4] = {0,0,0,0};
    for(int i=0; i<8; ++i) {
        if(distance[i] < THRESH) st[i] = 1;
    }

    float front = 1000, right = 1000, back = 1000, left = 1000;

    if(st[0] && st[1]) { front = (distance[0]+distance[1]) / 2; side[0] = 1; }
    if(st[1] && st[2]) { right = (distance[2]+distance[3]) / 2; side[1] = 1; }
    if(st[3] && st[4]) { back  = (distance[4]+distance[5]) / 2; side[2] = 1; }
    if(st[5] && st[6]) { left  = (distance[6]+distance[7]) / 2; side[3] = 1; }
    
    if(side[0] && side[1] && !side[2] && !side[3]) {
        state = "HALL";
        direction = "Y";
    }
    else if(side[2] && side[3] && !side[0] && !side[1]) {
        state = "HALL";
        direction = "X";
    }
    

}


#endif