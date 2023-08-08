#ifndef __X9C104_H
#define __X9C104_H
#include "sys.h"

#define CS PAout(12)
#define UD PAout(11)
#define INC PAout(10)

void X9C104_Init(void);
void X9C104_Change(int8_t x);


#define ADCS PBout(7)
#define ADUD PBout(5)
#define ADINC PBout(6)

void AD620_Init(void);
void AD620_Change(int8_t x);
#endif
