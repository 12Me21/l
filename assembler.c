#include "h.h"

struct opname {char *name; int arg;};

static struct opname ops[] = {
	{"invalid", 0},
	{"push", 1},
	{"print", 0},
	{"halt", 0},
};

void assemble(FILE *file, Instruction *bytecode){
	char name[10];
	Address ip = 0;
	int i;
	while(fscanf(file, " %s ", name) != EOF){
		struct opname op;
		for (i=0;i<sizeof ops/sizeof(struct opname) ;i++){
			if (!strcmp(ops[i].name, name)){
				op = ops[i];
				goto brk;
			}
		}
		printf("invalid opcode: %s\n", name);
		exit(1);
	brk:
		if(op.arg){
			Number x = 3;
			fscanf(file, " %lf \n", &x);
			bytecode[ip].value = Value_number(x, NULL);
		}else{
			fscanf(file, "\n");
		}
		bytecode[ip++].opcode=i;
	}
	bytecode[ip++].opcode=Op_halt;
}

void disassemble(Instruction *bytecode){
	Address ip=0;
	Instruction inst;
	while(1){
		inst = bytecode[ip];
		printf("%3d: %s ", ip, ops[inst.opcode].name);
		if(ops[inst.opcode].arg){
			Value_print(&inst.value);
		}else{
			puts("");
		}
		if(inst.opcode==Op_halt)
			break;
		ip++;
	}
}
