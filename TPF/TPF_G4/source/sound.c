/***************************************************************************/ /**
@file     sound.c
@brief    Some sound and SD funtions
@author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
@help	  https://github.com/vinodstanur/frdmk64f_mp3_player/tree/master
******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

// Main Lib
#include "sound.h"

// Fresscale Libs
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "diskio.h"

// MP3 dec lib
#include "mp3dec.h"

// Peripheral Drivers
#include "UI/Pdrivers/headers/DMA.h"
#include "UI/Pdrivers/headers/PIT.h"
#include "UI/Pdrivers/headers/DAC.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define FILE_READ_BUFFER_SIZE   (1024*16)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum play_e {eREPLAY, ePREVIOUS, eNEXT};

/*******************************************************************************
* Volatile
******************************************************************************/
volatile uint8_t forced_mono,bass_boosted,fast_forward;
volatile uint32_t r1,r2;
volatile uint8_t next, prev, replay, mute,ffd,reset, play, volume = 5;

/*******************************************************************************
* Prototypes
******************************************************************************/
static uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize);

static void RunDACsound(int sample_rate, int output_samples);

static void ProvideAudioBuffer(int16_t *samples, int cnt);

/*******************************************************************************
* Variables
******************************************************************************/
MP3FrameInfo	mp3FrameInfo;
HMP3Decoder     hMP3Decoder;
uint8_t 		read_buff[FILE_READ_BUFFER_SIZE];
uint32_t 		bytes_read;

char    *read_ptr;
int16_t pcm_buff[2304];
int16_t audio_buff[2304*2];




unsigned int br, btr;


int16_t *samples = pcm_buff;
/*******************************************************************************
* Code
******************************************************************************/
uint8_t SD_ReadSongs(uint8_t mp3_files[1000][15]){
	uint8_t 	mp3_total_files=0;
	uint8_t		mp3_file_index=0;

	static FATFS g_fileSystem; /* File system object */
	FRESULT error;
	DIR directory; /* Directory object */

	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};

	//printf("\r\nFATFS example to demonstrate how to use FATFS with SD card.\r\n");
	//PRINTF("\r\nPlease insert a card into board.\r\n");

	if (f_mount(&g_fileSystem, driverNumberBuffer, 0U)) {
		//PRINTF("Mount volume failed.\r\n");
		return -1;
	}

	#if (_FS_RPATH >= 2U)
	  error = f_chdrive((char const *)&driverNumberBuffer[0U]);
	  if (error) {
		//PRINTF("Change drive failed.\r\n");
		return -1;
	  }
	#endif

	//PRINTF("\r\nList the file in that directory......\r\n");
	if (f_opendir(&directory, "/")) {
		//PRINTF("Open directory failed.\r\n");
		return -1;
	}

	FILINFO files;
	FRESULT res;

	//char mp3_fname[50];
	//memset(mp3_fname, 0, 50);
	while(1) {
		res =  f_readdir(&directory, &files);
		if(res != FR_OK || strlen(files.fname) == 0) {
		  break;////f_opendir(&directory, "/");
		}

		if(strstr(files.fname, ".MP3")) {
			strcpy( mp3_files[mp3_file_index], files.fname );    //to save file names
			mp3_file_index++;
			mp3_total_files++;
		}
	}
	return mp3_total_files;
}


void play_file(char *mp3_fname) {
	/* PLay_File Vars */
	static FIL fil;        /* File object */
	static FRESULT fr;     /* FatFs return code */
	static int bytes_left;

	static int offset, err;
	static int outOfData;

	static uint8_t dac_started = 0;

	static bool fileOpen = false;

	static char *mp3_old_fname = NULL;
	if(*mp3_old_fname != *mp3_fname){
		mp3_old_fname = mp3_fname;
		bytes_left = 0;
		outOfData = 0;

		if(strlen(mp3_fname) == 0) {
			//PRINTF("No hay cancion pa");
			while(1);
		}

		if (fileOpen) {
			f_close(&fil);
			fileOpen = false;
		}

		if(f_open(&fil, mp3_fname, FA_READ) == FR_OK) {
			fileOpen = true;
		}
		else {
			//PRINTF("No funca el file read");
			while(1);
		}
		// Read ID3v2 Tag
		hMP3Decoder = MP3InitDecoder();

		char szArtist[120];
		char szTitle[120];
		Mp3ReadId3V2Tag(&fil, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle));
	}


	if( bytes_left < FILE_READ_BUFFER_SIZE/2 ) {
		memcpy( read_buff, read_ptr, bytes_left );
		read_ptr = read_buff;
		btr = FILE_READ_BUFFER_SIZE - bytes_left;

		// BLUE LED INDICATE FILE READING
		GPIO_WritePinOutput(GPIOB, BOARD_LED_BLUE_GPIO_PIN, 0);
		fr = f_read(&fil, read_buff + bytes_left, btr, &br);
		GPIO_WritePinOutput(GPIOB, BOARD_LED_BLUE_GPIO_PIN, 1);

		bytes_left = FILE_READ_BUFFER_SIZE;

		if(fr || br < btr) {
			f_close(&fil);
			return;//while(1);//change
		}
	}

	offset = MP3FindSyncWord((unsigned char*)read_ptr, bytes_left);
	if(offset == -1 ) {
		bytes_left = 0;
		return;
	}
	bytes_left -= offset;
	read_ptr += offset;

	err = MP3Decode(hMP3Decoder, (unsigned char**)&read_ptr, (int*)&bytes_left, samples, 0);
	if (err) {
		/* error occurred */
		switch (err) {
			case ERR_MP3_INDATA_UNDERFLOW:
				outOfData = 1;
			break;
			case ERR_MP3_MAINDATA_UNDERFLOW:
				/* do nothing - next call to decode will provide more mainData */
			break;
			case ERR_MP3_NULL_POINTER:
				bytes_left -=1;
				read_ptr+=1;
			case ERR_MP3_FREE_BITRATE_SYNC:
			default:
				outOfData = 1;
			break;
		}
	}
	else {
		// no error
		MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
		if(!dac_started) {
			dac_started = 1;
			RunDACsound(mp3FrameInfo.samprate, mp3FrameInfo.outputSamps);
			DAC_Enable(DAC_0, true);
		}
		// Duplicate data in case of mono to maintain playback speed
		if (mp3FrameInfo.nChans == 1) {
			for(int i = mp3FrameInfo.outputSamps;i >= 0;i--) {
				samples[2 * i]=samples[i];
				samples[2 * i + 1]=samples[i];
			}
			mp3FrameInfo.outputSamps *= 2;
		}
	}
	if (!outOfData) {
		ProvideAudioBuffer(samples, mp3FrameInfo.outputSamps);
	}
}


uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize) {
  pszArtist[0] = 0;
  pszTitle[0] = 0;

  BYTE id3hd[10];
  UINT unRead = 0;
  if((f_read(pInFile, id3hd, 10, &unRead) != FR_OK) || (unRead != 10)) {
    return 1;
  }
  else {
    uint32_t unSkip = 0;
    if((unRead == 10) &&
       (id3hd[0] == 'I') &&
         (id3hd[1] == 'D') &&
           (id3hd[2] == '3'))
    {
      unSkip += 10;
      unSkip = ((id3hd[6] & 0x7f) << 21) | ((id3hd[7] & 0x7f) << 14) | ((id3hd[8] & 0x7f) << 7) | (id3hd[9] & 0x7f);

      // try to get some information from the tag
      // skip the extended header, if present
      uint8_t unVersion = id3hd[3];
      if(id3hd[5] & 0x40) {
        BYTE exhd[4];
        f_read(pInFile, exhd, 4, &unRead);
        size_t unExHdrSkip = ((exhd[0] & 0x7f) << 21) | ((exhd[1] & 0x7f) << 14) | ((exhd[2] & 0x7f) << 7) | (exhd[3] & 0x7f);
        unExHdrSkip -= 4;
        if(f_lseek(pInFile, f_tell(pInFile) + unExHdrSkip) != FR_OK) {
          return 1;
        }
      }
      uint32_t nFramesToRead = 2;
      while(nFramesToRead > 0) {
        char frhd[10];
        if((f_read(pInFile, frhd, 10, &unRead) != FR_OK) || (unRead != 10)) {
          return 1;
        }
        if((frhd[0] == 0) || (strncmp(frhd, "3DI", 3) == 0)) {
          break;
        }
        char szFrameId[5] = {0, 0, 0, 0, 0};
        memcpy(szFrameId, frhd, 4);
        uint32_t unFrameSize = 0;
        uint32_t i = 0;
        for(; i < 4; i++) {
          if(unVersion == 3) {
            // ID3v2.3
            unFrameSize <<= 8;
            unFrameSize += frhd[i + 4];
          }
          if(unVersion == 4) {
            // ID3v2.4
            unFrameSize <<= 7;
            unFrameSize += frhd[i + 4] & 0x7F;
          }
        }

        if(strcmp(szFrameId, "TPE1") == 0) {
          // artist
          if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszArtist, unArtistSize) != 0) {
            break;
          }
          nFramesToRead--;
        }
        else if(strcmp(szFrameId, "TIT2") == 0) {
          // title
          if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszTitle, unTitleSize) != 0) {
            break;
          }
          nFramesToRead--;
        }
        else {
          if(f_lseek(pInFile, f_tell(pInFile) + unFrameSize) != FR_OK) {
            return 1;
          }
        }
      }
    }
    if(f_lseek(pInFile, unSkip) != FR_OK) {
      return 1;
    }
  }
  return 0;
}


void pauseSound(){
	PIT_Stop(PIT_CH1);
	// Usar funcion para cargar datos al DAC (ver DAC.h -> DAC_SetData)
	DAC0->DAT[0].DATH = 0x8U;
	DAC0->DAT[0].DATL = 0x00U;
}

