#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>

// Memory checking function
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

// int MOTOR_PIN_1 = 1;
// int MOTOR_PIN_2 = 9;
// int MOTOR_PIN_3 = 10;
// int MOTOR_PIN_4 = 13;
int MOTOR_PIN_1 = 3;
int MOTOR_PIN_2 = 5;
int MOTOR_PIN_3 = 6;
int MOTOR_PIN_4 = 9;

//FIX BASED ON TUNING
// Default, idle (balanced) speeds
int motor_power_1 = 1300;
int motor_power_2 = 1300;
int motor_power_3 = 1300;
int motor_power_4 = 1300;

// Used if lost packet or signal
int last_power1 = 1300;
int last_power2 = 1300;
int last_power3 = 1300;
int last_power4 = 1300;
int isOn = 0;


/*
 Motor Configuration:
 2 3
 1 4
 CCW CW
 CW  CCW

 Motor 1: Rear left CW
 Motor 2: Front left CCW
 Motor 3: Front right CW
 Motor 4: Rear right CCW
 */

double thrustTrim = 0.2;
double forwardMovementTrim = 0.2;
double sideMovementTrim = 0.2;
double rotationTrim = 0.2;


void setup() {
  Serial.begin(9600);
  motor1.attach(MOTOR_PIN_1);
  motor1.writeMicroseconds(1000);
  motor2.attach(MOTOR_PIN_2);
  motor2.writeMicroseconds(1000);
  motor3.attach(MOTOR_PIN_3);
  motor3.writeMicroseconds(1000);
  motor4.attach(MOTOR_PIN_4);
  motor4.writeMicroseconds(1000);

  while (!Serial);
  Serial.println("LoRa Receiver");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting Lora failed!");
    while (1);
  }

  LoRa.receive();

  delay(15000);
}

