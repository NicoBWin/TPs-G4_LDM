#define TSAMPLE	1	//Cada cuanto espero un pulso en enable, en ms
void mag_drv_INIT();
int mag_set_LIVE();
void mag_read_end();	//Se llama desde la IRQ cuando se termina de leer una palabra, desactiva las interrupciones. (Kill_process)
char* mag_drv_read();	//Devuelve un strinc de todos los campos de datos.
bool mag_get_data_ready();	//Devuelve verdadero si tiene data para procesar, y falso si no.
char* mag_get_ID();		//Devuelve un string del campo de datos PAN
char* mag_get_DATE();	//Devuelve un string del campo de datos adicionales
void mag_clear_active();
void mag_set_active();

