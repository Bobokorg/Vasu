/*                 ___           ___           ___     
      ___         /  /\         /  /\         /__/\    
     /__/\       /  /::\       /  /:/_        \  \:\   
     \  \:\     /  /:/\:\     /  /:/ /\        \  \:\  
      \  \:\   /  /:/~/::\   /  /:/ /::\   ___  \  \:\ 
  ___  \__\:\ /__/:/ /:/\:\ /__/:/ /:/\:\ /__/\  \__\:\
 /__/\ |  |:| \  \:\/:/__\/ \  \:\/:/~/:/ \  \:\ /  /:/
 \  \:\|  |:|  \  \::/       \  \::/ /:/   \  \:\  /:/ 
  \  \:\__|:|   \  \:\        \__\/ /:/     \  \:\/:/  
   \__\::::/     \  \:\         /__/:/       \  \::/   
       ~~~~       \__\/         \__\/         \__\/    

   ___    __    ___    __ __   ___        _   __  ___      ___
  / _ |  / /   / _ \  / // /  / _ |      | | / / / _ \    <  /
 / __ | / /__ / ___/ / _  /  / __ |      | |/ / / // / _  / / 
/_/ |_|/____//_/    /_//_/  /_/ |_|      |___/  \___/ (_)/_/  

01001000 01100101 01100011 01101000 01101111  01110100 01101111 
01110100 01100001 01101100 01101101 01100101 01101110 01110100 
01100101  01110000 01101111 01110010  01001101 01100001 01110100 
01100101 01101111  01000110 01101100 01101111 01110010 01101001 
01100100 01101111 01010000 01010101 01001010  00101101  01000011 
01101111 01101100 01101111 01101101 01100010 01101001 01100001                                                        

Este proyecto ha sido programado como proyecto final de Arduino 
en el primer semestre de 2017. 
Todo lo contenido en este código está bajo licencia MOZILLA PUBLIC LICENSE 2.0. 
Para saber más recurra a la página de GitHub del proyecto: 
https://github.com/Bobokorg/Vasu/blob/master/LICENSE
Los diseños de circuitos, bricks, adaptadores y demás objetos 
físicos usados tienen todos los derechos reservados.
Este código cubre un modelo de “Brick” para ser adaptado a vehículos 
o robots y verificar la cantidad de gases nocivos en el aire. 
Así mismo, este código funciona también de forma sencilla mostrando 
un índice de contaminación en el aire actual.

*/
#include <SFE_BMP180.h>
#include <LiquidCrystal.h>
#include <Wire.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
SFE_BMP180 pressure;

int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define ALTITUDE 2679.0
byte term[8]=
{
  0b00001110,
  0b00010001,
  0b00010001,
  0b00011111,
  0b00001110,
  0b00001110,
  0b00001110,
  0b00000100
};
byte psi[8]=
{
  0b00000100,
  0b00010101,
  0b00001110,
  0b00000100,
  0b00000100,
  0b00001110,
  0b00010101,
  0b00000100

};

int read_LCD_buttons()
{
 adc_key_in = analogRead(0); 
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   


 return btnNONE;   
}

void setup()
{
   Serial.begin(9600);
   pressure.begin();
   lcd.begin(16, 2);              // start the library
   lcd.createChar(1, term);
   lcd.createChar(2, psi);
   lcd.setCursor(1,0);
   lcd.print("Vasu Alpha 0.1"); 
   for (int i=0; i<100; i++)
   {
      lcd.setCursor(1,1);
      lcd.print("Calibrando- ");
      lcd.print(i);
      lcd.print("%");
      //delay(100);
   }
   lcd.clear();
}
 
void loop()
{
    char status;
    double P,T, p0,a;
    int val,dat;
    //LM35
    val=analogRead(9);//Connect LM35 on Analog 0
    dat=(500 * val) /1024;;
    lcd.setCursor(0,1);            // move cursor to second line "1" and 9 spaces over
    lcd.write(1);
    lcd.print("Temp: ");
    lcd.print(dat);
    delay(150);
    //BMP180
    status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");
          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  delay(500);  // Pause for 5 seconds.
    //Impresiones LCD
    lcd.setCursor(0,0);
    lcd.write(2);
    lcd.print("Pres: ");
    lcd.print(P/10);
    lcd.setCursor(16,1);
   // lcd_key = read_LCD_buttons();  // read the buttons

   /* switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
         {
         lcd.print("RIGHT ");
         break;
         }
       case btnLEFT:
         {
         lcd.print("LEFT   ");
         break;
         }
       case btnUP:
         {
         lcd.print("UP    ");
         break;
         }
       case btnDOWN:
         {
         lcd.print("DOWN  ");
         break;
         }
       case btnSELECT:
         {
         lcd.print("SELECT");
         break;
         }
         case btnNONE:
         {
         lcd.print("NONE  ");
         break;
         }
     }
*/
}
