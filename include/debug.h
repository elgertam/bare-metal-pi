#pragma once

#include "printf.h"

#define DEBUG(format, var) printf("\tDEBUG\t"    \
                                    "%s:%d\t"    \
                                    "%s()\t"     \
                                     #var ":\t"  \
                                     format "\n" \
                                 , __FILE__, __LINE__, __func__, var)
