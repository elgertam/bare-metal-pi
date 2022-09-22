#include "common.h"

u32 common_truncate_arr_ptr(void * somenum) {
  void * ptr = somenum;
  split_ptr * a_split_ptr = (split_ptr *)(&ptr);
  return a_split_ptr->bottom;
}
