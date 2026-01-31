#include "UART_Library.h"
				
//Envía un solo carácter por el puerto serial (UART).
void write(char texto){
	while ((UCSR0A & (1<<UDRIE0))==0);	//Esperamos a que el registro de datos de UART este vacio.
	UDR0= texto;						//se envia el dato.
}

//envia una cadena de texto completa a la terminal, caracter por caracter por UART 
void writeString(char* texto){
	for(uint8_t i = 0; *(texto+i) !='\0'; i++)		//
	write(*(texto+i));								//
}

//enviar datos numericos
void UART_sendUint8(uint8_t num)
{
	char buffer[4]; // máximo "255"
	uint8_t i = 0;

	if (num == 0) {
		write('0');
		return;
	}

	while (num > 0) {
		buffer[i++] = (num % 10) + '0';
		num /= 10;
	}

	// Enviar en orden correcto
	while (i > 0) {
		write(buffer[--i]);
	}
}


//Inicializa el USART a 9600 baudios, 8 bits, sin paridad, 1 bit de parada.
void initUART_9600(){
	//Configurar los pines PD1 Tx y PD0 Rx
	DDRD |= (1<<PORTD1);
	DDRD &= ~(1<<PORTD0);
	UCSR0A = 0;		//No se utiliza doble speed.
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);  //Habilitamos interrupciones al recibir, habilitar recepcion y transmision
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);	//Asincrono, deshabilitado el bit de paridad, un stop bit, 8 bits de datos.
	UBRR0=103;	//UBBRR0=103; -> 9600 con frecuencia de 16MHz
}

uint8_t str_len(char *str){
	uint8_t len = 0;

	while (str[len] != '\0')
	len++;

	return len;
}

uint8_t str_to_uint8(char *str, uint8_t len, uint8_t *result)
{
	uint16_t valor = 0;

	if (len == 0 || len > 3)
	return 0;

	for (uint8_t i = 0; i < len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
		return 0;

		valor = valor * 10 + (str[i] - '0');

		if (valor > 255)
		return 0;  // overflow detectado
	}

	*result = (uint8_t)valor;
	return 1;
}

