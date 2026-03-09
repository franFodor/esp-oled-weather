/**
 * I2C.cpp
 *
 * Created on: Mar 9, 2026
 *     Author: Fran Fodor
 */

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "../include/I2C.h"
#include "freertos/FreeRTOS.h"

static const char* TAG = "ESP_I2C";

/**
 * @brief Initialize I2C bus in master mode for QWIIC port.
 *
 */
I2C::I2C()
{
  i2c_master_bus_config_t bus_config = {};
  bus_config.i2c_port = I2C_NUM_0;
  bus_config.sda_io_num = I2C_SDA;
  bus_config.scl_io_num = I2C_SCL;
  bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
  bus_config.glitch_ignore_cnt = 7;
  bus_config.flags.enable_internal_pullup = true;

  ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &m_busHandle));

  i2c_device_config_t dev_config = {};
  dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_config.device_address = SSD_ADDR;
  dev_config.scl_speed_hz = 100000;

  ESP_ERROR_CHECK(i2c_master_bus_add_device(m_busHandle, &dev_config, &m_devHandle));
  ESP_LOGI(TAG, "I2C initilization finished!");
}

/**
 * @brief Function to test I2C by sending display ON/OFF code.
 *
 */
void I2C::testDisplay()
{
  // turn display off
  uint8_t cmd[2] = {0x00, 0xAE}; 
  ESP_ERROR_CHECK(i2c_master_transmit(m_devHandle, cmd, sizeof(cmd), -1));

  vTaskDelay(500);

  // turn display on
  cmd[0] = 0x00;
  cmd[1] = 0xAF; 
  ESP_ERROR_CHECK(i2c_master_transmit(m_devHandle, cmd, sizeof(cmd), -1));
}
