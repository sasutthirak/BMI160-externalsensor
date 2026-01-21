#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu; 

// กำหนดขา I2C
const int sda_pin = 21;
const int scl_pin = 22;

void setup() {
  Serial.begin(115200);
  while (!Serial); 


  Wire.begin(sda_pin, scl_pin);
  Wire.setClock(400000);
  delay(100);            

  Serial.println("Initializing MPU6050...");
  mpu.initialize();


  uint8_t devId = mpu.getDeviceID();
  Serial.print("Device ID read: 0x"); 
  Serial.println(devId, HEX); 

  if (devId != 0x68) {
    Serial.println("Warning: Connection check failed (Wait for reset...)");
  } else {
    Serial.println("MPU6050 connection successful!");
  }
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // --- ส่งเป็น JSON (Raw Data) ---
  Serial.print("{\"ax\":"); Serial.print(ax); 
  Serial.print(", \"ay\":"); Serial.print(ay);
  Serial.print(", \"az\":"); Serial.print(az);
  Serial.print(", \"gx\":"); Serial.print(gx);
  Serial.print(", \"gy\":"); Serial.print(gy);
  Serial.print(", \"gz\":"); Serial.print(gz);
  Serial.println("}");

  delay(50); 
}
