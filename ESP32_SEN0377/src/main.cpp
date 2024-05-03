#include <Arduino.h>
#include "DFRobot_MICS.h"

#define CALIBRATION_TIME   3                      // Default calibration time is three minutes
#define Mics_I2C_ADDRESS MICS_ADDRESS_0
DFRobot_MICS_I2C mics(&Wire, Mics_I2C_ADDRESS);

// When using the Breakout version, use the following program to construct an object from DFRobot_MICS_ADC
/**!
  adcPin is A0~A5
  powerPin is General IO
*/
// #define ADC_PIN   A0
// #define POWER_PIN 10
//DFRobot_MICS_ADC mics(/*adcPin*/ADC_PIN, /*powerPin*/POWER_PIN);

void setup() 
{
  Serial.begin(115200);
  while(!Serial);
  while(!mics.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  } Serial.println("Device connected successfully !");

  /**!
    Gets the power mode of the sensor
    The sensor is in sleep mode when power is on,so it needs to wake up the sensor. 
    The data obtained in sleep mode is wrong
   */
  uint8_t mode = mics.getPowerState();
  if(mode == SLEEP_MODE){
    mics.wakeUpMode();
    Serial.println("wake up sensor success!");
  }else{
    Serial.println("The sensor is wake up mode");
  }

  /**!
     Do not touch the sensor probe when preheating the sensor.
     Place the sensor in clean air.
     The default calibration time is 3 minutes.
  */
  while(!mics.warmUpTime(CALIBRATION_TIME)){
    Serial.println("Please wait until the warm-up time is over!");
    delay(1000);
  }
}

void loop() 
{
  /**!
    Gas type:
    MICS-4514 You can get all gas concentration
    MICS-5524 You can get the concentration of CH4, C2H5OH, H2, NH3, CO
    MICS-2714 You can get the concentration of NO2
      Methane          (CH4)    (1000 - 25000)PPM
      Ethanol          (C2H5OH) (10   - 500)PPM
      Hydrogen         (H2)     (1    - 1000)PPM
      Ammonia          (NH3)    (1    - 500)PPM
      Carbon Monoxide  (CO)     (1    - 1000)PPM
      Nitrogen Dioxide (NO2)    (0.1  - 10)PPM
  */
  float gasdata = mics.getGasData(C2H5OH);
  Serial.print("C2H5OH ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  gasdata = mics.getGasData(CH4);
  Serial.print("CH4 ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  gasdata = mics.getGasData(H2);
  Serial.print("H2 ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  gasdata = mics.getGasData(NH3);
  Serial.print("NH3 ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  gasdata = mics.getGasData(CO);
  Serial.print("CO ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  gasdata = mics.getGasData(NO2);
  Serial.print("NO2 ");
  Serial.print(gasdata,1);
  Serial.println(" PPM");
  delay(1000);
  //mics.sleepMode();
}