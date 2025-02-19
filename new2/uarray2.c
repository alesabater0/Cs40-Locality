/**************************************************************
 *
 *                     uarray2
 *
 *     Assignment: iii
 *     Authors:  Alejandra Sabater (asabat01)
 *     Date:     2/2/25
 *
 *     summary
 *     This file implements a 2D array (UArray2_T) using a 1D 
 *     UArray_T as the underlying data structure. It provides 
 *     functions for creation, access, and traversal of a 2D array
 *     
 *
 **************************************************************/
 #include <stdio.h>
 #include <stdlib.h>
 #include "uarray.h"
 #include "uarray2.h"
 #include <assert.h>
 #include "mem.h"
 
 #define T UArray2_T
 
 struct T { /* Struct to hold */ 
         int width;         /* Number of columns */
         int height;        /* Number of rows */ 
         int element_size;  /* Size of each element in bytes */ 
         UArray_T array;    /* Underlying 1D UArray storing the data */ 
 };
  
 /********** UArray2_new ********
   *
   * Creates and returns new UArray2_T struct representing 2D array
   * with given dimensions and element size
   *
   * Parameters:
   *      int dim1: Number of columns in 2D array
   *      int dim2: Number of rows in 2D array
   *      int element_size: Size of each element in bytes
   *
   * Return: new UArray2_T struct representing 2D array
   *
   * Expects:
   *      - dim1 and dim2 must be greater than 0
   *      - element_size must be greater than 0
   *      - Memory allocation for struct must succeed
   *
   * Notes:
   *      - underlying storage is a long 1D UArray_T array
   *      - Will throw checked runtime error if memory allocation fails
   ************************/
 UArray2_T UArray2_new(int dim1, int dim2, int element_size)
 {      
         /* Check for valid input */
         assert(dim1 > 0 && dim2 > 0 && element_size > 0);
 
         /* Allocate memory for the struct */
         T uarray2 = malloc(sizeof(*uarray2));  
 
         /* Ensure memory allocation succeeded */ 
         assert(uarray2 != NULL);    
 
         /* Fill struct */
         uarray2->width = dim1;
         uarray2->height = dim2;
         uarray2->element_size = element_size;
 
         int total_elements = dim1 * dim2;
 
         /* Create the underlying 1D array */
         uarray2->array = UArray_new(total_elements, element_size);  
 
         return uarray2;
 }
  
 /********** UArray2_free ********
  *
  * Deallocates memory associated with given UArray2_T struct
  * and sets pointer to NULL
  *
  * Parameters:
  *      UArray2_T *uarray2: Pointer to UArray2_T struct that needs to be freed
  *
  * Return: Nothing
  *
  * Expects:
  *      - uarray2 must not be NULL
  *      - *uarray2 must not be NULL
  *
  * Notes:
  *      - Frees both UArray_T array and UArray2_T struct itself
  *      - Will throw a checked runtime error if uarray2 or *uarray2 is NULL
  ************************/
 void UArray2_free(T *uarray2)
 {
         assert(uarray2 && *uarray2);
         UArray_free(&((*uarray2)->array));
         FREE(*uarray2);
 }
 
 /********** UArray2_width ********
  *
  * Returns number of columns in given UArray2_T struct
  *
  * Parameters:
  *      UArray2_T uarray2: UArray2_T struct whose width we need to get
  *
  * Return: number of columns in array
  *
  * Expects:
  *      - uarray2 must not be NULL
  *
  * Notes:
  *      - Will throw a checked runtime error if uarray2 is NULL
  ************************/
 int UArray2_width(T uarray2)
 {
         assert(uarray2 != NULL);
         return uarray2->width;
 }
 
 /********** UArray2_height ********
  *
  * Returns number of rows in given UArray2_T struct
  *
  * Parameters:
  *      UArray2_T uarray2: UArray2_T structure whose height we need to get.
  *
  * Return: number of rows in array
  *
  * Expects:
  *      - uarray2 must not be NULL
  *
  * Notes:
  *      - Will throw a checked runtime error if uarray2 is NULL
  ************************/
 int UArray2_height(T uarray2)
 {       
         assert(uarray2 != NULL);
         return uarray2->height;
 }
 
 /********** UArray2_size ********
  *
  * Returns size (in bytes) of each element in given UArray2_T struct
  *
  * Parameters:
  *      UArray2_T uarray2: UArray2_T struct whose element size we want
  *
  * Return: size of each element in bytes
  *
  * Expects:
  *      - uarray2 cannot be NULL
  *
  * Notes:
  *      - Will throw a checked runtime error if uarray2 is NULL
  ************************/
 int UArray2_size(T uarray2)
 {       
         assert(uarray2 != NULL);
         return uarray2->element_size;
 }
 
 /********** UArray2_at ********
  *
  * Returns pointer to element at given (col, row) in the 2d array
  *
  * Parameters:
  *      UArray2_T uarray2: UArray2_T structure whose element we want
  *      int col:  column index 
  *      int row:  row index 
  *
  * Return: pointer to element at (col, row)
  *
  * Expects:
  *      - uarray2 must not be NULL
  *      - col and row must be within array limits
  *
  * Notes:
  *      - Will throw checked runtime error if uarray2 is NULL or 
  *        if col/row are out of bounds
  ************************/
 void *UArray2_at(T uarray2, int col, int row) 
 {
         assert(uarray2 != NULL);
         assert((col >= 0) && (row >= 0));
         assert((col <= uarray2->width) && (row <= uarray2->height));
 
         int index = (row * uarray2->width) + col; /* calculate for long array */
 
         return UArray_at(uarray2->array, index); /* returns an address */
 }
 
 /********** UArray2_map_row_major ********
  *
  * Applies given function to each element of array in row-major order
  *
  * Parameters:
  *      UArray2_T uarray2: 2d array we want to iterate over
  *      void (*apply_function)(int col, int row, UArray2_T uarray2, void *elem,
  *                             void *cl):
  *          Function to apply to each element. Takes column, row, array, 
  *          element pointer, and a closure pointer
  *      void *cl: A closure pointer passed to the apply function
  *
  * Return: Nothing
  *
  * Expects:
  *      - uarray2 must not be NULL
  *      - apply_function must not be NULL
  *
  * Notes:
  *      - Iterates over array in row-major order
  *      - Will throw checked runtime error if uarray2 or apply_function is NULL
  ************************/
 void UArray2_map_row_major(T uarray2, void (*apply_function)(int col, int row, 
                         T uarray2, void *elem, void *cl), void *cl)
 {
         /* make sure array and apply_function correctly given and not NULL*/
         assert(uarray2 != NULL);
         assert(apply_function != NULL);
         
         /* iterate through array */
         for (int row = 0; row < uarray2->height; row++) {
                 for (int col = 0; col < uarray2->width; col++) {
                         int *value = (int *)UArray2_at(uarray2, col, row);
                         apply_function(col, row, uarray2, value, cl);
                 }
         }
 }
 
 /********** UArray2_map_col_major ********
  *
  * Applies given function to each element of UArray2_T struct in 
  * column-major order
  *
  * Parameters:
  *      UArray2_T uarray2: UArray2_T struct to iterate over
  *      void (*apply_function)(int col, int row, UArray2_T uarray2, void *elem, 
  *                       void *cl):
  *          Function to apply to elements. Takes column, row, array, element,
  *          and closure pointer.
  *      void *cl: closure pointer passed to apply function
  *
  * Return: nothing
  *
  * Expects:
  *      - uarray2 must not be NULL
  *      - apply_function must not be NULL
  *
  * Notes:
  *      - Iterates over array in column-major order
  *      - Will throw unchecked runtime error if uarray2 or apply_function is 
  *        NULL.
  ************************/
 void UArray2_map_col_major(T uarray2, void (*apply_function)(int col, int row, 
                         T uarray2, void *elem, void *cl), void *cl)
 {
         /* make sure array and apply_function correctly given and not NULL*/
         assert(uarray2 != NULL);
         assert(apply_function != NULL); 
 
         /* iterate through array */
         for (int col = 0; col < uarray2->width; col++) {
                 for (int row = 0; row < uarray2->height; row++) {
                         int *value = (int *)UArray2_at(uarray2, col, row);
                         apply_function(col, row, uarray2, value, cl);
                 }
         }
 }