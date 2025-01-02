#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>

Servo servo1, servo2, servo3, servo4;

int MOTOR_PIN_1 = 1;
int MOTOR_PIN_2 = 9;
int MOTOR_PIN_3 = 10;
int MOTOR_PIN_4 = 13;

int motor_power_1 = 1500;
int motor_power_2 = 1500;
int motor_power_3 = 1500;
int motor_power_4 = 1500;


void setup() {
  // Init
  Serial.begin(9600);
  // Init motors, set to neutral
  servo1.attach(MOTOR_PIN_1);
  servo1.writeMicroseconds(1500);
  servo2.attach(MOTOR_PIN_2);
  servo2.writeMicroseconds(1500);
  servo3.attach(MOTOR_PIN_3);
  servo3.writeMicroseconds(1500);
  servo4.attach(MOTOR_PIN_4);
  servo4.writeMicroseconds(1500);

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
  int throttleTrim = 0;
  int motorBitmask = 0b0000;

  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      // Read whether each motor is active for trim adjustment
      int motorActive1 = (int) LoRa.read();
      int motorActive2 = (int) LoRa.read();
      int motorActive3 = (int) LoRa.read();
      int motorActive4 = (int) LoRa.read();

      // Edit bitmask and adjust respective motor powers
      if (motorActive1 > -100){
        throttleTrim = motorActive1;
        motorBitmask = 0b0001;
        motor_power_1 += motorActive1;
      } 
      if (motorActive2 > -100){
        throttleTrim = motorActive2;
        motorBitmask = motorBitmask | 0b0010;
        motor_power_2 += motorActive2;
      } 
      if (motorActive3 > -100){
        throttleTrim = motorActive3;
        motorBitmask = motorBitmask | 0b0100;
        motor_power_3 += motorActive3;
      } 
      if (motorActive4 > -100){
        throttleTrim = motorActive4;
        motorBitmask = motorBitmask | 0b1000;
        motor_power_4 += motorActive4;
      } 

    }
  }

  // note signal strength
  int receivedRssi = LoRa.packetRssi();

  // Set motor powers
  servo1.writeMicroseconds(motor_power_1);
  servo2.writeMicroseconds(motor_power_2);
  servo3.writeMicroseconds(motor_power_3);
  servo4.writeMicroseconds(motor_power_4);
  
  // Return signal confirming all received data
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
