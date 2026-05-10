#include <Wire.h>

#define addressSlave1 0x18
#define addressSlave2 0x27

#define SDA 21
#define SCL 22

uint8_t error=0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA,SCL);

  i2cScanner();

}

void loop() {
  delay(200);

  uint8_t sensor= 'S';
  Wire.beginTransmission(addressSlave1);
  Wire.write(sensor);
  Serial.println(sensor);

  error= Wire.endTransmission(true);
  Serial.print(" endTransmission ");
  Serial.println(error);

delay(100);

  uint8_t bytesReceived = Wire.requestFrom( addressSlave1, 4);
  Serial.print("RequestFrom: ");
  Serial.println(bytesReceived);
  if (bytesReceived>0){
    uint8_t temp[10];
    for (int i=0;i<bytesReceived; i++){
      temp[i]=Wire.read();
      Serial.print("Byte: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(temp[i]);
    }
  } else {
    Serial.println("No data received");
  }

}

void i2cScanner(){
  byte error, address;
  int nDevices;
  Serial.println("Scanning.....");
  nDevices=0;
  for (address =1;address<127;address++){
    Wire.beginTransmission(address);
    error= Wire.endTransmission();
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address<16){
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error==4){
      Serial.print("Unknow error at address 0x");
      if (address<16){
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (nDevices==0){
    Serial.print("no I2C devices found\n");
  }
  else {
    Serial.print("Done\n");
  }
}
