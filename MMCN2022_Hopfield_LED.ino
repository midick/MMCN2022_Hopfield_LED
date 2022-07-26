/****************************************************************************************************************************
  Argument_None.ino
  For Arduino and Adadruit AVR 328(P) and 32u4 boards
  Written by Khoi Hoang
  Built by Khoi Hoang https://github.com/khoih-prog/TimerInterrupt
  Licensed under MIT license
  Now we can use these new 16 ISR-based timers, while consuming only 1 hardware Timer.
  Their independently-selected, maximum interval is practically unlimited (limited only by unsigned long miliseconds)
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
  Notes:
  Special design is necessary to share data between interrupt code and the rest of your program.
  Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
  variable can not spontaneously change. Because your function may change variables while your program is using them,
  the compiler needs this hint. But volatile alone is often not enough.
  When accessing shared variables, usually interrupts must be disabled. Even with volatile,
  if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
  If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
  or the entire sequence of your code which accesses the data.
 *****************************************************************************************************************************/

// These define's must be placed at the beginning before #include "TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_1     true

#if ( defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)  || \
        defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_MINI) ||    defined(ARDUINO_AVR_ETHERNET) || \
        defined(ARDUINO_AVR_FIO) || defined(ARDUINO_AVR_BT)   || defined(ARDUINO_AVR_LILYPAD) || defined(ARDUINO_AVR_PRO)      || \
        defined(ARDUINO_AVR_NG) || defined(ARDUINO_AVR_UNO_WIFI_DEV_ED) || defined(ARDUINO_AVR_DUEMILANOVE) || defined(ARDUINO_AVR_FEATHER328P) || \
        defined(ARDUINO_AVR_METRO) || defined(ARDUINO_AVR_PROTRINKET5) || defined(ARDUINO_AVR_PROTRINKET3) || defined(ARDUINO_AVR_PROTRINKET5FTDI) || \
        defined(ARDUINO_AVR_PROTRINKET3FTDI) )
  #define USE_TIMER_2     true
  #warning Using Timer1, Timer2
#else          
  #define USE_TIMER_3     true
  #warning Using Timer1, Timer3
#endif

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "TimerInterrupt.h"

#define ROW_INTERVAL_MS     10
#define UPDATE_INTERVAL_MS     1000
#define COLUMNS 2
#define ROWS 2

#define ROW_1 5
#define ROW_2 6

#define COL_1 10
#define COL_2 11


volatile bool M[COLUMNS][ROWS];

void RowHandler(void)
{
  static int column = 0;
  
  digitalWrite(COL_1, 1);
  digitalWrite(COL_2, 1);

  if (column == 0) digitalWrite(COL_1, 0);
  if (column == 1) digitalWrite(COL_2, 0);
  
  digitalWrite(ROW_1, M[column][0]);
  digitalWrite(ROW_2, M[column][1]);
  column = (column + 1) % COLUMNS;
}


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
