// -------------------- FIREBASE Setting ----------------------//
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define WIFI_SSID "infinergy"
#define WIFI_PASSWORD "okeokeoke"
#define API_KEY "AIzaSyBwXYpbgHjGhfrOy6M5IijOP03OMSyJy3A"
#define DATABASE_URL "https://filtertfi2024-default-rtdb.firebaseio.com" 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// -------------------- PRIVATE VARIABLE ----------------------//
#define IN1 D1
#define IN2 D2

// Firebase Variable
unsigned long currentMillis = 0;
unsigned long sendDataPrevMillis = 0;
unsigned long previousMillis = 0;
const long interval = 100;
int count = 0;
bool signupOK = false;
int control = 0;

void getFirebaseControl(){
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

      if (Firebase.ready())
      {
        if (Firebase.RTDB.getInt(&fbdo, "/control", &control)) {
          
            Serial.println(control);
          
          } else {
            Serial.println(fbdo.errorReason());
          }
      }

  }
}

void handleRelay(){
  if(control == 0){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  else if(control == 1){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else if(control == 2){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  else if(control == 3){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
  }
}

void setup(){

  Serial.begin(9600);
  // ------------------------- SETUP WIFI -----------------------------//
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);            

  // ------------------------- SETUP PIN -----------------------------//
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop(){
  getFirebaseControl();
  handleRelay();
}