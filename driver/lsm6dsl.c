#include "lsm6dsl.h"

#include <string.h>

#include "sl_app_log.h"
#include "sl_spidrv_instances.h"
#include "spidrv.h"

#define SPI_HANDLE sl_spidrv_spi_lsm6dsl_handle

#define LSM6DSL_R_MODE_BIT 0x80
#define LSM6DSL_W_MODE_BIT 0x00

#define LSM6DSL_REGISTER_WHO_AM_I 0x0f
#define LSM6DSL_REGISTER_FIFO_CTRL5 0x0A
#define LSM6DSL_REGISTER_CTRL1_XL 0x10
#define LSM6DSL_REGISTER_CTRL2_G 0x11
#define LSM6DSL_REGISTER_CTRL3_C 0x12
#define LSM6DSL_REGISTER_OUTX_L_G 0x22
#define LSM6DSL_REGISTER_OUTX_H_G 0x23
#define LSM6DSL_REGISTER_OUTY_L_G 0x24
#define LSM6DSL_REGISTER_OUTY_H_G 0x25
#define LSM6DSL_REGISTER_OUTZ_L_G 0x26
#define LSM6DSL_REGISTER_OUTZ_H_G 0x27
#define LSM6DSL_REGISTER_OUTX_L_XL 0x28
#define LSM6DSL_REGISTER_OUTX_H_XL 0x29
#define LSM6DSL_REGISTER_OUTY_L_XL 0x2A
#define LSM6DSL_REGISTER_OUTY_H_XL 0x2B
#define LSM6DSL_REGISTER_OUTZ_L_XL 0x2C
#define LSM6DSL_REGISTER_OUTZ_H_XL 0x2D

typedef struct {
  union {
    uint8_t byte;
    struct {
      uint8_t odr_xl3 : 1;
      uint8_t odr_xl2 : 1;
      uint8_t odr_xl1 : 1;
      uint8_t odr_xl0 : 1;
      uint8_t fs_xl1 : 1;
      uint8_t fs_xl0 : 1;
      uint8_t lpf1_bw_sel : 1;
      uint8_t bw0_xl : 1;
    } bit;
  } filed;
} ctrl1_xl_t;

typedef struct {
  union {
    uint8_t byte;
    struct {
      uint8_t odr_g3 : 1;
      uint8_t odr_g2 : 1;
      uint8_t odr_g1 : 1;
      uint8_t odr_g0 : 1;
      uint8_t fs_g1 : 1;
      uint8_t fs_g0 : 1;
      uint8_t fs_125 : 1;
      uint8_t reserved : 1;
    } bit;
  } filed;
} ctrl2_g_t;

typedef struct {
  union {
    uint8_t byte;
    struct {
      uint8_t boot : 1;
      uint8_t bdu : 1;
      uint8_t h_lactive : 1;
      uint8_t ppod : 1;
      uint8_t sim : 1;
      uint8_t if_inc : 1;
      uint8_t ble : 1;
      uint8_t sw_reset : 1;
    } bit;
  } filed;
} ctrl3_c_t;

typedef struct {
  union {
    uint8_t byte;
    struct {
      uint8_t reserved : 1;
      uint8_t odr_fifo_3 : 1;
      uint8_t odr_fifo_2 : 1;
      uint8_t odr_fifo_1 : 1;
      uint8_t odr_fifo_0 : 1;
      uint8_t fifo_mode_2 : 1;
      uint8_t fifo_mode_1 : 1;
      uint8_t fifo_mode_0 : 1;
    } bit;
  } filed;
} fifo_ctrl5_t;

typedef union {
  ctrl1_xl_t ctrl1_xl;
  ctrl2_g_t ctrl2_g;
  ctrl3_c_t ctrl3_c;
  fifo_ctrl5_t fifo_ctrl5;
} lsm6dsl_register_t;

static uint8_t s_spi_write_buf[256];
static uint8_t s_spi_read_buf[256];

static lsm6dsl__set_callback_t s_set_callback;
static lsm6dsl__read_callback_t s_read_callback;
static lsm6dsl__read_t s_read;
static lsm6dsl_register_t s_lsm6dsl_register;

