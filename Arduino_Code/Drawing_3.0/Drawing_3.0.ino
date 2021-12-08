#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>

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

Adafruit_StepperMotor *motorRF = AFMSbot.getStepper(200, 1);
Adafruit_StepperMotor *motorRB = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *motorLF = AFMSbot.getStepper(200, 2);
Adafruit_StepperMotor *motorLB = AFMStop.getStepper(200, 2);

//Servo penServo;  // create servo object to control a servo

// wrapper to translate Motor Shield to Accel object
void forwardRF() {
  motorRF->onestep(FORWARD, MICROSTEP);
}
void backwardRF() {
  motorRF->onestep(BACKWARD, MICROSTEP);
}
void forwardRB() {
  motorRB->onestep(FORWARD, MICROSTEP);
}
void backwardRB() {
  motorRB->onestep(BACKWARD, MICROSTEP);
}
void forwardLF() {
  motorLF->onestep(FORWARD, MICROSTEP);
}
void backwardLF() {
  motorLF->onestep(BACKWARD, MICROSTEP);
}
void forwardLB() {
  motorLB->onestep(FORWARD, MICROSTEP);
}
void backwardLB() {
  motorLB->onestep(BACKWARD, MICROSTEP);
}


AccelStepper RF(forwardRF, backwardRF);
AccelStepper RB(forwardRB, backwardRB);
AccelStepper LF(forwardLF, backwardLF);
AccelStepper LB(forwardLB, backwardLB);

MultiStepper steppers;

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

  RF.setMaxSpeed(100.0);
  RB.setMaxSpeed(100.0);
  LF.setMaxSpeed(100.0);
  LB.setMaxSpeed(100.0);

  steppers.addStepper(RF);
  steppers.addStepper(RB);
  steppers.addStepper(LF);
  steppers.addStepper(LB);

  //  penServo.attach(9);
}

void loop() {
  setInstruction();
  switch (instruction[0]) {
    case 'G':
      drive();
      Serial.println("vroom");
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

  int magnitude = 1000;
  double diag_1 = sin(theta - 1 / 4 * PI) * magnitude;
  double diag_2 = sin(theta + 1 / 4 * PI) * magnitude;


  
  long positions[4] = {pos,pos,pos,pos};
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
  delay(1000);
  pos = pos+500;
  
//  RF.setCurrentPosition(0);
//  RF.setSpeed(diag_1);
//  RF.moveTo(500);
  //RF.runSpeedToPosition();

  
//  LB.setCurrentPosition(0);
//  LB.setSpeed(diag_1);
//  LB.moveTo(500);
  //LB.runSpeedToPosition();

//  LF.setCurrentPosition(0);
//  LF.setSpeed(diag_2);
//  LF.moveTo(500);
  //LF.runSpeedToPosition();

  
//  RB.setCurrentPosition(0);
//  RB.setSpeed(diag_2);
//  RB.moveTo(500);
  //RB.runSpeedToPosition();

//  RF.setCurrentPosition(0);
//  RF.runToNewPosition(500);
//  
//  RB.setCurrentPosition(0);
//  RB.runToNewPosition(500);
//  
//  LF.setCurrentPosition(0);
//  LF.runToNewPosition(500);
//
//  LB.setCurrentPosition(0);
//  LB.runToNewPosition(500);

  xcur = atoi(xstr);
  ycur = atoi(ystr);

  //delay(1000);

//  steppersDriving();
//  while(steppersDriving()){
//      Serial.println("driving");
//   }

//  while(RF.run() || LF.run() || RB.run() || LB.run()){
//    Serial.print("a");
//  }

}

bool steppersDriving() {
  Serial.println(RF.isRunning());
  //  return (RF.distanceToGo() == 0);
//  return (RF.distanceToGo() != 0) ;
  return (RF.distanceToGo() != 0) || (RB.distanceToGo() != 0) || (LF.distanceToGo() != 0) || (LB.distanceToGo() != 0);
}

//void penUp(){
//  penServo.write(upPos);
//  delay(200);
//}

//void penDown(){
//  penServo.write(downPos);
//  delay(200);
//}
