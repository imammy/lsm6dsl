#pragma once

#include <stdint.h>

#define LSM6DSL__WHO_AM_I_DEVICE_ID 0x6A

// Sensitivity value g full scale（mg/LSB）
#define LSM6DSL__ACC_SENSITIVITY_FOR_FS_2G   0.061  
#define LSM6DSL__ACC_SENSITIVITY_FOR_FS_4G   0.122  
#define LSM6DSL__ACC_SENSITIVITY_FOR_FS_8G   0.244  
#define LSM6DSL__ACC_SENSITIVITY_FOR_FS_16G  0.488  

// Sensitivity value  dps full scale（mdps/LSB）
#define LSM6DSL__GYRO_SENSITIVITY_FOR_FS_125DPS   04.375  
#define LSM6DSL__GYRO_SENSITIVITY_FOR_FS_245DPS   08.750  
#define LSM6DSL__GYRO_SENSITIVITY_FOR_FS_500DPS   17.500  
#define LSM6DSL__GYRO_SENSITIVITY_FOR_FS_1000DPS  35.000  
#define LSM6DSL__GYRO_SENSITIVITY_FOR_FS_2000DPS  70.000  

/*******************************************************************************
* Register      : CTRL1_XL
* Address       : 0x10
* Bit Group Name: ODR_XL
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_POWER_DOWN  =0x00,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_13Hz         =0x10,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_26Hz 		 =0x20,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_52Hz 		 =0x30,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_104Hz 		 =0x40,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_208Hz 		 =0x50,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_416Hz 		 =0x60,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_833Hz 		 =0x70,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_1660Hz 		 =0x80,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_3330Hz 		 =0x90,
  	LSM6DSL__ACC_ODR_XL_POWER_MODE_6660Hz 		 =0xA0,
} lsm6dsl__acc_odr_xl_power_mode_t;

#define LSM6DSL__ACC_ODR_XL_MASK  	0xF0

/*******************************************************************************
* Register      : CTRL1_XL
* Address       : 0x10
* Bit Group Name: FS_XL
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__ACC_FS_XL_2g 		 =0x00,
  	LSM6DSL__ACC_FS_XL_16g 		 =0x04,
  	LSM6DSL__ACC_FS_XL_4g 		 =0x08,
  	LSM6DSL__ACC_FS_XL_8g 		 =0x0C,
} lsm6dsl__acc_fs_xl_t;

#define LSM6DSL__ACC_FS_XL_MASK 0x0C

/*******************************************************************************
* Register      : CTRL2_G
* Address       : 0x11
* Bit Group Name: ODR_G
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_POWER_DOWN 		 =0x00,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_13Hz 		 =0x10,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_26Hz 		 =0x20,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_52Hz 		 =0x30,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_104Hz 		 =0x40,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_208Hz 		 =0x50,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_416Hz 		 =0x60,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_833Hz 		 =0x70,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_1660Hz 		 =0x80,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_3330Hz 		 =0x90,
  	LSM6DSL__GYRO_ODR_G_POWER_MODE_6660Hz 		 =0xA0,
} lsm6dsl__gyro_odr_g_power_mode_t;

#define LSM6DSL__GYRO_ODR_G_MASK 0xF0

/*******************************************************************************
* Register      : CTRL2_G
* Address       : 0X11
* Bit Group Name: FS_G
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__GYRO_FS_G_245dps 		 =0x00,
  	LSM6DSL__GYRO_FS_G_500dps 		 =0x04,
  	LSM6DSL__GYRO_FS_G_1000dps 		 =0x08,
  	LSM6DSL__GYRO_FS_G_2000dps 		 =0x0C,
} lsm6dsl__gyro_fs_g_t;

#define LSM6DSL__GYRO_FS_G_MASK 0x0C

/*******************************************************************************
* Register      : CTRL2_G
* Address       : 0x11
* Bit Group Name: FS_125
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__GYRO_FS_125_DISABLED 		 =0x00,
  	LSM6DSL__GYRO_FS_125_ENABLED 		 =0x02,
} lsm6dsl__gyro_fs_125_t;

#define LSM6DSL__GYRO_FS_125_MASK 0x02

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0x12
* Bit Group Name: IF_INC
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__ACC_GYRO_IF_INC_DISABLED 		 =0x00,
  	LSM6DSL__ACC_GYRO_IF_INC_ENABLED 		 =0x04,
} lsm6dsl__acc_if_inc_t;

#define LSM6DSL__ACC_IF_INC_MASK 0x04

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0x12
* Bit Group Name: BDU
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__ACC_BDU_CONTINUOS 		 =0x00,
  	LSM6DSL__ACC_BDU_BLOCK_UPDATE 		 =0x40,
} lsm6dsl__acc_bdu_t ;

#define LSM6DSL__ACC_BDU_MASK 0x40

/*******************************************************************************
* Register      : FIFO_CTRL5
* Address       : 0x0A
* Bit Group Name: FIFO_MODE
* Permission    : RW
*******************************************************************************/
typedef enum {
  	LSM6DSL__FIFO_MODE_BYPASS 		 =0x00,
  	LSM6DSL__FIFO_MODE_FIFO 		 =0x01,
  	LSM6DSL__FIFO_MODE_STREAM 		 =0x02,
  	LSM6DSL__FIFO_MODE_STF 		 =0x03,
  	LSM6DSL__FIFO_MODE_BTS 		 =0x04,
  	LSM6DSL__FIFO_MODE_DYN_STREAM 		 =0x05,
  	LSM6DSL__FIFO_MODE_DYN_STREAM_2 		 =0x06,
  	LSM6DSL__FIFO_MODE_BTF 		 =0x07,
} lsm6dsl__fifo_mode_t;

