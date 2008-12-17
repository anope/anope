#include <stdlib.h>
#include <stdarg.h>

void foo(int i, ...)
{
	va_list ap1, ap2;
	va_start(ap1, i);
	ap2 = ap1;
	if (va_arg(ap2, int) != 123 || va_arg(ap1, int) != 123) exit(1);
	va_end(ap1);
	va_end(ap2);
}

int main()
{
	foo(0, 123);
	return 0;
}

