#include "inertial_sensor.h"
#include "lsm6dsl.h"

static acc_gyro_flash_data_t s_acc_gyro_data;

static sl_sleeptimer_timer_handle_t s_lsm6dsl_timer_handle;

static bool s_lsm6dsl_init_finished;

// static float s_X_Last_ODR = 104.0f;
// static float s_G_Last_ODR = 104.0f;
// static uint8_t s_X_isEnabled = 0;
// static uint8_t s_G_isEnabled = 0;

static int32_t s_x_acc, s_y_acc, s_z_acc;
static int32_t s_x_gyro, s_y_gyro, s_z_gyro;

static inertial_sensor__get_callback_t s_get_callback;
static inertial_sensor__set_callback_t s_set_callback;
static inertial_sensor__acc_gyro_t s_acc_gyro;

/*
static void on_timer_lsm6dsl_polling_start(RTCDRV_TimerID_t id, void* user);

static void on_get_gyro_fs_g_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read gyro failed!!\n");
    return;
  }

  // TODO: 多分こいつ読むの一発だけでいいはず
  float sensitivity = 0;
  switch (p_read->data.fs_g) {
    case LSM6DSL__GYRO_FS_G_245dps:
      sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_245DPS;
      break;
    case LSM6DSL__GYRO_FS_G_500dps:
      sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_500DPS;
      break;
    case LSM6DSL__GYRO_FS_G_1000dps:
      sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_1000DPS;
      break;
    case LSM6DSL__GYRO_FS_G_2000dps:
      sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_2000DPS;
      break;
    default:
      sl_app_log("invalid fullscale(gyro) value!!\n");
      break;
  }

  sl_app_log("sensitivity = %f\n", sensitivity);

  // registerの値読んだら、sennsitivityを掛けるらしい？
  s_x_gyro = (int32_t)(s_x_gyro * sensitivity);
  s_y_gyro = (int32_t)(s_y_gyro * sensitivity);
  s_z_gyro = (int32_t)(s_z_gyro * sensitivity);
  sl_app_log("x_gyro = %d\n", s_x_gyro);
  sl_app_log("y_gyro = %d\n", s_y_gyro);
  sl_app_log("z_gyro = %d\n", s_z_gyro);
  RTCDRV_StartTimer(s_lsm6dsl_timer_id, rtcdrvTimerTypeOneshot, 100,
on_timer_lsm6dsl_polling_start, NULL);
}

static void on_get_gyro_fs_125_callback(lsm6dsl__ret_code_t ret,
lsm6dsl__read_t* p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read gyro failed!!\n");
    return;
  }

  if (p_read->data.fs_125 != LSM6DSL__GYRO_FS_125_ENABLED) {
    lsm6dsl__get_gyro_fs_g(on_get_gyro_fs_g_callback);
    return;
  }

  float sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_125DPS;
  // registerの値読んだら、sennsitivityを掛けるらしい？
  s_x_gyro = (int32_t)(s_x_gyro * sensitivity);
  s_y_gyro = (int32_t)(s_y_gyro * sensitivity);
  s_z_gyro = (int32_t)(s_z_gyro * sensitivity);
  sl_app_log("x_gyro = %d\n", s_x_gyro);
  sl_app_log("y_gyro = %d\n", s_y_gyro);
  sl_app_log("z_gyro = %d\n", s_z_gyro);
  RTCDRV_StartTimer(s_lsm6dsl_timer_id, rtcdrvTimerTypeOneshot, 100,
on_timer_lsm6dsl_polling_start, NULL);
}

static void on_get_gyro_z_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read gyro failed!!\n");
    return;
  }

  sl_app_log("gyro_z:%d\n", p_read->data.z_gyro);
  s_z_gyro = p_read->data.z_gyro;

  // Read full scale 125 selection from sensor
  lsm6dsl__get_gyro_fs_125(on_get_gyro_fs_125_callback);
}

static void on_get_gyro_y_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read gyro failed!!\n");
    return;
  }

  sl_app_log("gyro_y:%d\n", p_read->data.y_gyro);
  s_y_gyro = p_read->data.y_gyro;
  lsm6dsl__get_gyro_z(on_get_gyro_z_callback);
}

static void on_get_gyro_x_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read gyro failed!!\n");
    return;
  }

  sl_app_log("gyro_x:%d\n", p_read->data.x_gyro);
  s_x_gyro = p_read->data.x_gyro;
  lsm6dsl__get_gyro_y(on_get_gyro_y_callback);
}

static void on_get_acc_fs_xl_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read acceleration failed!!\n");
    return;
  }

  // TODO: 多分こいつ読むの一発だけでいいはず
  float sensitivity = 0;
  switch (p_read->data.fs_xl) {
    case LSM6DSL__ACC_FS_XL_2g:
      sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_2G;
      break;
    case LSM6DSL__ACC_FS_XL_4g:
      sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_4G;
      break;
    case LSM6DSL__ACC_FS_XL_8g:
      sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_8G;
      break;
    case LSM6DSL__ACC_FS_XL_16g:
      sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_16G;
      break;
    default:
      sl_app_log("invalid fullscale value!!\n");
      break;
  }

  sl_app_log("sensitivity = %f\n", sensitivity);

  // registerの値読んだら、sennsitivityを掛けるらしい？
  s_x_acc = (int32_t)(s_x_acc * sensitivity);
  s_y_acc = (int32_t)(s_y_acc * sensitivity);
  s_z_acc = (int32_t)(s_z_acc * sensitivity);
  sl_app_log("x_acc = %d\n", s_x_acc);
  sl_app_log("y_acc = %d\n", s_y_acc);
  sl_app_log("z_acc = %d\n", s_z_acc);

  lsm6dsl__get_gyro_x(on_get_gyro_x_callback);
}

static void on_get_acc_z_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read acceleration failed!!\n");
    return;
  }

  sl_app_log("acceleration_z:%d\n", p_read->data.z_acc);
  s_z_acc = p_read->data.z_acc;

  // Get LSM6DSL actual sensitivity.
  lsm6dsl__get_acc_fs_xl(on_get_acc_fs_xl_callback);
}

static void on_get_acc_y_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read acceleration failed!!\n");
    return;
  }

  sl_app_log("acceleration_y:%d\n", p_read->data.y_acc);
  s_y_acc = p_read->data.y_acc;
  lsm6dsl__get_acc_z(on_get_acc_z_callback);
}

static void on_get_acc_x_callback(lsm6dsl__ret_code_t ret, lsm6dsl__read_t*
p_read) { sl_app_log("%s\n", __func__);

  if (ret != LSM6DSL__SUCCESS) {
    sl_app_log("read acceleration failed!!\n");
    return;
  }

  sl_app_log("acceleration_x:%d\n", p_read->data.x_acc);
  s_x_acc = p_read->data.x_acc;
  lsm6dsl__get_acc_y(on_get_acc_y_callback);
}

static void on_timer_lsm6dsl_polling_start(RTCDRV_TimerID_t id, void* user) {
  sl_app_log("%s\n", __func__);
  (void)user;

  // レジスタからaccelerationの値を読み出す
  lsm6dsl__get_acc_x(on_get_acc_x_callback);
}
*/

