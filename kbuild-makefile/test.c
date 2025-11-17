#include <unistd.h>
#include <stdio.h>

void lib_a();
void lib_b();

//---------------------------------------------THE-END-------------------------------------------------------//

int main()
{
    lib_a();
    lib_b();
    return 0;
}