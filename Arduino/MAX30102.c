#include "global.h"

#if defined(__cplusplus)
extern "C" {
#endif

uint32_t timeout_Soft_Reset = 0;

void max30102_write_reg(uint8_t reg, uint8_t data)
{
  uint8_t respond_sensor;
  uint8_t respond_sensor_2;

  TWI_Start_Send();
  TWI_Address_Write(MAX30102_I2C_ADDR);
  TWI_Data_Send(reg,&respond_sensor);
  TWI_Data_Send(data,&respond_sensor_2);
  TWI_Stop_Send();

}
uint8_t max30102_read_reg(uint8_t reg)
{
  uint8_t one_data = 0;
  uint8_t respond_sensor_3;

  TWI_Start_Send();
  TWI_Address_Write(MAX30102_I2C_ADDR);
  TWI_Data_Send(reg,&respond_sensor_3);

  TWI_Restart_Send();
  TWI_Address_Read(MAX30102_I2C_ADDR);
  TWI_Data_Read(&one_data);
  TWI_Stop_Send();

  return one_data;
}
void max30102_reset(void)
{
  /* Set RESET bit in MODE_CONFIG register */
  max30102_write_reg(REG_MODE_CONFIG, 0x40);
}

void max30102_clear_fifo(void)
{
  /* Datasheet recommends clearing FIFO pointers before new SpO2/HR conversion */
  max30102_write_reg(REG_FIFO_WR_PTR, 0x00);
  max30102_write_reg(REG_OVF_COUNTER, 0x00);
  max30102_write_reg(REG_FIFO_RD_PTR, 0x00);
}

void max30102_init()
{
  /* Disable interrupts first */
  max30102_write_reg(REG_INTR_ENABLE_1, 0x00);
  max30102_write_reg(REG_INTR_ENABLE_2, 0x00);

  /* Clear FIFO pointers */
  //max30102_clear_fifo();
  max30102_reset();

  delay_ms(500);
  /*
    * FIFO_CONFIG = 0x00
    * SMP_AVE = 000 -> no averaging
    * FIFO_ROLLOVER_EN = 0
    * FIFO_A_FULL = 0000
    */
  max30102_write_reg(REG_FIFO_CONFIG, 0x00);

  /*
    * MODE_CONFIG = 0x03
    * MODE[2:0] = 011 -> SpO2 mode (Red + IR)
    */
  max30102_write_reg(REG_MODE_CONFIG, 0x03);

  /*
    * SPO2_CONFIG = 0x27
    * Bits [6:5] SPO2_ADC_RGE = 01 -> 4096 nA full scale
    * Bits [4:2] SPO2_SR      = 001 -> 100 samples/sec
    * Bits [1:0] LED_PW       = 11  -> 411 us pulse width, 18-bit ADC
    *
    * 0b0010 0111 = 0x27
    */
  max30102_write_reg(REG_SPO2_CONFIG, 0x27);

  /*
    * LED1_PA = Red LED current
    * LED2_PA = IR LED current
    * 0x24 is a safe starting point
    */
  max30102_write_reg(REG_LED1_PA, 0x24);
  max30102_write_reg(REG_LED2_PA, 0x24);

  max30102_clear_fifo();

  //return 1;
  
}

uint8_t max30102_read_sample(uint32_t *red, uint32_t *ir)
{
  uint8_t temp[6];
  uint8_t ack;

  // 1. Start burst read from FIFO data register
  TWI_Start_Send();
  TWI_Address_Write(MAX30102_I2C_ADDR);
  TWI_Data_Send(REG_FIFO_DATA, &ack);
  
  TWI_Restart_Send();
  TWI_Address_Read(MAX30102_I2C_ADDR);

  // 2. Read 6 bytes (Red x 3, IR x 3)
  // Use TWI_Data_Read_ACK for first 5 bytes, TWI_Data_Read (NACK) for the last
  TWI_Data_Read_ACK(&temp[0]); // Red High
  TWI_Data_Read_ACK(&temp[1]); // Red Mid
  TWI_Data_Read_ACK(&temp[2]); // Red Low
  TWI_Data_Read_ACK(&temp[3]); // IR High
  TWI_Data_Read_ACK(&temp[4]); // IR Mid
  TWI_Data_Read(&temp[5]);     // IR Low (NACK to end)
  TWI_Stop_Send();

  // 3. Bit-shift bytes into 32-bit integers
  *red = ((uint32_t)temp[0] << 16) | ((uint32_t)temp[1] << 8) | (uint32_t)temp[2];
  *ir  = ((uint32_t)temp[3] << 16) | ((uint32_t)temp[4] << 8) | (uint32_t)temp[5];

  // 4. Mask to 18-bit (based on your SPO2_CONFIG pulse width)
  *red &= 0x03FFFF;
  *ir  &= 0x03FFFF;

  return 1; // Success
}

#if defined(__cplusplus)
}
#endif