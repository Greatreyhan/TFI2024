/***********************************************************************************
*This program is a demo of displaying string
*This demo was made for LCD modules with 8bit or 16bit data port.
*This program requires the the LCDKIWI library.

* File                 display_string.ino
* Hardware Environment Arduino UNO&Mega2560
* Build Environment    Arduino

*Set the pins to the correct ones for your development shield or breakout board.
*This demo use the BREAKOUT BOARD only and use these 8bit data lines to the LCD,
*pin usage as follow
*                  LCD_CS  LCD_CD  LCD_WR  LCD_RD  LCD_RST  SD_SS  SD_DI  SD_DO  SD_SCK 
*     Arduino Uno    A3      A2      A1      A0      A4      10     11     12      13                            
*Arduino Mega2560    A3      A2      A1      A0      A4      10     11     12      13                           

*                  LCD_D0  LCD_D1  LCD_D2  LCD_D3  LCD_D4  LCD_D5  LCD_D6  LCD_D7  
*     Arduino Uno    8       9       2       3       4       5       6       7
*Arduino Mega2560    8       9       2       3       4       5       6       7 

*Remember to set the pins to suit your display module!
*
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE 
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************/

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//if the IC model is known or the modules is unreadable,you can use this constructed function
//LCDWIKI_KBV mylcd(ST7789V,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
LCDWIKI_KBV mylcd(240,320,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

//define some colour values
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// DHT Variable
    bool connect = false;

    float humidity = 0;
    float temperature = 0;
    float C2H5OHgas = 0;
    float CH4gas = 0;
    float H2gas = 0;
    float NH3gas = 0;
    float COgas = 0;
    float NO2gas = 0;
    float percentageCO2 = 0;

void ReadData(){
  if (Serial.available() >= 36) {
    humidity = 0;
    temperature = 0;
    C2H5OHgas = 0;
    CH4gas = 0;
    H2gas = 0;
    NH3gas = 0;
    COgas = 0;
    NO2gas = 0;
    percentageCO2 = 0;
    Serial.readBytes((char*)&humidity, sizeof(humidity));
    Serial.readBytes((char*)&temperature, sizeof(temperature));
    Serial.readBytes((char*)&C2H5OHgas, sizeof(C2H5OHgas));
    Serial.readBytes((char*)&CH4gas, sizeof(CH4gas));
    Serial.readBytes((char*)&H2gas, sizeof(H2gas));
    Serial.readBytes((char*)&NH3gas, sizeof(NH3gas));
    Serial.readBytes((char*)&COgas, sizeof(COgas));
    Serial.readBytes((char*)&NO2gas, sizeof(NO2gas));
    Serial.readBytes((char*)&percentageCO2, sizeof(percentageCO2));
    Serial.println(humidity);
    Serial.println(temperature);
    Serial.println(C2H5OHgas);
    Serial.println(CH4gas);
    Serial.println(H2gas);
    Serial.println(NH3gas);
    Serial.println(COgas);
    Serial.println(NO2gas);
    Serial.println(percentageCO2);
    connect = true;
  }
}

void setup() 
{
  Serial.begin(9600);
  mylcd.Init_LCD();
  Serial.println(mylcd.Read_ID(), HEX);
  mylcd.Fill_Screen(BLACK);
  mylcd.Set_Rotation(135);
}

void loop() 
{
  ReadData();
  if(connect){
    mylcd.Set_Text_Mode(0);
    mylcd.Fill_Screen(BLACK);
    mylcd.Set_Text_colour(WHITE);
    mylcd.Set_Text_Back_colour(BLACK);
    mylcd.Set_Text_Size(3);
    mylcd.Print_String("FILTER UDARA TFI", 18, 5);
    mylcd.Set_Text_colour(GREEN);
    mylcd.Set_Text_Back_colour(BLACK);
    mylcd.Set_Text_Size(2);
    mylcd.Print_String("Humidity", 0, 50+0);
    mylcd.Print_Number_Float(humidity, 2, 180, 50+0, '.', 0, ' ');
    mylcd.Print_String("%", 280, 50+0); 
    mylcd.Print_String("Temperature", 0, 50+20);
    mylcd.Print_Number_Float(temperature, 2, 180, 50+20, '.', 0, ' ');  
    mylcd.Print_String("C", 280, 50+20); 
    mylcd.Print_String("C2H5OH", 0, 50+40);
    mylcd.Print_Number_Float(C2H5OHgas, 2, 180, 50+40, '.', 0, ' '); 
    mylcd.Print_String("ppm", 280, 50+40);  
    mylcd.Print_String("CH4", 0, 50+60);
    mylcd.Print_Number_Float(CH4gas, 2, 180, 50+60, '.', 0, ' '); 
    mylcd.Print_String("ppm", 280, 50+60); 
    mylcd.Print_String("H2", 0, 50+80);
    mylcd.Print_Number_Float(H2gas, 2, 180, 50+80, '.', 0, ' ');  
    mylcd.Print_String("ppm", 280, 50+80); 
    mylcd.Print_String("NH3", 0, 50+100);
    mylcd.Print_Number_Float(NH3gas, 2, 180, 50+100, '.', 0, ' ');  
    mylcd.Print_String("ppm", 280, 50+100); 
    mylcd.Print_String("CO", 0, 50+120);
    mylcd.Print_Number_Float(COgas, 2, 180, 50+120, '.', 0, ' ');  
    mylcd.Print_String("ppm", 280, 50+120); 
    mylcd.Print_String("NO2", 0, 50+140);
    mylcd.Print_Number_Float(NO2gas, 2, 180, 50+140, '.', 0, ' ');  
    mylcd.Print_String("ppm", 280, 50+140); 
    mylcd.Print_String("CO2", 0, 50+160);
    mylcd.Print_Number_Float(percentageCO2/100, 2, 180, 50+160, '.', 0, ' '); 
    mylcd.Print_String("ppm", 280, 50+160); 
    connect = false;
  }
}
