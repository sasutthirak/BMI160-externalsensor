#include <BMI160Gen.h>
#include <Wire.h>

const int i2c_addr = 0x69;
const int sda_pin = 21;
const int scl_pin = 22;

void setup() {
  Serial.begin(115200); // ต้องตรงกับ Python
  while (!Serial);
  Wire.begin(sda_pin, scl_pin);
  
  // เริ่มต้น Sensor
  BMI160.begin(BMI160GenClass::I2C_MODE, i2c_addr);
}

void loop() {
  int gxRaw, gyRaw, gzRaw, axRaw, ayRaw, azRaw;

  // อ่านค่า
  BMI160.readGyro(gxRaw, gyRaw, gzRaw);
  BMI160.readAccelerometer(axRaw, ayRaw, azRaw);

  // คำนวณ (Manual Calculation)
  float ax = axRaw / 16384.0;
  float ay = ayRaw / 16384.0;
  float az = azRaw / 16384.0;
  float gx = gxRaw / 131.0;
  float gy = gyRaw / 131.0;
  float gz = gzRaw / 131.0;

  // --- ส่งเป็น JSON (สำคัญมาก!) ---
  Serial.print("{\"ax\":"); Serial.print(ax, 5);  // <--- เติม , 5
  Serial.print(", \"ay\":"); Serial.print(ay, 5); // <--- เติม , 5
  Serial.print(", \"az\":"); Serial.print(az, 5); // <--- เติม , 5
  Serial.print(", \"gx\":"); Serial.print(gx, 5); // <--- เติม , 5
  Serial.print(", \"gy\":"); Serial.print(gy, 5); // <--- เติม , 5
  Serial.print(", \"gz\":"); Serial.print(gz, 5); // <--- เติม , 5
  Serial.println("}");

  delay(50); 
}