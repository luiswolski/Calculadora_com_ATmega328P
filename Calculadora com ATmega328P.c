#define F_CPU 16000000
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>



enum{ LCD_CLEAR     = 1,
      RETURN_HOME   = 2,
      DISPLAY_ON    = 12,
      DISPLAY_OFF   = 8,
      CURSOR_ON     = 10,
      CURSOR_OFF    = 8,
      CURSOR_BLK_ON = 9,
      CURSOR_BLK_OFF= 8};
      
void pulso_E();      
void Lcd_cmd(unsigned char comando);
void envia_dados(unsigned char comando);
void Lcd_out(char linha_lcd, char coluna_lcd, char *ponteiro);
void Lcd_init();


char result_char [16], val_1_texto [9], val_2_texto [9];
double val_1 = 0, val_2 = 0, resultado, value_btn;
int op = 0, screen = 1, aux_1 = 0, ponto = 0;
char value_char [20];

int le_teclado() {

	int btn = 99;

	PORTC &= ~(1<<PC0); //turn on 1st line
	_delay_ms(1);

	if(~PINB & (1<<PB2)) //button n° '1'
		btn = 1;

	else if(~PINB & (1<<PB3)) //button n° '2'
		btn = 2;

	else if( ~PINB & (1<<PB4)) //button n° '3'
		btn = 3;

	PORTC|=(1<<PC0); //turn off 1st line
	PORTC &=~(1<<PC1); //turn on 2nd line
	_delay_ms(1);

	if(~PINB & (1<<PB2)) //button n° '4'
		btn = 4;

	else if(~PINB & (1<<PB3)) //button n° '5'
		btn = 5;

	else if( ~PINB &(1<<PB4)) //button n° '6'
		btn = 6;

	PORTC|=(1<<PC1); //turn off 2nd line
	PORTC &=~(1<<PC2); //turn on 3rd line

	_delay_ms(1);

	if(~PINB & (1<<PB2)) //button n° '7'
		btn = 7;

	else if(~PINB & (1<<PB3)) //button n° '8'
		btn = 8;

	else if( ~PINB &(1<<PB4)) //button n° '9'
		btn = 9;

	PORTC|=(1<<PC2); //turn off 3rd line
	PORTC &=~(1<<PC3); //turn on 4th line

	_delay_ms(1);

	if(~PINB & (1<<PB2)) //button decimal point
		btn = 10;
	else if(~PINB & (1<<PB3)) //button n° '0'
		btn = 0;
	else if(~PINB &(1<<PB4)) //button enter
		btn = 12;

	PORTC|=(1<<PC3); //turn off 4th line


	if(btn != 99)
		_delay_ms(200);


	return btn;
}


void first_value_screen() {
	Lcd_out(1,1, "1st value:    ");
	if(value_btn != 12 && (aux_1 - ponto + 1) < 9){
		aux_1++;
		if(value_btn != 10) {
			dtostrf(value_btn, 1, 0,value_char);
			val_1_texto[aux_1-1] = value_btn + 48;
			val_1_texto[aux_1] = '\0';
			Lcd_out(2, aux_1, value_char);
		}
		
		else {
			ponto = 1;
			val_1_texto[aux_1-1] = '.';
			val_1_texto[aux_1] = '\0';
			Lcd_out(2, aux_1, ".");
		}
	}
}


void operation_screen() {
	val_1 = atof(val_1_texto);
	Lcd_out(1, 1, "1 (+) | 2 (-)");
	Lcd_out(2, 1, "3 (/) | 4 (*)");
	aux_1 = 0;
	ponto = 0;
	
	if(value_btn > 0 && value_btn < 5) {
		op = value_btn;
		value_btn = 13;
		Lcd_out(1,1, "2nd value:          ");
		Lcd_out(2,1, "                ");
		screen++;
	}
}


void second_value_screen() {
	Lcd_out(1,1, "2nd value:            ");

	if(op == 0) {
		Lcd_cmd(LCD_CLEAR);
		Lcd_out(1, 1, "ERROR! Press #");

		if(op == 0)
		Lcd_out(2, 1, "Select an oper.");

		op = 0;
		ponto = 0;
		_delay_ms(500);
		screen = 1;
	}

	if(value_btn != 12 && (aux_1 - ponto + 1) < 9 && value_btn != 13) {
		aux_1++;
		if(value_btn != 10) {
			dtostrf(value_btn, 1, 0,value_char);
			val_2_texto[aux_1-1] = value_btn + 48;
			val_2_texto[aux_1] = '\0';
			Lcd_out(2, aux_1, value_char);
		}
		else {
			val_2_texto[aux_1-1] = '.';
			val_2_texto[aux_1] = '\0';
			Lcd_out(2, aux_1, ".");
		}
	}
}


