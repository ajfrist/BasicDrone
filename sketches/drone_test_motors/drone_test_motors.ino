#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>

Servo servo1, servo2, servo3, servo4;

int MOTOR_PIN_1 = 3;
int MOTOR_PIN_2 = 5;
int MOTOR_PIN_3 = 6;
int MOTOR_PIN_4 = 9;

int motor_power = 1000;


void setup() {
  // Init
  Serial.begin(9600);
  // Init motors, set to neutral
  servo1.attach(MOTOR_PIN_1);
  servo2.attach(MOTOR_PIN_2);
  servo3.attach(MOTOR_PIN_3);
  servo4.attach(MOTOR_PIN_4);
  servo1.writeMicroseconds(motor_power);
  servo2.writeMicroseconds(motor_power);
  servo3.writeMicroseconds(motor_power);
  servo4.writeMicroseconds(motor_power);

  while (!Serial);
  Serial.println("LoRa Receiver");
  // if (!LoRa.begin(915E6)) {
  //   Serial.println("Starting Lora failed!");
  //   while (1);
  // }

//  LoRa.receive();

  delay(15000);
}

void loop() {
  motor_power += 1;
  Serial.print("Loop. power: ");
  Serial.println(motor_power);
  
  // Set motor powers
  servo1.writeMicroseconds(motor_power+500);
  servo2.writeMicroseconds(motor_power+500);
  servo3.writeMicroseconds(motor_power+500);
  servo4.writeMicroseconds(motor_power+500);
  
  Serial.println("delaying")
  delay(500000);
}
