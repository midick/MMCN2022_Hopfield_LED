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

#define ROW_INTERVAL_MS     2
#define UPDATE_INTERVAL_MS     500
#define COLUMNS 8
#define ROWS 8


/* the rows need resistors */
#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9

#define COL_1 10
#define COL_2 11
#define COL_3 12
#define COL_4 13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3




volatile bool M[COLUMNS][ROWS];


bool one_led_first_columns_then_rows(void)
{
    static int row = 0;
    static int column = 0;
    static bool first_round = true;

    if (first_round){
        first_round = false;
        M[0][0] = 1;
        return false;
    }
    M[column][row] = 0;
    column = (column + 1) % COLUMNS;
    if (column == 0) row = row + 1;
    if (row == ROWS){
        row = 0;
        column = 0;
        first_round = true;
        return true;
    }
    M[column][row] = 1;
    return false;

}

bool all_on_or_off(bool state)
{
    for (int i = 0; i<= ROWS; i++){
        for (int j = 0; j<= COLUMNS; j++){
          M[j][i] = state;
        }
    }
    return true;
}

bool square(int start_row, int start_col, int sidelength){
    all_on_or_off(0);
    for (int row = start_row; row <= start_row + sidelength; row ++){
            M[start_col][row] = 1;
            M[start_col + sidelength][row] = 1;
        }
    for (int col = start_col; col <= start_col + sidelength; col ++){
            M[col][start_row] = 1;
            M[col][start_row + sidelength] = 1;
    }
    return true;
}

bool centered_square(void){
    static int a = ROWS / 2 - 1;
    static int r = 1;
    square(a, a, r);
    if (a == 0) return true;
    a -=1;
    r += 2;
    return false;
}
        

bool identity(void){
    all_on_or_off(0);
    for (int i = 0; i< ROWS; i++){
        M[i][i] = 1;
    }
    return true;
}

void write_column(int column, int value){
    switch(column){
        case 0: digitalWrite(COL_1, value); break;
        case 1: digitalWrite(COL_2, value); break;
        case 2: digitalWrite(COL_3, value); break;
        case 3: digitalWrite(COL_4, value); break;
        case 4: digitalWrite(COL_5, value); break;
        case 5: digitalWrite(COL_6, value); break;
        case 6: digitalWrite(COL_7, value); break;
        case 7: digitalWrite(COL_8, value); break;
    } 
}

void RowHandler(void)
{
    static int column = 0;

    /* turn old column off */
    write_column(column, 1);

    column = (column + 1) % COLUMNS;

    /* write row data */

    if (ROWS >= 1 ) digitalWrite(ROW_1, M[column][0]);
    if (ROWS >= 2) digitalWrite(ROW_2, M[column][1]);
    if (ROWS >= 3) digitalWrite(ROW_3, M[column][2]);
    if (ROWS >= 4) digitalWrite(ROW_4, M[column][3]);
    if (ROWS >= 5) digitalWrite(ROW_5, M[column][4]);
    if (ROWS >= 6) digitalWrite(ROW_6, M[column][5]);
    if (ROWS >= 7) digitalWrite(ROW_7, M[column][6]);
    if (ROWS >= 8) digitalWrite(ROW_8, M[column][7]);
    /* turn new column on */
    write_column(column, 0);

}
void UpdateHandler(void)
{
    static int function = 0;
    int functions = 5;

    switch(function){
        case 0: if (one_led_first_columns_then_rows()) function += 1; break;
        case 1: if (one_led_first_columns_then_rows()) function += 1; break;
        case 4: if (all_on_or_off(1)) function += 1; break;
        case 2: if (identity()) function += 1; break;
        case 3: if (centered_square()) function += 1; break;
        /* case 3: if (all_on_or_off(1)) function += 1; break; */
    }
    function %= functions;
}

void setup()
{
  for (byte i = 2; i <= 13; i++)
    pinMode(i, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

    digitalWrite(COL_1, 1);
    digitalWrite(COL_2, 1);
    digitalWrite(COL_3, 1);
    digitalWrite(COL_4, 1);
    digitalWrite(COL_5, 1);
    digitalWrite(COL_6, 1);
    digitalWrite(COL_7, 1);
    digitalWrite(COL_8, 1);

  for (int i = 0; i<= ROWS; i++){
        for (int j = 0; j<= COLUMNS; j++){
          M[j][i] = 1;
        }
    }
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
