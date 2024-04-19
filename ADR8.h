#ifndef ADR8_H_
#define ADR8_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

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

#define ADR8_MEM_SIZE 2048
typedef struct{
  uint16_t mount_address;
  uint8_t data[ADR8_MEM_SIZE];
  ADR8_Bus* bus;
} ADR8_Memory;

void ADR8_Memory_print(ADR8_Memory* mem, uint16_t n);
void ADR8_Memory_clock(ADR8_Memory* mem);

#ifdef ADR8_IMPLEMENTATION

void ADR8_Memory_print(ADR8_Memory* mem, uint16_t n){
  for(uint16_t i = 0; i < n; ++i){
    if (i%8 == 0) printf("\n%04hX:",i);
    printf(" %02hX", mem->data[i]);
  }
  printf("\n");
}

void ADR8_Memory_clock(ADR8_Memory* mem){
  uint16_t mem_address = mem->bus->address - mem->mount_address;
  if(mem_address < ADR8_MEM_SIZE){
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

  // load from memory
  ADR8_Op_LDAL = 0x10,
  ADR8_Op_LDAH = 0x11,
  ADR8_Op_LDBL = 0x12,
  ADR8_Op_LDBH = 0x13,

  ADR8_Op_LDA  = 0x1A,
  ADR8_Op_LDB  = 0x1B,
  
  // Store ops
  ADR8_Op_STAL = 0x20,
  ADR8_Op_STAH = 0x21,
  ADR8_Op_STBL = 0x22,
  ADR8_Op_STBH = 0x23,

  ADR8_Op_STA  = 0x2A,
  ADR8_Op_STB  = 0x2B,
  
  // ALU ops
  ADR8_Op_ADD  = 0x30,
  ADR8_Op_SUB  = 0x31,
  ADR8_Op_MUL  = 0x32,
  ADR8_Op_DIV  = 0x33,

  // relative control flow
  ADR8_Op_JMPR = 0x40,
  ADR8_Op_JEQR = 0x41,
  ADR8_Op_JGTR = 0x42,
  ADR8_Op_JLTR = 0x43,

  ADR8_Op_JSRR = 0x4A,
  ADR8_Op_RSRR = 0x4B,

  // absolute control flow
  ADR8_Op_JMPA = 0x44,
  ADR8_Op_JEQA = 0x45,
  ADR8_Op_JGTA = 0x46,
  ADR8_Op_JLTA = 0x47,

  ADR8_Op_JSRA = 0x4A,
  ADR8_Op_RSRA = 0x4B,

  // stack push
  ADR8_Op_PUAL = 0x50,
  ADR8_Op_PUAH = 0x51,
  ADR8_Op_PUBL = 0x52,
  ADR8_Op_PUBH = 0x53,

  ADR8_Op_PUA  = 0x5A,
  ADR8_Op_PUB  = 0x5B,

  // stack pop
  ADR8_Op_POAL = 0x60,
  ADR8_Op_POAH = 0x61,
  ADR8_Op_POBL = 0x62,
  ADR8_Op_POBH = 0x63,

  ADR8_Op_POA  = 0x6A,
  ADR8_Op_POB  = 0x6B,

  // stack misc
  ADR8_Op_SKST = 0x70, // stack set
  
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

void ADR8_Core_print(ADR8_Core* core);
void ADR8_Core_next_instruction(ADR8_Core* core);
void ADR8_Core_fetch_next_operand(ADR8_Core* core);
uint8_t ADR8_Core_get_operand_data(ADR8_Core* core);
void ADR8_Core_clock(ADR8_Core* core);

#ifdef ADR8_IMPLEMENTATION

void ADR8_Core_print(ADR8_Core* core){
  static char cpu_state[2][10] = {
    "EXEC",
    "FETCH",
  };
  printf("pc: [%04hX] adr: [%04hX] stk: [%04hX] a: [%04hX] b: [%04hX] cmd.op: [%02hX] cmd.state: [%02hX] %s\n"
      ,core->reg.pc.full
      ,core->reg.adr.full
      ,core->reg.stk.full
      ,core->reg.a.full
      ,core->reg.b.full
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
    case ADR8_Op_NOP: break;
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
    case ADR8_Op_SKST:
    {
      switch (core->reg.cmd.state) {
        case 0:{
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 1:{
          core->reg.stk.half.l = ADR8_Core_get_operand_data(core);
          ADR8_Core_fetch_next_operand(core);
        } break;
        case 2:{
          core->reg.stk.half.h = ADR8_Core_get_operand_data(core);
          ADR8_Core_next_instruction(core);
        } break;
      }
    } break;
  }


  if(core->reg.cmd.opcode){
    core->reg.cmd.state++;
  }
}
#endif // ADR8_IMPLEMENTATION


#endif // ADR8_H_
