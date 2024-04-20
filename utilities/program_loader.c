#define ADR8_IMPLEMENTATION
#include "../ADR8.h"
#include "../devices/serialbus.h"

int main(void){

  ADR8_Bus bus = {0};

  // init memory of 4096 bytes and mount at address 0x0
  ADR8_Memory mem = {0};
  ADR8_Memory_init(&mem, &bus, 0x1000, 0x0);
  
  // init serial bus device and map at address 0x1000
  ADR8_SerialBus serial = {0};
  ADR8_SerialBus_init(&serial,stdin,stdout, &bus, 0x1000);
  
  // init CPU
  ADR8_Core core = {0};
  ADR8_Core_init(&core, &bus);
  

  // load 16 bit integer indicating how long the program is from serial bus
  mem.data[0x00] = ADR8_Op_LDAL;
  mem.data[0x01] = 0x00;
  mem.data[0x02] = 0x10;
  mem.data[0x03] = ADR8_Op_LDAH;
  mem.data[0x04] = 0x00;
  mem.data[0x05] = 0x10;
  mem.data[0x06] = ADR8_Op_SETY; // set pointer to start of program location
  mem.data[0x07] = 0x16;
  mem.data[0x08] = 0x00;

  mem.data[0x0A] = ADR8_Op_LDBL; // read program byte from serial bus
  mem.data[0x0B] = 0x00;
  mem.data[0x0C] = 0x10;
  mem.data[0x0D] = ADR8_Op_SYBL; // write program byte to memory
  mem.data[0x0E] = ADR8_Op_INCY; // increment program pointer
  mem.data[0x0F] = ADR8_Op_DEC;  // decrement A
  mem.data[0x10] = ADR8_Op_SETB; // set B to zero
  mem.data[0x11] = 0x00;
  mem.data[0x12] = 0x00;
  mem.data[0x13] = ADR8_Op_JGTA; // if A > B(0) keep copying
  mem.data[0x14] = 0x0A;
  mem.data[0x15] = 0x00;
  mem.data[0x16] = 0x00; // start program location

  while(!core.halt){
    ADR8_Core_clock(&core);
    ADR8_Memory_clock(&mem);
    ADR8_SerialBus_clock(&serial);
  }

  return 0;
}
