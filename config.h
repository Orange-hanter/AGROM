
#define PIN_PEDAL A1
#define PIN_TACHOMETER 2

#define PIN_USE_CH_1 8
#define PIN_USE_CH_2 9

#define PIN_DRIVER_SPEED_CH1 5
#define PIN_DRIVER_DIR_CH1 4
#define PIN_DRIVER_SPEED_CH2 6
#define PIN_DRIVER_DIR_CH2 7

#define Min_RPM 0
#define Max_RPM 1500
#define COUNT_OF_BLADES 126

#define Min_ValPedal 1.0
#define Max_ValPedal 2.0


volatile long time_front = 0;
volatile long time_to_print = 0;
volatile long time_to_update_A1 = 0;
volatile long time_to_update_channel = 0;

volatile int load_RPM = 0;
volatile int load_PEDAL = 0;
int diff = 0;


const float voltageMultiplyer = 5.0/1023.0;