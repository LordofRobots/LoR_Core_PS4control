/* LORD of ROBOTS - LoR_Core_PS4control - 202305222112
  Control inputs - LED Indication:
    PS4 - Rainbow LED
    none/Stop/standby - Red LED

  Drive configurations:
    Mecanum 
    Standard tank style

*/
#include <PS4Controller.h>
#include <ps4.h>
#include <ps4_int.h>
#include <Adafruit_NeoPixel.h>

// version control
String Version = "LoR Core Version: PS4 Control : 1.1.2";

// IO Interface Definitions
#define LED_DataPin 12
#define LED_COUNT 36
#define ControllerSelectPin 34
#define MotorEnablePin 13
#define channel1Pin 16
#define channel2Pin 17
#define channel3Pin 21
#define channel4Pin 22

// Motor Pin Definitions
#define motorPin_M1_A 14
#define motorPin_M1_B 5
#define motorPin_M2_A 26
#define motorPin_M2_B 18
#define motorPin_M3_A 23
#define motorPin_M3_B 19
#define motorPin_M4_A 15
#define motorPin_M4_B 33
#define motorPin_M5_A 25
#define motorPin_M5_B 27
#define motorPin_M6_A 4
#define motorPin_M6_B 32
const int motorPins_A[] = { motorPin_M1_A, motorPin_M2_A, motorPin_M3_A, motorPin_M4_A, motorPin_M5_A, motorPin_M6_A };
const int motorPins_B[] = { motorPin_M1_B, motorPin_M2_B, motorPin_M3_B, motorPin_M4_B, motorPin_M5_B, motorPin_M6_B };

// Motor PWM Configuration Definitions
const int Motor_M1_A = 0;
const int Motor_M1_B = 1;
const int Motor_M2_A = 2;
const int Motor_M2_B = 3;
const int Motor_M3_A = 4;
const int Motor_M3_B = 5;
const int Motor_M4_A = 6;
const int Motor_M4_B = 7;
const int Motor_M5_A = 8;
const int Motor_M5_B = 9;
const int Motor_M6_A = 10;
const int Motor_M6_B = 11;
const int MOTOR_PWM_Channel_A[] = { Motor_M1_A, Motor_M2_A, Motor_M3_A, Motor_M4_A, Motor_M5_A, Motor_M6_A };
const int MOTOR_PWM_Channel_B[] = { Motor_M1_B, Motor_M2_B, Motor_M3_B, Motor_M4_B, Motor_M5_B, Motor_M6_B };
const int PWM_FREQUENCY = 20000;
const int PWM_RESOLUTION = 8;

// Process joystick input and calculate motor speeds
bool MecanumDrive_Enabled = false;
const int DEAD_BAND = 20;
const float TURN_RATE = 1.5;
int Motor_FrontLeft_SetValue, Motor_FrontRight_SetValue, Motor_BackLeft_SetValue, Motor_BackRight_SetValue = 0;
void Motion_Control(int LY_Axis, int LX_Axis, int RX_Axis) {
  int FrontLeft_TargetValue, FrontRight_TargetValue, BackLeft_TargetValue, BackRight_TargetValue = 0;
  int ForwardBackward_Axis = LY_Axis;
  int StrafeLeftRight_Axis = LX_Axis;
  int TurnLeftRight_Axis = -RX_Axis;

  //Set deadband
  if (abs(ForwardBackward_Axis) < DEAD_BAND) ForwardBackward_Axis = 0;
  if (abs(StrafeLeftRight_Axis) < DEAD_BAND) StrafeLeftRight_Axis = 0;
  if (abs(TurnLeftRight_Axis) < DEAD_BAND) TurnLeftRight_Axis = 0;

  //Calculate strafe values
  FrontLeft_TargetValue = -ForwardBackward_Axis + (StrafeLeftRight_Axis * MecanumDrive_Enabled);
  BackLeft_TargetValue = -ForwardBackward_Axis - (StrafeLeftRight_Axis * MecanumDrive_Enabled);
  FrontRight_TargetValue = ForwardBackward_Axis + (StrafeLeftRight_Axis * MecanumDrive_Enabled);
  BackRight_TargetValue = ForwardBackward_Axis - (StrafeLeftRight_Axis * MecanumDrive_Enabled);

  //calculate rotation values
  if (abs(TurnLeftRight_Axis) > DEAD_BAND) {
    FrontLeft_TargetValue += (TURN_RATE * TurnLeftRight_Axis);
    BackLeft_TargetValue += (TURN_RATE * TurnLeftRight_Axis);
    FrontRight_TargetValue += (TURN_RATE * TurnLeftRight_Axis);
    BackRight_TargetValue += (TURN_RATE * TurnLeftRight_Axis);
  }

  //constrain to joystick range
  FrontLeft_TargetValue = constrain(FrontLeft_TargetValue, -127, 127);
  BackLeft_TargetValue = constrain(BackLeft_TargetValue, -127, 127);
  FrontRight_TargetValue = constrain(FrontRight_TargetValue, -127, 127);
  BackRight_TargetValue = constrain(BackRight_TargetValue, -127, 127);

  //set motor speed through slew rate function
  Motor_FrontLeft_SetValue = SlewRateFunction(FrontLeft_TargetValue, Motor_FrontLeft_SetValue);
  Motor_FrontRight_SetValue = SlewRateFunction(FrontRight_TargetValue, Motor_FrontRight_SetValue);
  Motor_BackLeft_SetValue = SlewRateFunction(BackLeft_TargetValue, Motor_BackLeft_SetValue);
  Motor_BackRight_SetValue = SlewRateFunction(BackRight_TargetValue, Motor_BackRight_SetValue);
}

