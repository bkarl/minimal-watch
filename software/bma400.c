#include "bma400.h"
#include "i2c_bitbanging.h"
#include "rtc.h"
#include "nfc.h"

uint8_t bma400_get_chip_id() {
    uint8_t chid;
    i2c_read_register(BMA400_ADDRESS, BMA400_CHIPID_ADDRESS, &chid, 1);
    return chid;
}

void bma400_init() {
    uint8_t acc0_cfg_in[] = {BMA400_ACC_CONFIG0_DATA, BMA400_ACC_CONFIG1_DATA, BMA400_ACC_CONFIG2_ADDRESS, BMA400_ACC_CONFIG2_DATA};
    i2c_write_register(BMA400_ADDRESS, BMA400_ACC_CONFIG0_ADDRESS, acc0_cfg_in, 1);
    HAL_Delay(2);
    i2c_write_register(BMA400_ADDRESS, BMA400_ACC_CONFIG1_ADDRESS, &acc0_cfg_in[1], 3);
    uint8_t int_cfg_in[] = {BMA400_INT_CONFIG0_DATA, 
                            BMA400_INT_CONFIG1_ADDRESS, BMA400_INT_CONFIG1_DATA, 
                            BMA400_INT1_MAP_ADDRESS, BMA400_INT_MAP1_DATA,
                            BMA400_INT2_MAP_ADDRESS, BMA400_INT_MAP2_DATA, 
                            BMA400_INT12_MAP_ADDRESS, BMA400_INT_MAP12_DATA, 
                            BMA400_INT12_IO_CTRL_ADDRESS, BMA400_INT12_IO_CTRL_DATA};
    i2c_write_register(BMA400_ADDRESS, BMA400_INT_CONFIG0_ADDRESS, int_cfg_in, sizeof(int_cfg_in));
}

void bma400_clear_interrupt_status() {
    uint8_t int_stat[3];
    i2c_read_register(BMA400_ADDRESS, BMA400_ACC_INT_STAT0_ADDRESS, int_stat, 3);
    return;
}

void bma400_write_step_ctr_value_to_nfc(bool clear_ctr) {
    RTC_DateTypeDef sDate;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    uint32_t steps = bma400_read_step_cnt(clear_ctr);
    nfc_update_step_ctr_record(&sDate, steps);
}

uint32_t bma400_read_step_cnt(bool clear_ctr) {
    uint8_t raw_step_cnt[3];
    uint8_t cmd_reset = BMA400_CMD_RST_STEP_CNT;
    i2c_read_register(BMA400_ADDRESS, BMA400_ACC_STEP_CNT0_ADDRESS, raw_step_cnt, 3);
    uint32_t step_cnt = raw_step_cnt[0] + ((uint32_t)raw_step_cnt[1] << 8) + ((uint32_t)raw_step_cnt[2] << 16);
    if (clear_ctr)
        i2c_write_register(BMA400_ADDRESS, BMA400_CMD_ADDRESS, &cmd_reset, 1);
    return step_cnt;
}