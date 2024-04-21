#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <obstack.h>

#include "map.h"
//~ #include "type.h"
size_t exprs_size;

void fatal(const char* format, ...) {
    va_list v;
    va_start(v, format);
    vfprintf(stderr, format, v);
    va_end(v);
    exit(EXIT_FAILURE);
}

void* xmalloc (size_t size)
{
    register void *value = malloc (size);
    if (value == 0) fatal("virtual memory exhausted");
    return value;
}

typedef struct {
    char func_name[64];
    char args[7][64];
} expression;

static struct obstack exprs;

typedef char cmd[64];
static struct obstack commands;

#define BUF_SIZE 256
#define DELIMS " \n\t"

char* parse_priority(char* str) {
    if (str == NULL) return NULL;
    if (*str == '\0' || *str == '\n') return NULL;

    while (*str && *str != '(') ++str;
    if (*str != '(') return NULL;
    ++str;

    char* begin = str-1;
    char buf[256]; buf[0] = ' '; buf[1] = '('; int len = 2;

    while (*str && *str != ')' && *str != '\n') {
        if (*str == '(') {
            str = parse_priority(str);
            continue;
        }

        buf[len++] = *str++;
    };
    buf[len++] = *str++;

    return mempcpy(mempcpy(begin, str+1, strlen(str)-2), buf, len);
}

const char* parse_brackets(const char* str) {
again:
    if (str == NULL) return NULL;

    while (isspace(*str)) ++str;

    if (*str == '\0') return NULL;
    if (*str == ')') return str+1;
    if (*str == '(') {
        str = parse_brackets(++str);
        goto again;
    }

    cmd buf; char *ptr = buf;

    while (*str && !isspace(*str) && *str != ')') *ptr++ = *str++; *ptr++ = '\0';

    str = parse_brackets(++str);

    obstack_grow(&exprs, buf, sizeof(cmd));

    return str;
}

int isinfix(const char* str) {
    // return (int)hm_get(infixes, str);
    return str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/';
}

void parse_infix(cmd* command) {
    for (int i = 0; i < exprs_size; ++i) {
        if (isinfix(command[i])) {
            char buf[64];
            strcpy(buf, command[i+1]);
            strcpy(command[i+1], command[i]);
            strcpy(command[i], buf);
            ++i;
        }
    }
}

/*
 * func -> pop args -> put result => run func
 * otherwise -> put args          => put arg
 */

void* parse_file(FILE* file) {
    obstack_blank(&exprs, 0);
    char buf[BUF_SIZE];

    while (fgets(buf, BUF_SIZE, file)) {
        parse_priority(buf);
        puts(buf);
        parse_brackets(buf);
    }

    exprs_size = obstack_object_size(&exprs);
    cmd* commands = obstack_finish(&exprs);

    parse_infix(commands);

    return commands;
}

int main() {
    obstack_init(&exprs);

    FILE* file = fopen("object.cript", "r");
    cmd* es = parse_file(file);
    fclose(file);

    for (int i = 0; i < exprs_size / sizeof(cmd); ++i) {
        puts(es[i]);
    }

    return 0;
}
