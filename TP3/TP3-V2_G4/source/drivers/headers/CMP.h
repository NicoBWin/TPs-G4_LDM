/***************************************************************************/ /**
  @file     CMP.h
  @brief    CMP
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/
#ifndef _CMP_H_
#define _CMP_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define DAC_VIN 3.33
#define DAC_VOUT 1.65
#define VOUT_SELECT (DAC_VOUT/(DAC_VIN/64)-1)


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum{
    FILTER_SAMPLE_0,
    FILTER_SAMPLE_1,
    FILTER_SAMPLE_2,
    FILTER_SAMPLE_3,
    FILTER_SAMPLE_4,
    FILTER_SAMPLE_5,
    FILTER_SAMPLE_6,
    FILTER_SAMPLE_7

}Filter_CNT_t;

typedef enum{
  HYSTCTR_0,
  HYSTCTR_1,
  HYSTCTR_2,
  HYSTCTR_3
}Hyst_CTR_t;

typedef struct{
  Filter_CNT_t Filter_CNT;
  Hyst_CTR_t Hyst_CTR;
}CMP_CR0_t;

typedef struct{
  bool Sample_Enable;
  bool Windowig_Enable;
  bool Power_Mode;
  bool Invert;
  bool Output;
  bool Output_Pin_Enable;
  bool Module_Enable;

}CMP_CR1_t;

typedef struct{
  bool DMAEN;
  bool IER;
  bool IEF;
  bool CFR;
  bool CFF;
  bool COUT;

}CMP_SCR_t;

typedef enum{
    Vin1,
    Vin2
}DAC_Vref_t;

typedef struct{
  bool DACEN;
  DAC_Vref_t VRSEL; //Vref
  uint8_t VOSEL;
}CMP_DACCR_t;

typedef enum{
    IN0,
    IN1,
    IN2,
    IN3,
    IN4,
    IN5,
    IN6,
    IN7
}Mux_input_t;

typedef struct{
  bool PSTM;
  Mux_input_t PSEL;
  Mux_input_t MSEL;

}CMP_MUXCR_t; 

typedef struct{
  CMP_CR0_t CR_0;
  CMP_CR1_t CR_1;
  CMP_SCR_t S_CR;
  CMP_DACCR_t DAC_CR;
  CMP_MUXCR_t MUX_CR;

}CMP_config_t;

typedef enum{
    CMP_0,
    CMP_1,
    CMP_2
}CMP_X_t;

typedef enum{
    FTM1_CH0,
    FTM2_CH0
}CMP_output;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void CMP_init(CMP_X_t n);
void CMP_set_MUXinput (Mux_input_t p_input, Mux_input_t m_input, CMP_X_t n);
void CMP_set_output(CMP_output output);
void Prueba_CMP(void);



/*******************************************************************************
 ******************************************************************************/

#endif // _CMP_H_






