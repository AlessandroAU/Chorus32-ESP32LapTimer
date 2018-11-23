#define TO_BYTE(i) (i <= '9' ? i - 0x30 : i - 0x41 + 10)

uint8_t HEX_TO_BYTE (uint8_t hi, uint8_t lo) {
    return TO_BYTE(hi)*16+TO_BYTE(lo);
}

uint16_t HEX_TO_UINT16 (uint8_t * buf) {
    return (HEX_TO_BYTE(buf[0], buf[1]) << 8) + (HEX_TO_BYTE(buf[2], buf[3]));
}

int32_t HEX_TO_SIGNED_LONG (uint8_t * buf) {
    #define LEN 8
    int32_t temp = 0;
    for (int i = 0; i < LEN; i++) {
        temp += TO_BYTE(buf[LEN-1-i])*(int32_t)1<<(i*4);
    }
    return temp;
}

#define TO_HEX(i) (i <= 9 ? 0x30 + i : 0x41 + i - 10)

void halfByteToHex(uint8_t *buf, uint8_t val) {
    buf[0] = TO_HEX((val & 0x0F));
}

void byteToHex(uint8_t *buf, uint8_t val) {
    halfByteToHex(buf, val >> 4);
    halfByteToHex(&buf[1], val);
}

void intToHex(uint8_t *buf, uint16_t val) {
    byteToHex(buf, val >> 8);
    byteToHex(&buf[2], val & 0x00FF);
}

void longToHex(uint8_t *buf, uint32_t val) {
    intToHex(buf, val >> 16);
    intToHex(&buf[4], val & 0x0000FFFF);
}
