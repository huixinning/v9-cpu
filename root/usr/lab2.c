// lab2 支持建立页表

#include <u.h>

enum { // page table entry flags
    PTE_P = 0x001,       // Present
    PTE_W = 0x002,       // Writeable
    PTE_U = 0x004,       // User
    PTE_PWT = 0x008,       // Write-Through
    PTE_PCD = 0x010,       // Cache-Disable
    PTE_A = 0x020,       // Accessed
    PTE_D = 0x040,       // Dirty
    PTE_PS = 0x080,       // Page Size
    PTE_MBZ = 0x180,       // Bits must be zero
};

enum { // processor fault codes
    FMEM,   // bad physical address
    FTIMER, // timer interrupt
    FKEYBD, // keyboard interrupt
    FPRIV,  // privileged instruction
    FINST,  // illegal instruction
    FSYS,   // software trap
    FARITH, // arithmetic trap
    FIPAGE, // page fault on opcode fetch
    FWPAGE, // page fault on write
    FRPAGE, // page fault on read
    USER = 16 // user mode exception
};

char pg_mem[34 * 4096]; // page dir + 32 entries + alignment

int *pg_dir, *pg0;

int current;

int in(int port) { asm(LL,8); asm(BIN); }

void out(int port, int val) { asm(LL,8); asm(LBL,16); asm(BOUT); }

void ivec(void *isr) { asm(LL,8); asm(IVEC); }

void lvadr() { asm(LVAD); }

void stmr(int val) { asm(LL,8); asm(TIME); }

void pdir(int value) { asm(LL,8); asm(PDIR); }

void spage(int value) { asm(LL,8); asm(SPAG); }

void halt(int value) { asm(LL,8); asm(HALT); }

void *memcpy() { asm(LL,8); asm(LBL, 16); asm(LCL,24); asm(MCPY); asm(LL,8); }

void *memset() { asm(LL,8); asm(LBLB,16); asm(LCL,24); asm(MSET); asm(LL,8); }

void *memchr() { asm(LL,8); asm(LBLB,16); asm(LCL,24); asm(MCHR); }

void write(int fd, char *p, int n) {
    while (n--)
        out(fd, *p++);
}

int strlen(char *s) { return memchr(s, 0, -1) - (void *) s; }

enum {
    BUFSIZ = 32
};

int vsprintf(char *s, char *f, va_list v) {
    char *e = s, *p, c, fill, b[BUFSIZ];
    int i, left, fmax, fmin, sign;

    while (c = *f++) {
        if (c != '%') {
            *e++ = c;
            continue;
        }
        if (*f == '%') {
            *e++ = *f++;
            continue;
        }
        if (left = (*f == '-')) f++;
        fill = (*f == '0') ? *f++ : ' ';
        fmin = sign = 0;
        fmax = BUFSIZ;
        if (*f == '*') {
            fmin = va_arg(v, int);
            f++;
        } else while ('0' <= *f && *f <= '9') fmin = fmin * 10 + *f++ - '0';
        if (*f == '.') {
            if (*++f == '*') {
                fmax = va_arg(v, int);
                f++;
            } else for (fmax = 0; '0' <= *f && *f <= '9'; fmax = fmax * 10 + *f++ - '0');
        }
        if (*f == 'l') f++;
        switch (c = *f++) {
            case 0:
                *e++ = '%';
                *e = 0;
                return e - s;
            case 'c':
                fill = ' ';
                i = (*(p = b) = va_arg(v, int)) ? 1 : 0;
                break;
            case 's':
                fill = ' ';
                if (!(p = va_arg(v, char *))) p = "(null)";
                if ((i = strlen(p)) > fmax) i = fmax;
                break;
            case 'u':
                i = va_arg(v, int);
                goto c1;
            case 'd':
                if ((i = va_arg(v, int)) < 0) {
                    sign = 1;
                    i = -i;
                }
            c1:
                p = b + BUFSIZ - 1;
                do { *--p = ((uint) i % 10) + '0'; } while (i = (uint) i / 10);
                i = (b + BUFSIZ - 1) - p;
                break;
            case 'o':
                i = va_arg(v, int);
                p = b + BUFSIZ - 1;
                do { *--p = (i & 7) + '0'; } while (i = (uint) i >> 3);
                i = (b + BUFSIZ - 1) - p;
                break;
            case 'p':
                fill = '0';
                fmin = 8;
                c = 'x';
            case 'x':
            case 'X':
                c -= 33;
                i = va_arg(v, int);
                p = b + BUFSIZ - 1;
                do { *--p = (i & 15) + ((i & 15) > 9 ? c : '0'); } while (i = (uint) i >> 4);
                i = (b + BUFSIZ - 1) - p;
                break;
            default:
                *e++ = c;
                continue;
        }
        fmin -= i + sign;
        if (sign && fill == '0') *e++ = '-';
        if (!left && fmin > 0) {
            memset(e, fill, fmin);
            e += fmin;
        }
        if (sign && fill == ' ') *e++ = '-';
        memcpy(e, p, i);
        e += i;
        if (left && fmin > 0) {
            memset(e, fill, fmin);
            e += fmin;
        }
    }
    *e = 0;
    return e - s;
}

