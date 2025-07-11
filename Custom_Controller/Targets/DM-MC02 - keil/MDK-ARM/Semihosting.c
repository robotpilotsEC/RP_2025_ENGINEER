__asm(".global __ARM_use_no_argv\n\t");
__asm(".global __use_no_semihosting");

void _sys_exit(int ret)
{
    (void)ret;
    while(1) {}
}

void _ttywrch(int ch)
{
    (void)ch;
}

#include <stdio.h>

int fputc(int ch, FILE *f)
{
    (void) f;
    (void) ch;
    
    return ch;
}

#include <rt_sys.h>

FILEHANDLE $Sub$$_sys_open(const char *name, int openmode)
{
    (void) name;
    (void) openmode;
    return 0;
}