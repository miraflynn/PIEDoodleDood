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

int pos_1 = 0;
int pos_2 = 0;

int downPos = 22;
int upPos = downPos + 30;


Adafruit_MotorShield AFMSbot(0x60); // Default address, no jumpers
Adafruit_MotorShield AFMStop(0x61); // Rightmost jumper closed

Adafruit_StepperMotor *motorRF = AFMSbot.getStepper(200, 1);
Adafruit_StepperMotor *motorRB = AFMStop.getStepper(200, 1);
Adafruit_StepperMotor *motorLF = AFMSbot.getStepper(200, 2);
Adafruit_StepperMotor *motorLB = AFMStop.getStepper(200, 2);

class MiraStepper
{
  public:
    void setSpeedRPM(float rpm1, float rpm2, float rpm3, float rpm4);
    void setMaxSpeedRPM(float rpm);
    void setSpeedSPS(float sps1, float sps2, float sps3, float sps4);
    void setMaxSpeedSPS(float sps);
    void setTargets(int target1, int target2, int target3, int target4);
    void runToTargets();
    void calculateSpeeds();
  private:
    float _maxMsPerStep;
    float _msPerStep1;
    float _msPerStep2;
    float _msPerStep3;
    float _msPerStep4;
    int _currentPos1;
    int _currentPos2;
    int _currentPos3;
    int _currentPos4;
    int _targetPos1;
    int _targetPos2;
    int _targetPos3;
    int _targetPos4;
};

void MiraStepper::setSpeedRPM(float rpm1, float rpm2, float rpm3, float rpm4){
  setSpeedSPS(200*rpm1/60, 200*rpm2/60, 200*rpm3/60, 200*rpm4/60);
}

void MiraStepper::setMaxSpeedRPM(float rpm){
  setMaxSpeedSPS(200*rpm/60);
}

void MiraStepper::setSpeedSPS(float sps1, float sps2, float sps3, float sps4){
  _msPerStep1 = 1000/sps1;
  _msPerStep2 = 1000/sps2;
  _msPerStep3 = 1000/sps3;
  _msPerStep4 = 1000/sps4;
}

void MiraStepper::setMaxSpeedSPS(float sps){
  _maxMsPerStep = 1000/sps;
}

void MiraStepper::setTargets(int target1, int target2, int target3, int target4){
  _targetPos1 = target1;
  _targetPos2 = target2;
  _targetPos3 = target3;
  _targetPos4 = target4;
}

void MiraStepper::runToTargets(){
  calculateSpeeds();
  unsigned long startTime = millis();
  int targetSteps1 = _targetPos1 - _currentPos1;
  int targetSteps2 = _targetPos2 - _currentPos2;
  int targetSteps3 = _targetPos3 - _currentPos3;
  int targetSteps4 = _targetPos4 - _currentPos4;

  int steps1 = 0;
  int steps2 = 0;
  int steps3 = 0;
  int steps4 = 0;
  
  
  while(_currentPos1 != _targetPos1 || _currentPos2 != _targetPos2 || _currentPos3 != _targetPos3 || _currentPos4 != _targetPos4){
    unsigned long thisLoopTime = millis();
    
    if((thisLoopTime - startTime) > _msPerStep1*steps1 && steps1 != abs(targetSteps1)){
      if(targetSteps1 > 0){
        motorRF->onestep(FORWARD, DOUBLE);
        _currentPos1 += 1;
        steps1 += 1;
      } else {
        motorRF->onestep(BACKWARD, DOUBLE);
        _currentPos1 -= 1;
        steps1 += 1;
      }
    }

    if((thisLoopTime - startTime) > _msPerStep2*steps2 && steps2 != abs(targetSteps2)){
      if(targetSteps2 > 0){
        motorRB->onestep(FORWARD, DOUBLE);
        _currentPos2 += 1;
        steps2 += 1;
      } else {
        motorRB->onestep(BACKWARD, DOUBLE);
        _currentPos2 -= 1;
        steps2 += 1;
      }
    }

    if((thisLoopTime - startTime) > _msPerStep3*steps3 && steps3 != abs(targetSteps3)){
      if(targetSteps3 > 0){
        motorLF->onestep(BACKWARD, DOUBLE);
        _currentPos3 += 1;
        steps3 += 1;
      } else {
        motorLF->onestep(FORWARD, DOUBLE);
        _currentPos3 -= 1;
        steps3 += 1;
      }
    }

    if((thisLoopTime - startTime) > _msPerStep4*steps4 && steps4 != abs(targetSteps4)){
      if(targetSteps4 > 0){
        motorLB->onestep(BACKWARD, DOUBLE);
        _currentPos4 += 1;
        steps4 += 1;
      } else {
        motorLB->onestep(FORWARD, DOUBLE);
        _currentPos4 -= 1;
        steps4 += 1;
      }
    }
    
  }
//  Serial.println("end of run");
}

void MiraStepper::calculateSpeeds(){
  int targetSteps1 = abs(_targetPos1 - _currentPos1);
  int targetSteps2 = abs(_targetPos2 - _currentPos2);
  int targetSteps3 = abs(_targetPos3 - _currentPos3);
  int targetSteps4 = abs(_targetPos4 - _currentPos4);

  int maxSteps = max(max(targetSteps1, targetSteps2), max(targetSteps3, targetSteps4));
  float totalMS = _maxMsPerStep*maxSteps;
  
  _msPerStep1 = totalMS/targetSteps1;
  _msPerStep2 = totalMS/targetSteps2;
  _msPerStep3 = totalMS/targetSteps3;
  _msPerStep4 = totalMS/targetSteps4;
  
}

MiraStepper steppers;

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

  pos_1 = pos_1 + diag_1;
  pos_2 = pos_2 + diag_2;

  Serial.print(theta);
  Serial.print("   ");
  Serial.println(diag_1);
  
  // RF, RB, LF, LB
  steppers.setTargets(pos_1, pos_2, pos_2, pos_1);
  steppers.runToTargets();
  delay(1000);
  
  
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
