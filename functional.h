#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include "stack.h"

#define void_as(type, var) ( *((type*)(var)) )

#define val_as(type) void_as(type, val)
#define val_type TYPE(gettype(val))
#define val_size type_size[val_type]

/* STR */

typedef void* (*func_t) (void*);

const char* str_fmt[] = {
    "%i", "%f", "%c"
};

void* str_base(void* val) {
    stack_top->type = BASE_TYPE_MSG;

    snprintf(stack_top->b, 64, str_fmt[val_type], val_as(int64_t));

    return stack_top->b;
}

void* str_msg(void* val) {
    return strncpy(stack_top->b, val, 64);
}

func_t str_func[BASE_TYPES] = {
    str_base, str_base, str_base, str_msg
};


const char* str(void* val) {
    return str_func[val_type](val);
}

/* SUB */

#define sub_base(__type, id)                                                             \
void* sub_##__type(void* val) {                                                          \
    stack_top->type = BASE_TYPE_##id;                                                    \
    stack_top->__type##_v[0] = void_as(__type, val_as(tuple2).v1) - void_as(__type, val_as(tuple2).v2);\
    return stack_top->__type##_v;                                                        \
}
sub_base(int, INT)
sub_base(double, FLOAT)
sub_base(char, CHAR)
#undef sub_base

func_t sub_func[BASE_TYPES] = {
    sub_int, sub_double, sub_char
};

void* sub(void* val) {
    return sub_func[TYPE(gettype(val_as(tuple2).v1))](val);
}

/* ADD */

#define add_base(__type, id)                                                             \
void* add_##__type(void* val) {                                                          \
    stack_top->type = BASE_TYPE_##id;                                                    \
    void_as(__type, val_as(tuple2).v1) += void_as(__type, val_as(tuple2).v2);            \
    return (*(__type**)stack_top->__type##_v) = val_as(tuple2).v1;                       \
}
add_base(int, INT)
add_base(double, FLOAT)
add_base(char, CHAR)
#undef add_base

func_t add_func[BASE_TYPES] = {
    add_int, add_double, add_char
};

void* add(void* val) {
    return add_func[TYPE(gettype(val_as(tuple2).v1))](val);
}


/* SAY */

void* say(void* val) {
    while (val_as(void*)) {
        fputs(str(val_as(void*)), stdout);
        val += sizeof(void*);
    }

    return NULL;
}

#undef val_as
#undef val_type
#endif /* FUNCTIONAL_H */
