// lab1 支持时钟，中断，输出
#include <u.h>

int current;

void out(port, val) {
    asm(LL,8);
    asm(LBL,16);
    asm(BOUT);
}

void ivec(void *isr) {
    asm(LL,8);
    asm(IVEC);
}

void timeout(int val) {
    asm(LL,8);
    asm(TIME);
}

void halt(val) {
    asm(LL,8);
    asm(HALT);
}

void write(int f, char *s, int n)
{
    while (n--)
        out(f, *s++);
}

void timetraps() {
    asm(PSHA);
    asm(PSHB);

    write(1, "Hello world ", 12);
    out(1, current+'0');
    out(1, '\n');
    current++;

    asm(POPB);
    asm(POPA);
    asm(RTI);
}

void main() {
    current = 0;
    asm(CLI);
    timeout(1000);
    ivec(timetraps);
    asm(STI);

    while (current < 10) { }

    halt(0);
}
