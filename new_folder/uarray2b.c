/**************************************************************
 *
 *                     uarray2b.c
 *
 *     Assignment: locality
 *     Authors:Alejandra Sabater (asabat01), Darius-Stefan Iavorschi (diavor01)
 *     Date:     2/2/25
 *
 *     summary
 *     
 *     
 *
 **************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "uarray.h"    //THIS WASNT WORKING UNTIL I MADE A COPY OF IT IN MY DIRECTORY
#include "uarray2b.h"
#include <assert.h>

struct UArray2b_T {
        int width;
        int height;
        int blocksize;
        int size;
        UArray_T array;
};

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        UArray2b_T blocked_matrix = malloc(sizeof(*blocked_matrix));
        assert(blocked_matrix != NULL);

        int num_blocks_wide = (width + blocksize - 1) / blocksize;  
        int num_blocks_high = (height + blocksize - 1) / blocksize; 
        int total_blocks = num_blocks_wide * num_blocks_high;      

        int length = total_blocks * (blocksize * blocksize);

        blocked_matrix->array = UArray_new(length, size);
        blocked_matrix->blocksize = blocksize;
        blocked_matrix->width = width;
        blocked_matrix->height = height;
        blocked_matrix->size = size;

        return blocked_matrix;
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
void UArray2b_free (UArray2b_T *array2b)
{
        assert(array2b && *array2b);
        UArray_free(&((*array2b)->array));
        free(*array2b);
        array2b = NULL;
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        int BYTES_IN_64k = 1024 * 64;
        int blocksize = sqrt(BYTES_IN_64k / size);

        return UArray2b_new(width, height, size, blocksize);
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
int UArray2b_width (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
int UArray2b_height (UArray2b_T array2b) 
{
        assert(array2b != NULL);
        return array2b->height;
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
int UArray2b_size (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->size;
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column < array2b->width);
        assert(row < array2b->height);

        int blocksize = array2b->blocksize; 
        int blocks_per_row = array2b->width / blocksize;

        int block_row = row / blocksize;
        int block_col = column / blocksize;
        int block_index = (block_row * blocks_per_row) + block_col;

        int in_block_row = row % blocksize;
        int in_block_col = column % blocksize;
        int local_index = in_block_row * blocksize + in_block_col;

        int final_index = block_index * (blocksize * blocksize) + local_index;

        return UArray_at(array2b->array, final_index);
}

/********** UArray2b_new ********
 *
 * 
 *
 * Parameters:
 *   
 *
 * Return: 
 *
 * Expects: 
 *   
 *      
 * Notes:
 * 
 ************************/
void UArray2b_map(UArray2b_T array2b, void apply(int col, int row, UArray2b_T array2b, 
                  void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
        assert(apply != NULL);

        int blocksize = array2b->blocksize;

        int num_blocks_wide = (array2b->width + blocksize - 1) / blocksize;
        int num_blocks_high = (array2b->height + blocksize - 1) / blocksize;

        /*Iterate through blocks*/
        for (int block_row = 0; block_row < num_blocks_high; block_row++) {
                for (int block_col = 0; block_col < num_blocks_wide; block_col++) {
                        // printf("block_row %d\n", block_row);
                        // printf("block_col %d\n", block_col);
                    /*Iterate within the block*/
                        for (int in_block_row = 0; in_block_row < blocksize; in_block_row++) {
                                for (int in_block_col = 0; in_block_col < blocksize; in_block_col++) {
        
                                        /* Compute big picture position */
                                        int row = block_row * blocksize + in_block_row;
                                        int col = block_col * blocksize + in_block_col;
                        
                                        /* make sure we're in bounds of picture*/
                                        if (col < array2b->width && row < array2b->height) {
                                                void *value = UArray2b_at(array2b, col, row);
                                                apply(col, row, array2b, value, cl);
                                        }
                                }
                        }
                }
        }

}


