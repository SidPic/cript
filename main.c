#include <stdio.h>
#include <string.h>

#include "stack.h"
#include "functional.h"

void main() {
    memset(stack, BASE_TYPE_NONE, sizeof(stack));

    const char* src = "set a 10; sub 20 1; add a .; say \'a = \' a \'\n\'";

    void* a = newvar(BASE_TYPE_INT, (int[]){10});
    void* b = newvar(BASE_TYPE_INT, (int[]){2});

    tuple2 t = {a, b};

    diff(&t);

    t.v2 = stack_top->b;

    add(&t);

    printf("%i\n", astype(int, a));

    void* c1 = newvar(BASE_TYPE_MSG, "a = ");
    void* c2 = newvar(BASE_TYPE_CHAR, "\n");
    arglist args = (void*[]){c1, a, c2, NULL};

    say(args);
}
