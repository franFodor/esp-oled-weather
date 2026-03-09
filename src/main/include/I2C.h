/**
 * I2C.h
 *
 * Created on: Mar 9, 2026
 *     Author: Fran Fodor
 */

#ifndef I2C_H_
#define I2C_H_

#include "driver/i2c_master.h"

#define I2C_SDA GPIO_NUM_6
#define I2C_SCL GPIO_NUM_7
#define SSD_ADDR 0x3C

class I2C
{
public:
  I2C();
  void                    testDisplay();
private:
  i2c_master_bus_handle_t m_busHandle;
  i2c_master_dev_handle_t m_devHandle;
};

#endif
