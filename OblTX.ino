
#include <Manchester.h>
#include <Wire.h>
#include "Adafruit_MLX90614.h"
#include "crc.h"
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

struct Sensors {
  uint16_t val0 = 0;
  uint16_t val1 = 0;
  uint16_t val2 = 0;
  uint16_t val3 = 0;
  double mlxAT = 0.0;
  double mlxOT = 0.0;
  double mlx2AT = 0.0;
  double mlx2OT = 0.0;
  uint32_t realPressure;
  double realTemp;
} Sensors;

#define TX_PIN  PD1  //pin where your transmitter is connected
#define LED_PIN 13 //pin for blinking LED

uint8_t moo = 1; //last led status
uint8_t data[20] = {11, '1','2', '3', '4', '5', '6', '7', '8', '9','1','2','3','4','5','6','7','8','9'};
uint8_t data2[20];
int N_SUMS = 0;
long long int lastMillis = 0;

void setup() 
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, moo);

  mlx.begin();  
  lastMillis = millis();
  
  man.workAround1MhzTinyCore(); //add this in order for transmitter to work with 1Mhz Attiny85/84
  man.setupTransmit(TX_PIN, MAN_2400);
}


uint8_t datalength=2;   //at least two data
void loop() 
{

  Sensors.mlxAT+=mlx.readAmbientTempC(); 
  Sensors.mlxOT+=mlx.readObjectTempC();
  N_SUMS++;

  if ((millis()-lastMillis)>200)
  {
    lastMillis = millis();
    Sensors.mlxAT/=N_SUMS;
    Sensors.mlxOT/=N_SUMS;
    N_SUMS=0;
    //sprintf(data,"______%.2f %.2f\n",Sensors.mlxAT,Sensors.mlxOT);
    int16_t data1 = (int16_t)(Sensors.mlxAT*100.0f);
    int16_t data2 = (int16_t)(Sensors.mlxOT*100.0f);
    sprintf(data,"123456%d %d\n",data1,data2);
    data[0] = 0xAA;
    data[1] = (uint8_t)((data1&0xFF00)>>8);
    data[2] = (uint8_t)((data1&0xFF));
    data[3] = (uint8_t)((data2&0xFF00)>>8);
    data[4] = (uint8_t)((data2&0xFF));
    data[5] = Crc8(data, 4);
    man.transmitArray(20, data);
    moo = ++moo % 2;
    digitalWrite(LED_PIN, moo);


   
    Sensors.mlxAT = 0;
    Sensors.mlxOT = 0;
  }
  
}