int printf(char *f) {
    static char buf[4096];
    return write(1, buf, vsprintf(buf, f, &f));
} // XXX remove static from buf

trap(int c, int b, int a, int fc, int pc) {
    printf("TRAP: ");
    switch (fc) {
        case FINST:
            printf("FINST");
            break;
        case FRPAGE:
            printf("FRPAGE [0x%08x]", lvadr());
            break;
        case FWPAGE:
            printf("FWPAGE [0x%08x]", lvadr());
            break;
        case FIPAGE:
            printf("FIPAGE [0x%08x]", lvadr());
            break;
        case FSYS:
            printf("FSYS");
            break;
        case FARITH:
            printf("FARITH");
            break;
        case FMEM:
            printf("FMEM [0x%08x]", lvadr());
            break;
        case FTIMER:
            printf("FTIMER");
            current = 1;
            stmr(0);
            break;
        case FKEYBD:
            printf("FKEYBD [%c]", in(0));
            break;
        default:
            printf("other [%d]", fc);
            break;
    }
}

alltraps() {
    asm(PSHA);
    asm(PSHB);
    asm(PSHC);
    trap();
    asm(POPC);
    asm(POPB);
    asm(POPA);
    asm(RTI);
}

setup_paging() {
    int i;
    int *pgt;
    pg_dir = (int *) ((((int) &pg_mem) + 0xFFF) & 0XFFFFF000); //低12位地址对齐
    pg0 = pg_dir + 1024;
    for (i = 0; i < 32; ++i) {
        pgt = pg0 + 1024 * i;
        pg_dir[i] = (int) pgt | PTE_P | PTE_W | PTE_U;
    }

    for (i = 32; i < 1024; i++)
        pg_dir[i] = 0;

    for (i = 0; i < 1024 * 32; i++)
        pg0[i] = (i << 12) | PTE_P | PTE_W | PTE_U;  // trick to write all 4 contiguous pages

    pdir(pg_dir);
    spage(1);
}

main() {
    int t;
    asm(CLI);
    current = 0;
    ivec(alltraps);

    asm(STI);

    printf("test timer...");
    t = 0;
    stmr(10000);
    while (!current)
        t++;
    printf("(t=%d)...ok\n", t);

    printf("test invalid instruction...");
    asm(-1);
    printf("...ok\n");

    printf("test bad physical address...");
    t = *(int *) 0x20000000;
    printf("...ok\n");

    printf("test good physical address...");
    t = *(int *) 0x00000060;
    printf("...ok\n");

    printf("test divide by zero...");
    t = 10;
    t /= 0;
    printf("...ok\n");

    printf("test paging...");
    setup_paging();
    asm(LI, 7 * 1024 * 1024); // a = 7M 立即数最多只有24位
    asm(SSP); // ksp = a
    printf("identity map...ok\n");

    pg0[50] = 0;
    printf("test page fault read...");
    t = *(int *) (50 << 12);
    printf("...ok\n");

    printf("test page fault write...");
    *(int *) (50 << 12) = 5;
    printf("...ok\n");

    pg0[50] =  PTE_P | PTE_W | PTE_U;
    printf("test page right write...");
    *(int *) (50 << 12) = 89;
    printf("...ok\n");

    printf("test page right read...");
    t = *(int *) (50 << 12);
    if(t == 89)
        printf("...ok\n");
    else
        printf("...page read error\n");

    halt(0);
}
