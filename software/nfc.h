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

#define NUM_HEADER_BYTES_IN_NFC_PAYLOAD 3

typedef struct __attribute__((packed)) {
    uint8_t id_byte;
    uint8_t type_len;
    uint8_t payload_len;
    char* external_type;
    char* id;
    uint8_t *payload;
} external_type_ndef_record_t;

#define EXTERNAL_TYPE_STEPS  "minimal.watch:steps"
#define EXTERNAL_TYPE_LEN_STEPS sizeof(EXTERNAL_TYPE_STEPS) - 1
#define STEP_CTR_RECORD_SIZE_BYTES 7

typedef struct __attribute__((packed)) {
    uint8_t id_byte;
    uint8_t type_len;
    uint8_t payload_len;
    char external_type[EXTERNAL_TYPE_LEN_STEPS];
    uint8_t payload[STEP_CTR_RECORD_SIZE_BYTES];
} initial_external_type_ndef_record_t;


#define NFC_STEP_RECORD_MIN_LENGTH sizeof(initial_external_type_ndef_record_t)
#define NFC_OFFSET_TO_NDEF_MESSAGE 2
#define NFC_OFFSET_TO_NDEF_STEPS_PAYLOAD_LEN offsetof(initial_external_type_ndef_record_t, payload_len) + NFC_OFFSET_TO_NDEF_MESSAGE

#define OFFSET_TO_YEAR 0
#define OFFSET_TO_MONTH 1
#define OFFSET_TO_DAY 2
#define OFFSET_TO_HOUR 3
#define OFFSET_TO_MINUTE 4
#define OFFSET_TO_SECOND 5



void init_nfc();
void nfc_init_crc_engine();
void nfc_send_apdu_p(c_apdu_t* c_apdu_t_to_send, bool has_le);
void nfc_read_apdu_r(c_apdu_r* c_apdu_t_to_read, uint8_t LE);
void nfc_write_step_ctr_message(RTC_DateTypeDef *sDate, uint32_t n_steps);
void nfc_update_step_ctr_record(RTC_DateTypeDef *sDate, uint32_t n_steps, bool append_new_record);
bool nfc_read_timestamp_record(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
void nfc_read_data_from_memory(uint8_t *dst, uint8_t offset, uint8_t len);
void nfc_clear_ndef_message();

#define NFC_INT_PIN GPIO_PIN_9
#define NFC_INT_PORT GPIOB

#define NDEF_HEADER 0xD4 //0b11010100 first record in NDEF message, last in message, short record, no ID length, TNF 4 (external)