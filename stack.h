#ifndef STACK_H
#define STACK_H

#include <stdint.h>

// TYPES

enum {
    BASE_TYPE_INT, BASE_TYPE_FLOAT, BASE_TYPE_CHAR,

    BASE_TYPES, BASE_TYPE_NONE = (uint8_t)(-1)
};
typedef uint8_t type_t; // 127 max
const type_t type_cap = 1u << 7;

#define TYPE(type) ( (type) & ~type_cap )
#define CAP(type) ( (type) & type_cap )

// MEMORY BLOCK STRUCT

typedef struct {
    type_t type;
    union {
        char b[64];

        int16_t h[32];
        int32_t i[16];
        int64_t l[8];

        float f[16];
        double d[8];

        void* p[8];
    };
} memblock;

// STACK
memblock stack[64];

// GET TYPE FIELD FOR VARIABLE
type_t gettype(void* ptr) {
    return stack[((ptr - (void*)stack)) / sizeof(memblock)].type;
}

// ALLOCATOR

void* newvar(const uint8_t type) {
    memblock* mbl = stack;
    int32_t* i;

    do {
        if (CAP(mbl->type) == 0) continue;

        i = mbl->i;
        if (TYPE(mbl->type) == type) {
            do {
                if (*i == (uint64_t)(-1)) {
                    *i = 0;
                    if (i == &(mbl->i[15]))
                        mbl->type = type;

                    return i;
                }
            } while (i++ != &(mbl->i[15]));
        }
        else if (mbl->type == BASE_TYPE_NONE) {
            mbl->type = type | type_cap;
            *i = 0;
            return i;
        }
    } while(mbl++ != &(stack[63]));

    return NULL;
}

#endif /* STACK_H */
