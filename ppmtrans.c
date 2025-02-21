/**************************************************************
 *
 *                     ppmtrans.c
 *
 *     Assignment: locality
 *     Authors: Alejandra Sabater (asabat01), Darius-Stefan Iavorschi (diavor01)
 *     Date:     2/2/25
 *
 *     summary
 *     This program applies transformations to a PPM image.
 *     Program uses pnm.h to read the image and turn it into an array (uarray2b
 *     or uarray2b) depending on if the traversal specified.
 *     It supports rotations (0, 90, 180, 270 degrees) and 
 *     flips (horizontally and vertically). The program also 
 *     allows different traversals of the images for processing 
 *     (row-major, column-major, and block-major). 
 *     It measures the execution time per pixel if a 
 *     timing file is specified.
 *     Program outputs newly transformed image in binary to STDOUT.
 *
 *     
 *
 **************************************************************/

 #include <stdio.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <stdbool.h>
 
 #include "assert.h"
 #include "a2methods.h"
 #include "a2plain.h"
 #include "a2blocked.h"
 #include "pnm.h"
 #include "cputiming.h"
 
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
         A2Methods_T methods;
 };
 
 /*****************************************************************
  *                       Transformations
  *****************************************************************/
 
 /********** horizontal_flip ********
  *
  *
  * Parameters:
   *      int col: the column coordinate of the pixel in the A2 array
  *      int row: the row coordinate of the pixel in the A2 array
  *      A2Methods_Object *ptr: a pointer to the source pixel in the new image
  *      void *cl: a pointer to the closure struct, containing the translated
  *      image and the A2 methods
 *
 * Return: 
 *      None
 *
 * Expects: 
 *      array must not be NULL
 *      
 *      
 * Notes:
 *      Will checked runtime error if array is not valid, is NULL
 *      
 ************************/
 static void horizontal_flip(int col, int row, A2 array, A2Methods_Object *ptr, 
                         void *cl)
 {
         assert(array != NULL);
 
         struct Closure *closure = cl;
         A2Methods_T new_array = closure->new_array;
         A2Methods_T methods = closure->methods;
 
         /* find new row and column for pixel */
         int new_row = row;
         int new_col = methods->width(array) - col - 1;
         
         assert(!(new_col < 0 || new_col >= methods->width(new_array) ||
         new_row < 0 || new_row >= methods->height(new_array)));
 
         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
         Pnm_rgb src = ptr;
 
         /*set pixel in new spot*/
         *dest = *src; 
 }
 
 /********** vertical_flip ********
  *
  * 
  *
  * Parameters:
   * int col: the column coordinate of the pixel in the A2 array
  * int row: the row coordinate of the pixel in the A2 array
  * A2Methods_Object *ptr: a pointer to the source pixel in the new image
  * void *cl: a pointer to the closure struct, containing the translated
  * image and the A2 methods
 *
 * Return: 
 *      None
 *
 * Expects: 
 *      array must not be NULL
 *      
 *      
 * Notes:
 *      Will checked runtime error if array2  is NULL
 ************************/
 static void vertical_flip(int col, int row, A2 array, A2Methods_Object *ptr, 
                         void *cl)
 {
         assert(array != NULL);
 
         struct Closure *closure = cl;
         A2Methods_T new_array = closure->new_array;
         A2Methods_T methods = closure->methods;
 
         /* find new row and column for pixel */
         int new_row = methods->height(array) - row - 1;
         int new_col = col;
         
         assert(!(new_col < 0 || new_col >= methods->width(new_array) ||
         new_row < 0 || new_row >= methods->height(new_array)));
 
         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
         Pnm_rgb src = ptr;
 
         /*set pixel in new spot*/
         *dest = *src; 
 }
 
 // static void transpose(int col, int row, A2 array, A2Methods_Object *ptr, 
 //                       void *cl)
 // {
 //         struct Closure *closure = cl;
 //         A2Methods_T new_array = closure->new_array;
 //         A2Methods_T methods = closure->methods;
 
 //         int new_row = col;
 //         int new_col = methods->width(array) - col - 1;
 
 //         // printf("new col: %d, width of new array %d, new row %d, height of new array %d\n", new_col, methods->width(new_array), new_row, methods->height(new_array));
         
 //         assert(!(new_col < 0 || new_col >= methods->width(new_array) ||
 //         new_row < 0 || new_row >= methods->height(new_array)));
 
 //         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
 //         Pnm_rgb src = ptr;
 
 //         *dest = *src;        
 // }
 
 /********** rotate_90 ********
  *
   * Translates a pixel from the original matrix the its corresponding place in 
  * the new image so as the new image is rotated 90 degrees.
 *
  * Parameters:
  * int col: the column coordinate of the pixel in the A2 array
  * int row: the row coordinate of the pixel in the A2 array
  * A2Methods_Object *ptr: a pointer to the source pixel in the new image
  * void *cl: a pointer to the closure struct, containing the translated
  * image and the A2 methods
 *
  * Return: 
  * None
 *
  * Expects: 
  * None
  *      
  * Notes:
  * Will check runtime error if the calculated new dimensions are out
  * of bounds.
  ************************/
 static void rotate_90(int col, int row, A2 array, A2Methods_Object *ptr, void *cl)
 {
         /* grabbing arrays from closure */
         struct Closure *closure = cl;
         A2Methods_T new_array = closure->new_array;
         A2Methods_T methods = closure->methods;
 
         int new_row = col;
         int new_col = methods->height(array) - row - 1;
         
         assert(!(new_col < 0 || new_col >= methods->width(new_array) ||
         new_row < 0 || new_row >= methods->height(new_array)));
 
         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
         Pnm_rgb src = ptr;
 
         *dest = *src;
 
 }
 
 /********** rotate_180 ********
  *
  * Translates a pixel from the original matrix the its corresponding place in 
  * the new image so as the new image is rotated 180 degrees.
  *
  * Parameters:
  * int col: the column coordinate of the pixel in the A2 array
   * int row: the row coordinate of the pixel in the A2 array
  * A2Methods_Object *ptr: a pointer to the source pixel in the new image
  * void *cl: a pointer to the closure struct, containing the translated
  * image and the A2 methods
 *
  * Return: 
  * None
 *
  * Expects: 
  * None
  * 
  * Notes:
  * Will check runtime error if the calculated new dimensions are out
  * of bounds.
  ************************/
 static void rotate_180(int col, int row, A2Methods_UArray2 array, A2Methods_Object *ptr, void *cl)
 {
         /* grabbing arrays from closure */
         struct Closure *closure = cl;
         A2Methods_T new_array = closure->new_array;
         A2Methods_T methods = closure->methods;
 
         int new_row = methods->height(array) - row - 1;
         int new_col = methods->width(array) - col - 1;
         
         assert(!(new_col < 0 
                 || new_col >= methods->width(new_array) 
                 ||new_row < 0 
                 || new_row >= methods->height(new_array)));
 
         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
         Pnm_rgb src = ptr;
 
         *dest = *src;
 }
 
 /********** rotate_270 ********
  *
   * Translates a pixel from the original matrix the its corresponding place in 
  * the new image so as the new image is rotated 270 degrees.
 *
  * Parameters:
  * int col: the column coordinate of the pixel in the A2 array
  * int row: the row coordinate of the pixel in the A2 array
  * A2Methods_Object *ptr: a pointer to the source pixel in the new image
  * void *cl: a pointer to the closure struct, containing the translated
  * image and the A2 methods
 *
  * Return: 
  * None
 *
  * Expects: 
  * None
  *      
  * Notes:
  * Will check runtime error if the calculated new dimensions are out
  * of bounds.
  ************************/
 static void rotate_270(int col, int row, A2Methods_UArray2 array, A2Methods_Object *ptr, void *cl)
 {
         /* grabbing arrays from closure */
         struct Closure *closure = cl;
         A2Methods_T new_array = closure->new_array;
         A2Methods_T methods = closure->methods;
 
         int new_row = methods->width(array) - col - 1;
         int new_col = row;
 
         assert(!(new_col < 0 || new_col >= methods->width(new_array) ||
         new_row < 0 || new_row >= methods->height(new_array)));
 
         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
         Pnm_rgb src = ptr;
 
         *dest = *src;     
 }

 static void rotate_0(int col, int row, A2Methods_UArray2 array, A2Methods_Object *ptr, void *cl)
 {
         /* grabbing arrays from closure */
         struct Closure *closure = cl;
         A2Methods_T new_array = closure->new_array;
         A2Methods_T methods = closure->methods;
 
         (void) array;

         int new_row = row;
         int new_col = col;
 
         assert(!(new_col < 0 || new_col >= methods->width(new_array) ||
         new_row < 0 || new_row >= methods->height(new_array)));
 
         Pnm_rgb dest = methods->at(new_array, new_col, new_row);
         Pnm_rgb src = ptr;
 
         *dest = *src;     
 }
 
 static void mem_cleanup(Pnm_ppm image, Pnm_ppm new_image, FILE *fp,
         CPUTime_T timer) 
 {
         CPUTime_Free(&timer);
         Pnm_ppmfree(&new_image);
         Pnm_ppmfree(&image);
         
         fclose(fp);
 }
 
 static void rotation_flip(char *flip_type, A2Methods_mapfun *map, int rotation, int width, int height, 
         Pnm_ppm image, Pnm_ppm new_image, struct Closure cl)
 {
        fprintf(stderr, "rotation: %d\n", rotation);

         if (flip_type == NULL) {
                 if (rotation == 90) {
                         /* map with the called for order, array is pixel array 
                         from the Pnm_ppm struct*/
                         map(image->pixels, rotate_90, &cl);
         
                         new_image->width = height;   /*update rotated image*/
                         new_image->height = width;
                 } else if (rotation == 180) { 
                         map(image->pixels, rotate_180, &cl);
         
                         new_image->width = width;  
                         new_image->height = height;
                 } else if (rotation == 270) {
                         map(image->pixels, rotate_270, &cl);
                         
                         new_image->width = height;  
                         new_image->height = width;
                 } else if (rotation == 0) {
                        map(image->pixels, rotate_0, &cl);
                         
                        new_image->width = width;  
                        new_image->height = height;
                 } 
         } else {
                 if (strcmp(flip_type, "horizontal") == 0) {
                         map(image->pixels, horizontal_flip, &cl);
 
                         new_image->width = width;  
                         new_image->height = height;
                 } else if (strcmp(flip_type, "vertical") == 0) {
                         map(image->pixels, vertical_flip, &cl);
 
                         new_image->width = width;  
                         new_image->height = height;
                 }
         }
 }
 
 /*****************************************************************
  *                     Other useful functions
  *****************************************************************/
 static void write_the_timing(const char *time_file_name, double time_used, int width, int height) {
         // Validate input parameters
         if (time_file_name == NULL) {
             fprintf(stderr, "Error: time_file_name is NULL\n");
             return;
         }
         if (width <= 0 || height <= 0) {
             fprintf(stderr, "Error: Invalid image dimensions (width: %d, height: %d)\n", width, height);
             return;
         }
     
         // Open the file for appending
         FILE *timings_file = fopen(time_file_name, "a");
         if (timings_file == NULL) {
             perror("Error opening file");
             return;
         }
     
         // Calculate time per pixel
         double time_per_pix = time_used / (width * height);
     
         // Write data to file
         fprintf(timings_file, "Time taken per pixel: %.0f nanoseconds\n", time_per_pix);
         fprintf(stderr, "Before closing the timings_file\n");
     
         // Close the file safely
         fclose(timings_file);
     }
 
 static void execution(FILE *fp, A2Methods_T methods, A2Methods_mapfun *map, int rotation, 
                 char *flip_type, CPUTime_T timer, char *time_file_name, double time_used)
 {
         Pnm_ppm image = Pnm_ppmread(fp, methods); 
         
         /* grab information about image */
         int width = image->width;
         int height = image->height;
         
         A2 transImage = NULL; /*instance of new image*/
         
         /* Create a new Pnm_ppm struct for the rotated image */
         Pnm_ppm new_image = malloc(sizeof(*new_image));
         assert(new_image != NULL);
 
         int blocksize = methods->blocksize(image->pixels);
 
         /* create new array */
         if (rotation == 90 || rotation == 270) {
                 transImage = methods->new_with_blocksize(height, width, 
                                         sizeof(struct Pnm_rgb), blocksize);
         } 
         else if (rotation == 180 || rotation == 0) {
                 transImage = methods->new_with_blocksize(width, height, 
                                         sizeof(struct Pnm_rgb), blocksize);
         }
 
         /* create instance of struct with our new array */
         struct Closure cl = {transImage, methods};
 
         CPUTime_Start(timer); /*start timer*/
 
         /* Since rotation is set default to 0, if -rotation is not given 
         but -flip is, rotation will still be 0 */
         fprintf(stderr, "Before flip\n");
 
         rotation_flip(flip_type, map, rotation, width, height, 
                 image, new_image, cl);
 
         fprintf(stderr, "After flip\n");
 
         time_used = CPUTime_Stop(timer); /*stop timer*/
 
         fprintf(stderr, "Before write_the_timing");
 
         if (time_file_name != NULL) {
                 write_the_timing(time_file_name, time_used, width, height);
         }
         
         fprintf(stderr, "After write_the_timing");
         /*update remaining characteristics of rotated image*/
         new_image->denominator = image->denominator;
         new_image->pixels = transImage;
         new_image->methods = methods;
 
         /* Write the transformed image in binary format (P6) */
         Pnm_ppmwrite(stdout, new_image);
 
         fprintf(stderr, "Before cleanup\n");
 
         mem_cleanup(image, new_image, fp, timer);
 }
 
 
 /*****************************************************************
  *                        Running program
  *****************************************************************/
 int main(int argc, char *argv[])
 {
         char *time_file_name = NULL;
         int   rotation       = 0;
         int   i;
         char *flip_type = NULL; 

         int ok = 0;
         FILE *fp;
         /* Create a timer*/
         CPUTime_T timer = CPUTime_New();
         double time_used = 0; 
 
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
                        fprintf(stderr, "hey\n");
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
                 } else if (strcmp(argv[i], "-flip") == 0) {
                         if (!(i + 1 < argc)) {  /* No flip value provided */
                         usage(argv[0]);
                         }
                         
                         /*store if horizontal or vertical*/
                         if (strcmp(argv[i + 1], "horizontal") == 0) {
                                 flip_type = argv[i + 1];  
                                 fprintf(stderr, "481\n");
 
                         } else if (strcmp(argv[i + 1], "vertical") == 0) {
                                 flip_type = argv[i + 1];  
                         } else {
                                 fprintf(stderr, "Flip must be 'horizontal' or" 
                                         "'vertical'\n");
                                 usage(argv[0]);
                         }
                         i++; 
                 } else if (strcmp(argv[i], "-time") == 0) {
                         if (!(i + 1 < argc)) {      /* no time file */
                                 usage(argv[0]);
                         }
                         time_file_name = argv[++i];
                         fprintf(stderr, "496\n");
                 } else if (*argv[i] == '-') {
                         fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                 argv[i]);
                         usage(argv[0]);
                 } else if (argc - i > 1) {
                         fprintf(stderr, "Too many arguments\n");
                         usage(argv[0]);
                 } else {
                        fprintf(stderr, argv[i]);
                        fp = fopen(argv[i], "rb");
                        ok = 1;
                 }
         }
 
        if (ok == 0) {
                fp = stdin;
        }        

        //  if (argc == 1) { /* nothing provided */
        //          fp = stdin;
        //  } else 
         if (rotation != 0  && flip_type != NULL) { /* can't rotate and 
                                                         flip */
                 fprintf(stderr, "Too many arguments\n"); 
                 return EXIT_FAILURE;
         }
        //  } else {
        //          fp = fopen(argv[argc - 1], "rb"); /*open file*/ 
        //          if (fp == NULL) {
        //                  fprintf(stderr, "cannot open file\n"); 
        //                  return EXIT_FAILURE;
        //          }
 
        //  }
 
         execution(fp, methods, map, rotation, flip_type, timer, time_file_name, time_used);
         // if (rotation != 0) {
         //         execution(fp, methods, map, rotation, NULL, timer, time_file_name, time_used);
         // } else {
         //         execution(fp, methods, map, 0, flip_type, timer, time_file_name, time_used);
         // }
         
             
 
         /* read through file fp using given methods and returns a pixmap 
         containing a 2D array of the type returned by 'methods->new'*/
         // Pnm_ppm image = Pnm_ppmread(fp, methods); 
         
         // /* grab information about image */
         // int width = image->width;
         // int height = image->height;
         
         // A2 transImage = NULL; /*instance of new image*/
         
         // /* Create a new Pnm_ppm struct for the rotated image */
         // Pnm_ppm new_image = malloc(sizeof(*new_image));
         // assert(new_image != NULL);
 
         // int blocksize = methods->blocksize(image->pixels);
 
         // /* create new array */
         // if (rotation == 90 || rotation == 270) {
         //         transImage = methods->new_with_blocksize(height, width, 
         //                                 sizeof(struct Pnm_rgb), blocksize);
         // } else if (rotation == 180 || rotation == 0 || 
         //         (flip_type != NULL && strcmp(flip_type, "horizontal") == 0) 
         //         || (flip_type != NULL && strcmp(flip_type, "horizontal") == 0)){
         //         transImage = methods->new_with_blocksize(width, height, 
         //                                 sizeof(struct Pnm_rgb), blocksize);
         // }
 
         // /* create instance of struct with our new array */
         // struct Closure cl = {transImage, methods};
 
         // CPUTime_Start(timer); /*start timer*/
 
         // /* Since rotation is set default to 0, if -rotation is not given 
         // but -flip is, rotation will still be 0 */
         // if (flip_type == NULL) {
         //         if (rotation == 90) {
         //                 /* map with the called for order, array is pixel array 
         //                 from the Pnm_ppm struct*/
         //                 map(image->pixels, rotate_90, &cl);
         
         //                 new_image->width = height;   /*update rotated image*/
         //                 new_image->height = width;
         //         } else if (rotation == 180) { 
         //                 map(image->pixels, rotate_180, &cl);
         
         //                 new_image->width = width;  
         //                 new_image->height = height;
         //         } else if (rotation == 270) {
         //                 map(image->pixels, rotate_270, &cl);
                         
         //                 new_image->width = height;  
         //                 new_image->height = width;
         //         } else if (rotation == 0) {
         //                 time_used = CPUTime_Stop(timer);
         //                 Pnm_ppmwrite(stdout, image);
         //                 Pnm_ppmfree(&image);
         //                 return EXIT_SUCCESS;
         //         } 
         // } else {
         //         if (strcmp(flip_type, "horizontal") == 0) {
         //                 map(image->pixels, horizontal_flip, &cl);
 
         //                 new_image->width = width;  
         //                 new_image->height = height;
         //         } else if (strcmp(flip_type, "vertical") == 0) {
         //                 map(image->pixels, vertical_flip, &cl);
 
         //                 new_image->width = width;  
         //                 new_image->height = height;
         //         }
         // }
 
         
         // time_used = CPUTime_Stop(timer); /*stop timer*/
 
         // FILE *timings_file = fopen(time_file_name, "a"); /*open time file*/
         // double time_per_pix = time_used / (width * height); /*calc. per pix*/
 
         // fprintf(timings_file, "Time taken per pixel %.0f nanoseconds\n", 
         //         time_per_pix); /*output time data*/
 
         // /* update remaining characteristics of rotated image */
         // new_image->denominator = image->denominator;
         // new_image->pixels = transImage;
         // new_image->methods = methods;
 
         // /* Write the transformed image in binary format (P6) */
         // Pnm_ppmwrite(stdout, new_image);
 
         // /* Free memory */
         // CPUTime_Free(&timer);
         // Pnm_ppmfree(&new_image);
         // Pnm_ppmfree(&image);
         // fclose(timings_file);
         // fclose(fp);
 
 
         return EXIT_SUCCESS;
 }
 
 
