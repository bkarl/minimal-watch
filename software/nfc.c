#include "nfc.h"
#include "power_state.h"

void init_nfc() {
    power_enable_light_nfc();
    uint8_t rdata_out[255];

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

    uint8_t data_ndef_select[] = {0x00, 0x01};

    c_apdu_t select_ndef_file = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_SELECT_FILE,
        .P1 = 0x00,
        .P2 = 0x0C,
        .LC = 0x02,
        .data = data_ndef_select,
        .LE = 0
    };

    c_apdu_r response = {
        .data = rdata_out
    };

    HAL_Delay(NFC_WAIT_MS_AFTER_CMD);
    nfc_send_apdu_p(&select_ndef_tag_application, true);
    nfc_read_apdu_r(&response, select_ndef_tag_application.LE);

    nfc_send_apdu_p(&select_ndef_file, false);
    nfc_read_apdu_r(&response, select_ndef_file.LE);
}

void deinit_nfc() {
    power_disable_light_nfc();
}

uint8_t nfc_read_total_ndef_length() {
    uint8_t rdata_out[255];

    c_apdu_r response = {
        .data = rdata_out
    };

    c_apdu_t read_binary = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_READ_BINARY,
        .P1 = 0x00,
        .P2 = 0x00,
        .LC = 0x00,
        .data = 0,
        .LE = 0x02
    };

    nfc_send_apdu_p(&read_binary, true);
    nfc_read_apdu_r(&response, read_binary.LE);

    uint8_t total_len = response.data[1];
    return total_len;
}

bool nfc_read_timestamp_record(RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate) {
    uint8_t rdata_out[255];

    c_apdu_r response = {
        .data = rdata_out
    };

    c_apdu_t read_binary = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_READ_BINARY,
        .P1 = 0x00,
        .P2 = 0x02,
        .LC = 0x00,
        .data = 0,
        .LE = 0x02
    };

    uint8_t total_len = nfc_read_total_ndef_length();

    read_binary.P2 = 2;
    read_binary.LE = total_len;

    nfc_send_apdu_p(&read_binary, true);
    nfc_read_apdu_r(&response, read_binary.LE);
    external_type_ndef_record_t *timestamp_record = (external_type_ndef_record_t*)response.data;
    size_t offset_to_payload = timestamp_record->type_len + NUM_HEADER_BYTES_IN_NFC_PAYLOAD;

    timestamp_record->payload = &response.data[offset_to_payload];
    sDate->Year = timestamp_record->payload[OFFSET_TO_YEAR];
    sDate->Month = timestamp_record->payload[OFFSET_TO_MONTH];
    sDate->Date = timestamp_record->payload[OFFSET_TO_DAY];
    sTime->Hours = timestamp_record->payload[OFFSET_TO_HOUR];
    sTime->Minutes = timestamp_record->payload[OFFSET_TO_MINUTE];
    sTime->Seconds = timestamp_record->payload[OFFSET_TO_SECOND];

    return true;
}

void nfc_update_step_ctr_record(RTC_DateTypeDef *sDate, uint32_t n_steps) {
    uint8_t total_len = nfc_read_total_ndef_length();
    uint8_t rdata_out[255];

    c_apdu_r response = {
        .data = rdata_out
    };

    uint8_t new_record[8] = {sDate->Year, sDate->Month, sDate->Date, n_steps, n_steps >> 8, n_steps >> 16, n_steps >> 24};

    c_apdu_t update_binary = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_UPDATE_BINARY,
        .P1 = 0x00,
        .P2 = 0x02 + total_len,
        .LC = 0x08,
        .data = new_record,
        .LE = 0
    };

    if (total_len < NFC_STEP_RECORD_MIN_LENGTH) {
        nfc_write_step_ctr_message(sDate, n_steps);
        return;
    }

    nfc_send_apdu_p(&update_binary, false);
    nfc_read_apdu_r(&response, update_binary.LE);
}

void nfc_write_step_ctr_message(RTC_DateTypeDef *sDate, uint32_t n_steps) {
    uint8_t rdata_out[255];

    c_apdu_r response = {
        .data = rdata_out
    };

    initial_external_type_ndef_record_t new_record = {.total_message_len = sizeof(initial_external_type_ndef_record_t), .id_byte = NDEF_HEADER, .type_len = EXTERNAL_TYPE_LEN_STEPS, .external_type = EXTERNAL_TYPE_STEPS};

    new_record.payload[0] = sDate->Year;
    new_record.payload[1] = sDate->Month;
    new_record.payload[2] = sDate->Date;
    new_record.payload[3] = n_steps;
    new_record.payload[4] = n_steps >> 8;
    new_record.payload[5] = n_steps >> 16;
    new_record.payload[6] = n_steps >> 24;

    c_apdu_t update_binary = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_UPDATE_BINARY,
        .P1 = 0x00,
        .P2 = 0x00,
        .LC = new_record.total_message_len,
        .data = (uint8_t*)&new_record,
        .LE = 0
    };

    nfc_send_apdu_p(&update_binary, false);
    nfc_read_apdu_r(&response, update_binary.LE);
}

void nfc_send_apdu_p(c_apdu_t* c_apdu_t_to_send, bool has_le) {
    uint16_t crc;
    CRC->CR |= CRC_CR_RESET;
    i2c_start_condition();
    i2c_master_write(NFC_ADDRESS);
    for (int i = 0; i < 6; i++)
    {
        if (i == 5 && c_apdu_t_to_send->LC == 0)
            break;
        i2c_master_write(((uint8_t *)c_apdu_t_to_send)[i]);
        Accu_CRC8(((uint8_t *)c_apdu_t_to_send)[i]);
    }

    for (int i = 0; i < c_apdu_t_to_send->LC; i++) {
        i2c_master_write(c_apdu_t_to_send->data[i]);
        Accu_CRC8(c_apdu_t_to_send->data[i]);
    }
    if (has_le) {
        i2c_master_write(c_apdu_t_to_send->LE);
        Accu_CRC8(c_apdu_t_to_send->LE);
    }
    crc = CRC->DR;

    i2c_master_write(crc);
    i2c_master_write(crc >> 8);
    i2c_stop_condition();
    HAL_Delay(NFC_WAIT_MS_AFTER_CMD);
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
}