static on_set_gyro_odr_enable_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  s_lsm6dsl_init_finished = true;
}

static on_set_x_odr_enable_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    lsm6dsl__set_gyro_odr_power_mode(LSM6DSL__GYRO_ODR_G_POWER_MODE_104Hz,
                                     on_set_gyro_odr_enable_callback);
  }
}

static on_set_gyro_fs_g_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  lsm6dsl__set_acc_odr_power_mode(LSM6DSL__ACC_ODR_XL_POWER_MODE_104Hz,
                                  on_set_x_odr_enable_callback);
}

static on_set_gyro_fs_125_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    lsm6dsl__set_gyro_fs_g(LSM6DSL__GYRO_FS_G_1000dps,
                           on_set_gyro_fs_g_callback);
  }
}

static on_set_gyro_odr_power_mode_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    lsm6dsl__set_gyro_fs_125(LSM6DSL__GYRO_FS_125_DISABLED,
                             on_set_gyro_fs_125_callback);
  }
}

static on_set_acc_fs_xl_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    /* Output data rate selection - power down */
    lsm6dsl__set_gyro_odr_power_mode(LSM6DSL__GYRO_ODR_G_POWER_MODE_POWER_DOWN,
                                     on_set_gyro_odr_power_mode_callback);
  }
}

