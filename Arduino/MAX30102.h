#define MAX30102_I2C_ADDR         0x57 //slave address of the sensor
#define REG_PART_ID               0xFF

#define REG_INTR_STATUS_1         0x00
#define REG_INTR_STATUS_2         0x01
#define REG_INTR_ENABLE_1         0x02
#define REG_INTR_ENABLE_2         0x03
#define REG_FIFO_WR_PTR           0x04
#define REG_OVF_COUNTER           0x05 //when data o
#define REG_FIFO_RD_PTR           0x06
#define REG_FIFO_DATA             0x07
#define REG_FIFO_CONFIG           0x08 //set sample averaging and the FIFO almost full behaviour, how data is buffer
#define REG_MODE_CONFIG           0x09 //set to reset device, put it in shutdown, or choose operating mode :  Heart rate or spO2, control what operating state the sensor is
#define REG_SPO2_CONFIG           0x0A //spO2 register, set ADC range, the sample rate, and LED pulse width to determine ADC resolution, control how the measurement is taken
#define REG_LED1_PA               0x0C
#define REG_LED2_PA               0x0D
#define REG_TEMP_INT              0x1F
#define REG_TEMP_FRAC             0x20
#define REG_TEMP_CONFIG           0x21


#if defined(__cplusplus)
extern "C" {
#endif

extern void max30102_write_reg(uint8_t reg, uint8_t data);
extern uint8_t max30102_read_reg(uint8_t reg);
extern void max30102_reset(void);
extern void max30102_clear_fifo(void);
extern void max30102_init(void);
extern uint8_t max30102_read_sample(uint32_t *red, uint32_t *ir);

#if defined(__cplusplus)
}
#endif


