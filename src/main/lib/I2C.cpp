
/**
 * I2C.cpp
 *
 * Created on: Mar 9, 2026
 *     Author: Fran Fodor
 */

#include "I2C.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

/**
 * Helper macros for I2C communication.
 */
#define SET_HIGH(pin)  gpio_set_level(pin, 1);
#define SET_LOW(pin)   gpio_set_level(pin, 0);
#define DELAY(us)      ets_delay_us(us);

#define WAIT_LOW(pin)  while (gpio_get_level(pin)) continue;
#define WAIT_HIGH(pin) while (!gpio_get_level(pin)) continue;

/**
 * @brief   Constructor for I2C which sets up I2C pins.
 *
 */
I2C::I2C() {
  m_sda = I2C_SDA;
  m_scl = I2C_SCL;
  m_addr = SSD_ADDR;

  gpio_config_t io_conf = {};
  io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
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
  DELAY(3);
  
  SET_LOW(m_sda);
  DELAY(3);
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
  DELAY(3);

  SET_HIGH(m_sda);
  DELAY(3);
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
    DELAY(3);
    SET_LOW(m_scl);
    DELAY(3);

    byte <<= 1;
  }

  // ACK signal - SDA pulled down during SCL HIGH
  SET_HIGH(m_sda);
  SET_HIGH(m_scl);
  DELAY(3);

  bool ack = !gpio_get_level(m_sda);
  
  SET_LOW(m_scl);
  DELAY(3);

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
 * 
 */
void I2C::transaction(uint8_t *data, size_t len)
{
  start();

  if (!sendByte(m_addr << 1))
    stop();
  if (!sendByte(data[0]))
    stop();
  for(int i = 1; i < len; i++) {
    if (!sendByte(data[i]))
      stop();
  }

  stop();
}
