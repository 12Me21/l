#include "h.h"

static void Value_print_sub(Value *value){
	Type type = value->type;
	switch(type){
	when(Type_number):;
		Number num = value->number;
		if(isnan(num))
			printf("NaN");
		else if(isinf(num)){
			if(num<0)
				printf("-Inf");
			else
				printf("Inf");
		}else
			printf("%.*g", num == nearbyintf(num) ? 99999 : 15, num);
	when(Type_string):;
		BucketIndex i; //bad
		for(i=0;i<value->string->length;i++)
			printf("%c", value->string->string[i]);
	when(Type_boolean):;
		printf("%s", value->boolean ? "true" : "false");
	when(Type_function):;
		printf("<function>");
	when(Type_table):;
		printf("{");
		TableNode *item = value->table->shead;
		int printed=0;
		while(item){
			if (printed)
				printf(", ");
			Value_print_sub(&item->key);
			printf(": ");
			Value_print_sub(&item->variable->value);
			printed = 1;
			item=item->snext;
		}
		printf("}");
	when(Type_null):;
		printf("null");
	otherwise:
		puts("TRIED TO PRINT INVALID TYPE");
		exit(1);
	}
}

// Debug: print a value
void Value_print(Value *value){
	Value_print_sub(value);
	puts("");
}