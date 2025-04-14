#include <IRremote.hpp>

// #include <Wire.h>
// #include "RTClib.h"
#include <LiquidCrystal.h>
// RTC_DS3231 rtc;
#include <TimeLib.h>
#include "IRremote.h"
#include <SoftwareSerial.h>

//SoftwareSerial board2Serial(8, 9); // RX = pin 8, TX = pin 9

#define IR_RECEIVE_PIN 7


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

#define STEPPER_PIN_1 1
#define STEPPER_PIN_2 6
#define STEPPER_PIN_3 8
#define STEPPER_PIN_4 13
int step_number = 0;

const int trigPin = 10;
const int echoPin = 9;

//water level
int threshold = 5;

// Moisture control
int desiredMoistureValue = 500;
bool editing = false;
String inputBuffer = "";


// IR receiver setup
const int IR_RECV_PIN = 7;
IRrecv irrecv(IR_RECV_PIN);
decode_results results;

// Moisture level variable (0-1023, 0 = very wet)
// int desiredMoistureLevel = 500; // Default

// Input buffer for digits
int inputValue = 0;
int inputCount = 0;
bool editMode = false; // Tracks if editing moisture level

// Debouncing
unsigned long lastIRTime = 0;
const unsigned long debounceDelay = 200;

// #define TIME_HEADER  "T"   // Header tag for serial time sync message
// #define TIME_REQUEST  7    // ASCII bell character requests a time sync message 


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


int receivedDistance = 0;

void setup() {
  // put your setup code here, to run once:

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  setTime(5, 05, 0, 12, 4, 2025); // Set to 5:05:00, April 12, 2025
  lcd.print("Time Starting");
  delay(2000);
  lcd.clear();
  // Initialize IR receiver
  IrReceiver.begin(IR_RECEIVE_PIN);
  irrecv.enableIRIn();
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  static int lastCommand = -1;
  uint8_t command = -1; //was int

  long duration, distanceCm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration / 29 / 2;

  // Send the distance to Board 1
  Serial.println(distanceCm);

  delay(800);  // Reduce noise

  if(distanceCm < threshold){
    lcd.clear();
    lcd.print("Refill water");
  }
  else if (distanceCm>threshold){
    lcd.clear();
  }

  //int command = IrReceiver.decodedIRData.command;
  if (IrReceiver.decode()) {
    command = IrReceiver.decodedIRData.command;
    if(command == IR_BUTTON_9){
      Serial.println("Nine");
      //Serial.write("Nine");

    }
    IrReceiver.resume();
    
    //Serial.println(command);
  }

  // Ignore repeated signals of the same button (debounce)
  if (command == lastCommand) {
    return;
  }
  lastCommand = command;


  // Start edit mode on UP button
  if (command == IR_BUTTON_UP && !editMode) {
    editMode = true;
      inputValue = 0;
      inputCount = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Edit Moisture:");
      lcd.setCursor(0, 1);
      return; // Wait for next input
  }


  // Handle number input if editing
  if (editMode) {
    int digit = -1;
    if (command == IR_BUTTON_0) digit = 0;
    if (command == IR_BUTTON_1) digit = 1;
    if (command == IR_BUTTON_2) digit = 2;
    if (command == IR_BUTTON_3) digit = 3;
    if (command == IR_BUTTON_4) digit = 4;
    if (command == IR_BUTTON_5) digit = 5;
    if (command == IR_BUTTON_6) digit = 6;
    if (command == IR_BUTTON_7) digit = 7;
    if (command == IR_BUTTON_8) digit = 8;
    if (command == IR_BUTTON_9) digit = 9;

    if (digit != -1 && inputCount < 3) {
      inputValue = inputValue * 10 + digit;
      inputCount++;
      lcd.print(digit);

      if (inputCount == 3) {
        desiredMoistureValue = constrain(inputValue, 0, 1023);
        editMode = false;
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Moisture Set:");
        lcd.setCursor(0, 1);
        lcd.print(desiredMoistureValue);
      }
    }
  }

  // Example test print
  if (command == IR_BUTTON_0 && !editMode) {
    lcd.setCursor(0, 0);
    lcd.print("Works");
  }
  else {
    lastCommand = -1; // Reset when no button is being pressed
  }

  
  // long duration, inches, cm;
  // digitalWrite(trigPin, LOW);
  // delayMicroseconds(2);
  // digitalWrite(trigPin, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPin, LOW);

  // duration - pulseIn(echoPin, HIGH);

  // inches = microsecondsToInches(duration);
  // cm = microsecondsToCentimeters(duration);

  // Serial.print(inches);
  // Serial.print("inches, ");
  // Serial.print(cm);
  // Serial.print("cm");
  // Serial.println();

  // delay(100);

}

