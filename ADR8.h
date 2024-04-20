#ifndef ADR8_H_
#define ADR8_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define ADR8_IMPLEMENTATION

// ADR8_Bus

typedef struct{
  uint16_t address;
  uint8_t data;
  bool read;
} ADR8_Bus;

void ADR8_Bus_write(ADR8_Bus* bus, uint16_t address, uint8_t data);
void ADR8_Bus_read(ADR8_Bus* bus, uint16_t address);
uint32_t ADR8_Bus_get_data(ADR8_Bus* bus);

#ifdef ADR8_IMPLEMENTATION

void ADR8_Bus_write(ADR8_Bus* bus, uint16_t address, uint8_t data){
  bus->address = address;
  bus->data = data;
  bus->read = false;
}

void ADR8_Bus_read(ADR8_Bus* bus, uint16_t address){
  bus->address = address;
  bus->read = true;
}

uint32_t ADR8_Bus_get_data(ADR8_Bus* bus){
  return bus->data;
}

#endif // ADR8_IMPLEMENTATION


// ADR8_Memory

typedef struct{
  uint16_t mount_address;
  uint16_t size;
  uint8_t* data;
  ADR8_Bus* bus;
} ADR8_Memory;

void ADR8_Memory_init(ADR8_Memory* mem, ADR8_Bus* bus, uint16_t size, uint16_t mount_address);
void ADR8_Memory_print(ADR8_Memory* mem, uint16_t n);
void ADR8_Memory_clock(ADR8_Memory* mem);

#ifdef ADR8_IMPLEMENTATION

void ADR8_Memory_init(ADR8_Memory* mem, ADR8_Bus* bus, uint16_t size, uint16_t mount_address){
  mem->bus = bus;
  mem->mount_address = mount_address;
  mem->size = size;
  mem->data = malloc(size);
  assert(mem->data);
}

void ADR8_Memory_print(ADR8_Memory* mem, uint16_t n){
  for(uint16_t i = 0; i < n && i < mem->size; ++i){
    if (i%8 == 0) fprintf(stderr,"\n%04hX:",i);
    fprintf(stderr," %02hX", mem->data[i]);
  }
  fprintf(stderr,"\n");
}

void ADR8_Memory_clock(ADR8_Memory* mem){
  uint16_t mem_address = mem->bus->address - mem->mount_address;
  if(mem_address < mem->size){
    if(mem->bus->read){
      mem->bus->data = mem->data[mem_address];
    }else{
      mem->data[mem_address] = mem->bus->data;
    }
  }
}
#endif // ADR8_IMPLEMENTATION

// ADR8_Core