static on_set_acc_odr_power_mode_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    /* Full scale selection. */
    lsm6dsl__set_acc_fs_xl(LSM6DSL__ACC_FS_XL_2g, on_set_acc_fs_xl_callback);
  }
}

static on_set_fifo_mode_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    /* Output data rate selection - power down. */
    lsm6dsl__set_acc_odr_power_mode(LSM6DSL__ACC_ODR_XL_POWER_MODE_POWER_DOWN,
                                    on_set_acc_odr_power_mode_callback);
  }
}

static on_set_acc_bdu_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    /* FIFO mode selection */
    lsm6dsl__set_fifo_mode(LSM6DSL__FIFO_MODE_BYPASS,
                           on_set_fifo_mode_callback);
  }
}

static on_set_acc_if_inc_callback(lsm6dsl__ret_code_t ret) {
  // sl_app_log("%s ret:%02x\n", __func__, ret);
  if (ret == LSM6DSL__SUCCESS) {
    /* Enable BDU */
    lsm6dsl__set_acc_bdu(LSM6DSL__ACC_BDU_BLOCK_UPDATE,
                         on_set_acc_bdu_callback);
  }
}

static void on_get_lsm6dsl_device_id_callback(lsm6dsl__ret_code_t ret,
                                              lsm6dsl__read_t *p_read) {
  // sl_app_log("%s\n", __func__);
  // sl_app_log("device id:0x%02x\n", p_read->data.id);
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("read device id failed!!\n");
    return;
  }

  if (p_read->data.id == LSM6DSL__WHO_AM_I_DEVICE_ID) {
    /* Enable register address automatically incremented during a multiple byte
     * access with a serial interface. */
    lsm6dsl__set_acc_if_inc(LSM6DSL__ACC_GYRO_IF_INC_ENABLED,
                            on_set_acc_if_inc_callback);
  }
}

static void on_timer_lsm6dsl_init_start(sl_sleeptimer_timer_handle_t *handle,
                                        void *data) {
  lsm6dsl__get_device_id(on_get_lsm6dsl_device_id_callback);
}

void inertial_sensor__init(void) {
  // sl_app_log("%s start!!\n", __func__);

  s_lsm6dsl_init_finished = false;

  // 6軸センサー (lsm6dsl) の初期化
  // Turn-on time (35ms) 分待つ
  // データシート p24 4.2 Electrical characteristics より
  // https://www.st.com/resource/en/datasheet/lsm6dsl.pdf
  sl_status_t status = sl_sleeptimer_start_timer(
      &s_lsm6dsl_timer_handle, sl_sleeptimer_ms_to_tick(35),
      on_timer_lsm6dsl_init_start, NULL, 0, 0);
  if (status != SL_STATUS_OK) {
    // sl_app_log("sleep timer start error: %d\n", status);
  }
}

bool inertial_sensor__finished(void) { return s_lsm6dsl_init_finished; }

