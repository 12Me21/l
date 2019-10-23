#include "h.h"

struct opname {char *name; int arg;};

static struct opname ops[] = {
	{"invalid", 0},
	{"push", 1},
	{"print", 0},
	{"halt", 0},
	{"jump", 2},
	{"discard", 0},
	{"add", 0},
	{"variable",0},
};

void assemble(FILE *file, Instruction *bytecode){
	char name[10];
	char str_buf[1000];
	Address ip = 0;
	unsigned i;
	while(fscanf(file, " %s ", name) != EOF){
		struct opname op;
		for (i=0; i<DIM(ops) ;i++){
			if (!strcmp(ops[i].name, name)){
				op = ops[i];
				goto brk;
			}
		}
		printf("invalid opcode: %s\n", name);
		exit(1);
	brk:
		if(op.arg==1){
			char c = fgetc(file);
			if (c=='"'){
				fscanf(file, " %s\" \n", str_buf);
				bytecode[ip].value = Value_string(str_buf, strlen(str_buf)-1, NULL);
			}else{
				ungetc(c, file);
				Number x = 3;
				fscanf(file, " %lf \n", &x);
				bytecode[ip].value = Value_number(x, NULL);
			}
		}else if(op.arg==2){ //takes addr
			Address addr;
			fscanf(file, " %d \n", &addr);
			bytecode[ip].address=addr;
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
		if(ops[inst.opcode].arg == 1){
			Value_print(&inst.value);
		}else if(ops[inst.opcode].arg == 2){
			printf("%d\n", inst.address);
		}else{
			puts("");
		}
		if(inst.opcode==Op_halt)
			break;
		ip++;
	}
}

//need some kind of lazy-evaluated stack list
// so @(1,2,3)[0] only evaluates 1, not 2 or 3
// an extension of shortcutting, basically.
// perhaps store indexes to the start of each expression,
// and then jump to the relevant one, then jump to the end?

