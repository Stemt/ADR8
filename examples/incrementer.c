#define ADR8_IMPLEMENTATION
#include "../ADR8.h"

int main(void){

  // init bus
  ADR8_Bus bus = {0};

  // init memory of 48 bytes and mount at address 0x0
  ADR8_Memory mem = {0};
  ADR8_Memory_init(&mem, &bus, 0x30, 0x0);
  
  // init CPU
  ADR8_Core core = {0};
  ADR8_Core_init(&core, &bus);

  mem.data[0x00] = ADR8_Op_JMPR; // jump over data section
  mem.data[0x01] = 3;
  mem.data[0x02] = 0x01; // increment variable
  mem.data[0x03] = 0x00; // result variable 
  mem.data[0x04] = 0x0A; // limit variable
  mem.data[0x05] = ADR8_Op_SETK; // set stack ptr to 2F
  mem.data[0x06] = 0x2F;
  mem.data[0x07] = 0x00;
  mem.data[0x08] = ADR8_Op_LDBL; // load increment into lower B byte
  mem.data[0x09] = 0x02;
  mem.data[0x0A] = 0x00;
  mem.data[0x0B] = ADR8_Op_ADD; // add B register to A
  mem.data[0x0C] = ADR8_Op_PUAL; // push lower byte of A onto stack
  mem.data[0x0D] = ADR8_Op_LDBL; // load limit int into lower byte of B
  mem.data[0x0E] = 0x04;
  mem.data[0x0F] = 0x00;
  mem.data[0x10] = ADR8_Op_JLTA; // if A < B j jump back to 0x08
  mem.data[0x11] = 0x08;
  mem.data[0x12] = 0x00;
  mem.data[0x13] = ADR8_Op_STAL; // store lower byte of A into result
  mem.data[0x14] = 0x03;
  mem.data[0x15] = 0x00;
  mem.data[0x16] = ADR8_Op_HALT; // stop


  while(!core.halt){
    ADR8_Core_clock(&core);
    ADR8_Memory_clock(&mem);
    ADR8_Core_print(&core);
    ADR8_Memory_print(&mem, 0x30);
  }

  return 0;
}