static void on_get_acceleration_fs_xl_callback(lsm6dsl__ret_code_t ret,
                                               lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // TODO:
  // 多分こいつ読むの一発だけでいいはず（イニシャライズとかで保存しとけばいい）
  // 設定値変えたら再度読めばいい
  float sensitivity = 0;
  switch (p_read->data.fs_xl) {
  case LSM6DSL__ACC_FS_XL_2g:
    sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_2G;
    break;
  case LSM6DSL__ACC_FS_XL_4g:
    sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_4G;
    break;
  case LSM6DSL__ACC_FS_XL_8g:
    sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_8G;
    break;
  case LSM6DSL__ACC_FS_XL_16g:
    sensitivity = (float)LSM6DSL__ACC_SENSITIVITY_FOR_FS_16G;
    break;
  default:
    // sl_app_log("%s invalid fullscale value!!\n", __func__);
    break;
  }

  // TODO: sennsitivity を保存
  // sl_app_log("%s : sensitivity = %f\n", __func__, sensitivity);

  // sensistibity ここで掛ける策もあるが uint16 の範囲を超えて
  // flash の容量が足りなくなるので一旦保留
  // s_acc_gyro.x = (int32_t)(s_x_acc * sensitivity);
  // s_acc_gyro.y = (int32_t)(s_y_acc * sensitivity);
  // s_acc_gyro.z = (int32_t)(s_z_acc * sensitivity);
  // sl_app_log("%s : s_acc_gyro.x = %d\n", __func__, s_acc_gyro.x);
  // sl_app_log("%s : s_acc_gyro.y = %d\n", __func__, s_acc_gyro.y);
  // sl_app_log("%s : s_acc_gyro.z = %d\n", __func__, s_acc_gyro.z);

  s_acc_gyro.x = s_x_acc;
  s_acc_gyro.y = s_y_acc;
  s_acc_gyro.z = s_z_acc;

  // 3軸分取得できたので、コールバックする
  s_get_callback(INERTIAL_SENSOR__SUCCESS, &s_acc_gyro);
}

static void on_get_acceleration_z_callback(lsm6dsl__ret_code_t ret,
                                           lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // sl_app_log("%s : %d\n", __func__, p_read->data.z_acc);
  s_z_acc = p_read->data.z_acc;

  // Get LSM6DSL actual sensitivity.
  lsm6dsl__get_acc_fs_xl(on_get_acceleration_fs_xl_callback);
}

static void on_get_acceleration_y_callback(lsm6dsl__ret_code_t ret,
                                           lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // sl_app_log("%s : %d\n", __func__, p_read->data.y_acc);
  s_y_acc = p_read->data.y_acc;
  lsm6dsl__get_acc_y(on_get_acceleration_z_callback);
}

static void on_get_acceleration_x_callback(lsm6dsl__ret_code_t ret,
                                           lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // sl_app_log("%s : %d\n", __func__, p_read->data.x_acc);
  s_x_acc = p_read->data.x_acc;
  lsm6dsl__get_acc_y(on_get_acceleration_y_callback);
}

void inertial_sensor__get_acceleration(
    inertial_sensor__get_callback_t callback) {
  s_get_callback = callback;
  // X軸からスタート
  lsm6dsl__get_acc_x(on_get_acceleration_x_callback);
  return;
}

static void on_get_gyroscope_fs_g_callback(lsm6dsl__ret_code_t ret,
                                           lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    return;
  }

  // TODO: 多分こいつ読むの一発だけでいいはず
  float sensitivity = 0;
  switch (p_read->data.fs_g) {
  case LSM6DSL__GYRO_FS_G_245dps:
    sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_245DPS;
    break;
  case LSM6DSL__GYRO_FS_G_500dps:
    sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_500DPS;
    break;
  case LSM6DSL__GYRO_FS_G_1000dps:
    sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_1000DPS;
    break;
  case LSM6DSL__GYRO_FS_G_2000dps:
    sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_2000DPS;
    break;
  default:
    // sl_app_log("%s : invalid fullscale(gyro) value!!\n", __func__);
    break;
  }

  // TODO: sennsitivity を保存
  // sl_app_log("%s : sensitivity = %f\n", __func__, sensitivity);

  // sensistibity ここで掛ける策もあるが uint16 の範囲を超えて
  // flash の容量が足りなくなるので一旦保留
  // s_acc_gyro.x = (int32_t)(s_x_gyro * sensitivity);
  // s_acc_gyro.y = (int32_t)(s_y_gyro * sensitivity);
  // s_acc_gyro.z = (int32_t)(s_z_gyro * sensitivity);
  // sl_app_log("%s : s_acc_gyro.x = %d\n", __func__, s_acc_gyro.x);
  // sl_app_log("%s : s_acc_gyro.y = %d\n", __func__, s_acc_gyro.y);
  // sl_app_log("%s : s_acc_gyro.z = %d\n", __func__, s_acc_gyro.z);

  s_acc_gyro.x = s_x_gyro;
  s_acc_gyro.y = s_y_gyro;
  s_acc_gyro.z = s_z_gyro;

  // 3軸分取得できたので、コールバックする
  s_get_callback(INERTIAL_SENSOR__SUCCESS, &s_acc_gyro);
}

