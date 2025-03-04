#include "nfc.h"
#include "power_state.h"

void nfc_copy_step_ctr_payload(uint8_t *dst_buffer, RTC_DateTypeDef *sDate, uint32_t n_steps);
void nfc_write_data_to_memory(uint8_t *data, uint8_t offset, uint8_t len);
void nfc_set_int_to_wip_mode();

void init_nfc() {
    power_enable_light_nfc();
    uint8_t rdata_out[255];
    c_apdu_r response = {
        .data = rdata_out
    };

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

    nfc_init_crc_engine();
    do {
        i2c_write_register(NFC_ADDRESS, NFC_GET_I2C_SESSION, NULL, 0);

        HAL_Delay(NFC_WAIT_MS_AFTER_CMD);

        nfc_send_apdu_p(&select_ndef_tag_application, true);
        nfc_read_apdu_r(&response, select_ndef_tag_application.LE);
    } while (response.SW1 != 0x90);

    //nfc_set_int_to_wip_mode();

    nfc_send_apdu_p(&select_ndef_file, false);
    nfc_read_apdu_r(&response, select_ndef_file.LE);
}

void nfc_set_int_to_wip_mode() {
    c_apdu_r response;

    uint8_t data_ndef_select[] = {0xE1, 0x01};

    c_apdu_t select_system_file = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_SELECT_FILE,
        .P1 = 0x00,
        .P2 = 0x0C,
        .LC = 0x02,
        .data = data_ndef_select,
        .LE = 0
    };

    nfc_send_apdu_p(&select_system_file, false);
    nfc_read_apdu_r(&response, select_system_file.LE);

    uint8_t system_file[0x11];
    nfc_read_data_from_memory(system_file, 0, sizeof(system_file));

    uint8_t pw[16] = {0};

    c_apdu_t verify_file = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = 0x20,
        .P1 = 0x00,
        .P2 = 0x03,
        .LC = 0x10,
        .data = pw,
        .LE = 0
    };

    nfc_send_apdu_p(&verify_file, false);
    nfc_read_apdu_r(&response, verify_file.LE);

    uint8_t gpo_byte = 0x30;
    nfc_write_data_to_memory(&gpo_byte, 4, 1);
    nfc_read_data_from_memory(&gpo_byte, 4, 1);
    
    return;
}

void deinit_nfc() {
    power_disable_light_nfc();
}

uint8_t nfc_read_total_ndef_length() {
    uint8_t len[2];

    nfc_read_data_from_memory(len, 0, sizeof(len));
    uint8_t total_len = len[1];
    return total_len;
}

void nfc_read_data_from_memory(uint8_t *dst, uint8_t offset, uint8_t len) {
    c_apdu_r response = {.data = dst};

    c_apdu_t read_binary = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_READ_BINARY,
        .P1 = 0x00,
        .P2 = offset,
        .LC = 0x00,
        .data = 0,
        .LE = len
    };

    nfc_send_apdu_p(&read_binary, true);
    nfc_read_apdu_r(&response, read_binary.LE);
    return;
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

    if (total_len > NFC_STEP_RECORD_MIN_LENGTH || total_len == 0)
        return false;

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

void nfc_update_step_ctr_record(RTC_DateTypeDef *sDate, uint32_t n_steps, bool append_new_record) {
    uint8_t total_len = nfc_read_total_ndef_length();

    uint8_t full_msg[100];
    nfc_read_data_from_memory(full_msg, 2, total_len);

    if (total_len < NFC_STEP_RECORD_MIN_LENGTH) {
        nfc_write_step_ctr_message(sDate, n_steps);
        return;
    }

    uint8_t new_size = total_len + STEP_CTR_RECORD_SIZE_BYTES;
    uint8_t new_payload_len;
    uint8_t new_record[STEP_CTR_RECORD_SIZE_BYTES];
    uint8_t write_offset = total_len;

    if (!append_new_record)
        write_offset -= STEP_CTR_RECORD_SIZE_BYTES;

    nfc_read_data_from_memory(&new_payload_len, NFC_OFFSET_TO_NDEF_STEPS_PAYLOAD_LEN, 1);
    new_payload_len += STEP_CTR_RECORD_SIZE_BYTES;
    nfc_copy_step_ctr_payload(new_record, sDate, n_steps);
    nfc_clear_ndef_message();
    nfc_write_data_to_memory(new_record, write_offset + NFC_OFFSET_TO_NDEF_MESSAGE, STEP_CTR_RECORD_SIZE_BYTES);

    if (append_new_record) {
        nfc_write_data_to_memory(&new_payload_len, NFC_OFFSET_TO_NDEF_STEPS_PAYLOAD_LEN, 1);
        nfc_write_data_to_memory(&new_size, 1, 1);
    }
}

void nfc_write_data_to_memory(uint8_t *data, uint8_t offset, uint8_t len) {
    c_apdu_r response;
    //static uint8_t block_number = 0x00;
    c_apdu_t update_binary = {
        .PCB = NFC_PCB_BYTE,
        .CLA = 0,
        .INS = NFC_INS_UPDATE_BINARY,
        .P1 = 0x00,
        .P2 = 0,
        .LC = 0,
        .data = 0,
        .LE = 0
    };

    //block_number ^= 1;
    uint8_t n_bytes_written = 0;
    uint8_t current_address = offset;

    //https://community.st.com/t5/st25-nfc-rfid-tags-and-readers/m24sr64-we-can-only-write-to-the-ndef-file-within-16-byte-blocks/
    while(n_bytes_written < len) {
        uint8_t n_bytes_to_write = 16 - (current_address % 16);
        uint8_t remaining_bytes_to_write = len - n_bytes_written;
        if (n_bytes_to_write > remaining_bytes_to_write)
            n_bytes_to_write = remaining_bytes_to_write;

        update_binary.P2 = current_address;
        update_binary.LC = n_bytes_to_write;
        update_binary.data = data + n_bytes_written;

        nfc_send_apdu_p(&update_binary, false);
        nfc_read_apdu_r(&response, update_binary.LE);

        current_address += n_bytes_to_write;
        n_bytes_written += n_bytes_to_write;
    }
}

void nfc_clear_ndef_message() {
    uint8_t len_field[2] = {0};
    nfc_write_data_to_memory(len_field, 0, sizeof(len_field));
}

void nfc_copy_step_ctr_payload(uint8_t *dst_buffer, RTC_DateTypeDef *sDate, uint32_t n_steps) {
    dst_buffer[0] = sDate->Year;
    dst_buffer[1] = sDate->Month;
    dst_buffer[2] = sDate->Date;
    dst_buffer[3] = n_steps;
    dst_buffer[4] = n_steps >> 8;
    dst_buffer[5] = n_steps >> 16;
    dst_buffer[6] = n_steps >> 24;
}

void nfc_write_step_ctr_message(RTC_DateTypeDef *sDate, uint32_t n_steps) {

    initial_external_type_ndef_record_t new_record = {  
                                                        .id_byte = NDEF_HEADER, 
                                                        .type_len = EXTERNAL_TYPE_LEN_STEPS, 
                                                        .payload_len = sizeof(new_record.payload),
                                                        .external_type = EXTERNAL_TYPE_STEPS
                                                    };
    uint8_t new_total_len[2] = {0, sizeof(new_record)};
    nfc_copy_step_ctr_payload(new_record.payload, sDate, n_steps);
    nfc_clear_ndef_message();
    nfc_write_data_to_memory((uint8_t*)&new_record, NFC_OFFSET_TO_NDEF_MESSAGE, sizeof(new_record));
    nfc_write_data_to_memory(new_total_len, 0, sizeof(new_total_len));
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
