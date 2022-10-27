#include "../headers/filter.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////
// Filter Code Definitions
void intToFloat(uint16_t *input, double *output, int length);
void firFloatInit(void);
double firFloat(double *input , int filterLength);
//////////////////////////////////////////////////////////////
// maximum number of inputs that can be handled
// in one function call
#define MAX_INPUT_LEN 1
// maximum length of filter than can be handled
#define MAX_FLT_LEN 13
// buffer to hold all of the input samples
#define BUFFER_LEN (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)
// Indica el TIME DELAY que se aplicará antes de ingresar el filtro
#define DELTA 7
// Order of the filter
#define FILTER_LEN 13
// array to hold input samples

#define SAMPLES 1

// FIR init
//#define FILTER_LEN 24

static double insamp[BUFFER_LEN];
static double m_t[BUFFER_LEN];
static double coeffs[FILTER_LEN] = {0.008345, 0.021582, 0.042506, 0.068126, 0.093315, 0.111863, 0.118693, 0.111863, 0.093315,
		0.068126, 0.042506, 0.021582, 0.008345};
//static double coeffs[FILTER_LEN] = {0.024785592,	0.042349523,	0.064952969,	0.082381851,	0.088927359,	0.082381851,
//									0.064952969,	0.042349523,	0.024785592 };


//static double coeffs[FILTER_LEN];
void firFloatInit(void)
{
    memset(insamp, 0, sizeof(insamp));
}
// the FIR filter function
double firFloat(double *input , int filterLength)
{
    static double acc = 0;     // accumulator for MACs
    double *pointer_m_t; // pointer to m(t)
    int k;
    static double* coeffp;
    // put the new samples at the high end of the buffer
    memcpy(&insamp[filterLength - 1], input,
           sizeof(double));
    // apply the filter to each new input sample

        // calculate output n
        coeffp = coeffs;
        //inputp = &insamp[filterLength - 1 ]; // I take the ORDERFILTER-1 previous term
        pointer_m_t = &m_t[filterLength - 1 ]; // I take the ORDERFILTER-1 previous term
        acc = 0;

        *pointer_m_t = insamp[filterLength - 1 ] * (insamp[filterLength - 1 - DELTA]); // Multiply x(N*Ts) * x(N*Ts - DELTA*Ts)


        for (k = 0; k < filterLength; k++)
        {
        	acc += (*coeffp++) * (*pointer_m_t--);
        }
    // shift input samples back in time for next time
    memmove(&insamp[0], &insamp[1],
            (filterLength - 1) * sizeof(double));
    memmove(&m_t[0], &m_t[1],
            (filterLength - 1) * sizeof(double));
    return acc;
}
//////////////////////////////////////////////////////////////
// Test program
//////////////////////////////////////////////////////////////
// Low pass fp= 1200Hz, 5db and fs=2400,  75db
// sampling rate = 15695 Hz orden 23


void intToFloat(uint16_t *input, double *output, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        output[i] = ( (double)input[i]/(double)((double)0xFFFF/2.0) ) -1 ;
    }
}

double delay_filter(uint16_t sampless) // Filtra de a 1 sample y devuelve el resultado del filtro
{
	//uint16_t sample[1] = {sampless};
	double floatinput = ( (double)sampless/(double)((double)0xFFFF/2.0) ) -1 ;
	//double floatOutput[1];
	return  firFloat( &floatinput , FILTER_LEN);
	//static double floatOutput[SAMPLES];
	//intToFloat(sample, floatinput, SAMPLES);
    //firFloat(coeffs, floatinput, floatOutput, SAMPLES , FILTER_LEN);

    //return floatOutput[0];
}
int comparator( double* floatOutput)
{
	double result = 0;
	for(int i = 8; i<SAMPLES; i++)
	{
		result += floatOutput[i]/(SAMPLES -8);
	}
	if ( result > 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
// number of samples to read per loop
