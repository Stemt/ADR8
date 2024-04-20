#define ADR8_IMPLEMENTATION
#include "../ADR8.h"
#include "../devices/serialbus.h"

int main(void){

  ADR8_Bus bus = {0};
  ADR8_Memory mem = {0};
  ADR8_Memory_init(&mem, &bus, 0x100, 0x0);
  ADR8_SerialBus serial = {0};
  ADR8_SerialBus_init(&serial,NULL,stdout, &bus, 0x100);
  ADR8_Core core = {0};
  ADR8_Core_init(&core, &bus);

  mem.data[0x00] = ADR8_Op_JMPA;
  mem.data[0x01] = 0x10;
  mem.data[0x02] = 0x00;
  mem.data[0x03] = 'h';
  mem.data[0x04] = 'e';
  mem.data[0x05] = 'l';
  mem.data[0x06] = 'l';
  mem.data[0x07] = 'o';
  mem.data[0x08] = ' ';
  mem.data[0x09] = 'w';
  mem.data[0x0A] = 'o';
  mem.data[0x0B] = 'r';
  mem.data[0x0C] = 'l';
  mem.data[0x0D] = 'd';
  mem.data[0x0E] = '!';
  mem.data[0x0F] = '\n';
  mem.data[0x10] = '\0';
  mem.data[0x11] = ADR8_Op_SETX; // set x ptr
  mem.data[0x12] = 0x03;
  mem.data[0x13] = 0x00;
  mem.data[0x14] = ADR8_Op_LXAL; // load mem at x into AL
  mem.data[0x15] = ADR8_Op_STAL; // write AL to serial
  mem.data[0x16] = 0x00;
  mem.data[0x17] = 0x01;
  mem.data[0x18] = ADR8_Op_JEQA; // if zero exit loop
  mem.data[0x19] = 0x1E;
  mem.data[0x1A] = 0x00;
  mem.data[0x1B] = ADR8_Op_INCX;
  mem.data[0x1C] = ADR8_Op_JMPA; // repeat loop
  mem.data[0x1D] = 0x13;
  mem.data[0x1E] = 0x00;
  mem.data[0x1F] = ADR8_Op_HALT;


  while(!core.halt){
    ADR8_Core_print(&core);
    ADR8_Core_clock(&core);
    ADR8_Memory_clock(&mem);
    ADR8_Memory_print(&mem, 0x30);
    ADR8_SerialBus_clock(&serial);
    usleep(10000);
  }

  return 0;
}
