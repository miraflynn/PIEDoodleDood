#include <SPI.h>
#include <SD.h>
File pathFile;
char instruction[] = "UP";
char xstr[] = "000";
char ystr[] = "000";

void drive(int coordx, int coordy) {
  Serial.print("Driving to (");
  Serial.print(coordx);
  Serial.print(",");
  Serial.print(coordy);
  Serial.println(")");
}
void movePen(char dir) {
  if(dir == 'U'){
    Serial.println("Lifting Pen");
  } else if(dir == 'D') {
    Serial.println("Lowering Pen");
  } else {
    Serial.println("Well this is awkward... invalid pen movement!");
    Serial.println(dir);
  }
}

void readLine(){
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
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
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
    // read from the file until there's nothing else in it:
//    while (pathFile.available()) {
//      Serial.write(pathFile.read());
//    }
    // close the file:
//    pathFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening path.txt");
  }
}


void loop() {
  // For deciding what to do in this instruction, we assume that the instructions are all AA,BBB,CCC format
  // AA is the instruction (GO, UP, or DN), BBB is the x coordinate or filler, and CCC is the y coordinate or filler.
  // Because each instruction has a unique first letter, I only check that.
  readLine();
//  Serial.print(instruction);
//  Serial.print(xstr);
//  Serial.println(ystr);
  if(instruction[0] == 'G'){
    drive(atoi(xstr), atoi(ystr));
  } else if (instruction[0] == 'U' || instruction[0] == 'D'){
    movePen(instruction[0]);
  } else {
    Serial.println("INVALID INSTRUCTION!");
    Serial.println(instruction);
  }
}
