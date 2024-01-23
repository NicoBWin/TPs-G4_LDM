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
#include <stdlib.h>
#include <stdbool.h>

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
// VARIABLE GLOBAL BORRAR LUEGO


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
uint8_t SD_ReadSongs(uint8_t mp3_files[50][16]);

uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize);

void play_file(char *mp3_fname, uint8_t vol);

void pauseSound(void);

void resumeSound(void);

void getSongName(uint8_t Name[1][16]);

void On_Off_equalizer(int on);

int GetOnOffEq();

void setUpCascadeFilter(char* GaindB);

/*******************************************************************************
 ******************************************************************************/

#endif // _SOUND_H_
