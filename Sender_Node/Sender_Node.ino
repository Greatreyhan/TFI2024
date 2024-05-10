#include<SoftwareSerial.h>
SoftwareSerial s(3,1);
// -------------------- NTP Setting ----------------------//
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

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
// Firebase Variable
unsigned long sendDataPrevMillis = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
int count = 0;
bool signupOK = false;

// DHT Variable
volatile float humidity = 0;
volatile float temperature = 0;

// MICS Variable
volatile float C2H5OHgas = 0;
volatile float CH4gas = 0;
volatile float H2gas = 0;
volatile float NH3gas = 0;
volatile float COgas = 0;
volatile float NO2gas = 0;

// CO2 Variable
volatile float percentageCO2 = 0;
volatile float voltCO2 = 0;

void sendInArduino(){
  // Convert float to byte array
  byte* humidityBytes = (byte*)&humidity;
  byte* temperatureBytes = (byte*)&temperature;
  byte* C2H5OHgasBytes = (byte*)&C2H5OHgas;
  byte* CH4gasBytes = (byte*)&CH4gas;
  byte* H2gasBytes = (byte*)&H2gas;
  byte* NH3gasBytes = (byte*)&NH3gas;
  byte* COgasBytes = (byte*)&COgas;
  byte* NO2gasBytes = (byte*)&NO2gas;
  byte* percentageCO2Bytes = (byte*)&percentageCO2;

  // Send bytes over SoftwareSerial
  s.write(humidityBytes, sizeof(humidity));
  s.write(temperatureBytes, sizeof(temperature));
  s.write(C2H5OHgasBytes, sizeof(C2H5OHgas));
  s.write(CH4gasBytes, sizeof(CH4gas));
  s.write(H2gasBytes, sizeof(H2gas));
  s.write(NH3gasBytes, sizeof(NH3gas));
  s.write(COgasBytes, sizeof(COgas));
  s.write(NO2gasBytes, sizeof(NO2gas));
  s.write(percentageCO2Bytes, sizeof(percentageCO2));

  delay(100);
}


void getFirebaseIn(){
  // For Timing-----------------------------
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();
  // int currentHour = timeClient.getHours();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int hoursNow = ptm->tm_hour;
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(currentYear) + "/" + String(currentMonth) + "/" + String(monthDay)+ "/" + String(hoursNow);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

      if (Firebase.ready())
      {
        if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/Temperature/"+currentDate, &temperature)) {
          
            // Serial.println(temperature);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/Humidity/"+currentDate, &humidity)) {
          
            // Serial.println(humidity);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/CO2/"+currentDate, &percentageCO2)) {
          
            // Serial.println(percentageCO2);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/C2H5OH/"+currentDate, &C2H5OHgas)) {
          
            // Serial.println(C2H5OHgas);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/CH4/"+currentDate, &CH4gas)) {
          
            // Serial.println(CH4gas);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/H2/"+currentDate, &H2gas)) {
          
            // Serial.println(H2gas);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/NH3/"+currentDate, &NH3gas)) {
          
            // Serial.println(NH3gas);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/CO/"+currentDate, &COgas)) {
          
            // Serial.println(COgas);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.getFloat(&fbdo, "/dataIn/NO2/"+currentDate, &NO2gas)) {
          
            // Serial.println(NO2gas);
          
          } else {
            // Serial.println(fbdo.errorReason());
          }
      }

  }
}

void setup(){

  // Serial.begin(9600);
  s.begin(9600);

  // ------------------------- SETUP WIFI -----------------------------//
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    // Serial.print(".");
    delay(300);
  }
  // Serial.println();
  // Serial.print("Connected with IP: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    // Serial.println("ok");
    signupOK = true;
  }
  else{
    // Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);            

  // ------------------------- SETUP NTP -----------------------------//
  timeClient.begin();
  timeClient.setTimeOffset(25200);

}

void loop(){
  getFirebaseIn();
  sendInArduino();
  delay(1000);
  // ESP.restart();
}