// Function to handle slew rate for motor speed ramping
// Slew rate for ramping motor speed
const int SLEW_RATE_MS = 20;
int SlewRateFunction(int Input_Target, int Input_Current) {
  int speedDiff = Input_Target - Input_Current;
  if (speedDiff > 0) Input_Current += min(speedDiff, SLEW_RATE_MS);
  else if (speedDiff < 0) Input_Current -= min(-speedDiff, SLEW_RATE_MS);
  constrain(Input_Current, -127, 127);
  return Input_Current;
}

// Function to control motor output based on input values
// Motor speed limits and starting speed
const int MAX_SPEED = 255;
const int MIN_SPEED = -255;
const int MIN_STARTING_SPEED = 140;
const int STOP = 0;
bool INVERT = false;
void Set_Motor_Output(int Output, int Motor_ChA, int Motor_ChB) {
  if (INVERT) Output = -Output;
  Output = constrain(Output, -127, 127);
  int Mapped_Value = map(abs(Output), 0, 127, MIN_STARTING_SPEED, MAX_SPEED);
  int A, B = 0;
  if (Output < -DEAD_BAND) {  // Rotate Clockwise
    A = 0;
    B = Mapped_Value;
  } else if (Output > DEAD_BAND) {  // Rotate Counter-Clockwise
    A = Mapped_Value;
    B = 0;
  } else {  // Rotation Stop
    A = STOP;
    B = STOP;
  }
  ledcWrite(Motor_ChA, A);  //send to motor control pins
  ledcWrite(Motor_ChB, B);
}

// configure motor output
void Motor_Control() {
  Set_Motor_Output(Motor_FrontLeft_SetValue, Motor_M1_A, Motor_M1_B);
  Set_Motor_Output(Motor_BackLeft_SetValue, Motor_M2_A, Motor_M2_B);
  Set_Motor_Output(Motor_FrontRight_SetValue, Motor_M5_A, Motor_M5_B);
  Set_Motor_Output(Motor_BackRight_SetValue, Motor_M6_A, Motor_M6_B);
}

// stop motors from spinning
void Motor_STOP() {
  Set_Motor_Output(STOP, Motor_M1_A, Motor_M1_B);
  Set_Motor_Output(STOP, Motor_M2_A, Motor_M2_B);
  Set_Motor_Output(STOP, Motor_M5_A, Motor_M5_B);
  Set_Motor_Output(STOP, Motor_M6_A, Motor_M6_B);
}

// Tones created in the motors. Cycle through each motor.
void Start_Tone() {
  for (int i = 0; i < 6; i++) {
    long ToneTime = millis() + 200;
    bool state = 0;
    while (millis() < ToneTime) {
      digitalWrite(motorPins_A[i], state);
      digitalWrite(motorPins_B[i], !state);
      state = !state;
      long WaitTime = micros() + (100 * (i + 1));
      while (micros() < WaitTime) {}
    }
    digitalWrite(motorPins_A[i], 0);
    digitalWrite(motorPins_B[i], 0);
    delay(50);
  }
}


