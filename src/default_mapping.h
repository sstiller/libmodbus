#ifndef DEFAULT_MAPPING_H
#define DEFAULT_MAPPING_H

typedef struct {
    int nb_bits;
    int nb_input_bits;
    int nb_input_registers;
    int nb_registers;
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_input_registers;
    uint16_t *tab_registers;
} modbus_mapping_t;

modbus_storage_backend_t* modbus_default_mapping_new(int nb_bits, int nb_input_bits,
                                     int nb_registers, int nb_input_registers);

void modbus_default_mapping_free(modbus_storage_backend_t* storage_be);


#endif /* DEFAULT_MAPPING_H */
