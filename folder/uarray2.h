/*
 * uarray2.h
 * Darius-Stefan Iavorschi, Evren Uluer
 * 1/28/25
 * 
 * Interface for the UArray2_T data abstraction: a 2-dimensional unboxed array.
 * 
 * This module provides functions to:
 * - Create and free a UArray2_T instance.
 * - Access elements and their memory addresses.
 * - Retrieve the dimensions (width, height) and element size of the array.
 * - Apply a function to all elements in either row-major or column-major 
 *  order.
 * 
 * Row-major order traverses elements left-to-right, top-to-bottom.
 * Column-major order traverses elements top-to-bottom, left-to-right.
 */

 #ifndef UARRAY2_INCLUDED
 #define UARRAY2_INCLUDED
 
 #include "uarray.h"
 
 #define UArray2_T UArray2
 typedef struct UArray2_T *UArray2_T;
 
 extern UArray2_T UArray2_new(int width, int height, int size);
 
 extern void UArray2_free(UArray2_T *matrix);
 
 extern void *UArray2_at(UArray2_T matrix, int x, int y);
 
 extern int UArray2_height(UArray2_T matrix);
 
 extern int UArray2_width(UArray2_T matrix);
 
 extern int UArray2_size(UArray2_T matrix);
 
 extern void UArray2_map_row_major(UArray2_T matrix,
                                   void apply(int col, int row,
                                              UArray2_T matrix,
                                              void *elem, void *cl),
                                   void *cl);
 
 extern void UArray2_map_col_major(UArray2_T matrix,
                                   void apply(int col, int row,
                                              UArray2_T matrix,
                                              void *elem, void *cl),
                                   void *cl);
 
 #endif