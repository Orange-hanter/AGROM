
#define COUNT_OF_BLADES 7


#define PIN_PEDAL 11         //TODO Обязательно поменять номер пинов!
#define PIN_TACHOMETER 2


class RotatePerMinutes
{
private:
  volatile long data[5];
  short pos;

public:
  RotatePerMinutes();
  ~RotatePerMinutes();

  void update(long value);
  void update(/*time*/);
  long getAverage();
  float getRPM();
  void check(long timeNow);
};

RotatePerMinutes::RotatePerMinutes()
{
  pos = 0;
  for (size_t i = 0; i < 5; i++)
    data[i] = 0;
}

RotatePerMinutes::~RotatePerMinutes() {}

void RotatePerMinutes::update()
{
  data[(pos + 1) % 5] = micros() - data[pos];
  pos = pos++ % 5;
}

long RotatePerMinutes::getAverage()
{
  return (data[0] + data[1] + data[2] + data[3] + data[4]) / 5;
}

float RotatePerMinutes::getRPM()
{
  return 60.f * (1000000.f / (this->getAverage() * COUNT_OF_BLADES)); //TODO aproof math of calculation RPM
}

void RotatePerMinutes::check(long timeNow)
{
  bool tooLong = (timeNow - data[(pos-1)%5]) > 10000; //TODO confirm that this value (time between signals) not to 
  if(tooLong)
    for (size_t i = 0; i < 5; i++)
      data[i] = 0;
}

class mySmoothing
{
private:
  volatile float data[5];
  short pos;

public:
  mySmoothing();
  ~mySmoothing();

  void update(float value);
  float getAverage();
};

mySmoothing::mySmoothing()
{
  pos = 0;
  for (size_t i = 0; i < 5; i++)
    data[i] = 0;
}

void mySmoothing::update(float value)
{
  data[(pos + 1) % 5] = value - data[pos];
  pos = pos++ % 5;
}

float mySmoothing::getAverage()
{
  return (data[0] + data[1] + data[2] + data[3] + data[4]) / 5.f;
}

RotatePerMinutes *rpm;
mySmoothing *pedal;


void tachometer_Interrupt()
{
  rpm->update();
}

void setup()
{
  rpm = new RotatePerMinutes;
  pedal = new mySmoothing;
  attachInterrupt(digitalPinToInterrupt(PIN_TACHOMETER), tachometer_Interrupt, FALLING);
  Serial.begin(115200);

}

void loop()
{
  pedal->update(analogRead(PIN_PEDAL));
  if (millis()%1000)
    rpm->check(micros());
  

 /* 
  Serial.print("Value of RPM:");
  Serial.print(rpm->getRPM());
  Serial.print("\nValue of pedal(in %):");
  Serial.print(pedal->getAverage() / 1023.f);
  */
  delay(10);
}
