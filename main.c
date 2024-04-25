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

static struct obstack exprs;
typedef char cmd[64];

#define BUF_SIZE 256
#define DELIMS " \n\t"

//~ size_t exprlen(const char* str) {
    //~ size_t len = 0;
    //~ while (*str && *str != ')') ++len, ++str;
    //~ return len;
//~ }

//~ char* parse_priority(char* str) {
    //~ if (str == NULL) return NULL;
    //~ if (*str == '\0' || *str == '\n' || *str == ')') return NULL;

    //~ while (*str && *str != '(') ++str;
    //~ if (*str != '(') return NULL;
    //~ ++str;

    //~ char* begin = str-1;
    //~ char buf[256]; int len;
    //~ if ((*str - 3) != ' ') {
        //~ buf[0] = ' ';
        //~ buf[1] = *(str-3);
        //~ buf[2] = ' ';
        //~ buf[3] = '(';
        //~ len = 4;
    //~ } else {
        //~ buf[0] = ' ';
        //~ buf[1] = '(';
        //~ len = 2;
    //~ }
    //~ int shift = len;

    //~ while (*str && *str != ')' && *str != '\n') {
        //~ if (*str == '(') {
            //~ str = parse_priority(str);
            //~ continue;
        //~ }

        //~ buf[len++] = *str++;
    //~ };
    //~ buf[len++] = *str++;

    //~ return mempcpy(mempcpy(begin, str+1, exprlen(str)-2), buf, len);
//~ }

#define L_BRACKET (cmd){[0] = '(', [1] = '\0'}
#define R_BRACKET (cmd){[0] = ')', [1] = '\0'}
#define END       (cmd){[0] = ';', [1] = '\0'}
#define COMMA     (cmd){[0] = ',', [1] = '\0'}

/*
 * сделать "запятую", считать кол-во аргументов
 * но пока зачем....?
 */

const char* parse_brackets(const char* str) {
again:
    if (str == NULL) return NULL;

    while (isspace(*str)) ++str;

    if (*str == '\0') return NULL;
    if (*str == ',') {
        str = parse_brackets(++str);
        obstack_grow(&exprs, COMMA, sizeof(cmd));
        goto again;
    }
    if (str[0] == ')' && str[1] == ',') {
        str = parse_brackets(str+2);
        obstack_grow(&exprs, COMMA, sizeof(cmd));
        obstack_grow(&exprs, L_BRACKET, sizeof(cmd));
        goto again;
    }
    if (*str == ')') {
        obstack_grow(&exprs, L_BRACKET, sizeof(cmd));
        return str+1;
    }
    if (*str == '(') {
        str = parse_brackets(++str);
        obstack_grow(&exprs, R_BRACKET, sizeof(cmd));
        goto again;
    }

    cmd buf; char *ptr = buf;

    while (*str && !isspace(*str) && *str != ')' && *str != ',') *ptr++ = *str++; *ptr = '\0';

    for (str = parse_brackets(str); str && *str == ')'; str = parse_brackets(str));

    obstack_grow(&exprs, buf, sizeof(cmd));


    return str;
}

int isinfix(const char* str) {
    // return (int)hm_get(infixes, str);
    return str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/';
}

void swap(cmd c1, cmd c2) {
    cmd tmp; memcpy(tmp, c1, sizeof(cmd));
    memcpy(c1, c2, sizeof(cmd));
    memcpy(c2, tmp, sizeof(cmd));
}

void parse_infix(cmd* command) {
    for (int i = 0; i < exprs_size / sizeof(cmd); ++i) {
        if (isinfix(command[i]) && !isinfix(command[i+1])
        && (*command[i-1] != ')' || isinfix(command[i+2]))
        && !isinfix(command[i-1])
        && *command[i+1] != ')'
        ) {
            swap(command[i], command[i+1]);
            ++i;
        }
    }
}

void parse_priority(cmd* command) {

}

/*
 * func -> pop args -> put result => run func
 * otherwise -> put args          => put arg
 *
 * pop извлекает аргументы в правильном порядке!
 *
 * func {
 * val = pop()
 * if (type == cmd) -> return cmd(val, func())
 * else -> return val;
 * }
 *
 * END ";" -> очищает стэк, использованный командой (но pop тоже освобождает!),
 * но END очищает результат выполнения основной команды
 */

void* parse_file(FILE* file) {
    obstack_blank(&exprs, 0);
    char* buf = (char*)malloc(BUF_SIZE);
    size_t size = BUF_SIZE;
    size_t readed = 0;

    while ((readed = getline(&buf, &size, file)) != -1) {
        if (*buf && *buf != '\n' && *buf != '#') {
            //~ pre_parse(buf); // расставить скобки arg1 - команда-или нет
            parse_brackets(buf);
            obstack_grow(&exprs, END, sizeof(cmd));
        }
    }

    free(buf);

    exprs_size = obstack_object_size(&exprs);
    cmd* commands = obstack_finish(&exprs);

    //~ parse_priority(commands);
    parse_infix(commands);

    return commands;
}

int main() {
    obstack_init(&exprs);

    FILE* file = fopen("object.cript", "r");
    cmd* es = parse_file(file);
    fclose(file);

    for (int i = 0; i < exprs_size / sizeof(cmd); ++i) {
        printf("\e[7m%2.i\e[0m %s\n", i, es[i]);
    }

    return 0;
}
