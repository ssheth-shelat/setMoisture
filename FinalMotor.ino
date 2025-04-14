#include <Stepper.h>
#include <DHT.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 12

#define IR_BUTTON_PLAY 64
#define IR_BUTTON_VOL+ 70
#define IR_BUTTON_VOL- 21
#define IR_BUTTON_UP 9
#define IR_BUTTON_DOWN 7
#define IR_BUTTON_0 22
#define IR_BUTTON_1 12
#define IR_BUTTON_2 24
#define IR_BUTTON_3 94
#define IR_BUTTON_4 8
#define IR_BUTTON_5 28
#define IR_BUTTON_6 90
#define IR_BUTTON_7 66
#define IR_BUTTON_8 82
#define IR_BUTTON_9 74

#define DHTPIN 5       // Pin connected to DHT11 data pin
#define DHTTYPE DHT11  // Specify DHT11 sensor type

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

// 28BYJ-48: 2048 steps per rotation
const int STEPS_PER_REVOLUTION = 2048;
const int SMALL_MOVE_STEPS = 600; // 1/8 rotation (~45 degrees)
const int EXTRA_STEPS = 800;      // Unused, kept for potential future use
const int motorPin1 = 8;          // IN1 on ULN2003
const int motorPin2 = 9;          // IN2
const int motorPin3 = 10;         // IN3
const int motorPin4 = 11;         // IN4

// Initialize stepper library
Stepper myStepper(STEPS_PER_REVOLUTION, motorPin1, motorPin3, motorPin2, motorPin4);

int currentPosition = 0; // Track motor position (0 = home)

void setup() {
  myStepper.setSpeed(15); // 15 RPM
  Serial.begin(9600);
  Serial.println("Stepper Motor Control Started");
  dht.begin(); // Initialize DHT11 sensor
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Initialize IR receiver
}

void loop() {
  static int lastCommand = -1;
  int command = -1;

  if (IrReceiver.decode()) {
    command = IrReceiver.decodedIRData.command; // Extract command ID
    if (command == IR_BUTTON_DOWN && command != lastCommand) {
      Serial.println("good");
      myStepper.step(-SMALL_MOVE_STEPS); // Move clockwise
      currentPosition -= SMALL_MOVE_STEPS;
      Serial.println("Moved clockwise");
    }
    else if (command == IR_BUTTON_UP && command != lastCommand) {
      myStepper.step(SMALL_MOVE_STEPS); // Move counterclockwise
      currentPosition += SMALL_MOVE_STEPS + 50;
      Serial.println("Moved counterclockwise");
    }
    lastCommand = command;
    IrReceiver.resume(); // Prepare for next signal
  }
  else {
    lastCommand = -1; // Reset when no signal
  }

  delay(2000); // Wait 2 seconds between readings

  float humidity = dht.readHumidity(); // Read humidity
  float temperature = dht.readTemperature(true); // Read temperature in Fahrenheit

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    return;
  }
}