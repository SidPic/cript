#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "stack.h"

#define val_type TYPE(gettype(val))
#define val_size type_size[val_type]
#define val_tuple astype(tuple2, val)
#define val_list ((void**)(val))
#define val_arg  astype(void*, val)

#define vtype(vptr) TYPE(gettype(vptr))
#define astype(type, vptr) (*((type*)(vptr)))
#define astuple(vptr) astype(tuple2, vptr)

#define FUNCS(name)     name##_int, name##_double, name##_char
#define ALL_FUNCS(name) FUNCS, name##_msg

/* D E S C R I P T I O N
 *
 * general func:
 *  void* var -> void* result
 *  stack_top.value = result->value
 *  stack_top.type = result->type
 *
 * sub func:
 *  any args -> void* result
 *
 * */


/* STR */


#define str_func(_type, fmt)                                                             \
char* str_##_type(void* val) {                                                           \
    snprintf(stack_top->char_v, 64, fmt, astype(_type, val));                            \
    return stack_top->char_v;                                                            \
}
str_func(int,    "%i")
str_func(double, "%f")
str_func(char,   "%c")
#undef str_func

char* str_msg(void* val) {
    return strncpy(stack_top->b, val, 64);
}

typedef char* (*str_func_t) (void*);

str_func_t str_func[BASE_TYPES] = { FUNCS(str), str_msg };

void* str(void* val) {
    stack_top->type = BASE_TYPE_MSG;
    return str_func[val_type](val);
}


/* OPERATIONS */


#define op_func(name, _type, op)                                                         \
void* name##_##_type(void* val, void* other) {                                           \
    *stack_top->_type##_v = astype(_type, val) op astype(_type, other);                  \
    return stack_top->_type##_v;                                                         \
}

typedef void* (*op_func_t) (void*, void*);
#define op_funcs FUNCS
#define func_arr(name) op_func_t name##_func[BASE_TYPES] = { op_funcs(name) };

#define gen_func(name)                                                                   \
void* name(void* val) {                                                                  \
    stack_top->type = vtype(val_tuple.v1);                                               \
    return name##_func[stack_top->type](val_tuple.v1, val_tuple.v2);                     \
}

#define arr_genf(name) func_arr(name) gen_func(name)

#define op_group(_type)                                                                  \
op_func(sum, _type, +)        op_func(add, _type, +=)                                    \
op_func(diff, _type, -)       op_func(sub, _type, -=)                                    \
op_func(quot, _type, /)       op_func(div, _type, /=)                                    \
op_func(prod, _type, *)       op_func(mult, _type, *=)

op_group(int)
op_group(char)
op_group(double)

arr_genf(sum)   arr_genf(add)
arr_genf(diff)  arr_genf(sub)
arr_genf(quot)  arr_genf(div)
arr_genf(prod)  arr_genf(mult)

#undef op_func
#undef op_group
#undef op_funcs
#undef func_arr
#undef gen_func


/* SAY */


void* say(void* val) {
    while (val_arg) {
        fputs(str(val_arg), stdout);
        val += sizeof(void*);
    }

    return NULL;
}

#undef val_as
#undef val_type
#endif /* FUNCTIONAL_H */
