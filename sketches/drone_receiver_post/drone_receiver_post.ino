#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>
// #include <Wire.h> // used for all I2C devices

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


double applyDeadzone(int value, int deadzone_radius = 5);

// const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
// int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
// int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
// int16_t temperature; // variables for temperature data
// char tmp_str[7]; // temporary variable used in convert function - used to output data to serial console
// char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
//   sprintf(tmp_str, "%6d", i);
//   return tmp_str;
// }

Servo motor1;
Servo motor2;
Servo motor3;
Servo motor4;

// int MOTOR_PIN_1 = 1;
// int MOTOR_PIN_2 = 9;
// int MOTOR_PIN_3 = 10;
// int MOTOR_PIN_4 = 13;
const int MOTOR_PIN_1 = 3;
const int MOTOR_PIN_2 = 5;
const int MOTOR_PIN_3 = 6;
const int MOTOR_PIN_4 = 9;


/**
Motor microsecond active value ranges (low/off -> high/max power): 
motor1: ~1100-1900
motor2: ~1100-1900
motor3: ~1100-1900
motor4: ~1100-1900

DEFAULT -> 1000 = OFF
*/

//FIX BASED ON TUNING
// Default, idle (balanced) speeds
const int MOTOR_POWER_1 = 1300;
const int MOTOR_POWER_2 = 1300;
const int MOTOR_POWER_3 = 1300;
const int MOTOR_POWER_4 = 1300;

// State trim settings
int motor_trim_1 = 0;
int motor_trim_2 = 0;
int motor_trim_3 = 0;
int motor_trim_4 = 0;

// Used if lost packet or signal
double last_power1 = MOTOR_POWER_1;
double last_power2 = MOTOR_POWER_2;
double last_power3 = MOTOR_POWER_3;
double last_power4 = MOTOR_POWER_4;
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

