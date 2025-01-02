#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>

Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

int MOTOR_PIN_1 = 1;
int MOTOR_PIN_2 = 9;
int MOTOR_PIN_3 = 10;
int MOTOR_PIN_4 = 13;

//FIX BASED ON TUNING
int motor_power_1 = 1500;
int motor_power_2 = 1500;
int motor_power_3 = 1500;
int motor_power_4 = 1500;

double thrustTrim = 0.2;
double movementTrim = 0.2;
double rotationTrim = 0.2;


void setup() {
  Serial.begin(9600);
  motor1.attach(MOTOR_PIN_1);
  motor1.writeMicroseconds(1500);
  motor2.attach(MOTOR_PIN_2);
  motor2.writeMicroseconds(1500);
  motor3.attach(MOTOR_PIN_3);
  motor3.writeMicroseconds(1500);
  motor4.attach(MOTOR_PIN_4);
  motor4.writeMicroseconds(1500);

  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting Lora failed!");
    while (1);
  }

//  LoRa.receive();

  delay(10000);
}

void loop() {
  int throttle = (int) LoRa.read();
  int switch1 = (int) LoRa.read();
  int switch2 = (int) LoRa.read();
  int x1 = (int) LoRa.read();
  int y1 = (int) LoRa.read();
  int x2 = (int) LoRa.read();
  int y2 = (int) LoRa.read();
  
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      throttle = (int) LoRa.read();
      switch1 = (int) LoRa.read();
      switch2 = (int) LoRa.read();
      x1 = (int) LoRa.read();
      y1 = (int) LoRa.read();
      x2 = (int) LoRa.read();
      y2 = (int) LoRa.read();
    }
  }

  int receivedRssi = LoRa.packetRssi();

  //vertical ascent management
  power1 = motor_power_1 + (y1 - 512) * thrustTrim * throttle / 1023;
  power2 = motor_power_2 + (y1 - 512) * thrustTrim * throttle / 1023;
  power3 = motor_power_3 + (y1 - 512) * thrustTrim * throttle / 1023;
  power4 = motor_power_4 + (y1 - 512) * thrustTrim * throttle / 1023;

  //forward/backwards movement
  power1 += (y2 - 512) * movementTrim * throttle / 1023;
  power2 += (y2 - 512) * movementTrim * throttle / 1023;
  power3 -= (y2 - 512) * movementTrim * throttle / 1023;
  power4 -= (y2 - 512) * movementTrim * throttle / 1023;

  //left/right movement
  power1 += (x2 - 512) * movementTrim * throttle / 1023;
  power2 -= (x2 - 512) * movementTrim * throttle / 1023;
  power3 += (x2 - 512) * movementTrim * throttle / 1023;
  power4 -= (x2 - 512) * movementTrim * throttle / 1023;

  //rotation 
  power1 -= (x1 - 512) * rotationTrim * throttle / 1023;
  power2 += (x1 - 512) * rotationTrim * throttle / 1023;
  power3 += (x1 - 512) * rotationTrim * throttle / 1023;
  power4 -= (x1 - 512) * rotationTrim * throttle / 1023;
  

  motor1.writeMicroseconds(power1);
  motor2.writeMicroseconds(power2);
  motor3.writeMicroseconds(power3);
  motor4.writeMicroseconds(power4);
  

  LoRa.beginPacket();
  LoRa.write(throttleTrim);
  LoRa.write(motorBitmask);
  LoRa.write(motor_power_1);
  LoRa.write(motor_power_2);
  LoRa.write(motor_power_3);
  LoRa.write(motor_power_4);
  LoRa.write(receivedRssi);
  LoRa.endPacket();
  
}
