#ifndef ADR8_SERIAL_H
#define ADR8_SERIAL_H

#include "../ADR8.h"
#include <stdint.h>
#include <stdio.h>

typedef struct{
  uint16_t mount_address;
  uint8_t* data;
  size_t size;
  ADR8_Bus* bus;
} ADR8_ROM;

void ADR8_ROM_init(ADR8_ROM* rom, ADR8_Bus* bus, uint16_t mount_address);
void ADR8_ROM_clock(ADR8_ROM* rom);

#ifdef ADR8_IMPLEMENTATION

void ADR8_ROM_init(ADR8_ROM* rom, ADR8_Bus* bus, uint16_t mount_address, uint8_t* data, size_t size){
  rom->bus = bus;
  rom->mount_address = mount_address;
  rom->size = size;
  rom->data = (uint8_t*) calloc(size, 1);
  assert(rom->data);
}

void ADR8_ROM_clock(ADR8_ROM* rom){
  uint16_t rom_address = rom->bus->address - rom->mount_address;
  if(rom_address < rom->size && rom->bus->read){
    ADR8_DEBUG_LOG("ROM READ  %04hX: %02hX\n",rom_address,rom->data[rom_address]);
    rom->bus->data = rom->data[rom_address];
  }
}
#endif // ADR8_IMPLEMENTATION

#endif // ADR8_SERIAL_H
