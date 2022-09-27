#include "LiquidCrystal.h"

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

float vol_p = 0.0, vol_d = 0.0;
float temp=0.0;


void setup()
{
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps
   lcd.begin(16, 2);       //// set up the LCD's number of columns and rows: 
}
void loop()
{

//Conversion formula for voltage
   
   int Po = analogRead(A1);
   int Do = analogRead(A2);
   vol_p = (Po * 5.0) / 1024.0; 
   vol_d = (Do * 5.0) / 1024.0;

   
   if (vol_p < 0.1) 
   {
     vol_p=0.0;
   } 
    Serial.print("Po= ");
    Serial.println(vol_p);
    lcd.setCursor(0, 0);
    lcd.print("V Po= ");
    lcd.print(vol_p);
    delay(300);

   if (vol_d < 0.1) 
   {
     vol_d=0.0;
   } 
    Serial.print("Do= ");
    Serial.println(vol_d);
    lcd.setCursor(0, 1);
    lcd.print("V Do= ");
    lcd.print(vol_d);
    delay(300);
}
