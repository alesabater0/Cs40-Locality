#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

// static A2Methods_T methods;
typedef A2Methods_UArray2 A2;

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] "
		        "[-time time_file] "
		        "[filename]\n",
                        progname);
        exit(1);
}

/* struct so we can pass the arrays and methods into the apply function */
struct Closure { 
        A2Methods_T new_array;
        // A2Methods_T orig_array;
        A2Methods_T methods;
};

/*****************************************************************
*                       Transformations
*****************************************************************/

// static void horizontal_rotation(A2Methods_T array, A2Methods_T new_array, int row, int col)
// {
//         int width = methods->width(array);
//         (int *)methods->at(new_array, row, width - col - 1) = (int *)methods->at(array, row, col);
// }

// static void vertical_rotation(A2Methods_T array, A2Methods_T new_array, int row, int col)
// {
//         int heigth = methods->height(array);
//         (int *)methods->at(new_array, height - row - 1, col) = (int *)methods->at(array, row, col);
// }

// static void transpose(A2Methods_T array, A2Methods_T new_array, int row, int col)
// {
//         (int *)methods->at(new_array, col, row) = (int *)methods->at(array, row, col);        
// }

// static void rotate_90(A2Methods_T array, A2Methods_T new_array, int row, int col)
// {
//         int height = methods->height(array);
//         (int *)methods->at(new_array, col, height - row - 1) = (int *)methods->at(array, row, col);
// }

static void rotate_90(int col, int row, A2 array, A2Methods_Object *ptr, void *cl)
{
        (void) array; /* Unused */ 

        /* grabbing arrays from closure */
        struct Closure *closure = cl;
        A2Methods_T new_array = closure->new_array;
        A2Methods_T methods = closure->methods;

        int new_row = col;
        int new_col = methods->height(array) - row - 1;

        printf("old col: %d old row: %d to transformed col: %d, transformed row: %d \n", col, row, new_col, new_row);

        Pnm_rgb dest = methods->at(new_array, new_col, new_row);
        Pnm_rgb src = ptr;

        *dest = *src;
}

// static void rotate_180(A2Methods_T array, A2Methods_T new_array, int row, int col)
// {
//         int heigth = methods->height(array);
//         int width = methods->width(array);
//         (int *)methods->at(new_array, height - row - 1, width - col - 1) = (int *)methods->at(array, row, col);
// }

// static void rotate_270(A2Methods_T array, A2Methods_T new_array, int row, int col)
// {
//         int width = methods->width(array);
//         (int *)methods->at(new_array, width - col - 1, row) = (int *)methods->at(array, row, col);        
// }

/*****************************************************************
*                        Running program
*****************************************************************/

int main(int argc, char *argv[])
{
        // char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        if (!(i + 1 < argc)) {      /* no time file */
                                usage(argv[0]);
                        }
                        // time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        
        FILE *fp;
        if (argc == 1) { /* nothing provided*/
                fp = stdin;
        } else {
                fp = fopen(argv[argc - 1], "rb"); /*open file*/ 
                assert(fp != NULL);
        }


        Pnm_ppm image = Pnm_ppmread(fp, methods); /* read through file fp using 
                                     given methods and returns a pixmap 
                                     containing a 2D array of the type returned
                                     by 'methods->new'*/
        int width = image->width;
        int height = image->height;

        A2Methods_T transImage = methods->new(width, height, sizeof(int));     
        struct Closure cl = {transImage, methods};                      
                                      
        if (rotation == 90) {
                /* map with the called for order, array is pixel array 
                   from the Pnm_ppm struct*/
                map(image->pixels, rotate_90, &cl);
        } else if (rotation == 180) {

        } else if (rotation == 270) {

        } else if (rotation == 0) {
                
        } else if (rotation == 270) {
                
        }
        
        Pnm_ppmfree(&image);
        assert(false);    // the rest of this function is not yet implemented
}

