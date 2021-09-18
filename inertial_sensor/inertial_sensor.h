#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef struct __attribute__ ((__packed__)){
  uint16_t acc_x;
  uint16_t acc_y;
  uint16_t acc_z;
  uint16_t gyro_x;
  uint16_t gyro_y;
  uint16_t gyro_z;
} acc_gyro_flash_data_t;

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} inertial_sensor__acc_gyro_t;

typedef enum {
  	INERTIAL_SENSOR__SUCCESS =0,
    INERTIAL_SENSOR__ERROR,
} inertial_sensor__ret_code_t;

typedef void (*inertial_sensor__get_callback_t)(inertial_sensor__ret_code_t ret, inertial_sensor__acc_gyro_t *p_read);
typedef void (*inertial_sensor__set_callback_t)(inertial_sensor__ret_code_t ret);

void inertial_sensor__init(void);
bool inertial_sensor__finished(void);
void inertial_sensor__get_acceleration(inertial_sensor__get_callback_t);
void inertial_sensor__get_gyroscope(inertial_sensor__get_callback_t);
