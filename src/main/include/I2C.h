/**
 * @file   I2C.h
 *
 * @brief  Functions for I2C communication.
 *
 * @author Fran Fodor
 */

#ifndef I2C_H_
#define I2C_H_

#include "soc/gpio_num.h"
#include <cstddef>
#include <stdint.h>

/**
 * @brief Base class for I2C functionality.
 */
class I2C
{
public:
  I2C();
  bool       transaction(uint8_t *data, size_t len);
  bool       checkConnection();

private:
  void       start();
  void       stop();
  bool       sendByte(uint8_t byte);

  gpio_num_t m_sda;
  gpio_num_t m_scl;
  uint8_t    m_addr;
};

#endif
