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

           _    __   ___ ____     ____  ______
          | |  / /  <  // __ \   / __ \/ ____/
          | | / /   / // / / /  / /_/ / /     
          | |/ /   / // /_/ /  / _, _/ /___   
          |___/   /_(_)____/  /_/ |_|\____/   
                                              
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

struct sensorpres {
  float temp;
  float absolutakpa;
  float relativakpa;
  float altitudm;
  float altitudpies;
};
//CONSTANTES y Variables Globales
int lcd_key     = 0;
int adc_key_in  = 0;
const int MQ9 = A11;
const int ResModulo = 70;
const int R0 = 1;          
const int Intervalo = 100;
const int Muestras = 5;

//Regresión del DATASHEET MQ9
const float X0 = 200;
const float Y0 = 1.8;
const float X1 = 10000;
const float Y1 = 0.8;
const float punto0[] = { log10(X0), log10(Y0) };
const float punto1[] = { log10(X1), log10(Y1) };
const float pendiente = (punto1[1] - punto0[1]) / (punto1[0] - punto0[0]);
const float coord = punto0[1] - punto0[0] * pendiente;
//Bot LCD
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define ALTITUDE 2679.0
// Símbolos del LCD en Vectores con Binarios
byte term[8] =
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
byte psi[8] =
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
byte index[8] = {
  B01110,
  B10001,
  B10001,
  B11011,
  B10101,
  B01010,
  B00100,
  B11011
};

/*int read_LCD_buttons()
{
  adc_key_in = analogRead(0);
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;


  return btnNONE;
}*/

void setup()
{
  Serial.begin(9600);
  pressure.begin();
  lcd.begin(16, 2);
  lcd.createChar(1, term);
  lcd.createChar(2, psi);
  lcd.createChar(3, index);
  lcd.setCursor(1, 0);

  lcd.print(" Vasu v1.0 RC");
  for (int i = 0; i < 100; i++)
  {
    lcd.setCursor(1, 1);
    lcd.print("Calibrando- ");
    lcd.print(i);
    lcd.print("%");
    delay(100);
  }

  lcd.clear();
}

void loop()
{
  float resmedia, concentration, calidad;
  sensorpres presion;

  //BMP180
  presion = tasar();
  //MQ-9
  resmedia = promediar(MQ9);      // Obtener la Rs promedio
  concentration = concentracion(resmedia / R0); // Obtener la concentración
  calidad = aqi(concentration);
  lcd.setCursor(1, 0);
  lcd.write(1);
  lcd.print(presion.temp - 3.2, 0);
  lcd.setCursor(6, 0);
  lcd.write(2);
  lcd.print(presion.absolutakpa, 0);
  lcd.setCursor(11, 0);
  lcd.write(3);
  lcd.print(" ");
  lcd.print(calidad, 0);
  delay(1000);
  if (calidad > 0 && calidad < 50)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("    Bueno");
  }
  if (calidad > 50 && calidad < 100)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("   Moderado");
  }
  if (calidad > 100 && calidad < 150)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Ins. Pob. Riesgo");
  }
  if (calidad > 150 && calidad < 200)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("  Insalubre");
  }
  if (calidad > 200 && calidad < 300)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("Muy Insalubre");
  }
  if (calidad > 300 && calidad < 500)
  {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("  Peligroso");
  }
}


float promediar(int MQ9)
{
  float rs = 0;
  for (int i = 0; i < Muestras; i++) {
    rs += resmq(analogRead(MQ9));
    delay(Intervalo);
  }
  return rs / Muestras;
}


float resmq(int lecanalog)
{
  return (((float)ResModulo / 1000.0 * (1023 - lecanalog) / lecanalog));
}


float concentracion(float rs_ro_ratio)
{
  return pow(10, coord + pendiente * log(rs_ro_ratio));
}

//Función que devuelve el Air Quality Index
int aqi(int ppm)
{
  float calidad;
  calidad = (50 / 4.4) * ppm;
  return calidad;
}

sensorpres tasar()
{
  char status;
  double P, T, H, p0, a;
  sensorpres medida;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(H, T);
        if (status != 0)
        {
          medida.absolutakpa = H * 0.1;
          p0 = pressure.sealevel(H, ALTITUDE); // we're at 1655 meters (Boulder, CO)
          medida.relativakpa = p0 * 0.1;
          a = pressure.altitude(H, p0);
          medida.altitudm = a;
          medida.altitudpies = a * 3.28084;
          Serial.println(T);
          medida.temp = T;
        }
        else Serial.println("Error 1: Revisa la conexión BMP\n");
      }
      else Serial.println("Error 2: No se puede inciar BMP. REINICIAR!\n");
    }
    else Serial.println("No hay Temperatura. REINICIAR!\n");
  }
  else Serial.println("No se puede obtener Temperatura REINICIAR!\n");
  return medida;
}

