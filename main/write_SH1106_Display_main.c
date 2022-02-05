/* i2c - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "driver/i2c.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "sh1106.h"

static const char *TAG = "i2c-example";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DELAY_TIME_BETWEEN_ITEMS_MS \
  1000 /*!< delay time between different test items */

#define I2C_MASTER_SCL_IO \
  CONFIG_I2C_MASTER_SCL /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO \
  CONFIG_I2C_MASTER_SDA /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM                                                       \
  I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev \
                                          */
#define I2C_MASTER_FREQ_HZ                                                 \
  10000 /*!< I2C master clock frequency, 10k is slow enough for the cheapo \
           logic analyzer */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0 /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0       /*!< I2C ack value */
#define NACK_VAL 0x1      /*!< I2C nack value */

SemaphoreHandle_t print_mux = NULL;

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void) {
  int i2c_master_port = I2C_MASTER_NUM;
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
      // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_*
      // flags to choose i2c source clock here. */
  };
  esp_err_t err = i2c_param_config(i2c_master_port, &conf);
  if (err != ESP_OK) {
    return err;
  }
  return i2c_driver_install(i2c_master_port, conf.mode,
                            I2C_MASTER_RX_BUF_DISABLE,
                            I2C_MASTER_TX_BUF_DISABLE, 0);
}

esp_err_t check_SH1106() {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (SH1106_I2C_ADDRESS << 1) | WRITE_BIT,
                        ACK_CHECK_EN);
  i2c_master_stop(cmd);
  esp_err_t ret =
      i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  return ret;
}

esp_err_t sh1106_command(uint8_t commandByte, char *commandName) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (SH1106_I2C_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_DIS);
  i2c_master_write_byte(cmd, 0, ACK_CHECK_DIS);
  i2c_master_write_byte(cmd, commandByte, ACK_CHECK_EN);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 50 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  if(ret == ESP_OK) {
    ESP_LOGI(TAG, "COMMAND[%s] SUCCEEDED", commandName);
  } else {
   ESP_LOGI(TAG, "COMMAND[%s] FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!", commandName);
  }
  return ret;
}

esp_err_t initialize_SH1106() {
  esp_err_t ret = sh1106_command(SH1106_DISPLAYOFF, "SH1106_DISPLAYOFF");  // 0xAE
  if(ret == ESP_OK) {

  }
  return ret;
}

esp_err_t check_and_report_SH1106() {
  esp_err_t ret = check_SH1106();
  xSemaphoreTake(print_mux, portMAX_DELAY);
  printf("RESULT of check_SH1106: ");
  if (ret == ESP_OK) {
    printf("OK\r\n");
  } else if (ret == ESP_ERR_TIMEOUT) {
    printf("TIMEOUT\r\n");
  } else {
    printf("UNKNOWN %d\r\n", ret);
  }
  xSemaphoreGive(print_mux);
  return ret;
}

void SH1106_task(void *arg) {
  char *task_name = (char *)arg;
  ESP_LOGI(TAG, "TASK[%s] STARTED", task_name);
  int doContinue = 1;
  esp_err_t ret = check_and_report_SH1106();
  if (ret != ESP_OK) {
    ESP_LOGI(TAG, "TASK[%s] check_and_report_SH1106 FAILED", task_name);
    doContinue = 0;
  } else {
    ret = initialize_SH1106();
    if (ret != ESP_OK) {
      ESP_LOGI(TAG, "TASK[%s] initialize_SH1106 FAILED", task_name);
      doContinue = 0;
    }
  }
  int iteration = 0;
  while (doContinue) {
    ESP_LOGI(TAG, "TASK[%s] iteration: %d", task_name, iteration++);
    // Iterative stuff here
    vTaskDelay(SH1106_CHECK_DELAY_MS / portTICK_RATE_MS);
  }
  ESP_LOGI(TAG, "TASK[%s] ENDED", task_name);
  vSemaphoreDelete(print_mux);
  vTaskDelete(NULL);
}

void app_main(void) {
  print_mux = xSemaphoreCreateMutex();

  ESP_ERROR_CHECK(i2c_master_init());
  // xTaskCreate(i2c_test_task, "i2c_test_task_0", 1024 * 2, (void *)0, 10,
  // NULL); xTaskCreate(i2c_test_task, "i2c_test_task_1", 1024 * 2, (void *)1,
  // 10, NULL);
  xTaskCreate(SH1106_task, "SH1106_task", 1024 * 2, (void *)"SH1106_task", 10,
              NULL);
}