void loop() {
  Serial.print("Free memory: ");
  Serial.println(freeMemory());
  // int throttle = (int) LoRa.read();
  // int trimToggle = (int) LoRa.read();
  // int switch2 = (int) LoRa.read();
  // int x1 = (int) LoRa.read();
  // int y1 = (int) LoRa.read();
  // int x2 = (int) LoRa.read();
  // int y2 = (int) LoRa.read();
  int throttle = -1;
  int trimToggle = -1;
  int power = -1;
  int x1 = -1;
  int y1 = -1;
  int x2 = -1;
  int y2 = -1;
  // throttle = (int) LoRa.read();
  // trimToggle = (int) LoRa.read();
  // switch2 = (int) LoRa.read();
  // x1 = (int) LoRa.read();
  // y1 = (int) LoRa.read();
  // x2 = (int) LoRa.read();
  // y2 = (int) LoRa.read();
    
  // try to parse packet (extra packets?)
  int packetSize = LoRa.parsePacket();
  // Serial.print("packet size: ");
  // Serial.println(packetSize);
  if (packetSize) {
    // read packet
    Serial.print("Received packet '");

    // read packet
    // while (int x = LoRa.available() > 0) {
    //   // Serial.println((char)LoRa.read());
    //   Serial.println(x);
      
      // String buffer = "";
      // char c = (char) LoRa.read();
      // while (c != '_'){
      //   buffer += c;
      //   c = (char) LoRa.read();
      // }
      // Serial.print("String: ");
      // Serial.println(buffer);
      Serial.println("-");
      Serial.println(freeMemory());
      throttle =  readToInt();
      Serial.println(freeMemory());
      trimToggle = readToInt();
      Serial.println(freeMemory());
      power = readToInt();
      Serial.println(freeMemory());
      x1 = readToInt();
      Serial.println(freeMemory());
      y1 = readToInt(); // 1
      Serial.println(freeMemory());
      x2 = readToInt();
      Serial.println(freeMemory());
      y2 = readToInt(); // 1,1
      Serial.println(freeMemory());
      Serial.println("-");

    // }
    Serial.println("end");
    // while (LoRa.available()) {

      // throttle = (int) LoRa.read();
      // trimToggle = (int) LoRa.read();
      // switch2 = (int) LoRa.read();
      // x1 = (int) LoRa.read();
      // y1 = (int) LoRa.read();
      // x2 = (int) LoRa.read();
      // y2 = (int) LoRa.read();
    // }
  }
  Serial.print("Throttle: ");
  Serial.println(throttle);
  Serial.print("Trim Toggle: ");
  Serial.println(trimToggle);
  Serial.print("Power: ");
  Serial.println(power);
  Serial.print("x1: ");
  Serial.println(x1);
  Serial.print("y1: ");
  Serial.println(y1);
  Serial.print("x2: ");
  Serial.println(x2);
  Serial.print("y2: ");
  Serial.println(y2);

  int receivedRssi = LoRa.packetRssi();

  // If not -1, then packet was received
  if (throttle == -1 || trimToggle == -1 || power == -1 || x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1){ 
    Serial.println("No transmitted data received");
    // motor1.writeMicroseconds(last_power1);
    // motor2.writeMicroseconds(last_power2);
    // motor3.writeMicroseconds(last_power3);
    // motor4.writeMicroseconds(last_power4);
    return; 
  }

  // If less than 0, then corrupted transmission - can be combined with above, but for now is explicit statement for testing purposes
  if (throttle <= -1 || trimToggle <= -1 || power <= -1 || x1 <= -1 || y1 <= -1 || x2 <= -1 || y2 <= -1){ 
    Serial.println("Corrupted data received, returning.");
    return; 
  }

  // Check if power switched on
  if (power != -1){
    isOn = power;
  }
  if (isOn == 0){
    Serial.println("Power switch not enabled on transmitter.");
    motor1.writeMicroseconds(1000);
    motor2.writeMicroseconds(1000);
    motor3.writeMicroseconds(1000);
    motor4.writeMicroseconds(1000);
    return;
  }

  int power1 = motor_power_1;
  int power2 = motor_power_2;
  int power3 = motor_power_3;
  int power4 = motor_power_4;
  // If not trimming 
  if (trimToggle == 0){
    //vertical ascent management
    power1 = motor_power_1 + (y1 - 511) * thrustTrim * throttle / 1023;
    power2 = motor_power_2 + (y1 - 511) * thrustTrim * throttle / 1023;
    power3 = motor_power_3 + (y1 - 511) * thrustTrim * throttle / 1023;
    power4 = motor_power_4 + (y1 - 511) * thrustTrim * throttle / 1023;

    //forward/backwards movement
    power1 += (y2 - 511) * forwardMovementTrim * throttle / 1023;
    power2 -= (y2 - 511) * forwardMovementTrim * throttle / 1023;
    power3 -= (y2 - 511) * forwardMovementTrim * throttle / 1023;
    power4 += (y2 - 511) * forwardMovementTrim * throttle / 1023;

    //left/right movement
    power1 += (x2 - 511) * sideMovementTrim * throttle / 1023;
    power2 += (x2 - 511) * sideMovementTrim * throttle / 1023;
    power3 -= (x2 - 511) * sideMovementTrim * throttle / 1023;
    power4 -= (x2 - 511) * sideMovementTrim * throttle / 1023;

    //rotation 
    power1 -= (x1 - 511) * rotationTrim * throttle / 1023;
    power2 += (x1 - 511) * rotationTrim * throttle / 1023;
    power3 -= (x1 - 511) * rotationTrim * throttle / 1023;
    power4 += (x1 - 511) * rotationTrim * throttle / 1023;
  }
  // Trimming mode
  else if (trimToggle == 1){
    // Trim vertical ascent
    if (y1 > 511){ // up
      motor_power_1 += 10;
      motor_power_2 += 10;
      motor_power_3 += 10;
      motor_power_4 += 10;
    } 
    else if (y1 < 511){ // down
      motor_power_1 -= 10;
      motor_power_2 -= 10;
      motor_power_3 -= 10;
      motor_power_4 -= 10;
    }
    // Trim forward/backwards movement
    if (y2 > 511){ // forward
      motor_power_1 += 10;
      motor_power_2 -= 10;
      motor_power_3 -= 10;
      motor_power_4 += 10;
    } 
    else if (y2 < 511){ // backward
      motor_power_1 -= 10;
      motor_power_2 += 10;
      motor_power_3 += 10;
      motor_power_4 -= 10;
    }
    // Trim left/right movement
    if (x2 > 511){ // right
      motor_power_1 += 10;
      motor_power_2 += 10;
      motor_power_3 -= 10;
      motor_power_4 -= 10;
    } 
    else if (x2 < 511){ // left
      motor_power_1 -= 10;
      motor_power_2 -= 10;
      motor_power_3 += 10;
      motor_power_4 += 10;
    }
    // Trim rotation
    if (x1 > 511){ // CW
      motor_power_1 -= 10;
      motor_power_2 += 10;
      motor_power_3 -= 10;
      motor_power_4 += 10;
    } 
    else if (x2 < 511){ // CCW
      motor_power_1 += 10;
      motor_power_2 -= 10;
      motor_power_3 += 10;
      motor_power_4 -= 10;
    }

    
  }

  last_power1 = power1;
  last_power2 = power2;
  last_power3 = power3;
  last_power4 = power4;
  
  motor1.writeMicroseconds(power1);
  motor2.writeMicroseconds(power2);
  motor3.writeMicroseconds(power3);
  motor4.writeMicroseconds(power4);
  Serial.print("Motor powers: ");
  Serial.print(power1);
  Serial.print(" ");
  Serial.print(power2);
  Serial.print(" ");
  Serial.print(power3);
  Serial.print(" ");
  Serial.print(power4);
  Serial.println();
  
  
  // // Return detected received data
  // LoRa.beginPacket();
  // LoRa.write(throttle);
  // // LoRa.write(motorBitmask);
  // // LoRa.write(motor_power_1);
  // // LoRa.write(motor_power_2);
  // // LoRa.write(motor_power_3);
  // // LoRa.write(motor_power_4);
  // LoRa.write(power1);
  // LoRa.write(power2);
  // LoRa.write(power3);
  // LoRa.write(power4);
  // LoRa.write(receivedRssi);
  // LoRa.endPacket();
  
  // Serial.println("Packet sent");
}

// Negative if failed reading: invalid packet
int readToInt(){
  Serial.print("read:");
  String buffer = "";
  Serial.print("-");
  char c = (char) LoRa.read();
  Serial.print("-");
  Serial.print("\'");
  Serial.print((int)c);
  Serial.print("\'");
  while (c != '_' && c >= 0){
    Serial.print("1");
    buffer += c;
    Serial.print("2");
    c = (char) LoRa.read();
    Serial.print("3");
    Serial.print("\'");
    Serial.print((int)c);
    Serial.print("\'");
  }
  Serial.println("-");
  if (c < 0){
    Serial.println("\n\nTESTING RESTART...");
    delay(5000);
    return c;
  }
  return buffer.toInt();
}
