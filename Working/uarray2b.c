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

#include "uarray.h"   
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
 * Creates and returns a new blocked 2D array.
 *
 * Parameters:
 *      int width:     Number of columns in array
 *      int height:    Number of rows in array
 *      int size:      Size (in bytes) of each element in array
 *      int blocksize: Size of each block (width of block squared)
 *
 * Return:
 *      new blocked 2D array with specified dimensions
 *
 * Expects:
 *      width, height, size, and blocksize must be positive
 *      blocksize must be greater than 0
 *
 * Notes:
 *      May throw checked runtime error if memory allocation fails
 ************************/
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize)
{
        assert(blocksize >= 1);
        assert(width >= 0 && height >= 0 && size > 0);
        
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

/********** UArray2b_free ********
 *
 * Free memory associated with blocked 2D array
 *
 * Parameters:
 *      UArray2b_T *array2b: Pointer to blocked array to free
 *
 * Return:
 *      None (void)
 *
 * Expects:
 *      array2b must not be NULL
 *
 * Notes:
 *      Will throw checked runtime error if array2b is NULL
 ************************/
void UArray2b_free (UArray2b_T *array2b)
{
        assert(array2b && *array2b);
        UArray_free(&((*array2b)->array));
        free(*array2b);
        array2b = NULL;
}

/********** UArray2b_new_64K_block ********
 *
 * Create blocked 2D array using block size that fits in 64KB
 *
 * Parameters:
 *      int width:  Number of columns in array
 *      int height: Number of rows in array
 *      int size:   Size (in bytes) of each element in array
 *
 * Return:
 *      A blocked 2D array optimized for cache locality
 *
 * Expects:
 *      width, height, and size must be positive
 *
 * Notes:
 *      May checked runtime error if memory allocation fails
 *      If the elemnt size is larger than the number of bytes in a 64 KB block,
 *      the blocksize defaults to 1.
 *      If the blocksize calculation formula results in 0, it is automatically 
 *      set to 1. 
 ************************/
 UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
 {
        assert(width >= 0 && height >= 0 && size > 0);

        const int BYTES_IN_64K = 1024 * 64;
        
        /*If one element is larger than 64KB, enforce block size of 1*/ 
        if (size > BYTES_IN_64K) {
                return UArray2b_new(width, height, size, 1);
        }
        
        int max_cells_per_block = BYTES_IN_64K / size;
        int blocksize = (int) sqrt(max_cells_per_block);
        
        /*Ensure blocksize is at least 1*/ 
        if (blocksize < 1) {
                blocksize = 1;
        }
        
        return UArray2b_new(width, height, size, blocksize);
 }

/********** UArray2b_width ********
 *
 * Returns width of blocked 2D array
 *
 * Parameters:
 *      UArray2b_T array2b: blocked array
 *
 * Return:
 *      The width (number of columns) of array
 *
 * Expects:
 *      array2b must not be NULL
 *
 * Notes:
 *      Will checked runtime error if array2b is NULL
 ************************/
int UArray2b_width (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/********** UArray2b_height ********
 *
 * Returns height of blocked 2D array
 *
 * Parameters:
 *      UArray2b_T array2b: 2D array
 *
 * Return: 
 *      The height of array
 *
 * Expects: 
 *      array2b must not be NULL
 *      
 * Notes:
 *      Will checked runtime error if array2b is NULL
 ************************/
int UArray2b_height (UArray2b_T array2b) 
{
        assert(array2b != NULL);
        return array2b->height;
}

/********** UArray2b_size ********
 *
 * Returns size of each element in blocked 2D array
 *
 * Parameters:
 *      UArray2b_T array2b: 2D array
 *
 * Return: 
 *      size of each element in bytes
 *
 * Expects: 
 *      array2b must not be NULL
 *      
 * Notes:
 *      Will checked runtime error if array2b is NULL
 ************************/
int UArray2b_size (UArray2b_T array2b) //???
{
        assert(array2b != NULL);
        return array2b->size;
}

/********** UArray2b_blocksize ********
 *
 * Returns block size of blocked 2D array
 *
 * Parameters:
 *      UArray2b_T array2b:  2D array
 *
 * Return: 
 *      block size (dimension of square blocks)
 *
 * Expects: 
 *      array2b must not be NULL
 *      
 * Notes:
 *      Will checked runtime error if array2b is NULL
 ************************/
int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/********** UArray2b_at ********
 *
 * Returns a pointer to element at given column and row in blocked 
 * array
 *
 * Parameters:
 *      UArray2b_T array2b: 2D array
 *      int column:         column index
 *      int row:            row index
 *
 * Return: 
 *      A pointer to requested element
 *
 * Expects: 
 *      array2b must not be NULL
 *      column must be in range [0, width)
 *      row must be in range [0, height)
 *      
 * Notes:
 *      Will checked runtime error if array2b is NULL
 *      Will checked runtime error if column or row is out of bounds
 ************************/
void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert((column >= 0) && (row >= 0));
        assert((column < array2b->width) && (row < array2b->height));

        int blocksize = array2b->blocksize;
        int blocks_per_row = (array2b->width + blocksize - 1) / blocksize;

        int block_row = row / blocksize;
        int block_col = column / blocksize;
        int block_index = block_row * blocks_per_row + block_col;

        int in_block_row = row % blocksize;
        int in_block_col = column % blocksize;

        int local_index = in_block_row * blocksize + in_block_col;
        int final_index = block_index * (blocksize * blocksize) + local_index;

        return UArray_at(array2b->array, final_index);
}

/********** UArray2b_map ********
 *
 * Applies a function to each element of blocked 2D array, iterating through 
 * every pixel in one block before moving onto the next one
 *
 * Parameters:
 *      UArray2b_T array2b: 2D array
 *      void apply(int col, int row, UArray2b_T array2b, void *elem, void *cl):
 *           Function to apply to each element
 *      void *cl: Closure pointer to additional data
 *
 * Return: 
 *      None
 *
 * Expects: 
 *      array2b must not be NULL
 *      apply must not be NULL
 *      
 * Notes:
 *      Will checked runtime error if array2b or apply is NULL
 *      Iterates through blocks and processes elements within each block
 ************************/
void UArray2b_map(UArray2b_T array2b, void apply(int col, int row, 
                  UArray2b_T array2b, void *elem, void *cl), void *cl)
{
        assert(array2b != NULL);
        assert(apply != NULL);

        int blocksize = array2b->blocksize;

        int num_blocks_wide = (array2b->width + blocksize - 1) / blocksize;
        int num_blocks_high = (array2b->height + blocksize - 1) / blocksize;

        /*Iterate through blocks*/
        for (int block_row = 0; block_row < num_blocks_high; block_row++) {
                for (int block_col = 0; block_col < num_blocks_wide;
                     block_col++) {
                    /*Iterate within the block*/
                        for (int in_block_row = 0; in_block_row < blocksize; 
                             in_block_row++) {
                                for (int in_block_col = 0; 
                                    in_block_col < blocksize; in_block_col++) {
        
                                        /* Compute big picture position */
                                        int row = block_row * blocksize 
                                                  + in_block_row;
                                        int col = block_col * blocksize 
                                                  + in_block_col;
                                        
                                      /* make sure we're in bounds of picture*/
                                        if (col < array2b->width && 
                                                row < array2b->height) {
                                                void *value =  
                                                UArray2b_at(array2b, col, row);
                                                apply(col, row, array2b, value,
                                                                           cl);
                                        }
                                }
                        }
                }
        }

}


