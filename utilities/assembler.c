#include "../ADR8.h"
#include "da.h"
#include <errno.h>
#include <stdio.h>


char* InstructionTable[0x100] = {0};

void InstructionTable_init(){
  InstructionTable[ADR8_Op_NOP] = "NOP";
  InstructionTable[ADR8_Op_HALT] = "HALT";
  InstructionTable[ADR8_Op_TRAK] = "TRAK";
  InstructionTable[ADR8_Op_TRAB] = "TRAB";
  InstructionTable[ADR8_Op_TRBA] = "TRBA";
  InstructionTable[ADR8_Op_TRAX] = "TRAX";
  InstructionTable[ADR8_Op_TRXA] = "TRXA";
  InstructionTable[ADR8_Op_TRAY] = "TRAY";
  InstructionTable[ADR8_Op_TRYA] = "TRYA";
  InstructionTable[ADR8_Op_SETK] = "SETK";
  InstructionTable[ADR8_Op_SETA] = "SETA";
  InstructionTable[ADR8_Op_SETB] = "SETB";
  InstructionTable[ADR8_Op_SETX] = "SETX";
  InstructionTable[ADR8_Op_SETY] = "SETY";
  InstructionTable[ADR8_Op_JSR] = "JSR";
  InstructionTable[ADR8_Op_RSR] = "RSR";
  InstructionTable[ADR8_Op_LDAL] = "LDAL";
  InstructionTable[ADR8_Op_LDAH] = "LDAH";
  InstructionTable[ADR8_Op_LDBL] = "LDBL";
  InstructionTable[ADR8_Op_LDBH] = "LDBH";
  InstructionTable[ADR8_Op_LXAL] = "LXAL";
  InstructionTable[ADR8_Op_LXAH] = "LXAH";
  InstructionTable[ADR8_Op_LYBL] = "LYBL";
  InstructionTable[ADR8_Op_LYBH] = "LYBH";
  InstructionTable[ADR8_Op_LDA] = "LDA";
  InstructionTable[ADR8_Op_LDB] = "LDB";
  InstructionTable[ADR8_Op_LDX] = "LDX";
  InstructionTable[ADR8_Op_LDY] = "LDY";
  InstructionTable[ADR8_Op_LXA] = "LXA";
  InstructionTable[ADR8_Op_LYB] = "LYB";
  InstructionTable[ADR8_Op_STAL] = "STAL";
  InstructionTable[ADR8_Op_STAH] = "STAH";
  InstructionTable[ADR8_Op_STBL] = "STBL";
  InstructionTable[ADR8_Op_STBH] = "STBH";
  InstructionTable[ADR8_Op_SXAL] = "SXAL";
  InstructionTable[ADR8_Op_SXAH] = "SXAH";
  InstructionTable[ADR8_Op_SYBL] = "SYBL";
  InstructionTable[ADR8_Op_SYBH] = "SYBH";
  InstructionTable[ADR8_Op_STA] = "STA";
  InstructionTable[ADR8_Op_STB] = "STB";
  InstructionTable[ADR8_Op_STX] = "STX";
  InstructionTable[ADR8_Op_STY] = "STY";
  InstructionTable[ADR8_Op_SXA] = "SXA";
  InstructionTable[ADR8_Op_SYB] = "SYB";
  InstructionTable[ADR8_Op_ADD] = "ADD";
  InstructionTable[ADR8_Op_SUB] = "SUB";
  InstructionTable[ADR8_Op_MUL] = "MUL";
  InstructionTable[ADR8_Op_DIV] = "DIV";
  InstructionTable[ADR8_Op_INC] = "INC";
  InstructionTable[ADR8_Op_DEC] = "DEC";
  InstructionTable[ADR8_Op_INCX] = "INCX";
  InstructionTable[ADR8_Op_INCY] = "INCY";
  InstructionTable[ADR8_Op_DECX] = "DECX";
  InstructionTable[ADR8_Op_DECY] = "DECY";
  InstructionTable[ADR8_Op_JMPR] = "JMPR";
  InstructionTable[ADR8_Op_JEQR] = "JEQR";
  InstructionTable[ADR8_Op_JGTR] = "JGTR";
  InstructionTable[ADR8_Op_JLTR] = "JLTR";
  InstructionTable[ADR8_Op_JMPA] = "JMPA";
  InstructionTable[ADR8_Op_JEQA] = "JEQA";
  InstructionTable[ADR8_Op_JGTA] = "JGTA";
  InstructionTable[ADR8_Op_JLTA] = "JLTA";
  InstructionTable[ADR8_Op_PUAL] = "PUAL";
  InstructionTable[ADR8_Op_PUAH] = "PUAH";
  InstructionTable[ADR8_Op_PUBL] = "PUBL";
  InstructionTable[ADR8_Op_PUBH] = "PUBH";
  InstructionTable[ADR8_Op_PUA] = "PUA";
  InstructionTable[ADR8_Op_PUB] = "PUB";
  InstructionTable[ADR8_Op_PUX] = "PUX";
  InstructionTable[ADR8_Op_PUY] = "PUY";
  InstructionTable[ADR8_Op_POAL] = "POAL";
  InstructionTable[ADR8_Op_POAH] = "POAH";
  InstructionTable[ADR8_Op_POBL] = "POBL";
  InstructionTable[ADR8_Op_POBH] = "POBH";
  InstructionTable[ADR8_Op_POA] = "POA";
  InstructionTable[ADR8_Op_POB] = "POB";
  InstructionTable[ADR8_Op_POX] = "POX";
  InstructionTable[ADR8_Op_POY] = "POY";
}

