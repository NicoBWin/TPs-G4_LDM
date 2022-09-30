/***************************************************************************//**
  @file     display.c
  @brief    Driver display
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

//#include "../../MCAL/gpio.h"


#include "../board.h"

#include "MK64F12.h"

#include "../headers/I2C.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/*
#define I2C0_PORT 4
#define I2C1_PORT 2
#define I2C2_PORT 0

#define I2C0_SDA_PIN 25 // PTE25
#define I2C0_SCL_PIN 24 // PTE24
#define I2C1_SDA_PIN 11 // PTC11
#define I2C1_SCL_PIN 10 // PTC10
#define I2C2_SDA_PIN 13 // PTA13	
#define I2C2_SCL_PIN 14 // PTA14	

#define I2C0_ALT 5
#define I2C1_ALT 2
#define I2C2_ALT 5
*/
//#define PORTNUM2PIN(p, n) (((p) << 5) + (n))
//#define PIN2PORT(p) (((p) >> 5) & 0x07)
//#define PIN2NUM(p) ((p)&0x1F)

#define I2C0_DATA PORTNUM2PIN(PE, 25) // ALT5		SDA
#define I2C0_CLK PORTNUM2PIN(PE, 24)	 // ALT5	SCL

#define I2C1_DATA PORTNUM2PIN(PC, 11) // ALT2
#define I2C1_CLK PORTNUM2PIN(PC, 10)  // ALT2

#define I2C2_DATA PORTNUM2PIN(PA, 11) // ALT5
#define I2C2_CLK PORTNUM2PIN(PA, 12)  // ALT5

#define I2C_INTERRUPT PORTNUM2PIN(PD,1) // Interrupción

#define I2C0_ALT 5
#define I2C1_ALT 2
#define I2C2_ALT 5

#define I2C_ALT(num) ((num == 0) ? I2C0_ALT : ((num == 1) ? I2C1_ALT : I2C2_ALT))

#define I2C_DATA_PIN(num) ((num == 0) ? I2C0_DATA : ((num == 1) ? I2C1_DATA : I2C2_DATA))

#define I2C_CLK_PIN(num) ((num == 0) ? I2C0_CLK: ((num == 1) ? I2C1_CLK : I2C2_CLK))

typedef struct
{
	uint8_t address; 
	int cantwrite; // Contador de cantidad de bytes a enviar que se decrementa a medida que se envian. Si solo se quiere leer fijar en cero
	//uint8_t* writebuffer; // Se puede transmitir hasta 7 bits por lo tanto de cada int se agarrar
	int cantread;
	//uint8_t *readbuffer;
	uint8_t readbuffer[20];
	bool status; // Indica si esta siendo utilizado el canal por algun master (0 si no esta siendo utilizado )
	bool txrx; // Indica si se esta escribiendo o leyendo
	void (*callback_fn)(void); // fUNCION QUE SE LLAMA CUANDO SE COMPLETA UN ENVIO. bool = 1 si es exitoso y bool= 0 si no es exitoso

} i2c_data;

// Estados
#define START 0
#define FINISH 1
#define WRITE 2
#define READ 3

static uint8_t write_bufferxdd[8];

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Display  ********************************************************************
/**
 * @brief I2C_init: Initializes the protocol
 */
static i2c_data i2c_data_pointer[3]; // Vector con los datos de cada 12c para que use la interrupción
static PORT_Type *port_base_pointer[] = PORT_BASE_PTRS;
static I2C_Type *i2c_base_pointer1[] = I2C_BASE_PTRS;
static int ready = 0;

