/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

typedef struct {
    double x;
    double y;
} Mousepos;

extern const unsigned SCR_WIDTH;
extern const unsigned SCR_HEIGHT;

void     main_term(int status, const char* fmt, ...);
void     main_quit(void);
Mousepos main_getmousepos(void);
void     main_setmousepos(Mousepos mousepos);
