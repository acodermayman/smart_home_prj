#include <SoftwareSerial.h>
#include "ArduinoJson.h"
#include "DHT.h"
#include <Servo.h>
#define RX_PIN 10
#define TX_PIN 11 
#define DHTsensor_PIN 7
#define LED_LR_PIN 6
#define LED_BR_PIN 5  
#define PIR_PIN 4
#define SERVO_PIN 3

#define pinA0 A0
#define pinD0 12
#define BUZZER_PIN 13
int flameD0;

bool fireDetected = false;

float temperature = 0;
float humidity = 0;
unsigned long lastUpdateTime = 0;

bool livingRoomLedState = false;
bool bedRoomLedState = false;

bool doorState = false;
unsigned long motionTimer = 0;
const unsigned long timeout = 3000;

SoftwareSerial mySerial(RX_PIN, TX_PIN); 
StaticJsonDocument<100> json;
DHT dht(DHTsensor_PIN,DHT11);
Servo myservo;

void setup() {
  dht.begin();

  pinMode(LED_LR_PIN,OUTPUT);
  pinMode(LED_BR_PIN,OUTPUT);
  digitalWrite(LED_LR_PIN,LOW);
  digitalWrite(LED_BR_PIN,LOW);

  myservo.attach(SERVO_PIN);      
  myservo.write(90);              

  pinMode(PIR_PIN, INPUT);        
  pinMode(pinD0, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600);

  sendStatus();
}

void loop() {
  updateDHTSensor();
  handlePIRSensor();
  receiveCommands();
}

void updateDHTSensor() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= 2000) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    sendStatus();  // Send updated JSON data
    lastUpdateTime = currentTime;
  }
}

void handlePIRSensor() {
  if (digitalRead(PIR_PIN) == HIGH) {
    if (!doorState) {
      openDoor();
    }
    motionTimer = millis();  // Reset timer when motion detected
  } else if (doorState && millis() - motionTimer >= timeout) {
    closeDoor();
  }
}



void receiveCommands() {
  if (mySerial.available()) {
    String line = mySerial.readStringUntil('\n');
    DeserializationError error = deserializeJson(json, line);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    processCommand(json);
  }
}

void processCommand(JsonDocument& json) {
  if (json.containsKey("livingRoomLedState")) {
    livingRoomLedState = json["livingRoomLedState"];
    digitalWrite(LED_LR_PIN, livingRoomLedState);
  }
  if (json.containsKey("bedRoomLedState")) {
    bedRoomLedState = json["bedRoomLedState"];
    digitalWrite(LED_BR_PIN, bedRoomLedState);
  }
  if (json.containsKey("doorState")) {
    bool requestedDoorState = json["doorState"];
    if (requestedDoorState != doorState) {
      if (requestedDoorState) openDoor();
      else closeDoor();
    }
  }
  sendStatus();  // Send current status after processing command
}

void openDoor() {
  myservo.write(0);
  doorState = true;
  sendStatus();
}

void closeDoor() {
  myservo.write(90);
  doorState = false;
  sendStatus();
}

void handleFireSensor() {
  flameD0 = digitalRead(pinD0);
  if (flameD0 == 0) {
    digitalWrite(BUZZER_PIN, HIGH);
    fireDetected = true;
    delay(2000);
  } else {
    fireDetected = false;
    digitalWrite(BUZZER_PIN,LOW);
  }
  sendStatus();  
}
void sendStatus() {
  json.clear();
  json["livingRoomLedState"] = livingRoomLedState;
  json["bedRoomLedState"] = bedRoomLedState;
  json["doorState"] = doorState;
  json["temperature"] = temperature;
  json["humidity"] = humidity;
  json["fireDetected"] = fireDetected;
  serializeJson(json, mySerial);
  mySerial.println();
}
