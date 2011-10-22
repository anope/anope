#include <sys/eventfd.h>

int main()
{
	int i = eventfd(0, EFD_NONBLOCK);
	return i >= 0 ? 1 : 0;
}

