#include <Arduino.h>

#define MIN_SPEED 0
#define MAX_SPEED 255

class Motors
{
private:
    const int in1;
    const int in2;
    const int in3;
    const int in4;
    const int en1;
    const int en2;
    int leftWheelSpeed;
    int rightWheelSpeed;
    void setMotorState(int l1, int l2, int r1, int r2);

public:
    Motors(int in1, int in2, int in3, int in4, int en1, int en2);
    void begin();
    void forward();
    void backward();
    void left();
    void right();
    void stop();
    void setSpeed(int leftWheelSpeed, int rightWheelSpeed);
};
Motors::Motors(int in1, int in2, int in3, int in4, int en1, int en2)
    : in1(in1), in2(in2), in3(in3), in4(in4), en1(en1), en2(en2), leftWheelSpeed(MAX_SPEED), rightWheelSpeed(MAX_SPEED)
{
}

void Motors::begin()
{
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    pinMode(en1, OUTPUT);
    pinMode(en2, OUTPUT);
    stop();
}

void Motors::setMotorState(int l1, int l2, int r1, int r2) {
    digitalWrite(in1, l1); 
    digitalWrite(in2, l2);
    digitalWrite(in3, r1); 
    digitalWrite(in4, r2);
}

void Motors::forward()
{
    setMotorState(HIGH, LOW, HIGH, LOW);
}

void Motors::backward()
{
    setMotorState(LOW, HIGH, LOW, HIGH);
}

void Motors::left()
{
    setMotorState(HIGH, LOW, LOW, HIGH);
}

void Motors::right()
{
    setMotorState(LOW, HIGH, HIGH, LOW);
}

void Motors::stop()
{
    setMotorState(LOW, LOW, LOW, LOW);
}

void Motors::setSpeed(int newLeftWheelSpeed, int newRightWheelSpeed)
{
    leftWheelSpeed = constrain(newLeftWheelSpeed, MIN_SPEED, MAX_SPEED);
    rightWheelSpeed = constrain(newRightWheelSpeed, MIN_SPEED, MAX_SPEED);
    analogWrite(en1, leftWheelSpeed);
    analogWrite(en2, rightWheelSpeed);
}
