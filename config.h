
#define PIN_PEDAL A1
#define PIN_TACHOMETER 2

#define PIN_DRIVER_SPEED_CH1 5
#define PIN_DRIVER_DIR_CH1 4
#define PIN_DRIVER_SPEED_CH2 6
#define PIN_DRIVER_DIR_CH2 7

#define Min_RPM 700
#define Max_RPM 1300

#define Min_ValPedal 0
#define Max_ValPedal 5

#define monitor //turn on monitoring via USB

volatile long time_front = 0;
volatile long time_to_print = 0;
volatile long time_to_update_A1 = 0;

volatile int load_RPM = 0;
volatile int load_PEDAL = 0;
