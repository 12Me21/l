#include "h.h"

void Value_print(Value *value){
	Type type = value->type;
	if(type == Type_number){
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
	}else if(type == Type_string){
		BucketIndex i; //bad
		for(i=0;i<value->string->length;i++)
			printf("%c", value->string->string[i]);
	}else if(type == Type_boolean){
		printf("%s", value->boolean ? "true" : "false");
	}else if(type == Type_function){
		printf("<function>");
	}else if(type == Type_table){
		BucketIndex i;
		TableNode *item = value->table->shead;
		while(item){	
			Value_print(item->key);
			printf(": ");
			Value_print(item->variable->value);
			printf("\n");
			item=item->snext;
		}
	}else if(type == Type_null){
		printf("null");
	}
}
