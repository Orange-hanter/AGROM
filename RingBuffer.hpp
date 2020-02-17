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
  };

  void update(/*time in micros*/)
  {
    short next_pos = (pos + 1) % 5;
    data[next_pos] = micros() - prevValue;
    prevValue = micros();
    pos = next_pos;

 /*  Serial.print(data[0]);
    Serial.print(" ");
    Serial.print(data[1]);
    Serial.print(" ");
    Serial.print(data[2]);
    Serial.print(" ");
    Serial.print(data[3]);
    Serial.print(" ");
    Serial.print(data[4]);
    Serial.println();*/
  }

  T getAverage()
  {
    return (data[0] + data[1] + data[2] + data[3] + data[4]) / 5.f;
  };

  T getMid()
  {
    T newData[5]= {data[0] ,data[1], data[2],  data[3], data[4]};
    
    for (size_t i = 0; i < 5; i++)
    {
      size_t buf = i;
      for (size_t j = i; j < 5; j++)
      {
        if( data[buf] > data[j] )
          buf = j;
      }
      if(buf != i)
      {
        T tmp = data[i];
        data[i] = data[buf];
        data[buf] = tmp;
      }
      
    }
    
    return newData[2];
  };

  void check()
  {
    bool tooLong = (micros() - this->prevValue) > 5000000; //TODO confirm that this value (time between signals) not to big or small
    if (tooLong)
      for (size_t i = 0; i < 5; i++)
        data[i] = 0;
  }
};

class RPMMetr : public RingBuffer<long>
{
public:
  float getRPM( int COUNT_OF_BLADES = 1)
  {
    long calc = (this->getMid() * COUNT_OF_BLADES);
    return calc == 0 ? 0 : (60000000.f / calc); //TODO aproof math of calculation RPM
  }
};
