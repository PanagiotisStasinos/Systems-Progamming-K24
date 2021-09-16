#include <stdio.h>
#include <unistd.h>

main ()
{

alarm(3);   /* Schedule to receive an alarm signal */

printf("Looping forever...\n");

while (1);
printf("This line should never be executed.\n");


}
