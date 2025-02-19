
#include "a2methods.h"
#include "a2blocked.h"

int main()
{
    A2 bl_array = new_with_blocksize(4, 6, sizeof(int), 3);
    int *val = (int *)A2Methods_Object *at(bl_array, 2, 2  );
    *val = 6;
    printf("%d: \n", (int *)A2Methods_Object *at(bl_array, 2, 2));
}