#pragma once

#include "i2c_bitbanging.h"
#include <stdbool.h>
#define NFC_ADDRESS 0xAC
#define NFC_GET_I2C_SESSION 0x26

#define NFC_INS_SELECT_FILE 0xA4
#define NFC_INS_UPDATE_BINARY 0xD6
#define NFC_INS_READ_BINARY 0xB0
#define NFC_INS_VERIFY 0x20
#define NFC_DISABLE_VERIFICATION 0x26
#define NFC_PCB_BYTE 0x02

#define NFC_WAIT_MS_AFTER_CMD 100

#define Accu_CRC8(value) (*(__IO uint8_t *)(__IO void *)(&CRC->DR) = (value))

typedef struct __attribute__((packed)) {
    uint8_t PCB;
    uint8_t CLA;
    uint8_t INS;
    uint8_t P1;
    uint8_t P2; 
    uint8_t LC;
    uint8_t *data;
    uint8_t LE;
} c_apdu_t;

typedef struct __attribute__((packed)) {
    uint8_t* data;
    uint8_t SW1;
    uint8_t SW2;
} c_apdu_r;

void init_nfc();
void nfc_init_crc_engine();
void nfc_send_apdu_p(c_apdu_t* c_apdu_t_to_send, bool has_le);
void nfc_read_apdu_r(c_apdu_r* c_apdu_t_to_read, uint8_t LE);

#define NFC_INT_PIN GPIO_PIN_9
#define NFC_INT_PORT GPIOB