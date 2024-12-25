#include "bma400.h"
#include "i2c_bitbanging.h"

uint8_t bma400_get_chip_id() {
    uint8_t chid;
    i2c_read_register(BMA400_ADDRESS, BMA400_CHIPID_ADDRESS, &chid, 1);
    return chid;
}