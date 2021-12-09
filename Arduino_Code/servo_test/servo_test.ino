/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

int downPos = 28;
int upPos = downPos + 30;

Servo penServo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

void penUp(){
  penServo.write(upPos);
  delay(200);
}

void penDown(){
  penServo.write(downPos);
  delay(200);
}

void setup() {
  penServo.attach(9);  // attaches the servo on pin 9 to the servo object
}

//int pos = 0;    // variable to store the servo position
void loop() {
//  for (pos = downPos; pos <= downPos + 30; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  delay(2000);
//  for (pos = downPos + 30; pos >= downPos; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  delay(2000);
//  myservo.write(25);
  penUp();
  delay(2000);
  penDown();
  delay(2000);
}
