#include <string.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/********** new ********
 *
 * A abstraction level for the creation of UArray2. The function will be
 * passed to the A2Methods_T pointer table.
 *
 * Parameters:
 *      int width: the width of the newly created UArray2.
 *      int height: the height of the newly created UArray2.
 *      int size: the size in bytes of the UArray2 elements.
 *
 * Return:
 *      A2Methods_UArray2.
 *
 * Expects:
 *      None.
 *
 * Notes:
 *      None.
 ************************/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/********** new_with_blocksize ********
 *
 * An abstraction level for the creation of UArray2 with an additional blocksize
 * parameter. Since UArray2 does not support blocks, the blocksize is ignored.
 *
 * Parameters:
 *      int width: the width of the newly created UArray2.
 *      int height: the height of the newly created UArray2.
 *      int size: the size in bytes of the UArray2 elements.
 *      int blocksize: blocksize parameter (ignored in this implementation).
 *
 * Return:
 *      A2Methods_UArray2.
 *
 * Expects:
 *      None.
 *
 * Notes:
 *      The blocksize parameter is not used.
 ************************/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/********** a2free ********
 *
 * Frees the allocated UArray2.
 *
 * Parameters:
 *      A2Methods_UArray2 *array2p: pointer to the UArray2 to be freed.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      array2p must point to a valid UArray2.
 *
 * Notes:
 *      Casts the pointer to UArray2_T before freeing.
 ************************/
static void a2free(A2Methods_UArray2 * array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/********** width ********
 *
 * Returns the width of the UArray2.
 *
 * Parameters:
 *      A2Methods_UArray2 array2: the UArray2 instance.
 *
 * Return:
 *      The width of the array.
 *
 * Expects:
 *      array2 must be a valid UArray2.
 *
 * Notes:
 *      None.
 ************************/
static int width(A2Methods_UArray2 array2)
{
        return UArray2_width(array2);
}

/********** height ********
 *
 * Returns the height of the UArray2.
 *
 * Parameters:
 *      A2Methods_UArray2 array2: the UArray2 instance.
 *
 * Return:
 *      The height of the array.
 *
 * Expects:
 *      array2 must be a valid UArray2.
 *
 * Notes:
 *      None.
 ************************/
static int height(A2Methods_UArray2 array2)
{
        return UArray2_height(array2);
}

/********** size ********
 *
 * Returns the size (in bytes) of each element in the UArray2.
 *
 * Parameters:
 *      A2Methods_UArray2 array2: the UArray2 instance.
 *
 * Return:
 *      The size of each element.
 *
 * Expects:
 *      array2 must be a valid UArray2.
 *
 * Notes:
 *      None.
 ************************/
static int size(A2Methods_UArray2 array2)
{
        return UArray2_size(array2);
}

/********** blocksize ********
 *
 * Returns the block size of the UArray2.
 *
 * Parameters:
 *      A2Methods_UArray2 array2: the UArray2 instance.
 *
 * Return:
 *      Always returns 1.
 *
 * Expects:
 *      array2 must be a valid UArray2.
 *
 * Notes:
 *      UArray2 does not support block sizes; hence, a constant value of 1 is returned.
 ************************/
static int blocksize(A2Methods_UArray2 array2)
{
        (void) array2;
        return 1;
}

/********** at ********
 *
 * Returns a pointer to the element at the given indices.
 *
 * Parameters:
 *      A2Methods_UArray2 array2: the UArray2 instance.
 *      int i: column index.
 *      int j: row index.
 *
 * Return:
 *      Pointer to the element at (i, j).
 *
 * Expects:
 *      array2 must be a valid UArray2 and indices must be within bounds.
 *
 * Notes:
 *      None.
 ************************/
static A2Methods_Object *at(A2Methods_UArray2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

/********** map_row_major ********
 *
 * Applies the provided function to each element of the UArray2 in row-major order.
 *
 * Parameters:
 *      A2Methods_UArray2 uarray2: the UArray2 instance.
 *      A2Methods_applyfun apply: the function to be applied.
 *      void *cl: closure pointer for additional context.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      uarray2 must be a valid UArray2.
 *
 * Notes:
 *      The apply function is cast to UArray2_applyfun.
 ************************/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/********** map_col_major ********
 *
 * Applies the provided function to each element of the UArray2 in column-major order.
 *
 * Parameters:
 *      A2Methods_UArray2 uarray2: the UArray2 instance.
 *      A2Methods_applyfun apply: the function to be applied.
 *      void *cl: closure pointer for additional context.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      uarray2 must be a valid UArray2.
 *
 * Notes:
 *      The apply function is cast to UArray2_applyfun.
 ************************/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void *cl;
};

/********** apply_small ********
 *
 * Helper function to adapt a small apply function to the UArray2_applyfun signature.
 *
 * Parameters:
 *      int i: column index (unused).
 *      int j: row index (unused).
 *      UArray2_T uarray2: the UArray2 instance (unused).
 *      void *elem: pointer to the current element.
 *      void *vcl: pointer to a small_closure structure.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      vcl must be a valid pointer to a small_closure structure.
 *
 * Notes:
 *      The indices and uarray2 parameters are ignored as they are not required by the small apply function.
 ************************/
static void apply_small(int i, int j, UArray2_T uarray2,
        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/********** small_map_row_major ********
 *
 * Applies the provided small apply function to each element of the UArray2 in row-major order.
 *
 * Parameters:
 *      A2Methods_UArray2 a2: the UArray2 instance.
 *      A2Methods_smallapplyfun apply: the small apply function.
 *      void *cl: closure pointer for additional context.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      a2 must be a valid UArray2.
 *
 * Notes:
 *      Uses a small_closure to pass both the function and closure to the helper.
 ************************/
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/********** small_map_col_major ********
 *
 * Applies the provided small apply function to each element of the UArray2 in column-major order.
 *
 * Parameters:
 *      A2Methods_UArray2 a2: the UArray2 instance.
 *      A2Methods_smallapplyfun apply: the small apply function.
 *      void *cl: closure pointer for additional context.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      a2 must be a valid UArray2.
 *
 * Notes:
 *      Uses a small_closure to pass both the function and closure to the helper.
 ************************/
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/* Finally, the method table is defined and exported */
struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize, // Blocksize variant is provided though blocksize is ignored
        a2free,
        width,
        height,
        size,
        blocksize,           // blocksize
        at,
        map_row_major,       // map_row_major
        map_col_major,       // map_col_major
        NULL,                // map_block_major not implemented
        map_row_major,       // map_default
        small_map_row_major,            
        small_map_col_major,                   
        NULL,                // small_map_block_major not implemented
        small_map_row_major   // small_map_default
};

/* Exported pointer to the struct */
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
