/***************************************************************************//**
  @file     sound.h
  @brief    Timer driver. Advance implementation
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _SOUND_H_
#define _SOUND_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ff.h"

#include "board.h"
#include "pin_mux.h"
#include "fsl_sysmpu.h"
#include "clock_config.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void SD_ReadSongs(void);

uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize);

void play_file(char *mp3_fname);

/*******************************************************************************
 ******************************************************************************/

#endif // _SOUND_H_
