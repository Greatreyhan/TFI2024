#include <SoftwareSerial.h>

SoftwareSerial s(3, 1); // RX pin, TX pin

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
volatile int percentageCO2 = 0;

void setup() {
  s.begin(9600); // Initialize SoftwareSerial
}

void loop() {
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