// Set max bounding for motor movements; keeps joystick values within ms domain
const double THRUST_SCALER = 0.5;
const double FORWARD_MOVEMENT_SCALER = 0.5;
const double SIDE_MOVEMENT_SCALER = 0.5;
const double ROTATION_SCALER = 0.5;


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

  // // Setup for GY-521 Gyro/Accelerometer
  // Wire.begin();
  // Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  // Wire.write(0x6B); // PWR_MGMT_1 register
  // Wire.write(0); // set to zero (wakes up the MPU-6050)
  // Wire.endTransmission(true);

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


  // // Retrieve GY-521 Gyro/Accelerometer info
  // Wire.beginTransmission(MPU_ADDR); // access I2C device at listed address
  // Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  // Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  // Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  // // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  // accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  // accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  // accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  // temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  // gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  // gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  // gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  
  // // print out data
  // Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
  // Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
  // Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));
  // // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  // Serial.print(" | temp = "); Serial.print(temperature/340.00+36.53);
  // Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
  // Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
  // Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
  // Serial.println();
  

    
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
  // If powered off, turn motors off (1000 us)
  if (isOn == 0){
    DEBUG_PRINTLN("Power switch not enabled on transmitter.");
    motor1.writeMicroseconds(1000);
    motor2.writeMicroseconds(1000);
    motor3.writeMicroseconds(1000);
    motor4.writeMicroseconds(1000);
    return;
  }

  // Calculcate current iteration motor power amounts
  double power1 = 0;
  double power2 = 0;
  double power3 = 0;
  double power4 = 0;

  // If not trimming 
  if (trimToggle == 0){
    //vertical ascent management
    power1 += (applyDeadzone(y1) - 511) * THRUST_SCALER * throttle / 1023;
    power2 += (applyDeadzone(y1) - 511) * THRUST_SCALER * throttle / 1023;
    power3 += (applyDeadzone(y1) - 511) * THRUST_SCALER * throttle / 1023;
    power4 += (applyDeadzone(y1) - 511) * THRUST_SCALER * throttle / 1023;

    //forward/backwards movement
    power1 += (applyDeadzone(y2) - 511) * FORWARD_MOVEMENT_SCALER * throttle / 1023;
    power2 -= (applyDeadzone(y2) - 511) * FORWARD_MOVEMENT_SCALER * throttle / 1023;
    power3 -= (applyDeadzone(y2) - 511) * FORWARD_MOVEMENT_SCALER * throttle / 1023;
    power4 += (applyDeadzone(y2) - 511) * FORWARD_MOVEMENT_SCALER * throttle / 1023;

    //left/right movement
    power1 += (applyDeadzone(x2) - 511) * SIDE_MOVEMENT_SCALER * throttle / 1023;
    power2 += (applyDeadzone(x2) - 511) * SIDE_MOVEMENT_SCALER * throttle / 1023;
    power3 -= (applyDeadzone(x2) - 511) * SIDE_MOVEMENT_SCALER * throttle / 1023;
    power4 -= (applyDeadzone(x2) - 511) * SIDE_MOVEMENT_SCALER * throttle / 1023;

    //rotation 
    power1 -= (applyDeadzone(x1) - 511) * ROTATION_SCALER * throttle / 1023;
    power2 += (applyDeadzone(x1) - 511) * ROTATION_SCALER * throttle / 1023;
    power3 -= (applyDeadzone(x1) - 511) * ROTATION_SCALER * throttle / 1023;
    power4 += (applyDeadzone(x1) - 511) * ROTATION_SCALER * throttle / 1023;
  }
  // Trimming mode
  else if (trimToggle == 1){
    // Trim vertical ascent
    if (y1 > 511){ // up
      motor_trim_1 += 10;
      motor_trim_2 += 10;
      motor_trim_3 += 10;
      motor_trim_4 += 10;
    } 
    else if (y1 < 511){ // down
      motor_trim_1 -= 10;
      motor_trim_2 -= 10;
      motor_trim_3 -= 10;
      motor_trim_4 -= 10;
    }
    // Trim forward/backwards movement
    if (y2 > 511){ // forward
      motor_trim_1 += 10;
      motor_trim_2 -= 10;
      motor_trim_3 -= 10;
      motor_trim_4 += 10;
    } 
    else if (y2 < 511){ // backward
      motor_trim_1 -= 10;
      motor_trim_2 += 10;
      motor_trim_3 += 10;
      motor_trim_4 -= 10;
    }
    // Trim left/right movement
    if (x2 > 511){ // right
      motor_trim_1 += 10;
      motor_trim_2 += 10;
      motor_trim_3 -= 10;
      motor_trim_4 -= 10;
    } 
    else if (x2 < 511){ // left
      motor_trim_1 -= 10;
      motor_trim_2 -= 10;
      motor_trim_3 += 10;
      motor_trim_4 += 10;
    }
    // Trim rotation
    if (x1 > 511){ // CW
      motor_trim_1 -= 10;
      motor_trim_2 += 10;
      motor_trim_3 -= 10;
      motor_trim_4 += 10;
    } 
    else if (x1 < 511){ // CCW
      motor_trim_1 += 10;
      motor_trim_2 -= 10;
      motor_trim_3 += 10;
      motor_trim_4 -= 10;
    }    
  }

  // Add idle speed to motor powers
  power1 += MOTOR_POWER_1 + motor_trim_1;
  power2 += MOTOR_POWER_2 + motor_trim_2;
  power3 += MOTOR_POWER_3 + motor_trim_3;
  power4 += MOTOR_POWER_4 + motor_trim_4;

  
  // Only write to motors if value has changed since last loop - shouldn't need to continuously update
  if (abs(last_power1-power1)<0.001){
    motor1.writeMicroseconds((int) power1);
  }
  if (abs(last_power2-power2)<0.001){
    motor2.writeMicroseconds((int) power2);
  }
  if (abs(last_power3-power3)<0.001){
    motor3.writeMicroseconds((int) power3);
  }
  if (abs(last_power4-power4)<0.001){
    motor4.writeMicroseconds((int) power4);
  }
  last_power1 = power1;
  last_power2 = power2;
  last_power3 = power3;
  last_power4 = power4;

  // Debug print motor powers to console
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
  // // LoRa.write(MOTOR_POWER_1);
  // // LoRa.write(MOTOR_POWER_2);
  // // LoRa.write(MOTOR_POWER_3);
  // // LoRa.write(MOTOR_POWER_4);
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

/*
 Applies cushion around neutral joystick to account for drift & slight unintentional movements.
 Neutralizes values in deadzone and scales remaining values within [0, 1023].
 @params:
 int value - joystick value to be mapped;
 int deadzone_radius - radius of effect of the joystick deadzone;
 */
double applyDeadzone(int value, int deadzone_radius){

  // Apply zone around center (511, 511)
  if (abs(value - 511) <= deadzone_radius){
    return 511;
  } else {
    if (value > 511){ // value > 511 + deadzone
      return (double) map(value, 511, 1023, 511+deadzone_radius, 1023);
    } else { // value < 511 - deadzone
      return (double) map(value, 0, 511, 0, 511-deadzone_radius);
    }
  }
}
