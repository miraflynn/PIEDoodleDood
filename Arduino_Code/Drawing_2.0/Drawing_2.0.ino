#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include <SPI.h>
#include <SD.h>

File pathFile;
char instruction[] = "UP";
char xstr[] = "000";
char ystr[] = "000";


Adafruit_MotorShield AFMSbot(0x60); // Default address, no jumpers
Adafruit_MotorShield AFMStop(0x61); // Rightmost jumper closed

Adafruit_StepperMotor *motorRF = AFMSbot.getStepper(200, 1);
Adafruit_StepperMotor *motorRB = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *motorLF = AFMSbot.getStepper(200, 2);
Adafruit_StepperMotor *motorLB = AFMStop.getStepper(200, 2);

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

void setup() {
  Serial.begin(9600);
  while(!Serial){}
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
  RF.setAcceleration(100.0);

  RB.setMaxSpeed(100.0);
  RB.setAcceleration(100.0);
  
  LF.setMaxSpeed(100.0);
  LF.setAcceleration(100.0);
  
  LB.setMaxSpeed(100.0);
  LB.setAcceleration(100.0);

}

void loop() {
  setInstruction();
  switch(instruction[0]) {
    case 'G':
      drive();

      break;
    case 'U':
      Serial.println("UP");
      break;
    case 'D':
      Serial.println("DOWN");
      break;
    default:
      Serial.println("INVALID");
  }
}

void setInstruction(){
  if(pathFile.available()){
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

void drive(){
  int x_move = atoi(xstr);
  int y_move = atoi(ystr);

  double theta = atan2(y_move,x_move);

  int magnitude = 1000;
  double diag_1 = sin(theta - 1/4 * PI) * magnitude;
  double diag_2 = sin(theta + 1/4 * PI) * magnitude;

  RF.setSpeed(diag_1);
  RF.move(500);
  RF.runSpeedToPosition();
  
}