typedef enum{
  // Misc ops
  ADR8_Op_NOP  = 0x00,
  ADR8_Op_HALT = 0x01,
  
  ADR8_Op_TRAB = 0x03, // transfer A to B
  ADR8_Op_TRBA = 0x04, // transfer B to A
  ADR8_Op_TRAX = 0x05, // transfer A to X
  ADR8_Op_TRXA = 0x06, // transfer X to A
  ADR8_Op_TRAY = 0x07, // transfer A to Y
  ADR8_Op_TRYA = 0x08, // transfer Y to A
  
  ADR8_Op_SETK = 0x09, // stack set
  ADR8_Op_SETA = 0x0A,
  ADR8_Op_SETB = 0x0B,
  ADR8_Op_SETX = 0x0C,
  ADR8_Op_SETY = 0x0D,
  
  ADR8_Op_JSR  = 0x0E, // jump subroutine
  ADR8_Op_RSR  = 0x0F, // return from subroutine

  // load word from memory
  ADR8_Op_LDAL = 0x10,
  ADR8_Op_LDAH = 0x11,
  ADR8_Op_LDBL = 0x12,
  ADR8_Op_LDBH = 0x13,

  // load word by pointer
  ADR8_Op_LXAL = 0x14,
  ADR8_Op_LXAH = 0x15,
  ADR8_Op_LYBL = 0x16,
  ADR8_Op_LYBH = 0x17,

  // load dword from memory
  ADR8_Op_LDA  = 0x1A,
  ADR8_Op_LDB  = 0x1B,
  ADR8_Op_LDX  = 0x1C,
  ADR8_Op_LDY  = 0x1D,

  // load dword by pointer
  ADR8_Op_LXA  = 0x1E,
  ADR8_Op_LYB  = 0x1F,
  
  // store word to memory
  ADR8_Op_STAL = 0x20,
  ADR8_Op_STAH = 0x21,
  ADR8_Op_STBL = 0x22,
  ADR8_Op_STBH = 0x23,
  
  // store word by pointer
  ADR8_Op_SXAL = 0x24,
  ADR8_Op_SXAH = 0x25,
  ADR8_Op_SYBL = 0x26,
  ADR8_Op_SYBH = 0x27,

  // store dword to memory
  ADR8_Op_STA  = 0x2A,
  ADR8_Op_STB  = 0x2B,
  ADR8_Op_STX  = 0x2C,
  ADR8_Op_STY  = 0x2D,
  
  // store dword by pointer
  ADR8_Op_SXA  = 0x2E,
  ADR8_Op_SYB  = 0x2F,
  
  // ALU ops
  ADR8_Op_ADD  = 0x30,
  ADR8_Op_SUB  = 0x31,
  ADR8_Op_MUL  = 0x32,
  ADR8_Op_DIV  = 0x33,
  
  // pointer arithmatic
  ADR8_Op_INCX = 0x34,
  ADR8_Op_INCY = 0x35,
  ADR8_Op_DECX = 0x36,
  ADR8_Op_DECY = 0x37,

  // relative control flow
  ADR8_Op_JMPR = 0x40,
  ADR8_Op_JEQR = 0x41,
  ADR8_Op_JGTR = 0x42,
  ADR8_Op_JLTR = 0x43,

  // absolute control flow
  ADR8_Op_JMPA = 0x44,
  ADR8_Op_JEQA = 0x45,
  ADR8_Op_JGTA = 0x46,
  ADR8_Op_JLTA = 0x47,


  // stack push word
  ADR8_Op_PUAL = 0x50,
  ADR8_Op_PUAH = 0x51,
  ADR8_Op_PUBL = 0x52,
  ADR8_Op_PUBH = 0x53,

  // stack push dword
  ADR8_Op_PUA  = 0x5A,
  ADR8_Op_PUB  = 0x5B,
  ADR8_Op_PUX  = 0x5C,
  ADR8_Op_PUY  = 0x5D,

  // stack pop word
  ADR8_Op_POAL = 0x60,
  ADR8_Op_POAH = 0x61,
  ADR8_Op_POBL = 0x62,
  ADR8_Op_POBH = 0x63,

  // stack pop dword
  ADR8_Op_POA  = 0x6A,
  ADR8_Op_POB  = 0x6B,
  ADR8_Op_POX  = 0x6C,
  ADR8_Op_POY  = 0x6D,

  
}ADR8_OpCode;

typedef union{
  uint16_t full;
  struct{
    uint8_t l;
    uint8_t h;
  } half;
} Reg16_t;

typedef struct{
  Reg16_t ctrl;
  struct{
    uint8_t opcode;
    uint8_t state;
  } cmd;
  Reg16_t pc;
  Reg16_t stk;
  Reg16_t adr;
  Reg16_t a;
  Reg16_t b;
  Reg16_t x;
  Reg16_t y;
}ADR8_Registers;

typedef struct{
  ADR8_Registers reg;
  bool fetch;
  bool halt;
  ADR8_Bus* bus;
} ADR8_Core;

void ADR8_Core_init(ADR8_Core* core, ADR8_Bus* bus);
void ADR8_Core_print(ADR8_Core* core);
void ADR8_Core_next_instruction(ADR8_Core* core);
void ADR8_Core_fetch_next_operand(ADR8_Core* core);
uint8_t ADR8_Core_get_operand_data(ADR8_Core* core);
void ADR8_Core_clock(ADR8_Core* core);

#ifdef ADR8_IMPLEMENTATION

void ADR8_Core_init(ADR8_Core* core, ADR8_Bus* bus){
  core->bus = bus;
  core->fetch = true;
  memset(&core->reg, 0, sizeof(ADR8_Registers));
}

void ADR8_Core_print(ADR8_Core* core){
  static char cpu_state[2][10] = {
    "EXEC",
    "FETCH",
  };
  fprintf(stderr,"pc: [%04hX] adr: [%04hX] stk: [%04hX] a: [%04hX] b: [%04hX] x: [%04hX] y: [%04hX] cmd.op: [%02hX] cmd.state: [%02hX] %s\n"
      ,core->reg.pc.full
      ,core->reg.adr.full
      ,core->reg.stk.full
      ,core->reg.a.full
      ,core->reg.b.full
      ,core->reg.x.full
      ,core->reg.y.full
      ,core->reg.cmd.opcode
      ,core->reg.cmd.state
      ,cpu_state[core->fetch]);
}

