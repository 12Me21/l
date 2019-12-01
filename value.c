#include "h.h"

static void Value_print_sub(Value *value){
	Value_check(value);
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

void String_fin(GC_PTR obj, GC_PTR cd){
	(void)cd;
	free(((String *)obj)->string);
}

//make a new string
String *String_new(char *data, unsigned int length){
	String *string = GC_MALLOC_ATOMIC(sizeof(String));
	string->string = malloc(length);
	GC_register_finalizer(string, String_fin, NULL, NULL, NULL);
	memcpy(string->string, data, length);
	string->length = length;
	return string;
}

//make a new string
String *String_new_nocopy(char *data, unsigned int length){
	String *string = GC_MALLOC_ATOMIC(sizeof(String));
	GC_register_finalizer(string, String_fin, NULL, NULL, NULL);
	string->string = data;
	string->length = length;
	return string;
}

//copies a string value from `value` to `dest`
void Value_string_copy(Value *dest, Value *value){
	//todo: check if value type is string
	dest->type = Type_string;
	dest->string = String_new(value->string->string, value->string->length);
}

void Value_copy(Value *dest, Value *value){
	switch(value->type){
	when(Type_string):;
		Value_string_copy(dest, value);
	when(Type_table):;
		//oh gosh table copy...
		*dest = *value;
	otherwise:;
		*dest = *value;
	}
}

void Value_check(Value *v) {
	if (v->type == Type_default) {
		Error_message = "Uninitialized value used";
		longjmp(Error_jump, 1);
	}
}
