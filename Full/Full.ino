#include <Arduino.h>
#include "DHT.h"

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

// -------------------- DHT Setting ----------------------//
#define DHTPIN 13 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

// -------------------- MICS Setting ----------------------//
#include "DFRobot_MICS.h"
#define CALIBRATION_TIME   3                      
#define Mics_I2C_ADDRESS MICS_ADDRESS_0
DFRobot_MICS_I2C mics(&Wire, Mics_I2C_ADDRESS);

// -------------------- CO2 Setting ----------------------//
#define         MG_PIN                       (14)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (2)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in
#define         ZERO_POINT_VOLTAGE           (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define         REACTION_VOLTGAE             (0.030) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};

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

// NTP Variable

float MGRead(int mg_pin)
{
    int i;
    float v=0;

    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;
}

int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else {
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}

void readCO2(){

    voltCO2 = MGRead(MG_PIN);
    Serial.print( "SEN0159:" );
    Serial.print(voltCO2);
    Serial.print( "V           " );

    percentageCO2 = MGGetPercentage(voltCO2,CO2Curve);
    Serial.print("CO2:");
    if (percentageCO2 == -1) {
        Serial.print( "<400" );
    } else {
        Serial.print(percentageCO2);
    }

    Serial.print( "ppm" );
    Serial.print("\n");

    if (digitalRead(BOOL_PIN) ){
        Serial.print( "=====BOOL is HIGH======" );
    } else {
        Serial.print( "=====BOOL is LOW======" );
    }

    Serial.print("\n");

    delay(500);
}

bool readDHT(){
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("temperature: ");
  Serial.println(temperature);
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return false;
  }
  return true;
}

void readMICS(){
  C2H5OHgas = mics.getGasData(C2H5OH);
  CH4gas = mics.getGasData(CH4);
  H2gas = mics.getGasData(H2);
  NH3gas = mics.getGasData(NH3);
  COgas = mics.getGasData(CO);
  NO2gas = mics.getGasData(NO2);
  Serial.print("C2H5OH: ");
  Serial.println(C2H5OHgas);
  Serial.print("CH4: ");
  Serial.println(CH4gas);
  Serial.print("H2: ");
  Serial.println(H2gas);
  Serial.print("NH3: ");
  Serial.println(NH3gas);
  Serial.print("CO: ");
  Serial.println(COgas);
  Serial.print("NO2: ");
  Serial.println(NO2gas);
}

void sendFirebase(){
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
        if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/Temperature/"+currentDate, temperature)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/Humidity/"+currentDate, humidity)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/CO2/"+currentDate, percentageCO2/100)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/C2H5OH/"+currentDate, C2H5OHgas)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/CH4/"+currentDate, CH4gas)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/H2/"+currentDate, H2gas)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/NH3/"+currentDate, NH3gas)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/CO/"+currentDate, COgas)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
          if (Firebase.RTDB.setFloat(&fbdo, "/dataIn/NO2/"+currentDate, NO2gas)) {
    
            Serial.println(fbdo.dataPath());
          
            Serial.println(fbdo.pushName());
          
            Serial.println(fbdo.dataPath() + "/"+ fbdo.pushName());
          
          } else {
            Serial.println(fbdo.errorReason());
          }
      }

  }
}

void setup(){

  Serial.begin(115200);

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
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // ------------------------- SETUP MICS -----------------------------//
  while(!mics.begin()){
      Serial.println("NO Deivces !");
      delay(1000);
    } Serial.println("Device connected successfully !");
  uint8_t mode = mics.getPowerState();
  if(mode == SLEEP_MODE){
    mics.wakeUpMode();
    Serial.println("wake up sensor success!");
  }else{
    Serial.println("The sensor is wake up mode");
  }
  while(!mics.warmUpTime(CALIBRATION_TIME)){
    Serial.println("Please wait until the warm-up time is over!");
    delay(1000);
  }

  // ------------------------- SETUP CO2 -----------------------------//
  pinMode(BOOL_PIN, INPUT);                        
  digitalWrite(BOOL_PIN, HIGH);                    

  // ------------------------- SETUP NTP -----------------------------//
  timeClient.begin();
  timeClient.setTimeOffset(25200);

  // ------------------------- SETUP DHT -----------------------------//
  dht.begin();
}

void loop(){
  readMICS();
  readDHT();
  readCO2();
  sendFirebase();
}