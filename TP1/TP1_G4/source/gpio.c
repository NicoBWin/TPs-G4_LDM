/***************************************************************************//**
  @file     gpio.c
  @brief    Application functions
  @author   Grupo 4
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
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void privFunc(uint32_t veces)
{
    while (veces--);
}


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
void gpioMode (pin_t pin, uint8_t mode){
	if ( pin > PORTNUM2PIN(PE,31) ) return;
		if ( mode > INPUT_PULLDOWN) return;

		uint32_t port = PIN2PORT(pin);

		// 1) Clock enable
		SIM->SCGC5 |= SIM_SCGC5_PORT(port,1);

		// 2) PORT configuration
		PORT_Type* port_ptr = PORT_PTRS[port];
		port_ptr->PCR[PIN2NUM(pin)] = 0x0;//!!!
		port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_MUX(0b001);
		port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_DSE(0b1);

		if ((mode==INPUT_PULLDOWN)||(mode==INPUT_PULLUP)) {
			port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_PE(0b1);
			port_ptr->PCR[PIN2NUM(pin)] |= PORT_PCR_PS( mode==INPUT_PULLDOWN ? 0b0 : 0b1 );
		}

		// 3) GPIO configuration
		GPIO_Type* gpio_ptr = GPIO_PTRS[port];
		gpio_ptr->PDDR |= ((mode == OUTPUT) ? 1 : 0) << PIN2NUM(pin);

		return;
}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value){

}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin){

}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin){

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void privFunc(uint32_t veces)
{
    while (veces--);
}


/*******************************************************************************
 ******************************************************************************/
