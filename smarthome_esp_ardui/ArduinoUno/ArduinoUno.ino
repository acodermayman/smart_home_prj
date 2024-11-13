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

// Servo myservo;
// int pirState = LOW; //trạng thái PIR
// const unsigned long timeout = 3000; //thời gian chờ đóng cửa
// unsigned long motionTimer = 0; //đếm thời gian
// LiquidCrystal_I2C lcd(0x27,16,2);

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

  pinMode(PIR_PIN, INPUT);        // Chân cảm biến PIR là đầu vào

  pinMode(pinD0, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600);

  // // Hiển thị Hello World khi bắt đầu chương trình
  // Khởi tạo LCD 
  // lcd.init();
  // lcd.backlight();
  // lcd.setCursor(3, 0);
  // lcd.print("Welcome");
  // delay(3000);
  // lcd.clear();

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

// void sendInitialStatus() {
//   json.clear();
//   json["livingRoomLedState"] = livingRoomLedState;
//   json["bedRoomLedState"] = bedRoomLedState;
//   json["door"] = doorState;
//   json["temperature"] = temperature;
//   json["humidity"] = humidity;
//   serializeJson(json, mySerial);
//   mySerial.println();

// void loop() {
//   unsigned long currentTime = millis(); 
//   if (currentTime - lastUpdateTime >= 2000) {
//   // Đọc giá trị nhiệt độ và độ ẩm từ cảm biến
//   dhtEvent();
//   lastUpdateTime = currentTime;
//   }

//   json.clear();
//   if (mySerial.available()){
//     String line = mySerial.readStringUntil('\n');
//     DeserializationError error = deserializeJson(json, line);
//     if (error) {
//       Serial.print("deserializeJson() failed: ");
//       Serial.println(error.c_str());
//       return;
//     }
//     led = json["led"];  
//   }
//   digitalWrite(LED_PIN, led);

// }

// void dhtEvent(){
//   temperature = dht.readTemperature();
//   humidity = dht.readHumidity();
//   json["temperature"] = temperature;
//   json["humidity"] = humidity;
//   serializeJson(json, mySerial);
//   mySerial.println();
//   serializeJson(json, Serial);
//   Serial.println();
// }
//}

// void doorFunction() {
//   lcd.setCursor(0, 0);
//   lcd.print("Open The Door");    // Hiển thị "Open The Door"
//   myservo.write(90);             // Quay servo tới 90 độ
//   delay(1000);                   // Đợi một chút để servo quay
  
//   motionTimer = millis();        // Bắt đầu đếm thời gian
  
//   // Kiểm tra cảm biến PIR để phát hiện chuyển động
//   while (millis() - motionTimer < timeout) {
//     pirState = digitalRead(pirPin); // Đọc trạng thái cảm biến PIR
    
//     if (pirState == HIGH) {         // Nếu có chuyển động
//       motionTimer = millis();       // Reset bộ đếm thời gian
//     }

//     delay(100);  // Chờ đợi một chút trước khi đọc lại cảm biến PIR
//   }
  
//   // Nếu không có chuyển động sau 3 giây
//   myservo.write(0);              // Đưa servo về 0 độ
//   lcd.clear();                    // Xóa LCD
//   lcd.setCursor(0, 0);
//   lcd.print("Close The Door");    // Hiển thị "Close The Door"
//   delay(1000);                    // Đợi một chút trước khi tiếp tục
// }