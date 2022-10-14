
#ifndef DAC_H_
#define DAC_H_

#include "hardware.h"

typedef DAC_Type *DAC_t;
typedef uint16_t DACData_t;

void DAC_Init	 (void);

void DAC_SetData (DAC_t, DACData_t);

void DAC_PISR (void);

#endif /* void DAC_H_ */
