#include <Wire.h>
#include <MPU6050.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Motor and MPU6050 Pins
#define PWMA 14
#define AIN1 27
#define AIN2 26
#define PWMB 25
#define BIN1 33
#define BIN2 32
#define STBY 12

#define SDA 21
#define SCL 22

#define PWM_FREQ 5000
#define PWM_RES 8

#define LED_BUILTIN 2

MPU6050 mpu;

// PID variables
float Kp = 15.0;
float Ki = 0.0;
float Kd = 1.5;
float setpoint = -3.5;

float input_angle = 0;
float output = 0;
float error = 0;
float last_error = 0;
float integral = 0;
float derivative = 0;

const float alpha = 0.98f;
float angle = 0;
float filtered_angle = 0;

float gyro_offset_y = 0;
const int calib_samples = 1000;

unsigned long lastSensorTime = 0;
unsigned long lastPIDTime = 0;

char btCommand = 'S';
int accelerationStep = 5;
bool ledState = false;
unsigned long ledPrevMillis = 0;

// ------------------------- SETUP ---------------------

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BalancingBot");

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  enableMotors();

  ledcSetup(0, PWM_FREQ, PWM_RES);
  ledcSetup(1, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWMA, 0);
  ledcAttachPin(PWMB, 1);

  Wire.begin(SDA, SCL);
  mpu.initialize();
  if (!mpu.testConnection()) {
    while (true) { delay(100); }
  }

  calibrateGyro();

  lastSensorTime = millis();
  lastPIDTime = millis();
}

// ------------------------- MAIN LOOP ---------------------

void loop() {
  unsigned long now = millis();

  if (now - lastSensorTime >= 5) {
    sensorUpdate();
    lastSensorTime = now;
  }

  if (now - lastPIDTime >= 20) {
    pidCompute();
    lastPIDTime = now;
  }

  motorControl();
  ledBlink();
  bluetoothHandler();
}

// ------------------------- SENSORS ---------------------

void sensorUpdate() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accAngle = atan2((float)ay, (float)az) * 180.0 / PI;

  float dt = 0.005f;
  float gyroRate = ((float)gx - gyro_offset_y) / 131.0f;

  angle = alpha * (angle + gyroRate * dt) + (1.0f - alpha) * accAngle;
  filtered_angle = (filtered_angle * 0.98f) + (angle * 0.02f);

  input_angle = filtered_angle;
}

void calibrateGyro() {
  long sum = 0;
  int16_t ax, ay, az, gx, gy, gz;
  for (int i = 0; i < calib_samples; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sum += gy;
    delay(3);
  }
  gyro_offset_y = sum / (float)calib_samples;
}

// ------------------------- PID ---------------------

void pidCompute() {
  float dt = 0.02f;

  error = setpoint - input_angle;
  integral += error * dt;
  integral = constrain(integral, -150.0f, 150.0f);
  derivative = (error - last_error) / dt;

  output = (Kp * error) + (Ki * integral) + (Kd * derivative);

  last_error = error;

  output = constrain(output, -220, 220);

  if (abs(output) < 20) output = 0; // dead zone
}

// ------------------------- MOTORS ---------------------

void motorControl() {
  static int movePWM = 0;

  int targetPWM = 0;
  switch (btCommand) {
    case 'F': targetPWM = 60; break;
    case 'B': targetPWM = -60; break;
    default: targetPWM = 0; break;
  }

  if (movePWM < targetPWM) movePWM += accelerationStep;
  if (movePWM > targetPWM) movePWM -= accelerationStep;

  int leftOffset = movePWM;
  int rightOffset = movePWM;

  if (btCommand == 'L') { leftOffset = -40; rightOffset = 40; }
  if (btCommand == 'R') { leftOffset = 40; rightOffset = -40; }

  int leftSpeed = constrain(output + leftOffset, -180, 180);
  int rightSpeed = constrain(output + rightOffset, -180, 180);

  if (abs(input_angle - setpoint) > 18) {
    leftSpeed = 0;
    rightSpeed = 0;
  }

  setMotor(leftSpeed, rightSpeed);
}

void enableMotors() { digitalWrite(STBY, HIGH); }

void setMotor(int left, int right) {
  digitalWrite(AIN1, left >= 0);
  digitalWrite(AIN2, !(left >= 0));
  ledcWrite(0, abs(left));

  digitalWrite(BIN1, !(right >= 0));
  digitalWrite(BIN2, (right >= 0));
  ledcWrite(1, abs(right));
}

// ------------------------- LED ---------------------

void ledBlink() {
  unsigned long now = millis();
  unsigned long interval = (abs(error) < 2) ? 900 : 200;

  if (now - ledPrevMillis >= interval) {
    ledPrevMillis = now;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}

// ------------------------- BLUETOOTH ---------------------

void bluetoothHandler() {
  static String buff = "";

  while (SerialBT.available()) {
    char c = SerialBT.read();

    if (c == '\n' || c == '\r') {
      if (buff.length() > 0) processCommand(buff);
      buff = "";
    } else {
      buff += c;
    }
  }
}

void processCommand(String cmd) {
  cmd.toUpperCase();

  if (cmd == "F" || cmd == "B" || cmd == "L" || cmd == "R" || cmd == "S") {
    btCommand = cmd.charAt(0);
    SerialBT.println("ACK: CMD=" + cmd);
    return;
  }

  if (cmd.startsWith("SET ")) {
    int s1 = cmd.indexOf(' ');
    int s2 = cmd.indexOf(' ', s1 + 1);

    String var = cmd.substring(s1 + 1, s2);
    float val = cmd.substring(s2 + 1).toFloat();

    if (var == "KP") Kp = val;
    else if (var == "KI") Ki = val;
    else if (var == "KD") Kd = val;
    else if (var == "SP") setpoint = val;
    else { SerialBT.println("ERR"); return; }

    SerialBT.println("ACK: " + var + "=" + String(val));
  }
}
