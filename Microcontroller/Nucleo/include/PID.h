#ifndef PID_H
#define PID_H

#include <mbed.h>

class PID {
    public:
        PID(float, float, float, float, float);
        float compute(float, float);
        void pauseTimer(void);
        void resumeTimer(void);

    private:
        float Kp, Ki, Kd;
        float error, prevError, sumError, minVal, maxVal;
        Timer dt;
};

PID::PID(float Kp, float Ki, float Kd, float minVal, float maxVal) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;
    this->minVal = minVal;
    this->maxVal = maxVal;

    dt.start();
}
float PID::compute(float actual, float desired) {
    error = desired-actual;

    float Perror = error;
    float Ierror = sumError + (error*dt.read());
    float Derror = (error-prevError)/dt.read();

    prevError = error;
    sumError += error;
    dt.reset();

    float output = Kp*Perror + Ki*Ierror + Kd*Derror;
    output = max(output, minVal);
    output = min(output, maxVal);

    return output;
}
void PID::pauseTimer() {
    dt.stop();
    return;
}
void PID::resumeTimer() {
    dt.start();
    return;
}

#endif