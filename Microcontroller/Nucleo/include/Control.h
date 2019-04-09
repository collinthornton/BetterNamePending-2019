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
        PID X       = PID(1, 0, 0);
        PID Y       = PID(1, 0, 0);
        PID heading = PID(1, 0, 0);

        Position position();

        Motor motorFL = Motor(D3, D12);
        Motor motorFR = Motor(D10, D8);
        Motor motorBR = Motor(D11, D4);
        Motor motorBL = Motor(D13, D2);

};

Control::Control() {}
void Control::autonomous(void) {}
void Control::assisted(void) {}
void Control::manual(void) {}

Control control = Control();
#endif