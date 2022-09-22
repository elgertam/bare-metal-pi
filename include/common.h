#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef volatile u8 reg8;
typedef volatile u16 reg16;
typedef volatile u32 reg32;
typedef volatile u64 reg64;

typedef struct {
  // bottom above top due to little-endianness
  u32 bottom;
  u32 top;
} split_ptr;

u32 common_truncate_arr_ptr(void * somenum);

#define SAFE_TRUNCATE(u64num) (((void *)u64num == (void *)(&u64num)) \
                                ? common_truncate_arr_ptr((void *)u64num) \
                                : (((split_ptr*)(&u64num))->bottom))


