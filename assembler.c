#include "h.h"

struct opname {char *name; Opcode op; int arg;};

void assemble(FILE *file, Instruction *bytecode){
	char inst[10];
	Address ip = 0;
	static struct opname ops[] = {
		{"halt", Op_halt, 0},
		{"print", Op_print, 0},
	};
	puts("hello?");
	while(fscanf(file, " %s \n", inst) != EOF){
		puts("LINE");
		Opcode op;
		int i;
		for (i=0;i<2;i++){
			if (!strcmp(ops[i].name, inst)){
				op = ops[i].op;
				if(ops[i].arg){
					
				}
				goto brk;
			}
		}
		printf("invalid opcode: %s\n", inst);
		exit(1);
	brk:
		puts("a!");
		bytecode[ip++].opcode=op;
		puts("b");
	}
	bytecode[ip++].opcode=Op_halt;
}
