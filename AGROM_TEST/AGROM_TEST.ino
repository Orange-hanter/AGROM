#include <Arduino.h>
//***************************************************************************************************************
//                          ~~~~~~~~~~~DEFINES~~~~~~~~~~~
//***************************************************************************************************************

#define PIN_PEDAL A1
#define PIN_TACHOMETER 2

#define COUNT_OF_BLADES 1
#define Min_RPM 700
#define Max_RPM 1300

#define Min_ValPedal 0
#define Max_ValPedal 5

#define monitor
#define TEST_TACHOMETER_OFF
#define TEST_PEDAL_OFF

//***************************************************************************************************************
//                          ~~~~~~~~~~~DEFINES END~~~~~~~~~~~
//***************************************************************************************************************

int pinSignal = 8;
volatile long time_front = 0;
volatile long time_to_print = 0;
volatile long time_to_update = 0;

//***************************************************************************************************************
//                          ~~~~~~~~~~~CLASSES~~~~~~~~~~~
//***************************************************************************************************************

template <typename T>
class RingBuffer
{
private:
  volatile T data[5];
  volatile T prevValue;
  short pos;

public:
  RingBuffer()
  {
    for (size_t i = 0; i < 5; ++i)
      data[i] = 0;
    pos = 0;
    prevValue = 0;
  };
  ~RingBuffer();

  void update(T value)
  {
    data[(pos + 1) % 5] = value;
    pos = ++pos % 5;

    /*  Serial.print(data[0]);
      Serial.print(" ");
            Serial.print(data[1]);
      Serial.print(" ");
            Serial.print(data[2]);
      Serial.print(" ");
            Serial.print(data[3]);
      Serial.print(" ");
            Serial.print(data[4]);
      Serial.println();
      */
  };

  void update(/*time in micros*/)
  {
    short next_pos = (pos + 1) % 5;
    data[next_pos] = micros() - prevValue;
    prevValue = micros();
    pos = next_pos;
  }

  T getAverage()
  {
    return (data[0] + data[1] + data[2] + data[3] + data[4]) / 5.f;
  };

  void check(long timeNow)
  {
    bool tooLong = (timeNow - data[(pos - 1) % 5]) > 10000; //TODO confirm that this value (time between signals) not to
    if (tooLong)
      for (size_t i = 0; i < 5; i++)
        data[i] = 0;
  }

  T operator[](short index)
  {
    return this->data[index];
  }

  T getElement(int index)
  {
    return this->data[index];
  }
};

class RPMMetr : public RingBuffer<long>
{
public:
  float getRPM()
  {
    long calc = (this->getAverage() * COUNT_OF_BLADES);
    return calc == 0 ? 0 : (60000000.f / calc); //TODO aproof math of calculation RPM
  }
};

//***************************************************************************************************************
//                          ~~~~~~~~~~~CLASSES END~~~~~~~~~~~
//***************************************************************************************************************

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

#ifdef TEST_TACHOMETER
  pinMode(pinSignal, OUTPUT);
  digitalWrite(pinSignal, LOW);
#endif

#ifdef TEST_PEDAL
  pinMode(45, OUTPUT);
  analogWrite(45, 22);
#endif

#ifdef monitor
  Serial.begin(115200);
#endif
}


void loop()
{

#ifdef TEST_PEDAL
  if (millis() - time_to_update > 200)
  {
    pedal->update(analogRead(PIN_PEDAL));
    time_to_update = millis();
  }

#endif

#ifdef TEST_TACHOMETER

  if (digitalRead(pinSignal) == LOW)
  {
    digitalWrite(pinSignal, HIGH);
    time_front = millis();
  }
  if (digitalRead(pinSignal) == HIGH && (millis() - time_front) > 500)
  {
    digitalWrite(pinSignal, LOW);
    delay(1);
  }
#endif

#ifdef monitor
  if (millis() - time_to_print > 4000)
  {
    Serial.print("Value of RPM:");
    Serial.print(rpm->getRPM());
    Serial.print(" ");
    Serial.print("\nValue of pedal(in %):");
    Serial.print(pedal->getAverage());
    Serial.println();
    time_to_print = millis();
  }
#endif
}