#define LSM6DSL__FIFO_MODE_MASK 0x07

typedef union{
    uint8_t id;
    int16_t x_acc;
    int16_t y_acc;
    int16_t z_acc;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
    uint8_t fs_xl;
    uint8_t fs_125;
    uint8_t fs_g;
}lsm6dsl__u_read_t;

typedef struct {
    lsm6dsl__u_read_t data;
    uint16_t len;
}lsm6dsl__read_t;

typedef enum {
  	LSM6DSL__SUCCESS =0,
    LSM6DSL__ERROR,
} lsm6dsl__ret_code_t;

typedef void (*lsm6dsl__read_callback_t)(lsm6dsl__ret_code_t ret,lsm6dsl__read_t *p_read);
typedef void (*lsm6dsl__set_callback_t)(lsm6dsl__ret_code_t ret);


void lsm6dsl__init(void);
lsm6dsl__ret_code_t lsm6dsl__get_device_id(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_acc_x(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_acc_y(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_acc_z(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_acc_fs_xl(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_gyro_x(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_gyro_y(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_gyro_z(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_gyro_fs_125(lsm6dsl__read_callback_t);
lsm6dsl__ret_code_t lsm6dsl__get_gyro_fs_g(lsm6dsl__read_callback_t);

lsm6dsl__ret_code_t lsm6dsl__set_acc_odr_power_mode(lsm6dsl__acc_odr_xl_power_mode_t,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_acc_fs_xl(lsm6dsl__acc_fs_xl_t,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_acc_if_inc(lsm6dsl__acc_if_inc_t ,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_acc_bdu(lsm6dsl__acc_bdu_t ,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_fifo_mode(lsm6dsl__fifo_mode_t ,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_gyro_odr_power_mode(lsm6dsl__gyro_odr_g_power_mode_t,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_gyro_fs_125(lsm6dsl__gyro_fs_125_t,lsm6dsl__set_callback_t);
lsm6dsl__ret_code_t lsm6dsl__set_gyro_fs_g(lsm6dsl__gyro_fs_g_t,lsm6dsl__set_callback_t);


