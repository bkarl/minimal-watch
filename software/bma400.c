#include "bma400.h"
#include "i2c_bitbanging.h"

uint8_t bma400_get_chip_id() {
    uint8_t chid;
    i2c_read_register(BMA400_ADDRESS, BMA400_CHIPID_ADDRESS, &chid, 1);
    return chid;
}

void bma400_init() {
    uint8_t acc0_cfg_in[] = {BMA400_ACC_CONFIG0_DATA, BMA400_ACC_CONFIG1_DATA, BMA400_ACC_CONFIG2_DATA};
    i2c_write_register(BMA400_ADDRESS, BMA400_ACC_CONFIG0_ADDRESS, acc0_cfg_in, 1);
    HAL_Delay(2);
    i2c_write_register(BMA400_ADDRESS, BMA400_ACC_CONFIG1_ADDRESS, &acc0_cfg_in[1], 2);
    uint8_t int_cfg_in[] = {BMA400_INT_CONFIG0_DATA, BMA400_INT_CONFIG1_DATA, BMA400_INT_MAP1_DATA,
                            BMA400_INT_MAP2_DATA, BMA400_INT_MAP12_DATA, BMA400_INT12_IO_CTRL_DATA};
    i2c_write_register(BMA400_ADDRESS, BMA400_INT_CONFIG0_ADDRESS, int_cfg_in, sizeof(int_cfg_in));
}

void bma400_clear_interrupt_status() {
    uint8_t int_stat[3];
    i2c_read_register(BMA400_ADDRESS, BMA400_ACC_INT_STAT0_ADDRESS, int_stat, 3);
}