// check battery status of the ps4 controller
long DelaySerialPrint = 0;
void PS4controller_BatteryCheck() {
  if (millis() > DelaySerialPrint) {
    Serial.printf("Controller Battery Level : %d\n", PS4.Battery());
    DelaySerialPrint = millis() + 1000;
  }
  PS4.setRumble(0, 0);
  if (PS4.Charging()) PS4.setFlashRate(1000, 1000);
  else PS4.setFlashRate(0, 0);
  if (PS4.Battery() > 5) PS4.setLed(0, 255, 0);
  else if (PS4.Battery() > 2) PS4.setLed(255, 255, 0);
  else {
    PS4.setFlashRate(500, 500);
    PS4.setLed(255, 0, 0);
    PS4.setRumble(50, 50);
  }
  PS4.sendToController();
}

// controller rumble control for connecting
bool Connected_Rumble = false;
void Rumble_Once() {
  if (Connected_Rumble) return;
  Connected_Rumble = true;
  PS4.setRumble(255, 255);
  PS4.sendToController();
  delay(200);
  PS4.setRumble(0, 0);
  PS4.sendToController();
}

// Set a specific color for the entire NeoPixel strip
// NeoPixel Configurations
Adafruit_NeoPixel strip(LED_COUNT, LED_DataPin, NEO_GRB + NEO_KHZ800);
const uint32_t RED = strip.Color(255, 0, 0, 0);
const uint32_t GREEN = strip.Color(0, 255, 0, 0);
const uint32_t BLUE = strip.Color(0, 0, 255, 0);
const uint32_t WHITE = strip.Color(0, 0, 0, 255);
const uint32_t PURPLE = strip.Color(255, 0, 255, 0);
const uint32_t CYAN = strip.Color(0, 255, 255, 0);
const uint32_t YELLOW = strip.Color(255, 255, 0, 0);
void NeoPixel_SetColour(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {  // For each pixel in strip...
    strip.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    strip.show();                                // Update strip with new contents
  }
}
// Rainbow pattern for NeoPixel strip
long firstPixelHue = 0;
void NeoPixel_Rainbow() {
  strip.rainbow(firstPixelHue);
  strip.show();  // Update strip with new contents
  firstPixelHue += 256;
  if (firstPixelHue >= 5 * 65536) firstPixelHue = 0;
}


// Set up pins, LED PWM functionalities and begin PS4 controller, Serial and Serial2 communication
void setup() {
  // Set up the pins
  pinMode(LED_DataPin, OUTPUT);
  pinMode(ControllerSelectPin, INPUT_PULLUP);
  pinMode(MotorEnablePin, OUTPUT);

  for (int i = 0; i < 6; i++) {
    pinMode(motorPins_A[i], OUTPUT);
    pinMode(motorPins_B[i], OUTPUT);
    digitalWrite(motorPins_A[i], 0);
    digitalWrite(motorPins_B[i], 0);
  }

  // output preset bias
  digitalWrite(LED_DataPin, 0);
  digitalWrite(MotorEnablePin, 1);

  // Neopixels Configuration
  strip.begin();            // INITIALIZE NeoPixel strip object
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)

  // Motor test tones
  NeoPixel_SetColour(BLUE);
  Start_Tone();

  // configure LED PWM functionalitites
  for (int i = 0; i < 6; i++) {
    ledcSetup(MOTOR_PWM_Channel_A[i], PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(MOTOR_PWM_Channel_B[i], PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(motorPins_A[i], MOTOR_PWM_Channel_A[i]);
    ledcAttachPin(motorPins_B[i], MOTOR_PWM_Channel_B[i]);
  }
  NeoPixel_SetColour(PURPLE);


  // PS4 controller configuration (Target mac address saved on the controller)
  PS4.begin("e8:9e:b4:fc:b5:a4");  // REPLACE WITH THE MAC ADDRESS FROM YOUR PS4 CONTROLLER
  NeoPixel_SetColour(YELLOW);

  // Serial comms configurations (USB for debug messages)
  Serial.begin(115200);  // USB Serial
  NeoPixel_SetColour(GREEN);

  delay(1500);

  Serial.print("Drive Style: ");
  if (MecanumDrive_Enabled) Serial.println("MECANUM");
  else Serial.println("STANDARD");
  NeoPixel_SetColour(CYAN);

  Serial.println("CORE System Ready! " + Version);
}


void loop() {
  // Main loop to handle PS4 controller and serial input
  //PS4 Control
  if (PS4.isConnected()) {
    Rumble_Once();
    PS4controller_BatteryCheck();
    NeoPixel_Rainbow();                                           // LED Display
    Motion_Control(PS4.LStickY(), PS4.LStickX(), PS4.RStickX());  // Joystick control
    delay(5);
    Motor_Control();

    //Stop/Standby
  } else {
    NeoPixel_SetColour(RED);
    Motor_STOP();
    Connected_Rumble = false;
  }
}
