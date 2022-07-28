// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "driver.h"
#include "TimerInterrupt.h"

void UpdateHandler(void)

{
  static int count = 0;
  M[0][0] = 0;
  M[1][0] = 0;
  M[0][1] = 0;
  M[1][1] = 0;
  if (count == 0){
    M[0][0] = 1;
    M[0][1] = 1;
  }
  if (count == 1){
    M[0][1] = 1;
    M[1][1] = 1;
  }
  if (count == 2){
    M[1][1] = 1;
    M[1][0] = 1;
  }
  if (count == 3){
    M[1][0] = 1;
    M[0][0] = 1;
  }
  if (count == 4){
    M[0][0] = 1;
    M[1][0] = 1;
    M[0][1] = 1;
    M[1][1] = 1;
  }
  if (count == 5){
    M[1][0] = 1;
    M[0][0] = 1;
    M[0][1] = 1;
  }
  if (count == 6){
    M[1][0] = 1;
    M[0][0] = 1;
    M[1][1] = 1;
  }
  if (count == 7){
    M[0][0] = 1;
  }
  count = (count + 1) % 8;
}

void setup()
{
  for (byte i = 2; i <= 13; i++)
    pinMode(i, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  
  M[0][0] = 0;
  M[1][0] = 1;
  M[0][1] = 1;
  M[1][1] = 1;
  Serial.begin(115200);
  while (!Serial);

  // Timer0 is used for micros(), millis(), delay(), etc and can't be used
  // Select Timer 1-2 for UNO, 1-5 for MEGA, 1,3,4 for 16u4/32u4
  // Timer 2 is 8-bit timer, only for higher frequency
  // Timer 4 of 16u4 and 32u4 is 8/10-bit timer, only for higher frequency

  // Select Timer 1-2 for UNO, 0-5 for MEGA, 1,3,4 for 32u4
  // Timer 2 is 8-bit timer, only for higher frequency
  ITimer2.init();

  ITimer2.attachInterruptInterval(ROW_INTERVAL_MS, RowHandler);

  ITimer1.init();

  ITimer1.attachInterruptInterval(UPDATE_INTERVAL_MS, UpdateHandler);

}

void loop()
{

}
