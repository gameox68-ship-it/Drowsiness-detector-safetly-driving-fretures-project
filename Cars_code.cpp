#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int OBSTACLE_PIN = 2; 

// TB6612FNG Pins (Matched to your handwritten notes)
const int PWMA = 3; 
const int AIN1 = 4; 
const int AIN2 = 5; 
const int BIN2 = 6; 
const int BIN1 = 7; 
const int PWMB = 9; 

RF24 radio(A0, A1); // CE -> A0, CSN -> A1
const byte address[6] = "00001";

uint8_t receivedCommand = 0; // 1 = Go, 0 = Stop
unsigned long lastSignalTime = 0;
const unsigned long SIGNAL_TIMEOUT_MS = 500; 

void setup() {
  pinMode(OBSTACLE_PIN, INPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(10, OUTPUT); // Required for Nano SPI

  stopMotors();

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedCommand, sizeof(receivedCommand));
    lastSignalTime = millis();
  }

  // Failsafe: Stop if wireless signal is lost
  if (millis() - lastSignalTime > SIGNAL_TIMEOUT_MS) {
    receivedCommand = 0;
  }

  if (receivedCommand == 0) {
    stopMotors(); // Driver is asleep/drowsy
  } else {
    // Driver is awake, let car move automatically
    if (digitalRead(OBSTACLE_PIN) == HIGH) {
      moveForward();
    } else {
      moveReverse();
    }
  }
}

void moveForward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 255);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 255);
}

void moveReverse() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, 255);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, 255);
}

void stopMotors() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);
}