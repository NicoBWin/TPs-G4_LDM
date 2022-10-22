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
#include "math.h"
#include "../headers/FSKd.h"
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
static int delta0 ;
static int delta1 ;
static float *seno_base;
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
  int fmin = 0;
  f1 = f1_xd;
  f0 = f0_xd;
  if (f0 > f1)
  {
    fmax = f0;
    fmin = f1;
  }
  else
  {
    fmax = f1;
    fmin = f0;
  }
  fs = resolucion * fmax;
  fb = maximo_comun_divisor(f1,f0);
  delta0 = f0 / fb;
  delta1 = f1 / fb;
  nsamples_base = (1 / fb) * fs;
  Ts = 1 / fs;
  seno_base = (float*)malloc(nsamples_base * sizeof( float ));
  for( int i = 0; i < nsamples_base; i++)
  {
    seno_base[i] = (float)scaling_factor*sin(2 * M_PI * fb * Ts * i);
  }

}
void modulate(int bitstream)
{
  int nsample = fs * TBIT*pow(10, -6) * 8 ;
  float *fsk_msg = (float*)malloc(nsample * sizeof(float));
  int i = 0, deltatemp;
  int counter = 0;
  
  for (int j = 0; j<11; j++)
  {
    if(bitstream>>j & (0b00000001)) 
    {
      deltatemp = delta1;
    }
    else 
    {
      deltatemp = delta0;
    }
    for (; i < nsamples_base/8 * (j+1); i++)
    {
      fsk_msg[i] = seno_base[counter];
      if ((counter + deltatemp) < nsamples_base)
      {
        counter += deltatemp;
      }
      else
      {
        counter = nsamples_base - counter;
      }
    }
  }
  
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
