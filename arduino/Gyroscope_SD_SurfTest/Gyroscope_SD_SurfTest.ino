// Gyro
#include <Wire.h>
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23

// SD
#include <SdFat.h>
#define error(s) sd.errorHalt_P(PSTR(s))
const uint8_t chipSelect = SS;
SdFat sd;
ArduinoOutStream cout(Serial);

int Addr = 105;                 // I2C address of gyro
int x, y, z;

void setup(){
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  
  // Gyro
  Wire.begin();
  writeI2C(CTRL_REG1, 0x1F);    // Turn on all axes, disable power down
  writeI2C(CTRL_REG3, 0x08);    // Enable control ready signal
  writeI2C(CTRL_REG4, 0x80);    // Set scale (500 deg/sec)

    // SD
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) sd.initErrorHalt();
  char name[] = "APPEND.TXT";
  ofstream sdout(name, ios::out | ios::app);
  
  long currTime = millis();
  long targetTime = currTime + 3000;
  
  while(millis() < targetTime){
    digitalWrite(13, HIGH); 
  }
  
  digitalWrite(13, LOW);
  
  currTime = millis();
  targetTime = currTime + 15000;
  
  while(millis() < targetTime) {
    Serial.println("RECORDING");
    digitalWrite(12, HIGH); 
    getGyroValues();              // Get new values
    // In following Dividing by 114 reduces noise
    sdout << millis() << "," << x << "," << y << "," << z << endl; 
    delay(100);
  } 
  sdout.close(); 
  Serial.println("DONE");
  digitalWrite(12, LOW);
  
  currTime = millis();
}

void loop(){

}

void getGyroValues () {
  byte MSB, LSB;

  MSB = readI2C(0x29);
  LSB = readI2C(0x28);
  x = ((MSB << 8) | LSB);

  MSB = readI2C(0x2B);
  LSB = readI2C(0x2A);
  y = ((MSB << 8) | LSB);

  MSB = readI2C(0x2D);
  LSB = readI2C(0x2C);
  z = ((MSB << 8) | LSB);
}

int readI2C (byte regAddr) {
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);                // Register address to read
  Wire.endTransmission();             // Terminate request
  Wire.requestFrom(Addr, 1);          // Read a byte
  while(!Wire.available()) { 
  };       // Wait for receipt
  return(Wire.read());                // Get result
}

void writeI2C (byte regAddr, byte val) {
  Wire.beginTransmission(Addr);
  Wire.write(regAddr);
  Wire.write(val);
  Wire.endTransmission();
}

