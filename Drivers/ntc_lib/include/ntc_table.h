#include "main.h"

#include "pgmspace.h"

#define TEMPERATURE_UNDER -550
#define TEMPERATURE_OVER 1250
#define TEMPERATURE_TABLE_START -550
#define TEMPERATURE_TABLE_STEP 5

typedef uint16_t temperature_table_entry_type;
typedef uint16_t temperature_table_index_type;

#define TEMPERATURE_TABLE_READ(i) pgm_read_word(&termo_table[i])

extern const temperature_table_entry_type termo_table[] PROGMEM;

int16_t calc_temperature(temperature_table_entry_type adcsum);
