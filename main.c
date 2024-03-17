#include <stdio.h>
#include <string.h>

#include "stack.h"

void main() {
    memset(stack, BASE_TYPE_NONE, sizeof(stack));

    for (int i = 0; i < 16; ++i) newvar(BASE_TYPE_INT);
    void* a = newvar(BASE_TYPE_CHAR);
    printf("%i", TYPE(gettype(a)));
}
