#include "nfc.h"

CRC_HandleTypeDef hcrc;

void init_nfc() {
    nfc_init_crc_engine();
    i2c_write_register(NFC_ADDRESS, NFC_GET_I2C_SESSION, NULL, 0);
    uint8_t data[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
    c_apdu_t select_ndef_tag_application = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_SELECT_FILE,
        .P1 = 0x04,
        .P2 = 0x00,
        .LC = 0x07,
        .data = data,
        .LE = 0
    };

    c_apdu_r response;

    nfc_send_apdu_p(&select_ndef_tag_application);
    nfc_read_apdu_r(&response, select_ndef_tag_application.LE);
}

void nfc_send_apdu_p(c_apdu_t* c_apdu_t_to_send) {
    uint16_t crc;
    CRC->CR |= CRC_CR_RESET;
    i2c_start_condition();
    i2c_master_write(NFC_ADDRESS);
    for (int i = 0; i < 6; i++)
    {
        i2c_master_write(((uint8_t *)c_apdu_t_to_send)[i]);
        Accu_CRC8(((uint8_t *)c_apdu_t_to_send)[i]);
    }

    for (int i = 0; i < c_apdu_t_to_send->LC; i++) {
        i2c_master_write(c_apdu_t_to_send->data[i]);
        Accu_CRC8(c_apdu_t_to_send->data[i]);
    }
    i2c_master_write(c_apdu_t_to_send->LE);
    Accu_CRC8(c_apdu_t_to_send->LE);
    crc = CRC->DR;
    i2c_master_write(crc);
    i2c_master_write(crc >> 8);
    i2c_stop_condition();
}

void nfc_read_apdu_r(c_apdu_r* c_apdu_t_to_read, uint8_t LE) {

    i2c_start_condition();
    i2c_master_write(NFC_ADDRESS | 1);
    //PCB
    i2c_master_read(1);
    for (int i = 0; i < LE; i++)
    {
        c_apdu_t_to_read->data[i] = i2c_master_read(1);
    }
    c_apdu_t_to_read->SW1 = i2c_master_read(1);
    c_apdu_t_to_read->SW2 = i2c_master_read(1);
    // two CRC bytes
    i2c_master_read(1);
    i2c_master_read(0);
    i2c_stop_condition();
}

void nfc_init_crc_engine() {
    __HAL_RCC_CRC_CLK_ENABLE();
    CRC->INIT = 0xC6C6;
    CRC->POL = 0x1021;
    CRC->CR = CRC_CR_REV_OUT | CRC_CR_REV_IN_0 | CRC_CR_POLYSIZE_0;

    /*
    Example from the DS, CRC must be 0xC035
    uint8_t din[] = {0x02, 0x00, 0xA4, 0x04, 0x00, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00};
    for (unsigned i = 0; i < sizeof(din); i++) {
        //CRC->DR = din[i];
        Accu_CRC8(din[i]);
    }
    dout = CRC->DR;
    */
}