void I2C_init(uint16_t i2c_baudrate, int ic2num)
{
	// Apunto al I2C correspondiente
	I2C_Type *i2c_base_pointer = i2c_base_pointer1[ic2num]; // i2c_base_pointer1[i2cnum]


	// Configuro el puerto
	PORT_Type *port_pointer = port_base_pointer[PIN2PORT(I2C_DATA_PIN(ic2num))]; // Apunto al pcr del puerto correspondiente
	//SIM->SCGC5 |= 1 << (PIN2PORT(I2C_DATA_PIN(ic2num)) + CLK_GATING_OFFSET);	 // Prendo el clock del puerto donde esta el SDA y SCL
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Harcodeo y hago clock gating del pte
	// Habilito clcks del i2c correspondiente
	//Habilitar pin para medir interrupcion
	gpioMode(I2C_INTERRUPT,OUTPUT);
	switch (ic2num)
	{
	case 0:
		SIM->SCGC4 |= SIM_SCGC4_I2C0(1);
		break;
	case 1:
		SIM->SCGC4 |= SIM_SCGC4_I2C1(1);
		break;
	case 2:
		SIM->SCGC1 |= SIM_SCGC1_I2C2(1);
		break;
	}
	// Configuro el LOCK, el MUX, el OPEN_DRAIN del pin de SDA

	port_pointer->PCR[PIN2NUM(I2C_DATA_PIN(ic2num))] &= ~(1 << PORT_PCR_LK_SHIFT); // Desactiva el LOCK
	port_pointer->PCR[PIN2NUM(I2C_DATA_PIN(ic2num))] &= ~(3 << MUX_SHIFT_0);	   // Configuro los ceros de mux
	port_pointer->PCR[PIN2NUM(I2C_DATA_PIN(ic2num))] |= (I2C_ALT(ic2num) << MUX_SHIFT_1); // Configuro el mux
	port_pointer->PCR[PIN2NUM(I2C_DATA_PIN(ic2num))] |= PORT_PCR_ODE(1); // Hago Open Drain
	port_pointer->PCR[PIN2NUM(I2C_DATA_PIN(ic2num))] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK ;// Activo Pull enable y Pull select
	// Configuro el LOCK y el MUX, el OPEN_DRAIN del pin de SCL
	port_pointer->PCR[PIN2NUM(I2C_CLK_PIN(ic2num))] = 0x0;		  // Desactiva el LOCK
	port_pointer->PCR[PIN2NUM(I2C_CLK_PIN(ic2num))] &= ~(1 << PORT_PCR_LK_SHIFT);		  // Desactiva el LOCK
	port_pointer->PCR[PIN2NUM(I2C_CLK_PIN(ic2num))] &= ~(7 << MUX_SHIFT_1);				  // Configuro los ceros de mux
	port_pointer->PCR[PIN2NUM(I2C_CLK_PIN(ic2num))] |= (I2C_ALT(ic2num) << MUX_SHIFT_1);  // Configuro el mux en la alternativa correspondiente
	port_pointer->PCR[PIN2NUM(I2C_CLK_PIN(ic2num))] |= PORT_PCR_ODE(1) ;// Hago Open Drain
	port_pointer->PCR[PIN2NUM(I2C_CLK_PIN(ic2num))] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK ;// Activo Pull enable y Pull select

	// Configuro el I2C
	i2c_base_pointer->F = I2C_F_ICR(i2c_baudrate) + I2C_F_MULT(0); // Configuro el Baudrate
	//i2c_base_pointer->C1 = 0;
	i2c_base_pointer->C1 = I2C_C1_IICEN_MASK; // habilito el I2C antes habia |
	//Clear STARTF and STOPF flags
	i2c_base_pointer->FLT |= (I2C_FLT_STARTF_MASK | I2C_FLT_STOPF_MASK);

	// Ojo con la config del status que tiene el TCF = 1 acá

	if (ic2num == 0) // revisar si entra aca
	{
		NVIC_EnableIRQ(I2C0_IRQn);
	}
	else if( ic2num == 1)
	{
		NVIC_EnableIRQ(I2C1_IRQn);
	}
	else
	{
		NVIC_EnableIRQ(I2C2_IRQn);
	}
	ready = 0;
	//i2c_data_pointer[ic2num].status == NOT_BUSY;

}

bool i2cStartTransaction(int i2cnum, uint8_t address, int cantwrite, uint8_t* writebuffer, int cantread, uint8_t *readbuffer, void (*callback_fn)(void )) // Retorna 0 si el canal esta ocupado
{
	// Apunto al I2C correspondiente
	I2C_Type *i2c = i2c_base_pointer1[i2cnum];
	// Veo si el canal no esta en uso
	if (i2c_data_pointer[i2cnum].status == BUSY)
	{
		return 0;
	}
	// Relleno la data para la interrupción
	i2c_data_pointer[i2cnum].address = address;
	i2c_data_pointer[i2cnum].cantwrite = cantwrite;
	//i2c_data_pointer[i2cnum].writebuffer = writebuffer;
	for (int i = 0; i < cantwrite; i++)
	{
		write_bufferxdd[i] = writebuffer[i];
	}

	i2c_data_pointer[i2cnum].cantread = cantread;
	//i2c_data_pointer[i2cnum].readbuffer = readbuffer;
	i2c_data_pointer[i2cnum].txrx = WRITE_MODE;
	// Indico que el canal esta en uso
	i2c_data_pointer[i2cnum].status = BUSY;
	i2c_data_pointer[i2cnum].callback_fn = callback_fn;
	// Activo las interrupciones

	i2c->S |= I2C_S_IICIF_MASK; 
	i2c->C1 |= (I2C_C1_IICIE_MASK | I2C_C1_IICEN_MASK ); // Habilito interrupciones y el modulo i2c
// | I2C_C1_IICEN_MASK saque el habilitar i2c pq ya esta habilitado en la initializacion
	// Genero Start para arrancar la transmisión y activo el modo escritura 
	i2c->C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);

	// Escribo el addres + r/w en el data register
	i2c->D = (address << 1)   ; // agrego el address
	//i2c->D &= ~1; // Agrego el r/w = 0
	ready = 0;

	return 1;
}

