/***************************************************************************//**
  @file     gpio.c
  @brief    GPIO file
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "MK64F12.h"
#include "gpio.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static PORT_Type* port_base_pointer[]= PORT_BASE_PTRS;
static GPIO_Type* gpio_base_pointer[]= GPIO_BASE_ADDRS;
enum {DESABL,ENABL};



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint8_t mode)
{

	PORT_Type* port_pointer = port_base_pointer[ PIN2PORT(pin) ];	// Apunto al pcr del puerto correspondiente
	GPIO_Type* gpio_pointer = gpio_base_pointer[ PIN2PORT(pin) ];   // Apunto al GPIo del puerto correspondiente
	port_pointer->PCR[PIN2NUM(pin)]=0;
	SIM->SCGC5 |= 1<<(PIN2PORT(pin)+ CLK_GATING_OFFSET );	// Prendo el clock del puerto a configurar
	// Configuro el LOCK y el MUX
	port_pointer->PCR[PIN2NUM(pin)] &= ~(1<<PORT_PCR_LK_SHIFT);	// Desactiva el LOCK
	port_pointer->PCR[PIN2NUM(pin)] &= ~(3<< MUX_SHIFT_0);		// Configuro los ceros de mux
	port_pointer->PCR[PIN2NUM(pin)] |=  (1<< MUX_SHIFT_1);		// Configuro el 1 del mux para hacerlo gpio


	//**************************** Configuro el PCR ****************************//
	if(mode <=OUTPUT && mode >=INPUT)
	{
		port_pointer->PCR[PIN2NUM(pin)] &= ~(1<<1);	// Apago el Pull
	}
	else if(mode <=INPUT_PULLDOWN && mode >=INPUT_PULLUP)
	{
		port_pointer->PCR[PIN2NUM(pin)] |= 1<<1;	// Prendo el Pull
		if(mode == 2)
		{
			port_pointer->PCR[PIN2NUM(pin)] |= 1; // Configuro PulluP
		}
		else
		{
			port_pointer->PCR[PIN2NUM(pin)] &= ~1; // Configuro PullDown
		}
	}
	//**************************** Configuro el GPIO ****************************//

	if(mode == OUTPUT)
	{
		gpio_pointer->PDDR |= 1<<PIN2NUM(pin); 		 // Configuro como pin de salida
	}
	else if(mode ==  INPUT  || mode == INPUT_PULLUP || mode ==INPUT_PULLDOWN)
	{
		gpio_pointer->PDDR &= ~(1<<PIN2NUM(pin));	// Configuro como pin de entrada
	}


}

/**
 * @brief Configures how the pin reacts when an IRQ event ocurrs
 * @param pin the pin whose IRQ mode you wish to set (according PORTNUM2PIN)
 * @param irqMode disable, risingEdge, fallingEdge or bothEdges
 * @param irqFun function to call on pin event
 * @return Registration succeed
 */
bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun) {
	return true;
}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value)
{
	GPIO_Type* gpio_pointer = gpio_base_pointer[ PIN2PORT(pin) ];
	if (value == 1)
	{
		gpio_pointer->PSOR |= 1<<PIN2NUM(pin);	// Pone un uno en el Set del pin correspondiente
	}
	else
	{
		gpio_pointer->PCOR |= 1<<PIN2NUM(pin);  // Pone un uno en el Clear del pin correspondiente
	}
}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin)
{
	GPIO_Type* gpio_pointer = gpio_base_pointer[ PIN2PORT(pin) ];
	gpio_pointer->PTOR |= 1<<PIN2NUM(pin);     // Pone un uno en el Toggle del pin correspondiente
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin)
{
	bool value;
	GPIO_Type* gpio_pointer = gpio_base_pointer[ PIN2PORT(pin) ];

	if ( ((gpio_pointer->PDIR) & (1<<PIN2NUM(pin)) ) != 0 )	// Me quedo con la salida del pin corrrespondiente
	{
		value = 1;
	}
	else
	{
		value= 0;
	}
	return value;
}
