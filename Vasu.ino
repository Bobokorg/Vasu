#include <SFE_BMP180.h>
#include <LiquidCrystal.h>
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
SFE_BMP180 bmp180;

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

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
// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0); 
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   


 return btnNONE;  // when all others fail, return this...
}

void setup()
{
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
  //delay(150);
 }
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Push the bitch"); // print a simple message
}
 
void loop()
{
    double val,dat,espera,P,T;
    //LM35
    val=analogRead(9);//Connect LM35 on Analog 0
    dat=(500 * val) /1024;;
    lcd.setCursor(0,1);            // move cursor to second line "1" and 9 spaces over
    lcd.write(1);
    lcd.print("Temp: ");
    lcd.print(dat);
    lcd.setCursor(0,1);            // move to the begining of the second line
    delay(150);
    //BMP180
    espera = bmp180.startTemperature();//Inicio de lectura de temperatura
    if (espera != 0)
    {   
      delay(espera); //Pausa para que finalice la lectura
      espera = bmp180.getTemperature(T); //Obtener la temperatura
    }
    espera=bmp180.startPressure(3);
    if (espera !=0)
    {
      delay(espera);
      espera = bmp180.getPressure(P,T);
    }
    lcd.setCursor(0,0);
    lcd.write(2);
    lcd.print("Pres: ");
    lcd.print(P);
    lcd.print("mbar");
    
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
