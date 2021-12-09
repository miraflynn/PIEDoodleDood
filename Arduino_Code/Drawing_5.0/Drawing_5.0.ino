#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>

#include "MiraStepper.h"

File pathFile;
char instruction[] = "UP";
char xstr[] = "000";
char ystr[] = "000";

int xcur = 0;
int ycur = 0;

int pos = 500;

int downPos = 22;
int upPos = downPos + 30;


Adafruit_MotorShield AFMSbot(0x60); // Default address, no jumpers
Adafruit_MotorShield AFMStop(0x61); // Rightmost jumper closed

public Adafruit_StepperMotor *motorRF = AFMSbot.getStepper(200, 1);
public Adafruit_StepperMotor *motorRB = AFMStop.getStepper(200, 1);
public Adafruit_StepperMotor *motorLF = AFMSbot.getStepper(200, 2);
public Adafruit_StepperMotor *motorLB = AFMStop.getStepper(200, 2);


MiraStepper steppers(motorRF, motorRB, motorLF, motorLB);

//Servo penServo;  // create servo object to control a servo

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  // open the file for reading:
  pathFile = SD.open("path.txt");
  if (pathFile) {
    Serial.println("Path file successfully loaded");
  } else {
    Serial.println("error opening path.txt");
  }

  AFMSbot.begin(); // Start the bottom shield
  AFMStop.begin(); // Start the top shield

  steppers.setMaxSpeedRPM(20);

  //  penServo.attach(9);
}

void loop() {
  setInstruction();
  switch (instruction[0]) {
    case 'G':
      drive();
//      Serial.println("vroom");
      break;
    case 'U':
      Serial.println("UP");
      //      penUp();
      break;
    case 'D':
      Serial.println("DOWN");
      //      penDown();
      break;
    default:
      Serial.println("INVALID");
  }
}

void setInstruction() {
  if (pathFile.available()) {
    instruction[0] = pathFile.read(); // instruction is first 2 characters
    instruction[1] = pathFile.read();
    pathFile.read(); // read comma to get rid of it
    xstr[0] = pathFile.read(); // x coord is next 3 characters
    xstr[1] = pathFile.read();
    xstr[2] = pathFile.read();
    pathFile.read(); // read comma to get rid of it
    ystr[0] = pathFile.read(); // y coord is next 3 characters
    ystr[1] = pathFile.read();
    ystr[2] = pathFile.read();
    pathFile.read(); // newline
    pathFile.read(); // not 100% sure what this character is tbh
  } else {
    instruction[0] = 'N';
    instruction[1] = 'N';
  }
}

void drive() {
  int x_move = atoi(xstr) - xcur;
  int y_move = atoi(ystr) - ycur;

  double theta = atan2(y_move, x_move);

  int magnitude = 10000;
  double diag_1 = sin(theta - 1 / 4 * PI) * magnitude;
  double diag_2 = sin(theta + 1 / 4 * PI) * magnitude;


  // RF, RB, LF, LB
//  steppers.setTargets(diag_1, diag_2, diag_2, diag_1);
  steppers.setTargets(pos, pos, pos, pos);
  steppers.runToTargets();
  delay(1000);
  pos = pos+500;
  

  xcur = atoi(xstr);
  ycur = atoi(ystr);
}

//void penUp(){
//  penServo.write(upPos);
//  delay(200);
//}

//void penDown(){
//  penServo.write(downPos);
//  delay(200);
//}
