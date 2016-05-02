// Test internal.ldf

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e_lib.h"

extern int _PROG_SIZE_FOR_CORE_;
extern int _HEAP_SIZE_FOR_CORE_;
extern int _HALF_BANK_SIZE_;
extern int _NUM_ROWS_IN_CHIP_;
extern int _NUM_COLS_IN_CHIP_;
extern int _FIRST_CORE_ROW_;
extern int _FIRST_CORE_COL_;
extern int _CORE_ROW_;
extern int _CORE_COL_;
extern int _CORE_NUM_;
extern int _SHARED_DRAM_;

int main(void)
{
    return
		((int) &_PROG_SIZE_FOR_CORE_ ) +
		((int) &_HEAP_SIZE_FOR_CORE_ ) +
		((int) &_HALF_BANK_SIZE_) +
		((int) &_NUM_ROWS_IN_CHIP_) +
		((int) &_NUM_COLS_IN_CHIP_) +
		((int) &_FIRST_CORE_ROW_) +
		((int) &_FIRST_CORE_COL_) +
		((int) &_CORE_ROW_) +
		((int) &_CORE_COL_) +
		((int) &_CORE_NUM_) +
		((int) &_SHARED_DRAM_);
}
