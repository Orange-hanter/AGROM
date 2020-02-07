#include <Arduino.h>
#include "RingBuffer.hpp"

#define PIN_PEDAL A1
#define PIN_TACHOMETER 2

#define PIN_DRIVER_SPEED_CH1 5
#define PIN_DRIVER_DIR_1 4
#define PIN_DRIVER_SPEED_2 6
#define PIN_DRIVER_DIR_2 7

#define Min_RPM 700
#define Max_RPM 1300

#define Min_ValPedal 0
#define Max_ValPedal 5

#define monitor

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

#ifdef monitor
  Serial.begin(115200);
#endif
}
volatile long time_front = 0;
volatile long time_to_print = 0;
volatile long time_to_update_A1 = 0;

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
