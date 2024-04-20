#ifndef ADR8_SERIAL_H
#define ADR8_SERIAL_H

#include "../ADR8.h"
#include <stdint.h>
#include <stdio.h>

typedef struct{
  uint16_t mount_address;
  ADR8_Bus* bus;
  FILE* in_fp;
  FILE* out_fp;
} ADR8_SerialBus;

void ADR8_SerialBus_init(ADR8_SerialBus* serial, FILE* in_fp, FILE* out_fp, ADR8_Bus* bus, uint16_t mount_address);
void ADR8_SerialBus_clock(ADR8_SerialBus* serial);

#ifdef ADR8_IMPLEMENTATION

void ADR8_SerialBus_init(ADR8_SerialBus* serial, FILE* in_fp, FILE* out_fp, ADR8_Bus* bus, uint16_t mount_address){
  serial->in_fp = in_fp;
  serial->out_fp = out_fp;
  serial->bus = bus;
  serial->mount_address = mount_address;
}

void ADR8_SerialBus_clock(ADR8_SerialBus* serial){
  if(serial->bus->address == serial->mount_address){
    if(serial->bus->read){
      int c = fgetc(serial->in_fp);
      if(c == EOF){
        serial->bus->data = 0;
      }else{
        serial->bus->data = c;
      }
    }else{
      fputc(serial->bus->data, serial->out_fp);
    }
  }
}
#endif // ADR8_IMPLEMENTATION

#endif // ADR8_SERIAL_H