void lsm6dsl__init(void) {
  sl_app_log("%s\n", __func__);
  s_read_callback = NULL;
  memset(&s_read, 0x00, sizeof(s_read));
  memset(s_spi_write_buf, 0x00, sizeof(s_spi_write_buf));
  memset(s_spi_read_buf, 0x00, sizeof(s_spi_read_buf));
}

static void on_lsm6dsl_get_device_id_callback(struct SPIDRV_HandleData *handle,
                                              Ecode_t transferStatus,
                                              int itemsTransferred) {
  // sl_app_log("device id:0x%02x\n", s_spi_read_buf[1]);
  memcpy(&s_read, &s_spi_read_buf[1], 1);
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_read_callback(LSM6DSL__ERROR, &s_read);
  } else {
    s_read_callback(LSM6DSL__SUCCESS, &s_read);
  }
}

lsm6dsl__ret_code_t lsm6dsl__get_device_id(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_WHO_AM_I | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_device_id_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_gyro_x_callback(struct SPIDRV_HandleData *handle,
                                           Ecode_t transferStatus,
                                           int itemsTransferred) {
  static uint8_t reg_cnt = 0;
  static uint8_t reg_value[2];
  reg_value[reg_cnt] = s_spi_read_buf[1];
  reg_cnt++;

  // sl_app_log("gyro x[%d]:0x%02x\n", reg_cnt, s_spi_read_buf[1]);

  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  // 2byteレジスタなので２回読む
  if (reg_cnt == 1) {
    s_spi_write_buf[0] =
        (LSM6DSL_REGISTER_OUTX_L_G + reg_cnt) | LSM6DSL_R_MODE_BIT;
    s_spi_write_buf[1] = 0;
    if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                         on_lsm6dsl_get_gyro_x_callback) != ECODE_OK) {
      s_read_callback(LSM6DSL__ERROR, &s_read);
    }
    return;
  }

  reg_cnt = 0;
  s_read.data.x_gyro = ((((int16_t)reg_value[1]) << 8) + (int16_t)reg_value[0]);
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_gyro_x(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_OUTX_L_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_gyro_x_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_gyro_y_callback(struct SPIDRV_HandleData *handle,
                                           Ecode_t transferStatus,
                                           int itemsTransferred) {
  static uint8_t reg_cnt = 0;
  static uint8_t reg_value[2];
  reg_value[reg_cnt] = s_spi_read_buf[1];
  reg_cnt++;

  // sl_app_log("gyro y[%d]:0x%02x\n", reg_cnt, s_spi_read_buf[1]);

  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  // 2byteレジスタなので２回読む
  if (reg_cnt == 1) {
    s_spi_write_buf[0] =
        (LSM6DSL_REGISTER_OUTY_L_G + reg_cnt) | LSM6DSL_R_MODE_BIT;
    s_spi_write_buf[1] = 0;
    if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                         on_lsm6dsl_get_gyro_y_callback) != ECODE_OK) {
      s_read_callback(LSM6DSL__ERROR, &s_read);
    }
    return;
  }

  reg_cnt = 0;
  s_read.data.y_gyro = ((((int16_t)reg_value[1]) << 8) + (int16_t)reg_value[0]);
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_gyro_y(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_OUTY_L_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_gyro_y_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_gyro_z_callback(struct SPIDRV_HandleData *handle,
                                           Ecode_t transferStatus,
                                           int itemsTransferred) {
  static uint8_t reg_cnt = 0;
  static uint8_t reg_value[2];
  reg_value[reg_cnt] = s_spi_read_buf[1];
  reg_cnt++;

  // sl_app_log("gyro y[%d]:0x%02x\n", reg_cnt, s_spi_read_buf[1]);

  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  // 2byteレジスタなので２回読む
  if (reg_cnt == 1) {
    s_spi_write_buf[0] =
        (LSM6DSL_REGISTER_OUTZ_L_G + reg_cnt) | LSM6DSL_R_MODE_BIT;
    s_spi_write_buf[1] = 0;
    if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                         on_lsm6dsl_get_gyro_z_callback) != ECODE_OK) {
      s_read_callback(LSM6DSL__ERROR, &s_read);
    }
    return;
  }

  reg_cnt = 0;
  s_read.data.z_gyro = ((((int16_t)reg_value[1]) << 8) + (int16_t)reg_value[0]);
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_gyro_z(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_OUTZ_L_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_gyro_z_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void
on_lsm6dsl_get_gyro_fs_125_callback(struct SPIDRV_HandleData *handle,
                                    Ecode_t transferStatus,
                                    int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  s_read.data.fs_125 = LSM6DSL__GYRO_FS_125_MASK & s_spi_read_buf[1];
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t
lsm6dsl__get_gyro_fs_125(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_gyro_fs_125_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_gyro_fs_g_callback(struct SPIDRV_HandleData *handle,
                                              Ecode_t transferStatus,
                                              int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  s_read.data.fs_g = LSM6DSL__GYRO_FS_G_MASK & s_spi_read_buf[1];
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_gyro_fs_g(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_gyro_fs_g_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_acc_x_callback(struct SPIDRV_HandleData *handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred) {
  static uint8_t reg_cnt = 0;
  static uint8_t reg_value[2];
  reg_value[reg_cnt] = s_spi_read_buf[1];
  reg_cnt++;

  // sl_app_log("acceleration x[%d]:0x%02x\n", reg_cnt, s_spi_read_buf[1]);

  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  if (reg_cnt == 1) {
    s_spi_write_buf[0] =
        (LSM6DSL_REGISTER_OUTX_L_XL + reg_cnt) | LSM6DSL_R_MODE_BIT;
    s_spi_write_buf[1] = 0;
    if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                         on_lsm6dsl_get_acc_x_callback) != ECODE_OK) {
      s_read_callback(LSM6DSL__ERROR, &s_read);
    }
    return;
  }

  reg_cnt = 0;
  s_read.data.x_acc = ((((int16_t)reg_value[1]) << 8) + (int16_t)reg_value[0]);
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_acc_x(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_OUTX_L_XL | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_acc_x_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_acc_y_callback(struct SPIDRV_HandleData *handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred) {
  static uint8_t reg_cnt = 0;
  static uint8_t reg_value[2];
  reg_value[reg_cnt] = s_spi_read_buf[1];
  reg_cnt++;

  // sl_app_log("acceleration y[%d]:0x%02x\n", reg_cnt, s_spi_read_buf[1]);

  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  if (reg_cnt == 1) {
    s_spi_write_buf[0] =
        (LSM6DSL_REGISTER_OUTY_L_XL + reg_cnt) | LSM6DSL_R_MODE_BIT;
    s_spi_write_buf[1] = 0;
    if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                         on_lsm6dsl_get_acc_y_callback) != ECODE_OK) {
      s_read_callback(LSM6DSL__ERROR, &s_read);
    }
    return;
  }

  reg_cnt = 0;
  s_read.data.y_acc = ((((int16_t)reg_value[1]) << 8) + (int16_t)reg_value[0]);
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_acc_y(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_OUTY_L_XL | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_acc_y_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_acc_z_callback(struct SPIDRV_HandleData *handle,
                                          Ecode_t transferStatus,
                                          int itemsTransferred) {
  static uint8_t reg_cnt = 0;
  static uint8_t reg_value[2];
  reg_value[reg_cnt] = s_spi_read_buf[1];
  reg_cnt++;

  // sl_app_log("acceleration z[%d]:0x%02x\n", reg_cnt, s_spi_read_buf[1]);

  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  if (reg_cnt == 1) {
    s_spi_write_buf[0] =
        (LSM6DSL_REGISTER_OUTZ_L_XL + reg_cnt) | LSM6DSL_R_MODE_BIT;
    s_spi_write_buf[1] = 0;
    if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                         on_lsm6dsl_get_acc_z_callback) != ECODE_OK) {
      s_read_callback(LSM6DSL__ERROR, &s_read);
    }
    return;
  }

  reg_cnt = 0;
  s_read.data.z_acc = ((((int16_t)reg_value[1]) << 8) + (int16_t)reg_value[0]);
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_acc_z(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_OUTZ_L_XL | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_acc_z_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_acc_fs_xl_callback(struct SPIDRV_HandleData *handle,
                                              Ecode_t transferStatus,
                                              int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    s_read_callback(LSM6DSL__ERROR, &s_read);
    return;
  }

  s_read.data.fs_xl = LSM6DSL__ACC_FS_XL_MASK & s_spi_read_buf[1];
  s_read_callback(LSM6DSL__SUCCESS, &s_read);
}

lsm6dsl__ret_code_t lsm6dsl__get_acc_fs_xl(lsm6dsl__read_callback_t callback) {
  s_read_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL1_XL | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;
  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_acc_fs_xl_callback) != ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_set_ctrl1_xl_callback(struct SPIDRV_HandleData *handle,
                                             Ecode_t transferStatus,
                                             int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
  } else {
    s_set_callback(LSM6DSL__SUCCESS);
  }
}

static void on_lsm6dsl_get_ctrl1_xl_set_acc_odr_power_mode_callback(
    struct SPIDRV_HandleData *handle, Ecode_t transferStatus,
    int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL1_XL:0x%02x\n", s_spi_read_buf[1]);

  uint8_t ctrl1_xl = s_spi_read_buf[1];
  ctrl1_xl &= ~LSM6DSL__ACC_ODR_XL_MASK;
  ctrl1_xl |= s_lsm6dsl_register.ctrl1_xl.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL1_XL;
  s_spi_write_buf[1] = ctrl1_xl;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl1_xl_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t
lsm6dsl__set_acc_odr_power_mode(lsm6dsl__acc_odr_xl_power_mode_t mode,
                                lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl1_xl.filed.byte = mode;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL1_XL | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(
          SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
          on_lsm6dsl_get_ctrl1_xl_set_acc_odr_power_mode_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void
on_lsm6dsl_get_ctrl1_xl_set_acc_fs_xl_callback(struct SPIDRV_HandleData *handle,
                                               Ecode_t transferStatus,
                                               int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL1_XL:0x%02x\n", s_spi_read_buf[1]);
  uint8_t ctrl1_xl = s_spi_read_buf[1];
  ctrl1_xl &= ~LSM6DSL__ACC_FS_XL_MASK;
  ctrl1_xl |= s_lsm6dsl_register.ctrl1_xl.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL1_XL;
  s_spi_write_buf[1] = ctrl1_xl;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl1_xl_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t lsm6dsl__set_acc_fs_xl(lsm6dsl__acc_fs_xl_t value,
                                           lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl1_xl.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL1_XL | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_ctrl1_xl_set_acc_fs_xl_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_set_ctrl3_c_callback(struct SPIDRV_HandleData *handle,
                                            Ecode_t transferStatus,
                                            int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
  } else {
    s_set_callback(LSM6DSL__SUCCESS);
  }
}

static void
on_lsm6dsl_get_ctrl3_c_set_acc_if_inc_callback(struct SPIDRV_HandleData *handle,
                                               Ecode_t transferStatus,
                                               int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL3_C:0x%02x\n", s_spi_read_buf[1]);

  uint8_t ctrl3_c = s_spi_read_buf[1];
  ctrl3_c &= ~LSM6DSL__ACC_IF_INC_MASK;
  ctrl3_c |= s_lsm6dsl_register.ctrl3_c.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL3_C;
  s_spi_write_buf[1] = ctrl3_c;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl3_c_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t lsm6dsl__set_acc_if_inc(lsm6dsl__acc_if_inc_t value,
                                            lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl3_c.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL3_C | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_ctrl3_c_set_acc_if_inc_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void
on_lsm6dsl_get_ctrl3_c_set_acc_bdu_callback(struct SPIDRV_HandleData *handle,
                                            Ecode_t transferStatus,
                                            int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL3_C:0x%02x\n", s_spi_read_buf[1]);

  uint8_t ctrl3_c = s_spi_read_buf[1];
  ctrl3_c &= ~LSM6DSL__ACC_BDU_MASK;
  ctrl3_c |= s_lsm6dsl_register.ctrl3_c.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL3_C;
  s_spi_write_buf[1] = ctrl3_c;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl3_c_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t lsm6dsl__set_acc_bdu(lsm6dsl__acc_bdu_t value,
                                         lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl3_c.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL3_C | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_ctrl3_c_set_acc_bdu_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_set_fifo_ctrl5_callback(struct SPIDRV_HandleData *handle,
                                               Ecode_t transferStatus,
                                               int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
  } else {
    s_set_callback(LSM6DSL__SUCCESS);
  }
}

static void on_lsm6dsl_get_fifo_ctrl5_set_fifo_mode_callback(
    struct SPIDRV_HandleData *handle, Ecode_t transferStatus,
    int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("FIFO_CTRL5:0x%02x\n", s_spi_read_buf[1]);

  uint8_t fifo_ctrl5 = s_spi_read_buf[1];
  fifo_ctrl5 &= ~LSM6DSL__FIFO_MODE_MASK;
  fifo_ctrl5 |= s_lsm6dsl_register.fifo_ctrl5.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_FIFO_CTRL5;
  s_spi_write_buf[1] = fifo_ctrl5;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_fifo_ctrl5_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t lsm6dsl__set_fifo_mode(lsm6dsl__fifo_mode_t value,
                                           lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.fifo_ctrl5.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_FIFO_CTRL5 | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_fifo_ctrl5_set_fifo_mode_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_set_ctrl2_g_callback(struct SPIDRV_HandleData *handle,
                                            Ecode_t transferStatus,
                                            int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
  } else {
    s_set_callback(LSM6DSL__SUCCESS);
  }
}

static void on_lsm6dsl_get_ctrl2_g_set_gyro_odr_power_mode_callback(
    struct SPIDRV_HandleData *handle, Ecode_t transferStatus,
    int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL2_G:0x%02x\n", s_spi_read_buf[1]);

  uint8_t ctrl2_g = s_spi_read_buf[1];
  ctrl2_g &= ~LSM6DSL__GYRO_ODR_G_MASK;
  ctrl2_g |= s_lsm6dsl_register.ctrl2_g.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G;
  s_spi_write_buf[1] = ctrl2_g;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl2_g_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t
lsm6dsl__set_gyro_odr_power_mode(lsm6dsl__gyro_odr_g_power_mode_t value,
                                 lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl2_g.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(
          SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
          on_lsm6dsl_get_ctrl2_g_set_gyro_odr_power_mode_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void on_lsm6dsl_get_ctrl2_g_set_gyro_fs_125_callback(
    struct SPIDRV_HandleData *handle, Ecode_t transferStatus,
    int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL2_G:0x%02x\n", s_spi_read_buf[1]);

  uint8_t ctrl2_g = s_spi_read_buf[1];
  ctrl2_g &= ~LSM6DSL__GYRO_FS_125_MASK;
  ctrl2_g |= s_lsm6dsl_register.ctrl2_g.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G;
  s_spi_write_buf[1] = ctrl2_g;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl2_g_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t lsm6dsl__set_gyro_fs_125(lsm6dsl__gyro_fs_125_t value,
                                             lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl2_g.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_ctrl2_g_set_gyro_fs_125_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}

static void
on_lsm6dsl_get_ctrl2_g_set_gyro_fs_g_callback(struct SPIDRV_HandleData *handle,
                                              Ecode_t transferStatus,
                                              int itemsTransferred) {
  if (transferStatus != ECODE_OK) {
    sl_app_log("%s status:0x%08x\n", __func__, transferStatus);
    s_set_callback(LSM6DSL__ERROR);
    return;
  }

  sl_app_log("CTRL2_G:0x%02x\n", s_spi_read_buf[1]);

  uint8_t ctrl2_g = s_spi_read_buf[1];
  ctrl2_g &= ~LSM6DSL__GYRO_FS_G_MASK;
  ctrl2_g |= s_lsm6dsl_register.ctrl2_g.filed.byte;

  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G;
  s_spi_write_buf[1] = ctrl2_g;

  if (SPIDRV_MTransmit(SPI_HANDLE, s_spi_write_buf, 2,
                       on_lsm6dsl_set_ctrl2_g_callback) != ECODE_OK) {
    s_set_callback(LSM6DSL__ERROR);
  }
}

lsm6dsl__ret_code_t lsm6dsl__set_gyro_fs_g(lsm6dsl__gyro_fs_g_t value,
                                           lsm6dsl__set_callback_t callback) {
  s_lsm6dsl_register.ctrl2_g.filed.byte = value;
  s_set_callback = callback;
  s_spi_write_buf[0] = LSM6DSL_REGISTER_CTRL2_G | LSM6DSL_R_MODE_BIT;
  s_spi_write_buf[1] = 0;

  if (SPIDRV_MTransfer(SPI_HANDLE, s_spi_write_buf, s_spi_read_buf, 2,
                       on_lsm6dsl_get_ctrl2_g_set_gyro_fs_g_callback) !=
      ECODE_OK) {
    return LSM6DSL__ERROR;
  }
  return LSM6DSL__SUCCESS;
}
