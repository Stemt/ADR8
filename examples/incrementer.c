#define ADR8_IMPLEMENTATION
#include "../ADR8.h"

int main(void){

  ADR8_Bus bus = {0};
  ADR8_Memory mem = {
    .mount_address = 0x0,
    .data = {0},
    .bus = &bus,
  };

  mem.data[0x00] = ADR8_Op_JMPR; // jmp 0x05
  mem.data[0x01] = 3;
  mem.data[0x02] = 0x01; // increment
  mem.data[0x03] = 0x00; // result
  mem.data[0x04] = 0x0A; // limit
  mem.data[0x05] = ADR8_Op_SKST; // set stack ptr to 2F
  mem.data[0x06] = 0x2F;
  mem.data[0x07] = 0x00;
  mem.data[0x08] = ADR8_Op_LDBL; // load increment into B
  mem.data[0x09] = 0x02;
  mem.data[0x0A] = 0x00;
  mem.data[0x0B] = ADR8_Op_ADD; // add increment to A
  mem.data[0x0C] = ADR8_Op_PUAL; // push result onto stack
  mem.data[0x0D] = ADR8_Op_LDBL; // load limit
  mem.data[0x0E] = 0x04;
  mem.data[0x0F] = 0x00;
  mem.data[0x10] = ADR8_Op_JLTA; // if less than limit jmp to 0x08
  mem.data[0x11] = 0x08;
  mem.data[0x12] = 0x00;
  mem.data[0x13] = ADR8_Op_STAL; // store result into memory
  mem.data[0x14] = 0x03;
  mem.data[0x15] = 0x00;
  mem.data[0x16] = ADR8_Op_HALT; // stop

  ADR8_Core core = {
    .bus = &bus,
    .reg = {0},
    .fetch = true,
  };

  while(!core.halt){
    ADR8_Core_print(&core);
    ADR8_Core_clock(&core);
    ADR8_Memory_clock(&mem);
    ADR8_Memory_print(&mem, 0x30);
    usleep(50000);    
  }

  return 0;
}