static void on_get_gyroscope_fs_125_callback(lsm6dsl__ret_code_t ret,
                                             lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    return;
  }

  if (p_read->data.fs_125 != LSM6DSL__GYRO_FS_125_ENABLED) {
    lsm6dsl__get_gyro_fs_g(on_get_gyroscope_fs_g_callback);
    return;
  }

  float sensitivity = (float)LSM6DSL__GYRO_SENSITIVITY_FOR_FS_125DPS;
  // TODO: sennsitivity を保存
  // sl_app_log("%s : sensitivity = %f\n", __func__, sensitivity);

  // sensistibity ここで掛ける策もあるが uint16 の範囲を超えて
  // flash の容量が足りなくなるので一旦保留
  // s_acc_gyro.x = (int32_t)(s_x_gyro * sensitivity);
  // s_acc_gyro.y = (int32_t)(s_y_gyro * sensitivity);
  // s_acc_gyro.z = (int32_t)(s_z_gyro * sensitivity);
  // sl_app_log("%s : s_acc_gyro.x = %d\n", __func__, s_acc_gyro.x);
  // sl_app_log("%s : s_acc_gyro.y = %d\n", __func__, s_acc_gyro.y);
  // sl_app_log("%s : s_acc_gyro.z = %d\n", __func__, s_acc_gyro.z);

  s_acc_gyro.x = s_x_gyro;
  s_acc_gyro.y = s_y_gyro;
  s_acc_gyro.z = s_z_gyro;

  // 3軸分取得できたので、コールバックする
  s_get_callback(INERTIAL_SENSOR__SUCCESS, &s_acc_gyro);
}

static void on_get_gyroscope_z_callback(lsm6dsl__ret_code_t ret,
                                        lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // sl_app_log("%s : %d\n", __func__, p_read->data.z_gyro);
  s_z_gyro = p_read->data.z_gyro;

  // Read full scale 125 selection from sensor
  lsm6dsl__get_gyro_fs_125(on_get_gyroscope_fs_125_callback);
}

static void on_get_gyroscope_y_callback(lsm6dsl__ret_code_t ret,
                                        lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // sl_app_log("%s : %d\n", __func__, p_read->data.y_gyro);
  s_y_gyro = p_read->data.y_gyro;
  lsm6dsl__get_gyro_z(on_get_gyroscope_z_callback);
}

static void on_get_gyroscope_x_callback(lsm6dsl__ret_code_t ret,
                                        lsm6dsl__read_t *p_read) {
  if (ret != LSM6DSL__SUCCESS) {
    // sl_app_log("%s read failed!!\n", __func__);
    s_get_callback(INERTIAL_SENSOR__ERROR, &s_acc_gyro);
    return;
  }

  // sl_app_log("%s : %d\n", __func__, p_read->data.x_gyro);
  s_x_gyro = p_read->data.x_gyro;
  lsm6dsl__get_gyro_y(on_get_gyroscope_y_callback);
}

void inertial_sensor__get_gyroscope(inertial_sensor__get_callback_t callback) {
  s_get_callback = callback;
  lsm6dsl__get_gyro_x(on_get_gyroscope_x_callback);
}
