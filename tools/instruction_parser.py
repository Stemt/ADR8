
def parse_instructions(path):
    instructions = {}
    with open(path,'r') as file:
        inopdef = False
        for line in file.readlines():
            if 'enum{ // ADR8_OpCode' in line:
                inopdef = True
            if '}ADR8_OpCode;' in line:
                inopdef = False
            if not inopdef:
                continue
            if not 'ADR8_Op_' in line:
                continue
            line = line.split('Op_')
            line = line[1].split('=')
            name = line[0].strip()
            opcode = line[1].split(',')[0].strip()
            instructions[name] = opcode
    return instructions

def generate_c_lookup_table(instructions):
    for instruction in instructions:
        print(f'table[ADR8_Op_{instruction}] = "{instruction}";')


instructions = parse_instructions('ADR8.h')
generate_c_lookup_table(instructions)