long microsecondsToInches(long microseconds){
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds){
  return microseconds / 29 / 2;
}

  // if(command == IR_BUTTON_0){
  //   lcd.setCursor(0, 0);
  //   lcd.print("Works");
  // }
  // // Handle IR input
  // if (irrecv.decode(&results)) {
  //   unsigned long currentTime = millis();
  //   if (currentTime - lastIRTime > debounceDelay) {
  //     int digit = -1;

  //     // Map IR codes
  //     switch (results.value) {
  //       case 0xFF629D: // Volume Up (+)
  //         editMode = true; // Enter edit mode
  //         inputValue = 0;
  //         inputCount = 0;
  //         break;
  //       case 0xFF6897: digit = 0; break; // 0
  //       case 0xFF30CF: digit = 1; break; // 1
  //       case 0xFF18E7: digit = 2; break; // 2
  //       case 0xFF7A85: digit = 3; break; // 3
  //       case 0xFF10EF: digit = 4; break; // 4
  //       case 0xFF38C7: digit = 5; break; // 5
  //       case 0xFF5AA5: digit = 6; break; // 6
  //       case 0xFF42BD: digit = 7; break; // 7
  //       case 0xFF4AB5: digit = 8; break; // 8
  //       case 0xFF52AD: digit = 9; break; // 9
  //       case 0xFF02FD: // OK button
  //         if (editMode && inputCount > 0) { // Confirm input
  //           desiredMoistureLevel = constrain(inputValue, 0, 1023);
  //           editMode = false;
  //           inputValue = 0;
  //           inputCount = 0;
  //         }
  //         break;
  //     }

  //     // Process digit input in edit mode
  //     if (editMode && digit >= 0 && inputCount < 3) {
  //       inputValue = inputValue * 10 + digit; // Build number (e.g., 1→12→123)
  //       inputCount++;
  //       if (inputCount == 3) { // Auto-confirm after 3 digits
  //         desiredMoistureLevel = constrain(inputValue, 0, 1023);
  //         editMode = false;
  //         inputValue = 0;
  //         inputCount = 0;
  //       }
  //     }

  //     lastIRTime = currentTime;
  //   }
  //   irrecv.resume();
  // }
  
  // // Update LCD periodically
  // static unsigned long lastUpdate = 0;
  // unsigned long currentTime = millis();
  // if (currentTime - lastUpdate >= 500) {
  //   lcd.clear(); // Clear to prevent overlap


  // // Display time on first row
  // lcd.setCursor(0, 0);
  // lcd.print("Time: ");
  // if (hour() < 10) lcd.print("0");
  // lcd.print(hour());
  // lcd.print(":");
  // if (minute() < 10) lcd.print("0");
  // lcd.print(minute());
  // lcd.print(":");
  // if (second() < 10) lcd.print("0");
  // lcd.print(second());


  // // Display moisture level or input on second row
  //   lcd.setCursor(0, 1);
  //   if (editMode) {
  //     lcd.print("Edit: ");
  //     if (inputCount > 0) {
  //       lcd.print(inputValue);
  //     } else {
  //       lcd.print("   ");
  //     }
  //   } else {
  //     lcd.print("Moist:");
  //     lcd.print(desiredMoistureLevel);
  //   }

  //   lastUpdate = currentTime;
  // }



  // // // Display on LCD
  // // lcd.setCursor(0, 0);
  // // lcd.print("Desired: ");
  // // lcd.print(desiredMoistureLevel);
  // // lcd.print("   "); // Clear extra digits

  // // lcd.setCursor(0, 1);
  // // lcd.print("Input: ");
  // // if (inputCount > 0) {
  // //   lcd.print(inputValue);
  // // } else {
  // //   lcd.print("   ");
  // // }
  // // lcd.print(" (0-1023)");


  // // // Display time on first row
  // // lcd.setCursor(0, 0);
  // // lcd.print("Time: ");
  // // if (hour() < 10) lcd.print("0"); // Leading zero for hours
  // // lcd.print(hour());
  // // lcd.print(":");
  // // if (minute() < 10) lcd.print("0"); // Leading zero for minutes
  // // lcd.print(minute());
  // // lcd.print(":");
  // // if (second() < 10) lcd.print("0"); // Leading zero for seconds
  // // lcd.print(second());

  // // delay(1000); // Update every second

  

