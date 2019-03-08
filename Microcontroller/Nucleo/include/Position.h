#ifndef POSITION_H

#define POSITION_H
#include <mbed.h>
#include <string>
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
         
        string state = "UNKNOWN";                   // Possible states: "UNKNOWN", "HALL", "CORNER"
        bool is_updated = false;
    
    private:

        Ping* sonar;

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

}

#endif