void ADR8_Core_next_instruction(ADR8_Core* core){
  core->reg.pc.full++;
  core->fetch = true;
}

void ADR8_Core_fetch_next_operand(ADR8_Core* core){
  core->reg.pc.full++;
  ADR8_Bus_read(core->bus, core->reg.pc.full);
}

uint8_t ADR8_Core_get_operand_data(ADR8_Core* core){
  return ADR8_Bus_get_data(core->bus);
}

void ADR8_Core_clock(ADR8_Core* core){
  if(core->halt) return;

  if(core->fetch){
    core->reg.cmd.opcode = 0;
    core->reg.cmd.state = 0;
    ADR8_Bus_read(core->bus, core->reg.pc.full);
    core->fetch = false;
    return;
  }

  if(!core->reg.cmd.opcode){
    core->reg.cmd.opcode = ADR8_Bus_get_data(core->bus);
  }

  switch (core->reg.cmd.opcode) {
    case ADR8_Op_NOP: ADR8_Core_next_instruction(core); break;
    case ADR8_Op_HALT: core->halt = true; break;
    
    // Load ops
    case ADR8_Op_LDAL:
    case ADR8_Op_LDAH:
    case ADR8_Op_LDBL:
    case ADR8_Op_LDBH:
    {
      uint8_t* reg = &core->reg.a.half.l + (core->reg.cmd.opcode & 0x0F);
      switch(core->reg.cmd.state){
        case 0:{
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 1:{
          core->reg.adr.half.l = ADR8_Core_get_operand_data(core);
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 2:{
          core->reg.adr.half.h = ADR8_Core_get_operand_data(core);
          ADR8_Bus_read(core->bus, core->reg.adr.full);
        } break;
        case 3:{
          *reg = ADR8_Bus_get_data(core->bus);
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;

    // pointer load ops
    case ADR8_Op_LXAL:
    case ADR8_Op_LXAH:
    case ADR8_Op_LYBL:
    case ADR8_Op_LYBH:
    {
      switch(core->reg.cmd.state){
        case 0:{
          Reg16_t* ptr_reg = NULL;
          switch(core->reg.cmd.opcode){
            case ADR8_Op_LXAL:
            case ADR8_Op_LXAH: 
              ptr_reg = &core->reg.x;
              break;
            case ADR8_Op_LYBL: 
            case ADR8_Op_LYBH: 
              ptr_reg = &core->reg.y;
              break;
          }
          ADR8_Bus_read(core->bus,ptr_reg->full);
        }break;
        case 1:{
          switch(core->reg.cmd.opcode){
            case ADR8_Op_LXAL: core->reg.a.half.l = ADR8_Bus_get_data(core->bus); break;
            case ADR8_Op_LXAH: core->reg.a.half.h = ADR8_Bus_get_data(core->bus); break;
            case ADR8_Op_LYBL: core->reg.b.half.l = ADR8_Bus_get_data(core->bus); break;
            case ADR8_Op_LYBH: core->reg.b.half.h = ADR8_Bus_get_data(core->bus); break;
          }
          ADR8_Core_next_instruction(core);
        }break;
      }

    }break;

    // Store ops
    case ADR8_Op_STAL:
    case ADR8_Op_STAH:
    case ADR8_Op_STBL:
    case ADR8_Op_STBH:
    {
      uint8_t* reg = &core->reg.a.half.l + (core->reg.cmd.opcode & 0x0F);
      switch(core->reg.cmd.state){
        case 0:{
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 1:{
          core->reg.adr.half.l = ADR8_Core_get_operand_data(core);
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 2:{
          core->reg.adr.half.h = ADR8_Core_get_operand_data(core);
          ADR8_Bus_write(core->bus, core->reg.adr.full, *reg);
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;

    // ALU Ops
    case ADR8_Op_ADD:{
      uint16_t result = core->reg.b.full + core->reg.a.full;
      core->reg.a.full += core->reg.b.full;
      ADR8_Core_next_instruction(core);
    } break;

    // pointer arithmatic ops
    case ADR8_Op_INCX:{
      core->reg.x.full++;
      ADR8_Core_next_instruction(core);
    } break;
    case ADR8_Op_INCY:{
      core->reg.x.full++;
      ADR8_Core_next_instruction(core);
    } break;
    case ADR8_Op_DECX:{
      core->reg.x.full--;
      ADR8_Core_next_instruction(core);
    } break;
    case ADR8_Op_DECY:{
      core->reg.x.full--;
      ADR8_Core_next_instruction(core);
    } break;


    // relative control flow
    case ADR8_Op_JMPR:
    case ADR8_Op_JEQR:
    case ADR8_Op_JGTR:
    case ADR8_Op_JLTR:
    {
      switch(core->reg.cmd.state){
        case 0:{
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 1:{
          int8_t offset = ((int8_t)ADR8_Core_get_operand_data(core));
          switch (core->reg.cmd.opcode) {
            case ADR8_Op_JMPR: break;
            case ADR8_Op_JEQR: offset *= (core->reg.a.full == core->reg.b.full); break;
            case ADR8_Op_JGTR: offset *= (core->reg.a.full > core->reg.b.full); break;
            case ADR8_Op_JLTR: offset *= (core->reg.a.full < core->reg.b.full); break;
          }
          core->reg.pc.full += offset;
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;

    // absolute control flow
    case ADR8_Op_JMPA:
    case ADR8_Op_JEQA:
    case ADR8_Op_JGTA:
    case ADR8_Op_JLTA:
    {
      switch (core->reg.cmd.state) {
        case 0:{
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 1:{
          core->reg.adr.half.l = ADR8_Core_get_operand_data(core);
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 2:{
          core->reg.adr.half.h = ADR8_Core_get_operand_data(core);
          bool jmp = true;
          switch (core->reg.cmd.opcode) {
            case ADR8_Op_JMPR: break;
            case ADR8_Op_JEQA: jmp = (core->reg.a.full == core->reg.b.full); break;
            case ADR8_Op_JGTA: jmp = (core->reg.a.full > core->reg.b.full); break;
            case ADR8_Op_JLTA: jmp = (core->reg.a.full < core->reg.b.full); break;
          }
          if(jmp){
            core->reg.pc = core->reg.adr;
            core->reg.pc.full--;
          }
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;
    // stack push
    case ADR8_Op_PUAL:
    case ADR8_Op_PUAH:
    case ADR8_Op_PUBL:
    case ADR8_Op_PUBH:
    {
      uint8_t* reg = &core->reg.a.half.l + (core->reg.cmd.opcode & 0x0F);
      ADR8_Bus_write(core->bus, core->reg.stk.full, *reg);
      core->reg.stk.full--;
      ADR8_Core_next_instruction(core);
    } break;

    // stack pop
    case ADR8_Op_POAL:
    case ADR8_Op_POAH:
    case ADR8_Op_POBL:
    case ADR8_Op_POBH:
    {
      switch(core->reg.cmd.state){
        case 0:{
          core->reg.stk.full++;
          ADR8_Bus_read(core->bus, core->reg.stk.full);
        } break;
        case 1:{
          uint8_t* reg = &core->reg.a.half.l + (core->reg.cmd.opcode & 0x0F);
          *reg = ADR8_Bus_get_data(core->bus);
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;
    case ADR8_Op_SETK:
    case ADR8_Op_SETA:
    case ADR8_Op_SETB:
    case ADR8_Op_SETX:
    case ADR8_Op_SETY:
    {
      Reg16_t* reg = NULL;
      switch(core->reg.cmd.opcode){
        case ADR8_Op_SETK: reg = &core->reg.stk; break;
        case ADR8_Op_SETA: reg = &core->reg.a; break;
        case ADR8_Op_SETB: reg = &core->reg.b; break;
        case ADR8_Op_SETX: reg = &core->reg.x; break;
        case ADR8_Op_SETY: reg = &core->reg.y; break;
      }

      switch (core->reg.cmd.state) {
        case 0:{
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 1:{
          reg->half.l = ADR8_Core_get_operand_data(core);
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 2:{
          reg->half.h = ADR8_Core_get_operand_data(core);
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;
    default:{
      fprintf(stderr,"Emulator ERROR: Unknown instruction [%02X]\n",core->reg.cmd.opcode);
      core->halt = true;
    }break;
  }


  if(core->reg.cmd.opcode){
    core->reg.cmd.state++;
  }
}
#endif // ADR8_IMPLEMENTATION


#endif // ADR8_H_
