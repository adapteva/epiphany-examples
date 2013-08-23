#ifndef _ERMLIB_H_
#define _ERMLIB_H_

int e_reserve(unsigned search_mode, unsigned rows, unsigned cols, unsigned *p_row, unsigned *p_col);
int e_release(unsigned rows, unsigned cols, unsigned row, unsigned col);


#endif /* _ERMLIB_H_ */
