#include <BMI160Gen.h>
#include <Wire.h>

const int i2c_addr = 0x69;
const int sda_pin = 21;
const int scl_pin = 22;

void setup() {
  Serial.begin(115200); 
  while (!Serial);
  Wire.begin(sda_pin, scl_pin);
  
  BMI160.begin(BMI160GenClass::I2C_MODE, i2c_addr);
}

void loop() {
  // ตัวแปรเก็บค่าดิบ (Raw Data) เป็นจำนวนเต็ม (int)
  int gxRaw, gyRaw, gzRaw, axRaw, ayRaw, azRaw;

  // อ่านค่าจาก Sensor
  BMI160.readGyro(gxRaw, gyRaw, gzRaw);
  BMI160.readAccelerometer(axRaw, ayRaw, azRaw);


  // --- ส่งเป็น JSON (ค่าดิบ) ---
  Serial.print("{\"ax\":"); Serial.print(axRaw); 
  Serial.print(", \"ay\":"); Serial.print(ayRaw);
  Serial.print(", \"az\":"); Serial.print(azRaw);
  Serial.print(", \"gx\":"); Serial.print(gxRaw);
  Serial.print(", \"gy\":"); Serial.print(gyRaw);
  Serial.print(", \"gz\":"); Serial.print(gzRaw);
  Serial.println("}");

  delay(50); 
}
