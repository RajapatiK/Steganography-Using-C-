#ifndef DECODE_H
#define DECODE_H
#include "types.h"  // Contains user defined types 


/* 
 * Structure to store the information required for
 * decoding secret message to the file
 * decoding the magic string and length of the files
 */


typedef struct _DecodeInfo
{
    /* Source Image Info */
    char *src_image_fname;
    FILE *fptr_src_image;
    /* Secret File Info */
    char  secret_fname[200];
    FILE *fptr_secret_file;
    long secret_data;
    char *extn_secret_file;
    int  secret_file_extn_size;
    int  file_size;
} DecodeInfo;

/*Decoding function prototypes*/

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decodeinfo);

/* Get File pointers for i/p and o/p files */
Status file_open(DecodeInfo *decodeinfo);

/* Perform the decoding */ 
Status do_decoding(DecodeInfo *decodeinfo);

/* Decoding the magic String*/
Status decode_magic_string(FILE *fptr_src_image);

/* Logic to decode the character byte form lsb */
Status decode_byte_from_lsb(char *arr,char *buffer);

/* Decoding the secret file extennsion size */
Status decode_secret_file_extn_size(int size, DecodeInfo *decodeinfo);

/* Logic to Decode the size byte from lsb */
Status decode_size_from_lsb(char *arr, int *size);

/* Decode the secret file extesnion size */
Status decode_secret_file_extn(FILE *fptr_src_image,char*extn_secret_file,DecodeInfo *decodeinfo);

/* Decoding the secret file size */
Status decode_secret_file_size(FILE *fptr_src_image, DecodeInfo *decodeinfo);

/*Decoding the secret file data */
Status decode_secret_data(FILE *fptr_src_image, DecodeInfo *decodeinfo);

#endif
