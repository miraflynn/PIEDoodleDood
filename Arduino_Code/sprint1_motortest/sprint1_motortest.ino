#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor *rightMotor = AFMS.getMotor(2);

void setup() {
  // put your setup code here, to run once:
  AFMS.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  leftMotor->setSpeed(40);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(0);
  rightMotor->run(BACKWARD);
  delay(100);
}
