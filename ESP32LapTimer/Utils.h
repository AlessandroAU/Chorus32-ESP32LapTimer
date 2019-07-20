#include <esp_attr.h>
#include <stdint.h>

#define TO_BYTE(i) (i <= '9' ? i - 0x30 : i - 0x41 + 10)
#define TO_HEX(i) (i <= 9 ? 0x30 + i : 0x41 + i - 10)

uint8_t IRAM_ATTR HEX_TO_BYTE (uint8_t hi, uint8_t lo);
uint16_t IRAM_ATTR HEX_TO_UINT16 (uint8_t * buf);
int32_t IRAM_ATTR HEX_TO_SIGNED_LONG (uint8_t * buf);
void IRAM_ATTR halfByteToHex(uint8_t *buf, uint8_t val);
void IRAM_ATTR byteToHex(uint8_t *buf, uint8_t val);
void IRAM_ATTR intToHex(uint8_t *buf, uint16_t val);
void IRAM_ATTR longToHex(uint8_t *buf, uint32_t val);

float fmap(float x, float in_min, float in_max, float out_min, float out_max);

#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)
