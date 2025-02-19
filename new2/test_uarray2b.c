#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "uarray2b.h"

//apply function
void print_elements(int col, int row, UArray2b_T array2b, void *elem, void *cl) {
        (void)array2b; // Suppress unused variable warning
        (void)cl; 
        // (void)col; 
        // (void)row;      
        // (void)elem;
        printf("Element at (%d, %d) = %d\n", col, row, *(int *)elem);
}
      
void test_UArray2b() {
        int width = 5, height = 4, size = sizeof(int);
        // UArray2b_T array = UArray2b_new(width, height, size, blocksize);

        UArray2b_T array = UArray2b_new_64K_block(width, height, size);
        // printf("width %d height %d bs %d\n", UArray2b_width(array), UArray2b_height(array), UArray2b_blocksize(array));
        // Initialize elements
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                int *val = (int *)UArray2b_at(array, col, row);
                *val = row * col; // Fill with unique values
                }
        }

        // Apply function
        UArray2b_map(array, print_elements, NULL);

        UArray2b_free(&array);

}

int main() {
        test_UArray2b();
        return 0;
}