void result_screen() {
	Lcd_out(1, 1, "Result: ");
	val_2 = atof(val_2_texto);
	
	if(op == 1) //if sum
		resultado = val_1 + val_2;
	
	else if (op == 2) //if subtraction
		resultado = val_1 - val_2;
	
	else if (op == 3) //if division
		resultado = val_1 / val_2;

	else if (op == 4) //if multiply
		resultado = val_1 * val_2;


	dtostrf(resultado, 16, 7, result_char);

	Lcd_out(2, 1, "            ");
	

	if(op == 3 && val_2 == 0)
	{
		char inf[16] = "undefined";
		strcpy(result_char, inf);
	}

	Lcd_out(2, 1, result_char);
}


int main() {
	//entradas
	DDRB = DDRB & ~(1<<PB2);
	DDRB = DDRB & ~(1<<PB3);
	DDRB = DDRB & ~(1<<PB4);
	
	//saidas
	DDRB = DDRB | (1<<PB5);
	DDRC = DDRC | (1<<PC0);
	DDRC = DDRC | (1<<PC1);
	DDRC = DDRC | (1<<PC2);
	DDRC = DDRC | (1<<PC3);

	
	PORTB =  PORTB & ~(1<<PB5);
	PORTC =  PORTC | ((1<<PC0)+(1<<PC1)+(1<<PC2)+(1<<PC3));
	PORTB =  PORTB | ((1<<PB2)+(1<<PB3)+(1<<PB4));
	

	Lcd_init();
	Lcd_cmd(LCD_CLEAR);
	Lcd_out(1,1, "1st Value: ");

	while (1) {
		value_btn = le_teclado();
		if(value_btn != 99) {
			if(value_btn == 12) {
				screen++;
				Lcd_cmd(LCD_CLEAR);
				if(screen == 5) {
					screen = 1;
					aux_1 = 0;
					ponto = 0;
					op = 0;
					val_1 = 0;
					val_2 = 0;
					val_1_texto[0] = '\0';
					val_2_texto[0] = '\0';
				}
			}

			switch(screen) {
				case 1:
					first_value_screen();
					break;				
				case 2:
					operation_screen();
					break;
				case 3:
					second_value_screen();
					break;
				case 4:
					result_screen();
					break;
			}

		}

	}

}


#define RS 1
#define E  0

void  pulso_E()
{
	PORTB &= ~(1<<E);
	PORTB |= (1<<E);
	PORTB &= ~(1<<E);
	return;
}

void envia_dados(unsigned char comando)
{
	PORTD = ((comando & 0xF0) | (PORTD & 0x0F));
	pulso_E();
	
	PORTD = (((comando << 4) & 0xF0) | (PORTD & 0x0F));
	pulso_E();
	return;
}

void Lcd_cmd(unsigned char comando)
{
	_delay_ms(1);               // Temporizar 1ms
	PORTB &= ~(1<<RS);          // RS = 0
	envia_dados(comando);
	return;
}


void Lcd_out(char linha_lcd, char coluna_lcd, char *ponteiro)
{
	//Seta endere o da DDRAM
	if((linha_lcd > 0) && (linha_lcd < 3))  // verifica se a linha   valida
	if((coluna_lcd > 0) && (coluna_lcd < 41))  //verifica se a coluna   valida
	{
		Lcd_cmd(128 + (coluna_lcd - 1) + ((linha_lcd - 1) * 64));
		//Envia o texto
		while (*ponteiro)
		{
			_delay_ms(1);               // Temporizar 1ms
			PORTB |= (1<<RS);           // RS = 1
			envia_dados(*ponteiro);
			ponteiro++;
		}
	}
	return;
}


void  Lcd_init()
{
	//configura portas de saida
	DDRB = DDRB | ((1<<E) + (1<<RS));   // PB1 e PB0 como saida (RS/E)
	DDRD = DDRD | ((1<<PD4)+(1<<PD5)+(1<<PD6)+(1<<PD7));  // PD7 a PD4 Como saida (D7...D4)
	          
	
	//inicialiaza o lcd
	PORTB &= ~(1<<RS);                                   // RS = 0
	_delay_ms(15);                                       // Temporizar 15 ms
	PORTD = ((0x30 & 0xF0) | (PORTD & 0x0F));            // Enviar 0x30 ao LCD
	pulso_E();                                           // Pulso no pino E
	
	_delay_ms(5);                                        // Temporizar 5 ms
	pulso_E();                                           // Pulso no pino E
	
	_delay_ms(1);                                        // Temporizar 1 ms
	pulso_E();                                           // Pulso no pino E
	
	_delay_ms(1);                                        // Temporizar 1 ms
	PORTD = ((0x20 & 0xF0) | (PORTD & 0x0F));            // Enviar 0x20 ao LCD
	pulso_E();                                           // Pulso no pino E
	
	Lcd_cmd(0x28);                                      //Especifica a interface de comunica  o 4 bits, 2 linhas 5X8 pixels
	Lcd_cmd(0x08);                                      // Desliga o display
	Lcd_cmd(0x01);                                      // Limpa o diplay
	Lcd_cmd(0x06);                                      // N o desloca o display, incrementa o AC (posi  o do cursor)
	Lcd_cmd(DISPLAY_ON);
	
	return;

}



