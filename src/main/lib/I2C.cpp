/**
 * @file   I2C.cpp
 *
 * @brief  Functions for I2C communication.
 *
 * @author Fran Fodor
 */

#include "driver/gpio.h"
#include "rom/ets_sys.h"

#include "include/I2C.h"

// helper macros for I2C communication.
#define SET_HIGH(pin)  gpio_set_level(pin, 1);
#define SET_LOW(pin)   gpio_set_level(pin, 0);
#define DELAY(us)      ets_delay_us(us);

// datasheet recommends min 2.5 us for clock cycle
#define I2C_CLOCK_DELAY      3

/**
 * @brief   Constructor for I2C which sets up I2C pins.
 *
 */
I2C::I2C()
{
  m_sda = (gpio_num_t)CONFIG_I2C_SDA_PIN;
  m_scl = (gpio_num_t)CONFIG_I2C_SCL_PIN;
  m_addr = CONFIG_I2C_ADDRESS;

  gpio_config_t io_conf = {};
  io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
  // set 1 to pin in the mask
  io_conf.pin_bit_mask = ((1ULL << m_sda) | (1ULL << m_scl));
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

  gpio_config(&io_conf);

  SET_HIGH(m_scl);
  SET_HIGH(m_sda);
}

/**
 * @brief   Starts I2C transaction.
 *
 * SSD1306 datasheet pg. 21
 *
 */
void I2C::start()
{
  // start condition - pull SDA to LOW while SCL is HIGH
  SET_HIGH(m_sda);
  SET_HIGH(m_scl);
  DELAY(I2C_CLOCK_DELAY);
  
  SET_LOW(m_sda);
  DELAY(I2C_CLOCK_DELAY);
  SET_LOW(m_scl);
}

/**
 * @brief   Stops I2C transaction.
 *
 * SSD1306 datasheet pg. 21
 *
 */
void I2C::stop()
{
  // stop condition - pull SDA to HIGH while SCL is HIGH
  SET_LOW(m_sda);
  SET_HIGH(m_scl);
  DELAY(I2C_CLOCK_DELAY);

  SET_HIGH(m_sda);
  DELAY(I2C_CLOCK_DELAY);
}

/**
 * @brief   Send byte of data to I2C.
 *
 * SSD1306 datasheet pg. 19
 *
 * @param   uint8_t byte
 *          byte of data to be sent
 * 
 * @returns bool
 *          if the acknowledge signal from slave is recieved
 * 
 */
bool I2C::sendByte(uint8_t byte)
{
  for (int i = 0; i < 8; i++)
  {
    if (byte & 0x80)
    {
      SET_HIGH(m_sda);
    }
    else
    {
      SET_LOW(m_sda);
    }

    // transmit data on clock
    SET_HIGH(m_scl);
    DELAY(I2C_CLOCK_DELAY);
    SET_LOW(m_scl);
    DELAY(I2C_CLOCK_DELAY);

    // "select" next bit
    byte <<= 1;
  }

  // ACK signal - SDA pulled down during SCL HIGH
  SET_HIGH(m_sda);
  SET_HIGH(m_scl);
  DELAY(I2C_CLOCK_DELAY);

  bool ack = !gpio_get_level(m_sda);
  
  SET_LOW(m_scl);
  DELAY(I2C_CLOCK_DELAY);

  return ack;
}

/**
 * @brief   Perform I2C transaction.
 *
 * SSD1306 datasheet pg. 20
 *
 * @param   uint8_t *data
 *          data to be sent
 * @param   size_t len
 *          size of data array
 * @returns bool
 *          0 if transaction was successfull
 * 
 */
bool I2C::transaction(uint8_t *data, size_t len)
{
  start();

  if (!sendByte(m_addr << 1))
    goto end;
  if (!sendByte(data[0]))
    goto end;
  for(int i = 1; i < len; i++)
  {
    if (!sendByte(data[i]))
      goto end;
  }

  stop();
  return 0;
end:
  stop();
  return 1;
}

/**
 * @brief   Checks I2C connection by checking slave acknowledge.
 *
 * SSD1306 datasheet pg. 20
 *
 * @returns bool
 *          0 if connection exists
 * 
 */
bool I2C::checkConnection()
{
  bool ret = false;

  start();
  ret = sendByte(m_addr << 1);
  stop();

  return ret;
}
