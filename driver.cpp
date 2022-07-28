#include "driver.h"
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
