#include <Arduino.h>
#include "RingBuffer.hpp"
#include "config.h"

#define monitor //turn on monitoring via USB
#define COUNT_OF_BLADES 126

enum Controll_state
{
  USE_CH_1,
  USE_CH_2,
  NO
} controll_state;

RPMMetr *rpm;
RingBuffer<float> *pedal;

//-------------------------------------------------------------------------------------------------
//-----------------------------------functions-----------------------------------------------------
//-------------------------------------------------------------------------------------------------

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  float result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  result > 255 ? result = 255 : 0;
  result < 0 ? result = 0 : 0;
  return result;
}

//обновляем значение рычага каждые 200мс
void updateChannel()
{
  if (millis() - time_to_update_channel > 200)
  {
    if (digitalRead(PIN_USE_CH_1) && !digitalRead(PIN_USE_CH_2) && controll_state != Controll_state::USE_CH_1)
    {
      controll_state = Controll_state::USE_CH_1;
      analogWrite(PIN_DRIVER_SPEED_CH2, 0);
    }
    else if (!digitalRead(PIN_USE_CH_1) && digitalRead(PIN_USE_CH_2) && controll_state != Controll_state::USE_CH_2)
    {
      controll_state = Controll_state::USE_CH_2;
      analogWrite(PIN_DRIVER_SPEED_CH1, 0);
    }
    else if (!(digitalRead(PIN_USE_CH_1) ^ digitalRead(PIN_USE_CH_2)))
    {
      controll_state = Controll_state::NO;
      analogWrite(PIN_DRIVER_SPEED_CH1, 0);
      analogWrite(PIN_DRIVER_SPEED_CH2, 0);
    }
    time_to_update_channel = millis();
  }
}

void tachometer_Interrupt()
{
  rpm->update();
}

void setup()
{
  rpm = new RPMMetr;
  pedal = new RingBuffer<float>;
  
  pinMode(PIN_PEDAL, INPUT_PULLUP);

  pinMode(PIN_TACHOMETER, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_TACHOMETER), tachometer_Interrupt, FALLING);

  //select direction for the first channel
  pinMode(PIN_DRIVER_DIR_CH1, OUTPUT);
  digitalWrite(PIN_DRIVER_DIR_CH1, HIGH);
  //and the seckond channel
  pinMode(PIN_DRIVER_DIR_CH2, OUTPUT);
  digitalWrite(PIN_DRIVER_DIR_CH2, HIGH);

  pinMode(PIN_DRIVER_SPEED_CH1, OUTPUT);
  pinMode(PIN_DRIVER_SPEED_CH2, OUTPUT);

  analogWrite(PIN_DRIVER_SPEED_CH1, 0);
  analogWrite(PIN_DRIVER_SPEED_CH2, 0);

  pinMode(PIN_USE_CH_1, INPUT);
  pinMode(PIN_USE_CH_2, INPUT);
  controll_state = Controll_state::NO;
  
  Serial.begin(115200);
}

//-------------------------------------------------------------------------------------------------
//-----------------------------------MAIN FUNCTION-------------------------------------------------
//-------------------------------------------------------------------------------------------------
void loop()
{
  updateChannel();

  //обновляем значение на педали каждые 50мс
  if (millis() - time_to_update_A1 > 50)
  {
    pedal->update(analogRead(PIN_PEDAL));
    time_to_update_A1 = millis();
  }

  //проверяем не простаивает ли тахометр больше секунды
  if (millis() % 1000 == 0)
    rpm->check();

  /*--------------------------------------------------------------------------------------------------
    BUISNESS LOGIC
  --------------------------------------------------------------------------------------------------*/
  if (millis() - time_to_update_A1 > 50)
  {
    load_RPM = fmap(rpm->getRPM(COUNT_OF_BLADES), Min_RPM, Max_RPM, 0, 255);
    load_PEDAL = fmap(pedal->getAverage() * voltageMultiplyer, Min_ValPedal, Max_ValPedal, 0, 255); //some peace of sheet in the first argument is a calculation to voltage
    diff = load_RPM - load_PEDAL;
    if (diff > 255)
      diff = 255;
    if (diff < 0)
      diff = 0;

    switch (controll_state)
    {
    case USE_CH_1:
      analogWrite(PIN_DRIVER_SPEED_CH1, diff);
      break;

    case USE_CH_2:
      analogWrite(PIN_DRIVER_SPEED_CH2, diff);
      break;

    case NO:
      break;
    
    default:
      break;
    }
  }

#ifdef monitor
  if (millis() - time_to_print > 40)
  {
    Serial.print("Value_RPM:\tValue_pedal(%):\tLoad_RPM:\tLoad_PEDAL:\tDiff:\n");
    Serial.print(rpm->getRPM(COUNT_OF_BLADES));
    Serial.print("\t");
    Serial.print(pedal->getAverage());
    Serial.print("\t");
    Serial.print(load_RPM);
    Serial.print("\t");
    Serial.print(load_PEDAL);
    Serial.print("\t");
    Serial.print(diff);
    Serial.println("\t");
    

  /*  switch (controll_state)
    {
      case USE_CH_1:
        Serial.println("Channel I ON");
        break;

      case USE_CH_2:
        Serial.println("Channel II ON");
        break;

      case NO:
        Serial.println("Channel I and II OFF");
        break;
      
      default:
        Serial.println("ERROR: PIZDEC SUKA BLAT");
        break;
    }*/

    time_to_print = millis();
  }
#endif
}
