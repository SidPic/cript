#ifndef STACK_H
#define STACK_H

#include <stdint.h>

// TYPES

enum {
    BASE_TYPE_INT, BASE_TYPE_FLOAT, BASE_TYPE_CHAR, BASE_TYPE_MSG, // short message [64]
    //~ BASE_TYPE_ARRAY,

    BASE_TYPES
};

typedef uint8_t type_t; // 127 max
const type_t type_cap = 1u << 7;
const uint64_t BASE_TYPE_NONE = -1;

#define TYPE(type) ( (type) & ~type_cap )
#define CAP(type) ( (type) & type_cap )

const size_t type_size[BASE_TYPES] = {
    sizeof(int32_t), sizeof(double), sizeof(int8_t), sizeof(char[64])
};

// MEMORY BLOCK STRUCT

typedef void* pointer;

typedef struct {
    void *v1, *v2;
} tuple2;

typedef void** arglist;

typedef struct {
    type_t type;
    size_t size;
    void* data;
} array;

typedef struct {
    type_t type;
    union {
        char b[64]; char char_v[64];

        int16_t h[32];
        int32_t i[16]; int32_t int_v[16];
        int64_t l[8];

        float f[16];
        double d[8]; double double_v[8];

        void* p[8]; void* pointer_v[8];
    };
} memblock;

// STACK
memblock stack[64];
memblock* stack_top = &(stack[63]);

// GET TYPE FIELD FOR VARIABLE
type_t gettype(void* ptr) {
    return stack[((ptr - (void*)stack)) / sizeof(memblock)].type;
}

// ALLOCATOR

// add last_position remembering

void* newvar(const uint8_t type, void* value) {
    memblock* mbl = stack;
    char* i;

    while (mbl != stack_top) {

        if (CAP(mbl->type) == 0) continue;

        if (TYPE(mbl->type) == type) {

            i = (char*)mbl->b - type_size[type];

            while (i != &(mbl->b[64 - type_size[type]])) {
                i += type_size[type];

                if (memcmp(i, &BASE_TYPE_NONE, type_size[type]) == 0)
                {
                    memcpy(i, value, type_size[type]);

                    if (i == &(mbl->b[64 - type_size[type]]))
                        mbl->type = type;

                    return i;
                }
            }
        }
        else if (mbl->type == (type_t)BASE_TYPE_NONE)
        {
            mbl->type = type | type_cap;
            memcpy(mbl->b, value, type_size[type]);
            return mbl->b;
        }

        ++mbl;
    }

    return NULL;
}

#endif /* STACK_H */
