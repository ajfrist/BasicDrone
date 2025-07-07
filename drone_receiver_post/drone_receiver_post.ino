#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>

#define DEBUG 1  // Set to 1 to enable debug prints
#if DEBUG
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)    do {} while (0)
  #define DEBUG_PRINTLN(x)  do {} while (0)
#endif

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
  DEBUG_PRINTLN("LoRa Receiver");
  if (!LoRa.begin(915E6)) {
    DEBUG_PRINTLN("Starting Lora failed!");
    while (1);
  }

  LoRa.receive();

  delay(15000);
}

void loop() {
  DEBUG_PRINT("Free memory: ");
  DEBUG_PRINTLN(freeMemory());
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
  // DEBUG_PRINT("packet size: ");
  // DEBUG_PRINTLN(packetSize);
  if (packetSize) {
    // read packet
    DEBUG_PRINT("Received packet '");

    // read packet
    // while (int x = LoRa.available() > 0) {
    //   // DEBUG_PRINTLN((char)LoRa.read());
    //   DEBUG_PRINTLN(x);
      
      // String buffer = "";
      // char c = (char) LoRa.read();
      // while (c != '_'){
      //   buffer += c;
      //   c = (char) LoRa.read();
      // }
      // DEBUG_PRINT("String: ");
      // DEBUG_PRINTLN(buffer);
      DEBUG_PRINTLN("-");
      DEBUG_PRINTLN(freeMemory());
      throttle =  readToInt();
      DEBUG_PRINTLN(freeMemory());
      trimToggle = readToInt();
      DEBUG_PRINTLN(freeMemory());
      power = readToInt();
      DEBUG_PRINTLN(freeMemory());
      x1 = readToInt();
      DEBUG_PRINTLN(freeMemory());
      y1 = readToInt(); // 1
      DEBUG_PRINTLN(freeMemory());
      x2 = readToInt();
      DEBUG_PRINTLN(freeMemory());
      y2 = readToInt(); // 1,1
      DEBUG_PRINTLN(freeMemory());
      DEBUG_PRINTLN("-");

    // }
    DEBUG_PRINTLN("end");
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
  DEBUG_PRINT("Throttle: ");
  DEBUG_PRINTLN(throttle);
  DEBUG_PRINT("Trim Toggle: ");
  DEBUG_PRINTLN(trimToggle);
  DEBUG_PRINT("Power: ");
  DEBUG_PRINTLN(power);
  DEBUG_PRINT("x1: ");
  DEBUG_PRINTLN(x1);
  DEBUG_PRINT("y1: ");
  DEBUG_PRINTLN(y1);
  DEBUG_PRINT("x2: ");
  DEBUG_PRINTLN(x2);
  DEBUG_PRINT("y2: ");
  DEBUG_PRINTLN(y2);

  int receivedRssi = LoRa.packetRssi();

  // If not -1, then packet was received
  if (throttle == -1 || trimToggle == -1 || power == -1 || x1 == -1 || y1 == -1 || x2 == -1 || y2 == -1){ 
    DEBUG_PRINTLN("No transmitted data received");
    // motor1.writeMicroseconds(last_power1);
    // motor2.writeMicroseconds(last_power2);
    // motor3.writeMicroseconds(last_power3);
    // motor4.writeMicroseconds(last_power4);
    return; 
  }

  // If less than 0, then corrupted transmission - can be combined with above, but for now is explicit statement for testing purposes
  if (throttle <= -1 || trimToggle <= -1 || power <= -1 || x1 <= -1 || y1 <= -1 || x2 <= -1 || y2 <= -1){ 
    DEBUG_PRINTLN("Corrupted data received, returning.");
    return; 
  }

  // Check if power switched on
  if (power != -1){
    isOn = power;
  }
  if (isOn == 0){
    DEBUG_PRINTLN("Power switch not enabled on transmitter.");
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
  DEBUG_PRINT("Motor powers: ");
  DEBUG_PRINT(power1);
  DEBUG_PRINT(" ");
  DEBUG_PRINT(power2);
  DEBUG_PRINT(" ");
  DEBUG_PRINT(power3);
  DEBUG_PRINT(" ");
  DEBUG_PRINT(power4);
  DEBUG_PRINTLN();
  #if DEBUG
  #else
  Serial.print("Motor powers: ");
  Serial.print(power1);
  Serial.print(" ");
  Serial.print(power2);
  Serial.print(" ");
  Serial.print(power3);
  Serial.print(" ");
  Serial.print(power4);
  Serial.println();
  #endif
  
  
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
  
  // DEBUG_PRINTLN("Packet sent");
}

// Negative if failed reading: invalid packet
int readToInt(){
  DEBUG_PRINT("read:");
  String buffer = "";
  DEBUG_PRINT("-");
  char c = (char) LoRa.read();
  DEBUG_PRINT("-");
  DEBUG_PRINT("\'");
  DEBUG_PRINT((int)c);
  DEBUG_PRINT("\'");
  while (c != '_' && c >= 0){
    DEBUG_PRINT("1");
    buffer += c;
    DEBUG_PRINT("2");
    c = (char) LoRa.read();
    DEBUG_PRINT("3");
    DEBUG_PRINT("\'");
    DEBUG_PRINT((int)c);
    DEBUG_PRINT("\'");
  }
  DEBUG_PRINTLN("-");
  if (c < 0){
    DEBUG_PRINTLN("\n\nTESTING RESTART...");
    //delay(5000);
    return c;
  }
  return buffer.toInt();
}
