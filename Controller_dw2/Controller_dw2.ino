#include "Consts.hpp"

/*
для обновления прошивки выполни в консоли 2 команды
	cd wental/Documents/Arduino/AGROM/Controller_dw2/
	git pull
*/

int getInputMask();
void printInputStatus();
int getInputValue(int i);
void printMode(_MODE mode);
void setRelayPattern(int M1, int M2, int M3, int M4, int M5);
void cmdReprocess();
uint32_t OMASK(uint32_t i = 0);

void setup() {
  Serial.begin(115200);

  for (auto input_pin : inputs)
    pinMode(input_pin, INPUT_PULLUP);

  for (auto output : outputs) {
    pinMode(output, OUTPUT);
    digitalWrite(output, LOW);
  }
  printInputStatus();

  STATE = _STATE::INIT;
  P_MODE = _MODE::NA;
}

void loop() {

  switch (STATE) {
    case _STATE::INIT:
      {
        STATE = _STATE::MONITORING;
        Serial.println("SYSTEM STARTED");
        break;
      }

    case _STATE::MONITORING:
      {
        cmdReprocess();
        MODE = calculateMode(getInputMask());
        //if(MODE == _MODE::NA)
        //  break;
        if (P_MODE != MODE) {
          Serial.println("STARTED MONITORING UPDATE");
          delay(200);
          STATE = _STATE::CALC_MODE;
        }
        break;
      }

    case _STATE::CALC_MODE:
      {
        Serial.println("CALC MODE");
        MODE = calculateMode(getInputMask());
        printMode(MODE);
        STATE = _STATE::CHECK_MODE;
        break;
      }
    case _STATE::CHECK_MODE:
    {
      Serial.println("CHECK MODE");
      if(true)
      {
        STATE = _STATE::SWITCH_MODE;
      }
      else
      {
         STATE = _STATE::MONITORING;
      }
      P_MODE = MODE;
      break;
    }
    case _STATE::SWITCH_MODE:
      {
        Serial.println("SWITCH MODE");
        switch (MODE) {
          case F1: setRelayPattern(0, 1, 1, 1, 0); OMASK(0b01110); break;
          case F2: setRelayPattern(0, 0, 1, 1, 0); OMASK(0b00110); break;
          case F3: setRelayPattern(0, 0, 1, 0, 0); OMASK(0b00100); break;
          case F4: setRelayPattern(0, 0, 0, 0, 1); OMASK(0b00001); break;

          case R1: setRelayPattern(1, 1, 0, 1, 0); OMASK(0b11010); break;
          case R2: setRelayPattern(1, 0, 0, 1, 0); OMASK(0b10010); break;
          case R3: 
          case R4: setRelayPattern(1, 0, 0, 0, 0); OMASK(0b10000); break;

          case N1: 
          case N2: 
          case N3:
          case N4: setRelayPattern(0, 1, 0, 1, 0); OMASK(0b01010); break;
          //case N2: setRelayPattern(0, 0, 0, 1, 0); OMASK(0b00010); break;
          //case N3: case N4: setRelayPattern(0, 0, 0, 0, 0); break;
          default: 
            setRelayPattern(0, 0, 0, 0, 0); OMASK(0b00000);
            Serial.println("ERROR: MODE NOT DEFINED!");
        };

        STATE = _STATE::MONITORING;
        Serial.println(" ");
        break;
      }

    default:;
  };

  //BUISNESS LOGIK

  delay(100);
}

_MODE calculateMode(int inputMask) {
  _MODE newMode;
  switch (inputMask) {
    case 0b001101: newMode = F1; break;
    case 0b001100: newMode = F2; break;
    case 0b001110: newMode = F3; break;
    case 0b001111: newMode = F4; break;

    case 0b010101: newMode = R1; break;
    case 0b010100: newMode = R2; break;
    case 0b010110: newMode = R3; break;
    case 0b010111: newMode = R4; break;

    case 0b100101: newMode = N1; break;
    case 0b100100: newMode = N2; break;
    case 0b100110: newMode = N3; break;
    case 0b100111: newMode = N4; break;

    default: newMode = NA;
  };
  return newMode;
}

int getInputMask() {
  auto result = 0;
  for (auto i = 0; i < inputs_count; ++i) {
    result |= (!getInputValue(i) & 0b00000001) << i;
  }
  return result;
}

void printInputStatus() {
  for (auto i = 0; i < inputs_count; ++i) {
    Serial.print("D");
    Serial.print(inputs[i]);
    Serial.print("=");
    Serial.print(getInputValue(i));
    Serial.print("\t");
  }
  Serial.println("");
}

int getInputValue(int i) { return digitalRead(inputs[i]); }

void printMode(_MODE mode) {
  switch (mode) {
    case F1: Serial.println("Mode F1 selected"); break;
    case F2: Serial.println("Mode F2 selected"); break;
    case F3: Serial.println("Mode F3 selected"); break;
    case F4: Serial.println("Mode F4 selected"); break;
    case R1: Serial.println("Mode R1 selected"); break;
    case R2: Serial.println("Mode R2 selected"); break;
    case R3: Serial.println("Mode R3 selected"); break;
    case R4: Serial.println("Mode R4 selected"); break;
    case N1: Serial.println("Mode N1 selected"); break;
    case N2: Serial.println("Mode N2 selected"); break;
    case N3: Serial.println("Mode N3 selected"); break;
    case N4: Serial.println("Mode N4 selected"); break;
    default: Serial.println("ERROR: MODE NOT DEFINED!");
  }
}

void setRelayPattern(int M1, int M2, int M3, int M4, int M5) {
  Serial.print("Relay mask:");
  Serial.print(M1);
  Serial.print(M2);
  Serial.print(M3);
  Serial.print(M4);
  Serial.println(M5);
  digitalWrite(p_M1, M1);
  digitalWrite(p_M2, M2);
  digitalWrite(p_M3, M3);
  digitalWrite(p_M4, M4);
  digitalWrite(p_M5, M5);
}

void cmdReprocess(){
   if (Serial.available() > 0) 
   {
     // get incoming byte:
     auto inByte = Serial.read();
     if (inByte == 49) //show DI status
      printInputStatus();
     if(inByte == 50) // show current Mode
     {
        Serial.print("Currently selected:");
        printMode(MODE);
     }
     if(inByte == 51) // show current IMASC
     {
        Serial.print("Input mask:");
        Serial.println(getInputMask(), BIN);
     }
     if(inByte == 52) // show current OMASC
     {
        Serial.print("Onput mask:");
        Serial.println(OMASK(), BIN);
     }
   }
}


uint32_t OMASK(uint32_t i)
{
    static uint32_t mask = 0;
    if(i != 0)
        mask = i;
    return mask;
}
