#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
   int fd = open("/dev/MPlcd", O_RDWR);
   write( fd, "test", 5);
   return 0;
}
