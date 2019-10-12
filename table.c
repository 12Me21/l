#include "h.h"

static BucketIndex hash_pointer(void *ptr){
	return (size_t)ptr;
}

static BucketIndex Table_hash(Value *value){
	Type type = value->type;
	if(type == Type_number)
		return value->number;
	if(type == Type_string){
		unsigned int i;
		BucketIndex hash = 5381;
		for(i=0;i<value->string->length;i++)
			hash = hash*33 + value->string->string[i];
		return hash;
	}
	if(type == Type_boolean)
		return value->boolean ? 69420 : 14522;
	if(type == Type_function)
		return hash_pointer(value->function);
	if(type == Type_table)
		return hash_pointer(value->table);
	if(type == Type_null)
		return 10112019;
	exit(1);
}

//compare values for table keys
static int Value_compare(Value *v1, Value *v2){
	Type type = v1->type;
	if (type != v2->type)
		return 0;
	if (v1->class != v2->class)
		return 0;
	if (type == Type_number)
		return v1->number == v2->number;
	if (type == Type_string)
		return v1->string->length == v2->string->length && !memcmp(v1->string->string, v2->string->string, v1->string->length);
	if (type == Type_boolean)
		return v1->boolean == v2->boolean;
	if (type == Type_null)
		return 1;
	if (type == Type_function)
		return v1->function == v2->function;
	if (type == Type_table)
		return v1->table == v2->table;
	exit(1);
}

Table *Table_new(BucketIndex size){
	Table *new = malloc(sizeof(Table));
	new->shead = NULL;
	new->size = size;
	new->buckets = malloc(size * sizeof(TableNode *));
	BucketIndex i;
	for(i=0;i<size;i++)
		new->buckets[i] = NULL;
	return new;
}

TableNode *Table_get(Table *tb, Value *key){
	BucketIndex index = Table_hash(key) % tb->size; //someday I'm going to forget to % ...
	TableNode *bucket = tb->buckets[index];
	while(bucket){
		if (Value_compare(bucket->key, key))
			return bucket;
		bucket = bucket->bnext;
	}
	return NULL;
}

TableNode *Table_add(Table *tb, Value *key, Variable *var){
	BucketIndex index = Table_hash(key) % tb->size;
	TableNode *bucket = tb->buckets[index];	
	while(bucket){
		if (Value_compare(bucket->key, key)){
			bucket->variable = var;
			return bucket;
		}
		bucket = bucket->bnext;
	}
	bucket = malloc(sizeof(TableNode));
	bucket->bnext = tb->buckets[index];
	bucket->key = key;
	bucket->variable = var;
	tb->buckets[index] = bucket;
	if(!(tb->shead)){
		tb->shead = bucket;
		bucket->snext = NULL;
		bucket->sprev = bucket;
	}else{
		bucket->sprev = tb->shead->sprev;
		bucket->snext = NULL;
		tb->shead->sprev->snext = bucket;
		tb->shead->sprev = bucket;
	}
	return bucket;
}

TableNode *Table_remove(Table *tb, Value *key){
	TableNode *node = Table_get(tb, key);
	if (!node)
		return NULL;
	if(node->sprev)
		node->sprev->snext = node->next;
	//oh this is going to break with a circle list ughh
	
}
