/*
 * eeprom.h
 *
 * Created: 8/9/2016 5:59:37 PM
 *  Author: ocybr
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

/*init eeprom*/
void eeprom_init(int16_t address);

/*write data*/
void eeprom_write_word(uint16_t data,uint32_t address);
void eeprom_write_byte(uint8_t data);
void eeprom_page_write(uint8_t *data,uint32_t address);

/*read data*/
void eeprom_read_word(uint16_t *data,uint32_t address);
void eeprom_read_byte(uint8_t *data);
uint8_t eeprom_page_read(/*uint8_t *data,uint32_t address*/);

void split_address(uint32_t address);

#endif /* EEPROM_H_ */