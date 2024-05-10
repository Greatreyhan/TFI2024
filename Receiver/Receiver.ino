void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() >= 36) { // 4 bytes for each float
    // Read bytes into float variables
    // DHT Variable
    float humidity = 0;
    float temperature = 0;

    // MICS Variable
    float C2H5OHgas = 0;
    float CH4gas = 0;
    float H2gas = 0;
    float NH3gas = 0;
    float COgas = 0;
    float NO2gas = 0;

    // CO2 Variable
    int percentageCO2 = 0;

    Serial.readBytes((char*)&humidity, sizeof(humidity));
    Serial.readBytes((char*)&temperature, sizeof(temperature));
    Serial.readBytes((char*)&C2H5OHgas, sizeof(C2H5OHgas));
    Serial.readBytes((char*)&CH4gas, sizeof(CH4gas));
    Serial.readBytes((char*)&H2gas, sizeof(H2gas));
    Serial.readBytes((char*)&NH3gas, sizeof(NH3gas));
    Serial.readBytes((char*)&COgas, sizeof(COgas));
    Serial.readBytes((char*)&NO2gas, sizeof(NO2gas));
    Serial.readBytes((char*)&percentageCO2, sizeof(percentageCO2));

    // Print received float data
    Serial.print("Received data: ");
    Serial.print("humidity: ");
    Serial.print(humidity);
    Serial.print(", temperature: ");
    Serial.print(temperature);
    Serial.print(", C2H5OHgas: ");
    Serial.print(C2H5OHgas);
    Serial.print(", CH4gas: ");
    Serial.print(CH4gas);
    Serial.print(", H2gas: ");
    Serial.print(H2gas);
    Serial.print(", NH3gas: ");
    Serial.print(NH3gas);
    Serial.print(", COgas: ");
    Serial.print(COgas);
    Serial.print(", NO2gas: ");
    Serial.print(NO2gas);
    Serial.print(", percentageCO2: ");
    Serial.print(percentageCO2);
  }
}