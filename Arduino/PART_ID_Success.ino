#include "global.h"

#define LED_PIN						13
#define LED1_PIN					6
#define SW1_PIN						7
#define ADC_PIN						0

void Error_Handler(uint16_t code, uint32_t line, char* file) {
	char str[200];
	uint8_t i = 0;
	
	GPIO_Write(13, HIGH);
	GPIO_Mode(13, OUTPUT);

	UCSR0B = 0x08;											//enable transmitter
	UCSR0C = 0x06;											//8-bit character, 1 stop bit, no parity
	UBRR0 = 103;												//9600 baud @ 16MHz

	snprintf(str, sizeof(str), "ERROR: 0x%04X", code);
	snprintf(str, sizeof(str), "%s at line %u", str, line);
	snprintf(str, sizeof(str), "%s in %s\n", str, file);

	do	{
		UDR0 = str[i];									//transmit str[i]
		i++;														//point to next character in string
		while((UCSR0A & 0x20) == 0);		//wait until UDRE0 bit is set
		} while(str[i] != 0);						//do while str[i] is NOT NUL (0x00)

	for(;;);
	}

int main(void) {
	uint16_t err_code;
	uint8_t pin_value;
	uint32_t timeout = 0;
	uint8_t LED_state = 0;
	char str[150];

	uint32_t red_data, ir_data;
	uint32_t print_timer = 0;
	char uart_buf[64];

	char info_success[20];
	char info_failed[20];

	uint32_t ADC_timeout = SYS_TICK + 1000;
	uint16_t ADC_value[ADC_BUF_SIZE] = {0};
	uint32_t ADC_sum = 0;
	uint16_t ADC_average;
	uint8_t ADC_ptr = 0;

	uint8_t ack_bit;
	uint8_t ID;

	uint8_t BLE_connected_pre = 0;
	uint8_t BLE_connected = 0;
	char BLE_connected_str[] = "BLE Connected\n";
	char BLE_disconnected_str[] = "BLE DISCONNECTED\n";
	
	uint8_t received_byte;
	
	CHECK_ERROR_FATAL(TIMER2_Init());
	sei();

	CHECK_ERROR_FATAL(UART_Init());
	//CHECK_ERROR_FATAL(ADC_Init());
	CHECK_ERROR_FATAL(TWI_Init());
	CHECK_ERROR_FATAL(TIMER0_Init());
	CHECK_ERROR_FATAL(SWUART_Init());
	//CHECK_ERROR_FATAL(BLE_Init());
	max30102_init();

	CHECK_ERROR_FATAL(GPIO_Mode(SW1_PIN, INPUT));
	CHECK_ERROR_FATAL(GPIO_Write(LED_PIN, LOW));
	CHECK_ERROR_FATAL(GPIO_Mode(LED_PIN, OUTPUT));
	CHECK_ERROR_FATAL(GPIO_Write(LED1_PIN, LOW));
	CHECK_ERROR_FATAL(GPIO_Mode(LED1_PIN, OUTPUT));
	CHECK_ERROR_FATAL(GPIO_Mode(ADC_PIN, INPUT));

	snprintf(str, sizeof(str), "Compiled on %s %s\nSystem Initialised\n", __DATE__, __TIME__);
	CHECK_ERROR_FATAL(UART_Write_String(str, strlen(str)));

	// CHECK WHETHER IT GIVE 0x15
	if(SYS_TICK >= timeout) {
		timeout = SYS_TICK + 5000;
		
		TWI_Start_Send();                            //Send Start
		TWI_Address_Write(MAX30102_I2C_ADDR);        //Send Address Slave + Write
		TWI_Data_Send(REG_PART_ID, &ack_bit);               //Send PART ID + ACK/NACK from sensor

		if (ack_bit == TWI_ACK){                     //If Sensor said ACK
			TWI_Restart_Send();                        //Restart Start to extend the communication
			TWI_Address_Read(MAX30102_I2C_ADDR);       //Send Address Slave again + Read
			TWI_Data_Read(&ID);                        //This part, Sensor will reply whether 0x15 or something else and we store into address of ID + NACK
			TWI_Stop_Send();                           //Stop 
			snprintf(info_success, sizeof(info_success), "ID: 0x%02x\n", ID);   //Print Result to info_success
			UART_Write_String(info_success, strlen(info_success));              //Write to serial 
		}

		else{
			TWI_Stop_Send();
			UART_Write_String("Sensor NACK\n", 12);
		}
	}

	for(;;) {
		max30102_read_sample(&red_data, &ir_data);

		// Print to Serial every 100ms to avoid clogging the buffer
		//if (SYS_TICK >= print_timer) {
		//print_timer = SYS_TICK + 100;

		// Use %lu for uint32_t on Arduino Uno
		snprintf(uart_buf, sizeof(uart_buf), "%u, %u\n", red_data, ir_data);
		UART_Write_String(uart_buf, strlen(uart_buf));
		//}
	}
}
