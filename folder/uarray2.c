/*
 *     uarray2.c
 *     Darius-Stefan Iavorschi, Evren Uluer,
 *     1/28/25
 *     uarray2
 *
 *     The implementation file for a UArray2
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "uarray.h"
#include "uarray2.h"


#define ERR_ROW_OUT_OF_BOUNDS "Error: Row index is out of bounds " \
                            "(height = %d).\n"
#define ERR_COL_OUT_OF_BOUNDS "Error: Column index is out of bounds" \
                            "(width = %d).\n"
#define ERR_WIDTH "Error: Width must be positive (got %d).\n"
#define ERR_HEIGHT "Error: Height must be positive (got %d).\n"


/* The struct contains a one dimensional UArray as well as the dimensions
of the matrix*/
struct UArray2
{
    UArray_T UArray;
    int width;
    int height;
};


static void report_error_and_exit(const char *message, int val)
{
        fprintf(stderr, message, val);
        exit(EXIT_FAILURE);
}

/*
*  name:        return_index
*  purpose:     Returns the index for the UArra2 "matrix", which is defined as
                a one-dimensional array with its length equal to
                heigth * width. It us used as a helper function.
*  arguments:   The coordinates of the matrix element (row and col), as well as
                UArray2 width. Everything is passed as integers.
*  return type: Integer
*  effect:      Determines the index in the original one dimensional UArray
                where the element can be stored. The formula is:
                index_original_UArray = row * width + col
*  expects:     - The row argument must be smaller than the height of the 
                matrix. Otherwise it would be out of bounds.
                - All the arguments passed must be non-negative.
*/
int return_index(int col, int row, int width, int height)
{
        if (row >= height || row < 0) {
                report_error_and_exit(ERR_ROW_OUT_OF_BOUNDS, height);
        }

        if (col >= width || col < 0) {
                report_error_and_exit(ERR_COL_OUT_OF_BOUNDS, width);
        }

        return row * width + col;
}

/*
 *  name:        UArray2_new
 *  purpose:     Allocates and initializes a new 2D array with specified
 *               dimensions and element size
 *  arguments:   int width (columns), int height (rows),
 *               int size (bytes per element)
 *  return type: UArray2_T
 *  effect:      Allocates memory for the UArray2 and its underlying UArray for
 *               width*height elements. The caller is responsible for freeing 
 *               the memory using UArray2_free.
 *  expects:     - Non-negative width/height, positive size.
 *               - Returns NULL on allocation failure
 */
UArray2_T UArray2_new(int width, int height, int size)
{
        if (width <= 0) {
                report_error_and_exit(ERR_WIDTH, width);
        }

        if (height <= 0) {
                report_error_and_exit(ERR_HEIGHT, height);
        }

        UArray2_T matrix = malloc(sizeof(struct UArray2));
        if (matrix == NULL) {
                fprintf(stderr, 
                        "Error: Failed to allocate memory for UArray2.\n");
                exit(EXIT_FAILURE);
        }

        matrix->UArray = UArray_new(width * height, size);
        if (matrix->UArray == NULL) {
                free(matrix);
                fprintf(stderr, 
                        "Error: Failed to allocate memory for UArray.\n");
                exit(EXIT_FAILURE);
        }

        matrix->width = width;
        matrix->height = height;

        return matrix;
}

/*
 *  name:        UArray2_free
 *  purpose:     Deallocates a UArray2_T and its underlying storage
 *  arguments:   UArray2_T* (double pointer to matrix structure)
 *  return type: void
 *  effect:      - Frees both the container struct and contained UArray.
 *               - Nulls the pointer
 *  expects:     - Safe to call with NULL
 *               - Undefined behavior if matrix is already freed
 */
void UArray2_free(UArray2_T *matrix)
{
        if (matrix != NULL && *matrix != NULL) {
                UArray_free(&(*matrix)->UArray);
                free(*matrix);
                *matrix = NULL;
        }
}

/*
 *  name:        UArray2_at
 *  purpose:     Accesses element at specified column/row coordinates
 *  arguments:   UArray2_T matrix, int col, int row
 *  return type: void* (pointer to element)
 *  effect:      Computes flat index using row-major formula
 *  expects:     0 ≤ col < width, 0 ≤ row < height
 */
void *UArray2_at(UArray2_T matrix, int col, int row)
{
        return UArray_at(matrix->UArray,
                        return_index(col, row, matrix->width, matrix->height));
}

/*
 *  name:        UArray2_height
 *  purpose:     Returns number of rows in the 2D array
 *  arguments:   UArray2_T matrix
 *  return type: int
 *  effect:      Pure accessor function
 *  expects:     Valid initialized matrix
 */
int UArray2_height(UArray2_T matrix)
{
        return matrix->height;
}

/*
 *  name:        UArray2_width
 *  purpose:     Returns number of columns in the 2D array
 *  arguments:   UArray2_T matrix
 *  return type: int
 *  effect:      Pure accessor function
 *  expects:     Valid initialized matrix
 */
int UArray2_width(UArray2_T matrix)
{
        return matrix->width;
}

/*
 *  name:        UArray2_size
 *  purpose:     Returns size in bytes of each element
 *  arguments:   UArray2_T matrix
 *  return type: int
 *  effect:      Delegates to UArray_size on underlying storage
 *  expects:     Valid initialized matrix
 */
int UArray2_size(UArray2_T matrix)
{
        return UArray_size(matrix->UArray);
}

/*
*  name:        UArray2_map_row_major
*  purpose:     Applies function to elements in row-major order (left->right,
                top->bottom)
*  arguments:   UArray2_T matrix, apply function, void* closure
*  return type: void
*  effect:      Invokes apply(col, row, matrix, elem, cl) for each element
*  expects:     - apply non-NULL
                - Matrix structure must remain unchanged during mapping
*/
void UArray2_map_row_major(UArray2_T matrix,
    void apply(int col, int row,
               UArray2_T matrix, void *elem, void *cl),
    void *cl)
{
        int width = UArray2_width(matrix);
        int height = UArray2_height(matrix);

        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                        apply(j, i, matrix, UArray2_at(matrix, j, i), cl);
                }
        }
}

/*
*  name:        UArray2_map_col_major
*  purpose:     Applies function to elements in column-major order
                (top->bottom, left->right)
*  arguments:   UArray2_T matrix, apply function, void* closure
*  return type: void
*  effect:      Invokes apply(col, row, matrix, elem, cl) for each element
*  expects:     - apply non-NULL
                - Matrix structure must remain unchanged during mapping
*/
void UArray2_map_col_major(UArray2_T matrix,
    void apply(int col, int row,
               UArray2_T matrix,
               void *elem, void *cl),
    void *cl)
{
        int width = UArray2_width(matrix);
        int height = UArray2_height(matrix);

        for (int j = 0; j < width; j++) {
                for (int i = 0; i < height; i++) {
                        apply(j, i, matrix, UArray2_at(matrix, j, i), cl);
                }
        }
}
