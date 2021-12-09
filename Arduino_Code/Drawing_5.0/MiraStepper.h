#ifndef MIRASTEPPER_H
#define MIRASTEPPER_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MotorShield.h>

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

MiraStepper::MiraStepper(Adafruit_StepperMotor motorRF, Adafruit_StepperMotor motorRB, Adafruit_StepperMotor motorLF, Adafruit_StepperMotor motorLB){
  }

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
        motorLF->onestep(FORWARD, DOUBLE);
        _currentPos3 += 1;
        steps3 += 1;
      } else {
        motorLF->onestep(BACKWARD, DOUBLE);
        _currentPos3 -= 1;
        steps3 += 1;
      }
    }

    if((thisLoopTime - startTime) > _msPerStep4*steps4 && steps4 != abs(targetSteps4)){
      if(targetSteps4 > 0){
        motorLB->onestep(FORWARD, DOUBLE);
        _currentPos4 += 1;
        steps4 += 1;
      } else {
        motorLB->onestep(BACKWARD, DOUBLE);
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


};

#endif