typedef enum{
  TokenType_UNKNOWN = 0,
  TokenType_WHITESPACE,
  TokenType_NEWLINE,
  TokenType_LABEL,
  TokenType_INSTRUCTION,
  TokenType_HEXNUMBER,
  TokenType_DECNUMBER,
  TokenType_STRING,
  TokenType_COMMENT,
} TokenType;

#define TOKEN_BUFFER_SIZE 512
typedef struct{
  TokenType type;
  char buffer[TOKEN_BUFFER_SIZE];
  size_t len;
} Token;

void Token_append_char(Token* token, char c){
  assert(token->len < TOKEN_BUFFER_SIZE);
  token->buffer[token->len] = c;
  token->len++;
  token->buffer[token->len] = '\0';
}

bool Tokenizer_is_nl(char c){
  return c == '\n';
}

bool Tokenizer_is_ws(char c){
  return c == ' ' || c == '\t'; 
}

bool Tokenizer_is_digit(char c){
  return c >= '0' && c <= '9';
}

bool Tokenizer_is_hexdigit(char c){
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Tokenizer_is_letter(char c){
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void Tokenizer_consume_until_ws(FILE* stream, Token* token){
  do{
    int c = fgetc(stream);
    if(c == EOF) return;
    token->buffer[token->len] = c;
    token->len++;
  }while(!Tokenizer_is_ws(token->buffer[token->len-1]));
  token->buffer[token->len] = '\0';
  token->len--;
}

void Tokenizer_consume_until_nl(FILE* stream, Token* token){
  do{
    int c = fgetc(stream);
    if(c == EOF) return;
    token->buffer[token->len] = c;
    token->len++;
  }while(token->buffer[token->len-1] != '\n');
  token->buffer[token->len] = '\0';
  token->len--;
}

TokenType Tokenizer_identify_tokentype(char c){
  if(Tokenizer_is_ws(c)){
    return TokenType_WHITESPACE;
  }else if(Tokenizer_is_digit(c) || c == '-'){
    return TokenType_DECNUMBER;
  }else if(Tokenizer_is_letter(c)){
    return TokenType_INSTRUCTION;
  }else if(c == '/'){
    return TokenType_COMMENT;
  }else if(c == '"'){
    return TokenType_STRING;
  }else if(Tokenizer_is_nl(c)){
    return TokenType_NEWLINE;
  }else{
    return TokenType_UNKNOWN;
  }
}
char Tokenizer_resolve_escape_code(char c, size_t line){
  switch (c) {
    case '\\': return '\\';
    case 'n': return '\n';
    case '0': return '\0';
    case 'r': return '\r';
    case 't': return '\t';
    default:
      ADR8_ERROR_LOG("unsupported escape code '\\%c' on line %lu\n",c,line);
      exit(1);
  }
}
void Tokenizer_consume(FILE* stream, Token* token, size_t line){
  static int c = EOF;
  
  token->len = 0;
  if(c == EOF) c = fgetc(stream);
  if(c == EOF) return;

  token->type = Tokenizer_identify_tokentype(c);
  bool end_of_token = false;
  while(!end_of_token && c != EOF){
    ADR8_DEBUG_LOG("tokenizing '%c' ",c);
    switch(token->type){
      case TokenType_LABEL:
        ADR8_DEBUG_LOG("as LABEL\n");
      case TokenType_INSTRUCTION:
        ADR8_DEBUG_LOG("as INSTRUCTION\n");
        if(Tokenizer_is_letter(c) || c == '_'){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else if(c == ':'){
          Token_append_char(token, c);
          token->type = TokenType_LABEL;
          c = fgetc(stream);
          return;
        }else{
          return;
        }
        break;
      case TokenType_HEXNUMBER:
        ADR8_DEBUG_LOG("as HEXNUMBER\n");
        if(Tokenizer_is_hexdigit(c)){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else{
          return;
        }
        break;
      case TokenType_DECNUMBER:
        ADR8_DEBUG_LOG("as DECNUMBER\n");
        if(Tokenizer_is_digit(c)){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else if(c == '-' && token->len == 0){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else if(token->buffer[0] != '-' && c == 'x' && token->len == 1){
          token->type = TokenType_HEXNUMBER;
          Token_append_char(token,c);
          c = fgetc(stream);
        }else{
          return;
        }
        break;
      case TokenType_WHITESPACE:
        ADR8_DEBUG_LOG("as WHITESPACE\n");
        if(Tokenizer_is_ws(c)){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else{
          return;
        }
        break;
      case TokenType_STRING:
        ADR8_DEBUG_LOG("as STRING\n");
        if(c != '"'){
          if(c == '\\') c = Tokenizer_resolve_escape_code(fgetc(stream),line);
          Token_append_char(token, c);
          c = fgetc(stream);
        }else if(token->len == 0){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else{
          Token_append_char(token, c);
          c = fgetc(stream);
          return;
        }
        break;
      case TokenType_NEWLINE:
        ADR8_DEBUG_LOG("as NEWLINE\n");
        if(Tokenizer_is_nl(c)){
          Token_append_char(token, c);
          c = fgetc(stream);
          return;
        }else{
          assert(false && "unreachable");
        }
        break;
      case TokenType_COMMENT:
        ADR8_DEBUG_LOG("as COMMENT\n");
        if(!Tokenizer_is_nl(c)){
          Token_append_char(token, c);
          c = fgetc(stream);
        }else{
          return;
        }
        break;
      case TokenType_UNKNOWN:
        ADR8_DEBUG_LOG("as UNKNOWN\n");
        Token_append_char(token, c);
        c = fgetc(stream);
        return;
    }
  }
  return;
}

int Assembler_encode_instruction(Token* token){
  for(int i = 0; i < 0x100; ++i){
    if(InstructionTable[i]){
      int res = strcmp(InstructionTable[i],token->buffer) == 0;
      if(res){
        return i;
      }
    }
  }
  return -1;
}

typedef struct{
  char name[512];
  uint16_t loc;
  size_t line;
  char* file;
} Label;

DA_def(Label);
DA_def(uint8_t);
typedef char* char_ptr;
DA_def(char_ptr);

char* BOOTSTRAPPER_PATH = "utilities/bootstrapper.asm";

void Assembler_assemble(DA_char_ptr* input_files, char* outfile){
  DA_uint8_t program = {0};
  DA_Label label_definitions = {0};
  DA_Label label_uses = {0};
  FILE* outstream = fopen(outfile,"w");
  if(!outstream){
    ADR8_ERROR_LOG("unable to open output file '%s': %s\n",outfile,strerror(errno));
    exit(1);
  }

  // TODO load input files and update error logs
  Token token;

  DA_foreach(input_files, char**, input_file){
    FILE* instream = fopen(*input_file,"r");
    if(!instream){
      ADR8_ERROR_LOG(" couldn't open file '%s'\n",*input_file);
      exit(1);
    }
    size_t line = 1;
    Tokenizer_consume(instream, &token,line);
    ADR8_DEBUG_LOG("assembling file '%s'\n",*input_file);

    while(token.len > 0){
      ADR8_DEBUG_LOG("assembling token: '%s'\n",token.buffer);
      switch(token.type){
        case TokenType_NEWLINE:
          line++;
          break;
        case TokenType_COMMENT:
        case TokenType_WHITESPACE:
          break;
        case TokenType_INSTRUCTION:{
          int instruction = Assembler_encode_instruction(&token);
          if(instruction < 0){
            ADR8_DEBUG_LOG("'%s' not instruction, assuming label\n", token.buffer);
            Label label;
            label.loc = DA_len(&program);
            label.line = line;
            strcpy(label.name,token.buffer);
            DA_append(&label_uses, label);
            DA_append(&program,0x00); // add placeholder into program
            DA_append(&program,0x00);
          }else{
            ADR8_DEBUG_LOG("'%s' decoded instruction: 0x%02hX\n", token.buffer,instruction);
            DA_append(&program,instruction);
          }
        }break;
        case TokenType_LABEL:{
          Label label;
          label.loc = DA_len(&program);
          label.file = *input_file;
          token.buffer[token.len-1] = '\0';
          strcpy(label.name,token.buffer);
          DA_append(&label_definitions, label);
        }break;
        case TokenType_DECNUMBER:{
          int8_t n = atoi(token.buffer);
          DA_append(&program,n);
        }break;
        case TokenType_HEXNUMBER:{
          if(token.len == 4){
            uint8_t n = strtoul(token.buffer, NULL, 16);
            DA_append(&program,n);
          }else if(token.len == 6){
            uint16_t n = strtoul(token.buffer, NULL, 16);
            uint8_t h = (n&0xFF);
            DA_append(&program, h);
            uint8_t l = (n&0xFF00)>>8;
            DA_append(&program, l);
          }else{
            ADR8_ERROR_LOG("%s:%lu: Unclear or unsupported hex number '%s', \n"
                "hex number must be explicitly 8-bit (e.g. 0xFF) or \n"
                "explicitly 16-bit (e.g. 0xFFFF)\n",*input_file,line,token.buffer);
            exit(1);
          }
        }break;
        case TokenType_STRING:{
          for(size_t i = 1; i < token.len-1; ++i){
            ADR8_DEBUG_LOG("string append '%c'\n");
            DA_append(&program, token.buffer[i]);
          }
          DA_append(&program, '\0');
        }break;
        case TokenType_UNKNOWN:{
          ADR8_ERROR_LOG("%s:%lu: invalid token: '%s'\n", *input_file, line, token.buffer);
          exit(1);
        }break;
      }
      Tokenizer_consume(instream, &token, line);
    }
    fclose(instream);
  }

  DA_foreach(&label_uses, Label*, label_use){
    bool resolved = false;
    DA_foreach(&label_definitions, Label*, label_definition){
      if(strcmp(label_use->name, label_definition->name) == 0){
        size_t address = label_definition->loc;
        DA_set(&program, label_use->loc, address & 0xFF);
        DA_set(&program, label_use->loc+1, (address & 0xFF00)>>8);
        resolved = true;
      }
    }
    if(!resolved){
      ADR8_ERROR_LOG("%s:%lu: unable to resolve symbol '%s'\n", label_use->file, label_use->line, label_use->name);
      exit(1);
    }
  }

  // if assembled for bootstrapper
  if(strcmp(DA_at(input_files,0),BOOTSTRAPPER_PATH) == 0){
    ADR8_DEBUG_LOG("size: %lu [%08lX]\n",DA_len(&program), DA_len(&program));
    fputc(DA_len(&program)&0xFF,outstream);
    fputc((DA_len(&program)&0xFF00)>>8,outstream);
  }
  ADR8_DEBUG_LOG("program output:\n");
  uint32_t i = 0;
  DA_foreach(&program, uint8_t*, byte){
    ADR8_DEBUG_LOG("  %04X: %02hX\n",i,*byte);
    fputc(*byte, outstream);
    i++;
  }
  
}


int main(int argc, char** argv){
  DA_char_ptr input_files = {0};
  char* output_file = NULL;
  for(int i = 1; i < argc; ++i){
    if(argv[i][0] == '-'){
      switch (argv[i][1]) {
        case 'o': 
          ADR8_DEBUG_LOG("setting outfile to `%s`\n",argv[i]);
          output_file = argv[++i];
          break;
        case 'b':
          ADR8_DEBUG_LOG("input files before\n");
          DA_foreach(&input_files, char**, input_file){
            ADR8_DEBUG_LOG("'%s'\n",*input_file);
          }
          FILE* file = fopen(BOOTSTRAPPER_PATH, "r");
          if(!file){
            ADR8_ERROR_LOG(" couldn't find bootstrapper, please make sure the utilities folder is in PATH or in your current directory");
            return 1;
          }
          DA_insert(&input_files, BOOTSTRAPPER_PATH, 0);
          ADR8_DEBUG_LOG("input files after\n");
          DA_foreach(&input_files, char**, input_file){
            ADR8_DEBUG_LOG("'%s'\n",*input_file);
          }
          break;
      }
    }else{
      DA_append(&input_files, argv[i]);
    }
  }
  
  if(!output_file){
    ADR8_ERROR_LOG("Usage: asm -o [OUTFILE] [INFILE1 INFILE2 ... ]\n");
    return 1;
  }

  InstructionTable_init();
  
  Assembler_assemble(&input_files, output_file);
}
