/***************************************************************************//**
  @file     template.c
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

// +Incluir el header propio (ej: #include "template.h")+
#define _USE_MATH_DEFINES
#include <math.h>
#include "../headers/FSKd.h"
#include "../headers/circularbuffer.h"
#include "../headers/uart.h"
#include "../headers/filter.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
int maximo_comun_divisor(int a, int b);
// +ej: static void falta_envido (int);+

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static uint32_t f1;
static uint32_t f0;
static int delta[2];
static uint16_t *seno_base;
static int fs;
static int fb;
static float Ts;
static int nsamples_base;
// +ej: static int temperaturas_actuales[4];+

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void setup_params(uint32_t f1_xd, uint32_t f0_xd, uint16_t resolucion, uint16_t scaling_factor)
{
  int fmax=0;
  f1 = f1_xd;
  f0 = f0_xd;
  if (f0 > f1)
  {
    fmax = f0;
  }
  else
  {
    fmax = f1;
  }
 // fs = resolucion * fmax;
  fs = 100000;
  fb = maximo_comun_divisor(f1,f0);
  delta[0] = f0 / fb;
  delta[1] = f1 / fb;
  nsamples_base = (1.0 / (float)fb) * fs;
  Ts = 1.0 / (float)fs;
  seno_base = (uint16_t*)malloc(nsamples_base * sizeof( uint16_t ));
  for( int i = 0; i < nsamples_base; i++)
  {
    seno_base[i] = (uint16_t)(((float)(scaling_factor/2)) * ((sin(2 * M_PI * fb * Ts * i)) + 1.0));
  }

}
uint16_t FSK_get_sample(_Bool bit)
{
//static int nsample = fs * TBIT*pow(10, -6);
//  uint16_t testin[100];
//  	uint16_t fsk_msg[13];
//  uint16_t *fsk_msg = (uint16_t*)malloc(nsample * sizeof(uint16_t));
  static int counter = 0;
  counter = (counter + delta[bit]) % nsamples_base ;
  return seno_base[counter];

//       testin[i]=fsk_msg[i];


    /*
    double output[13];
    double test[90];
    double* xdd = delay_filter(fsk_msg, output);
    for(i=0; i < nsample; i++)
    {

       test[i]= output[i];
    }
    double* xdd_es_esta_pa = delay_filter(fsk_msg, output);
    for(i=13; i < 2*nsample; i++)
    {
       test[i]= output[i-13];
    }

    double* xdd_v3 = delay_filter(fsk_msg, output);
    for(i=26; i < 3*nsample; i++)
    {
       test[i]= output[i-26];
    }
    double* xdd_v4 = delay_filter(fsk_msg, output);

    for(i=39; i< 4*nsample; i++)
    {

       test[i]= output[i-39];

    }
    double* xdd_v5 = delay_filter(fsk_msg, output);

    for(i=52; i< 5*nsample; i++)
    {

       test[i]= output[i-52];

    }
*/
//    free(seno_base);

    //return fsk_msg;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
int maximo_comun_divisor(int a, int b)
{
  int temporal; // Para no perder b
  while (b != 0)
  {
    temporal = b;
    b = a % b;
    a = temporal;
  }
  return a;
}
