#include <Arduino.h>
#include "RingBuffer.hpp"
#include "config.h"

RPMMetr *rpm;
RingBuffer<float> *pedal;

void tachometer_Interrupt()
{
  rpm->update();
}

void setup()
{
  rpm = new RPMMetr;
  pedal = new RingBuffer<float>;

  attachInterrupt(digitalPinToInterrupt(PIN_TACHOMETER), tachometer_Interrupt, FALLING);

  //select direction for the first channel
  pinMode(PIN_DRIVER_DIR_CH1, OUTPUT); 
  digitalWrite(PIN_DRIVER_DIR_CH1, LOW);
  //and the seckond channel
  pinMode(PIN_DRIVER_DIR_CH2, OUTPUT); 
  digitalWrite(PIN_DRIVER_DIR_CH1, LOW);
  
  pinMode(PIN_DRIVER_SPEED_CH1, OUTPUT); 
  pinMode(PIN_DRIVER_SPEED_CH2, OUTPUT); 


#ifdef monitor
  Serial.begin(115200);
#endif
}


void loop()
{
  //обновляем значение на педали каждые 50мс
  if (millis() - time_to_update_A1 > 50)
  {
    pedal->update(analogRead(PIN_PEDAL));
    time_to_update_A1 = millis();
  }

  //проверяем не простаивает ли тахометр больше секунды
  if (millis() % 1000)
    rpm->check(micros());
  
  /*
    READY TO WRITE BUISNESS LOGIC
  */
  if (millis() - time_to_update_A1 > 50)
  {
    load_RPM = map(rpm->getRPM(127), Min_RPM, Max_RPM, 0, 255);
    load_PEDAL = map(pedal->getAverage(), Min_ValPedal, Max_ValPedal, 0, 255);
    analogWrite(PIN_DRIVER_SPEED_CH1, load_RPM - load_PEDAL);
    analogWrite(PIN_DRIVER_SPEED_CH2, load_RPM - load_PEDAL);
  }
 
  #ifdef monitor
    if (time_to_print - millis() > 40)
    {
      Serial.print("Value of RPM:");
      Serial.print(rpm->getRPM(1));
      Serial.print(rpm->getAverage());
      Serial.print("\nValue of pedal(in %):");
      Serial.print(pedal->getAverage() / 1023.f);
      time_to_print = millis();
    }
  #endif
}