//void testi2c()
//{
//	uint8_t writebuffer[8] = {}
//}

void i2c_irq_handler(uint32_t i2cnum)
{
	gpioWrite(I2C_INTERRUPT,HIGH);
	static int status = 0; // Variable que indica el estado
	static int start_count = 0; // Cuenta cantidad de start para verificar si es un restart o un start
	static int cant_write = 0; // cantidad de datos escritos
	static int cant_read = -1; // cantidad de datos leidos
	I2C_Type *i2c = i2c_base_pointer1[i2cnum]; // Puntero al i2c correspondiente

	i2c->S |= I2C_S_IICIF_MASK; // CLEAR IICIF, preg si se cleerea con 1

//	if (i2c->FLT &= I2C_FLT_FLT_MASK) // SI STOPF esta en 1 cambio de estado
//	{
//		status = FINISH;
//	}
/*	if (i2c->FLT & I2C_FLT_STARTF_MASK) // SI STARTF esta en 1 cambio de estado
	{
		if (start_count == 0) // Es un START
		{
			status = WRITE;
		}
		else				// Es un RESTART
		{
			if (i2c->D & 1) // Si esta en  1 R/W está en modo write
			{
				status = READ;
			}
			else
			{
				status = WRITE;
			}
		}
	}
*/
	if (i2c_data_pointer[i2cnum].txrx == WRITE_MODE)
	{
		status = WRITE;
	}
	else{
		status = READ;
	}
		switch (status)
		{
/*		case FINISH:						// preguntar si luego de la señal STOPF salta nuevamente una interrupt
			i2c->FLT |= I2C_FLT_STOPF_MASK; // CLEAN STOPF, preg si se cleerea con 1
			start_count = 0;				// ZERO START COUNT
			cant_read = 0;
			cant_write = 0;

			break;

		case START:
			i2c->FLT |= I2C_FLT_STARTF_MASK; // CLEAN STARTF, preg si se cleerea con 1

			start_count++; // START COUNT + 1
			break;
*/
		case WRITE:
			if (cant_write == i2c_data_pointer[i2cnum].cantwrite) // Si termino de escribir, comienza a leer
			{
				if (i2c_data_pointer[i2cnum].cantread == 0) // Si no hay nada que leer
				{
					i2c->C1 |= I2C_C1_TX_MASK;						  // Cambio a modo Tx
					i2c->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TXAK_MASK ); // genero STOPF, deshabilito interrupciones, Saque esto para prbar | I2C_C1_IICIE_MASK
					start_count = 0;								  // ZERO START COUNT
					cant_read = -1;									  // Reinicio contadores
					cant_write = 0;
					i2c_data_pointer[i2cnum].status = NOT_BUSY; // libero el canal
					if (i2c_data_pointer[i2cnum].callback_fn)	// Indico que se termino la transmición
					{
						(i2c_data_pointer[i2cnum].callback_fn)(); // Se completo la transacción exitosamente
					}

					ready = 1; // Error
				}
				else
				{
					// Genero ReStart para arrancar la transmisión y activo el modo lectura obs: Preg si se genera asi el restart
					i2c->C1 |= (I2C_C1_RSTA_MASK | I2C_C1_TX_MASK); // Genero a repeated start y pongo tx en uno pq quiero escribir el address
					
					// Escribo el addres + r/w = 1en el data register
					i2c->D = (i2c_data_pointer[i2cnum].address << 1) | 1;
					i2c_data_pointer[i2cnum].txrx = READ_MODE; // cambio a modo lectura
					// genero restart y mando adress con Read
				}
			}
			else
			{
				if ((i2c->S & I2C_S_RXAK_MASK) != 0) // Veo se ACK esta en 1, osea que hubo un error
				{
					i2c->C1 |= I2C_C1_TX_MASK;						  // Cambio a modo Tx
					i2c->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TXAK_MASK | I2C_C1_IICIE_MASK); // genero STOPF, deshabilito interrupciones,
					start_count = 0;								  // ZERO START COUNT
					cant_read = -1;									  // Reinicio contadores
					cant_write = 0;
					i2c_data_pointer[i2cnum].status = NOT_BUSY; // libero el canal
					if (i2c_data_pointer[i2cnum].callback_fn)	// Indico que se termino la transmición
					{
						(i2c_data_pointer[i2cnum].callback_fn)(); // Se completo la transacción sin exito
					}
					ready = 2; // Error
				}
				else
				{
					//i2c->D = i2c_data_pointer[i2cnum].writebuffer[cant_write++]; // Escribo dato e increment puntero
					i2c->D = (write_bufferxdd[cant_write++]);
					return;
					//i2c->C1 |= I2C_C1_TX_MASK;

				}
			}
			break;
		case READ:
			if ( cant_read == -1) // El primer dato lo descarto pues es basura
			{
				i2c->C1 &=  ~I2C_C1_TX_MASK; // Cambio a modo lectura
				if (i2c->S & I2C_S_RXAK_MASK) // Veo se ACK esta en 1, osea que hubo un error
				{
					i2c->C1 |= I2C_C1_TX_MASK;						  // Cambio a modo Tx
					i2c->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TXAK_MASK ); // genero STOPF, deshabilito interrupciones, | I2C_C1_IICIE_MASK
					start_count = 0;								  // ZERO START COUNT
					cant_read = -1;									  // Reinicio contadores
					cant_write = 0;
					i2c_data_pointer[i2cnum].status = NOT_BUSY; // libero el canal
					if (i2c_data_pointer[i2cnum].callback_fn)	// Indico que se termino la transmición
					{
						(i2c_data_pointer[i2cnum].callback_fn)(); // Se completo la transacción sin exito
					}
					ready = 2; // Error
				}
				else
				{
					uint8_t basura = i2c->D;    // Leo data para generar clks
					cant_read++;
				}
			}
			else if (cant_read == (i2c_data_pointer[i2cnum].cantread-1)) // Si termino de leer
			{

				//i2c->C1 |= I2C_C1_TX_MASK; // Cambio a modo Tx para que no me mande otro dato lueego de leer
				i2c->C1 |= I2C_C1_TXAK_MASK; // Genero un NACK
				i2c_data_pointer[i2cnum].readbuffer[cant_read] = i2c->D; // Leo ultimo dato del registro

				// Genero STOPF con un notack OBS falta implementar
				//| I2C_C1_TXAK_MASK

				//
				//i2c->C1 &= ~(I2C_C1_MST_MASK  ); // genero STOPF, deshabilito interrupciones, | I2C_C1_IICIE_MASK
				cant_read++;
				/* Test Ojala funcione
				start_count = 0;			 // ZERO START COUNT
				cant_read = -1;				// Reinicio contadores
				cant_write = 0;
				i2c_data_pointer[i2cnum].status = NOT_BUSY; // libero el canal
				// Aviso que se termino la transmición de datos
				if (i2c_data_pointer[i2cnum].callback_fn)	// Indico que se termino la transmición
				{
					(i2c_data_pointer[i2cnum].callback_fn)(); // Se completo la transacción exitosamente
				}
				ready = 1; // Exito
				*/


				/*
				i2c->C1 |= I2C_C1_TX_MASK;						  // Cambio a modo Tx
				i2c->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_TXAK_MASK ); // genero STOPF, deshabilito interrupciones, Saque esto para prbar | I2C_C1_IICIE_MASK
				start_count = 0;								  // ZERO START COUNT
				cant_read = -1;									  // Reinicio contadores
				cant_write = 0;
				i2c_data_pointer[i2cnum].status = NOT_BUSY; // libero el canal
				if (i2c_data_pointer[i2cnum].callback_fn)	// Indico que se termino la transmición
				{
					(i2c_data_pointer[i2cnum].callback_fn)(); // Se completo la transacción exitosamente
				}
				*/
			}
			else if(cant_read == (i2c_data_pointer[i2cnum].cantread))
			{
				//i2c->C1 |= I2C_C1_TX_MASK; 						  // Cambio a modo Tx
				i2c->C1 &= ~(I2C_C1_MST_MASK); // genero STOPF, deshabilito interrupciones, Saque esto para prbar | I2C_C1_IICIE_MASK
				start_count = 0;								  // ZERO START COUNT
				cant_read = -1;									  // Reinicio contadores
				cant_write = 0;
				i2c_data_pointer[i2cnum].status = NOT_BUSY; // libero el canal
				if (i2c_data_pointer[i2cnum].callback_fn)	// Indico que se termino la transmición
				{
					(i2c_data_pointer[i2cnum].callback_fn)(); // Se completo la transacción exitosamente
				}
			}
			else
			{
				i2c_data_pointer[i2cnum].readbuffer[cant_read++] = i2c->D; // Leo data del registro
				i2c->C1 &= ~I2C_C1_TXAK_MASK;	// Mando un ACK

			}

		break;
	}
	gpioWrite(I2C_INTERRUPT,LOW);
	i2c->C1 |= (I2C_C1_IICIE_MASK | I2C_C1_IICEN_MASK );
}
int get_ready()
{
	return ready;
}

uint8_t* get_read_buffer(int i2cnum)
{
	return i2c_data_pointer[i2cnum].readbuffer;
}

void I2C0_IRQHandler(void) { 
	i2c_irq_handler(0); 
	}
void I2C1_IRQHandler(void) { 
	i2c_irq_handler(1); 
	}
void I2C2_IRQHandler(void) { 
	i2c_irq_handler(2); 
	}