void resumeSound() {
	PIT_Start(PIT_CH1);
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Used by Mp3ReadId3V2Tag
static uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize) {
  UINT unRead = 0;
  BYTE byEncoding = 0;
  if((f_read(pInFile, &byEncoding, 1, &unRead) == FR_OK) && (unRead == 1)) {
    unDataLen--;
    if(unDataLen <= (unBufferSize - 1)) {
      if((f_read(pInFile, pszBuffer, unDataLen, &unRead) == FR_OK) || (unRead == unDataLen)) {
        if(byEncoding == 0) {
          // ISO-8859-1 multibyte
          // just add a terminating zero
          pszBuffer[unDataLen] = 0;
        }
        else if(byEncoding == 1) {
          // UTF16LE unicode
          uint32_t r = 0;
          uint32_t w = 0;
          if((unDataLen > 2) && (pszBuffer[0] == 0xFF) && (pszBuffer[1] == 0xFE)) {
            // ignore BOM, assume LE
            r = 2;
          }
          for(; r < unDataLen; r += 2, w += 1) {
            // should be acceptable for 7 bit ascii
            pszBuffer[w] = pszBuffer[r];
          }
          pszBuffer[w] = 0;
        }
      }
      else {
        return 1;
      }
    }
    else {
      // we won't read a partial text
      if(f_lseek(pInFile, f_tell(pInFile) + unDataLen) != FR_OK) {
        return 1;
      }
    }
  }
  else {
    return 1;
  }
  return 0;
}


void RunDACsound(int sample_rate, int output_samples) {
	// DAC
	DAC_Init(DAC_0);

	// DMA Config
	DMA_config_t DMAConfigOutput = {.source_buffer = audio_buff, .destination_buffer = &(DAC0->DAT),
								 .request_source = DMAALWAYS63, .source_offset = sizeof(uint32_t), .destination_offset = 0x00,
								 .transfer_bytes = sizeof(uint16_t), .major_cycles = output_samples, .wrap_around = output_samples*4};
	DMA_Init(DMA_1, DMAConfigOutput);

  	// PIT Config
	PIT_Init(((CLOCK_GetFreq(kCLOCK_BusClk) / (sample_rate))), PIT_CH1, false);
	PIT_Start(PIT_CH1);
}

/* FILTRO -> NO ANDA POR AHORA
float a0 = 0.00005029912027879971;
float a1 = 0.00010059824055759942;
float a2 = 0.00005029912027879971;
float b1 = -1.9821252053783214;
float b2 = 0.9823264018594366;
*/
static void ProvideAudioBuffer(int16_t *samples, int cnt) {
  static float z1,z2;

  static uint8_t state = 0;

  int32_t tmp = 0;
  if(1) {
    for(int i = 0; i < cnt; i++) {
      if(i%2 == 0) {
        tmp =   samples[i] + samples[i+1];
        tmp /= 2;
        samples[i] = (int16_t)tmp * (int32_t)volume / 10;;
      }
    }
  }
  float w,out;//, in;
  //bass_boosted = 1;
  if(bass_boosted) {
	  /*
    for(int i = 0; i < cnt; i++) {
      if( i % 2 == 0) {
        w = (float)samples[i] - b1*z1 - b2*z2;
        out = a0*w + a1*z1 + a2*z2;
     //   z2 = z1;
     //   z1 = w;
      }
      samples[i] = (int16_t)out;
    }
    for(int i = 0; i < cnt; i++) {
      if( i % 2 == 0) {
        w = (float)samples[i] - b1_1*z1 - b2_1*z2;
        out = a0_1*w + a1*z1 + a2_1*z2;
     //   z2 = z1;
     //   z1 = w;
      }
      samples[i] = (int16_t)out;
    }

    for(int i = 0; i < cnt; i++) {
      if( i % 2 == 0) {
        w = (float)samples[i] - b1_2*z1 - b2_2*z2;
        out = a0_2*w + a1_2*z1 + a2_2*z2;
  //      z2 = z1;
    //    z1 = w;
      }
      samples[i] = (int16_t)out;
    }
    */
    for(int i = 0; i < cnt; i++) {
      if( i % 2 == 0) {
        //w = (float)samples[i] - b1_3*z1 - b2_3*z2;
        //out = a0_3*w + a1_3*z1 + a2_3*z2;
        z2 = z1;
        z1 = w;
      }
      samples[i] = (int16_t)out;
    }
  }
  if(state == 0) {
    r1 =  DMA_GetRemainingMajorLoopCount(DMA_1) - cnt/2;
    while( DMA_GetRemainingMajorLoopCount(DMA_1) > cnt/2 ) {
      if(fast_forward){
        goto end1;
      }
    }
    for(int i = 0; i < cnt; i++) {
      audio_buff[i] = *samples / 16;
      audio_buff[i] += (4096/2);
      samples++;
    }
    state = 1;
  end1:
    return;
  }

  if(state == 1) {
    r2 = DMA_GetRemainingMajorLoopCount(DMA_1);
    while( DMA_GetRemainingMajorLoopCount(DMA_1) < cnt/2 ) {
      if(fast_forward) {
        goto end2;
      }
    }
    for(int i = 0; i < cnt; i++) {
      audio_buff[i + cnt] = *samples / 16;
      audio_buff[i + cnt] += (4096/2);
      samples++;
    }
  end2:
    state = 0;
  }
}
