/***************************************************************************//**
  @file     PIT.h
  @brief    Header encoder
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _PIT_H_
#define _PIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

//OS
#include <os.h>


void PIT_init(uint32_t frec, int channel, _Bool chained, OS_SEM *PitSem);


#endif /* PIT_H */
