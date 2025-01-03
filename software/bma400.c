#include "bma400.h"
#include "i2c_bitbanging.h"

uint8_t bma400_get_chip_id() {
    uint8_t chid;
    i2c_read_register(BMA400_ADDRESS, BMA400_CHIPID_ADDRESS, &chid, 1);
    return chid;
}

uint8_t bma400_init() {
    uint8_t acc0_cfg_in = 0x02;
    i2c_write_register(BMA400_ADDRESS, BMA400_ACC_CONFIG0, &acc0_cfg_in, 1);
    uint8_t acc0_cfg;
    i2c_read_register(BMA400_ADDRESS, BMA400_ACC_CONFIG0, &acc0_cfg, 1);
    return acc0_cfg;
}
