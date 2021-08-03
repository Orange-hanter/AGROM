using pin_t = const int;


pin_t p_B1 = 12;
pin_t p_B2 = 11;
pin_t p_B3 = 10;
pin_t p_V = 9;
pin_t p_R = 8;
pin_t p_AS = 7;
pin_t p_KD = 6;
pin_t p_TMP = 2;

pin_t inputs[] = {p_B1, p_B2, p_B3, p_V, p_R, p_AS, p_KD, p_TMP};
const auto inputs_count = sizeof(inputs)/sizeof(pin_t);

pin_t p_M1 = A0;//14;  // A0
pin_t p_M2 = A1;//15;  // A1
pin_t p_M3 = A2;//16;  // A2
pin_t p_M4 = A3;//17;  // A3
pin_t p_M5 = A4;//18;  // A4

pin_t outputs[] = {p_M1, p_M2, p_M3, p_M4, p_M5};
const auto outputs_count = sizeof(outputs)/sizeof(pin_t);

/*    B1  B2  B3  V R AS
* F1  1   0   1   1 0 0
* F2  0   0   1   1 0 0
* F3  0   1   1   1 0 0
* F4  1   1   1   1 0 0
*
* R1  1   0   1   0 1 0
* R2  0   0   1   0 1 0
* R3  0   1   1   0 1 0
* R4  1   1   1   0 1 0
*
* N1  1   0   1   0 0 1
* N2  0   0   1   0 0 1
* N3  0   1   1   0 0 1
* N4  1   1   1   0 0 1
*/
typedef enum _MODE {
  F1 = 0b00001101,
  F2 = 0b00001100,
  F3 = 0b00001110,
  F4 = 0b00001111,
  
  R1 = 0b00010101,
  R2 = 0b00010100,
  R3 = 0b00010110,
  R4 = 0b00010111,

  N1 = 0b00100101,
  N2 = 0b00100100,
  N3 = 0b00100110,
  N4 = 0b00100111,
  NA = 0b00111000,
} ;
static _MODE MODE, P_MODE;

enum _STATE{
  INIT,
  MONITORING,
  CALC_MODE,
  CHECK_MODE,
  SWITCH_MODE
} static STATE;