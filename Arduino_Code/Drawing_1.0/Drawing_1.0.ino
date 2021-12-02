#include <Adafruit_MotorShield.h>
#include <Arduino_NineAxesMotion.h>

NineAxesMotion IMU;
Adafruit_MotorShield AFMS;
Adafruit_StepperMotor *rfStepper, *lfStepper, *rbStepper, *lbStepper;

int length = 4;
int angles[4] = {0,0.75,0,1.25};
int lengths[4] = {20,40,20,40};

int index = 0;
int x0,y0, x, y; // initial IMU readings, current coordinates

void setup() {
  Serial.begin(115200);
  I2C.begin();

  IMU.initSensor();
  IMU.setOperationMode(OPERATION_MODE_NDOF);
  IMU.setUpdateMode(MANUAL);

  AFMS = Adafruit_MotorShield();
  
  rfStepper = AFMS.getStepper(200,1);
  lfStepper = AFMS.getStepper(200,2);
  rbStepper = AFMS.getStepper(200,3);
  lbStepper = AFMS.getStepper(200,4);

  // take initial IMU reading ? 

  x = 0;
  y = 0;
}

void loop() {
  next_x = coordinates(index)
  next_y = 
  index = index+1;

  if(index >= 4){
   index = 0;
  }
  getDirection();
  
}

int getCurrentX(){ return 1;}
int getCurrentY(){ return 1;}

int[] getDirection(currentX,currentY,nextX,nextY){
  int angle = atan2((nextY-currentY), (nextX-currentX));
  int dist = sqrt((nextX-currentX)^2 + (nextY-currentY)^2);
  return [angle,dist]
}

void mecanumDrive(int angle){
  
  drive1 = sin(angle - 1/4 * PI);
  drive2 = sin(angle + 1/4 * PI);

  // Front right and Left back should be drive 1
  // Front left and Right back should be drive 2
  
}

void penUp(){}
void penDown(){}
