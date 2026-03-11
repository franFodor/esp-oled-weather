/**
 * I2C.h
 *
 * Created on: Mar 9, 2026
 *     Author: Fran Fodor
 */

#ifndef I2C_H_
#define I2C_H_

#include "soc/gpio_num.h"
#include <cstddef>
#include <stdint.h>

class I2C
{
public:
  I2C();
  bool       transaction(uint8_t *data, size_t len);

private:
  void       start();
  void       stop();
  bool       sendByte(uint8_t byte);

  gpio_num_t m_sda;
  gpio_num_t m_scl;
  uint8_t    m_addr;
};

#endif
