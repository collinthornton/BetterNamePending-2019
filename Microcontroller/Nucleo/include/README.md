# Nuceleo Function Definitions

These files are libraries to interface the STM32 Nucleo F746ZG ARM microcontroller with various sensors, chips, and breakout boards. 

Credits for the 'Ping.h' library are given to Tim Eckel and his 'New Ping' library for Arduino. Most of the logic is based on his work. 

## Files:
- Ping.h
- WiFly.h
- Control.h
- HC05.h
- Motor.h
- PID.h
- Position.h
- Robot.h
- SerialPC.h
- Drive.h

## Documentation:
### Ping.h
- Interfaces Mbed OS based system with generic ultrasonic sensors. 
- Only a single pin is needed - the library will toggle it between output and input.
- Can form array of sensors. __Example:__
`Ping sonar[NUM_DEVICES]= {  
    Ping(PinName, MAX_DISTANCE),  
    Ping(PinName, MAX_DISTANCE)  
 };`
        
#### Methods:
        -Ping::Ping(PinName, unsigned int max_cm_distance)
        -unsigned int Ping::ping(unsigned int max_cm_distance)     Returns round-trip time in uS
        -unsigned int Ping::ping_cm(unsigned int max_cm_distance)  Retruns distnace in cm

### WiFly.h
- Static library of function
- Defines DigitalOut, InterruptIn, Timer, and Timeour interfaces necessary for WiFly
- Handles interrupts
        
#### Methods:
        -Wifly::Wifly()
        -void WiFly::wifiConfig()
        -void WiFly::wifiDisconnectHost()
        -int  WiFly::wifiInit()

### Control.h
- Contains two modes of control: autonomous and manual
  - Autonomous continues moving in the current direction until a corner is detected, then follows the new hall.
  - Manual is meant for testing purposes. Keyboard input is mapped to motor power, direction of travel, and the rotation ratio
            
#### Methods:
        -Control::Control()
        -void Control::autonomous(void)
        -void Control::manual(float rho, float theta, float phi)
        
### HC05.h
-Brief class to interface with an HC05 bluetooth module
        
#### Methods:
        -Bluetooth::Bluetooth()
        -void Bluetooth::transmit(string &out, bool cr=true)
        -void Bluetooth::transmit(char out, bool cr=true)
        -void Bluetooth::transmit(const char* out, bool cr=true)
        -void Bluetooth::processBT(void)
> _**Note:** cr is short for 'carriage return'_
        
### Motor.h
- Driver for generic brushed DC motor controllers
        
#### Variables:
        -float maxSpeed = 0.4   Maximum percentage of motor power
        
#### Methods:
        -void Motor::Motor()
        -void Motor::drive(float mag, int dir)
        
### PID.h
- Contains methods to easily create PID controllers for multiple dimensions of control
        
#### Methods:
        -PID::PID(float Kp, float Ki, float Kd, float minOutput, float maxOutput)
        -float PID::compute(float actual, float desired)
        -void PID::pauseTimer(void)
        -void PID::resumeTimer(void)
> _**Note:** the timer functions are to monitor the elapsed time between functions calls, influnce integrall and derivative errors_
        
### Position.h
- Algorithms for determining position state relative to the challenge course. Is restricted to an environment where at least 2 sides of the robot are near to a wall at all times. Method positionTimer() will use a hardware interrupt timer to update the estimated position at 40 Hz
  
#### Variables:
        -struct location {
            short driveAxis;    0=Y, 1=X **has mecanum wheels**
            short driveDir;     0=forward, 1=backward
            short X;            If travelling on Y axis, X=distance from center between walls. Else X=distance to wall in direction of travel
            short Y;            If travellin on X axis, Y=distance from center between walls. Else Y=distance to wall in direction of travel
            float heading;      Heading as compared to the walls
        };
        -const short ROBOT_WIDTH=40, HALL_WIDTH=61, TUNNEL_WIDTH=45 **Units: cm**
        -bool is_updated=false
        
#### Methods:
        -Position::Position()
        -string Position::toString(void)
        -int findPosition(void)
        -void positionTimer(void)

### Robot.h
- Top level control for the robot.
        
#### Variables:
        -bool MOTORS_ARMED=true
        -Timer wifiTimer
        
#### Methods:
        -Robot::Robot()
        -void Robot::init(void)
        -void Robot::run(void)
        
### SerialPC.h
        -Essentially identical to HC05.h Most of the variables keep track of user input for testing purposes.
        
        Variables:
        -string pcData
        -bool pcUpdated=false, printSonar=false, printWifi=false, printPosition=false, wifiConnectEnabled=false, motorUpdated=false
        -double speed=0, theta=0, phi=0
        -float speedConstant=0.075
