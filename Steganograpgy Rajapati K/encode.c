#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    /*argv[2]=>.bmp
      true=>store argv[2] in src_image_fname
      failure=>return e_failure*/
    if(argv[2]==NULL)
    {
	printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
    }
    else if(strstr(argv[2],".bmp"))
    {
	encInfo->src_image_fname=argv[2];
    }
    else
    {
	printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
	return e_failure;
    }
    /*argv[3]=>.txt or .c or .sh
      true=>argv[3] in secret_fname
      false=>return e_failure*/
    if(argv[3]==NULL)
    {
	printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
	return e_failure;
    }	
    else if(strstr(argv[3],".txt"))
    {
	encInfo->secret_fname=argv[3];
	strcpy(encInfo->extn_secret_file,".txt");
    }
    else if(strstr(argv[3],".sh"))
    {
	encInfo->secret_fname=argv[3];
	strcpy(encInfo->extn_secret_file,".sh");

    }
    else if(strstr(argv[3],".c"))
    {
	encInfo->secret_fname=argv[3];
	strcpy(encInfo->extn_secret_file,".c");
    }
    else
    {
	printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file>[output file]\n");
	return e_failure;
    }
    /*check argv[4] is passed or not
      passed=>argv[4]=>.tmp
      true=>store argv[4] in stego_image_fname
      false=>return e_failure
      not passed=> stego_image_fname="default.bmp"*/
    if((argv[4]!=NULL))
    {
	if((strstr(argv[4],".bmp")))
	{
	    encInfo->stego_image_fname=argv[4];
	}
	else
	{
	    printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt> output.bmp\n");
	    return e_failure;
	}
    }
    else
    {
	printf("INFO: Output File not mentioned. Creating stego_bmp_image as default\n");
	encInfo->stego_image_fname="default.bmp";
    }

    return e_success;
}
/* Encoding process starts */
/* Encoding function calls */
Status do_encoding(char *argv[],EncodeInfo *encInfo)
{
    if((open_files(encInfo)==e_success))
    {
	printf("INFO: ## Encoding Procedure Started ##\n");
	printf("INFO: Checking for %s size\nINFO: Done. Not Empty\n",encInfo->secret_fname);
	if((check_capacity(encInfo)==e_success))
	{
	    printf("INFO: Checking for %s Capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
	    printf("INFO: Done. Found OK\n");
	    if((copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image))==e_success)
	    {
		printf("INFO: Done\n");
		if((encode_magic_string(MAGIC_STRING,encInfo)==e_success))
		{
		    printf("INFO: Done\n");
		    int len=0;
		    if(strstr(argv[3],".txt")){len=4;}else if(strstr(argv[3],".sh")){len=3;}else if(strstr(argv[3],".c")){len=2;}
		    if((encode_secret_file_extn_size((len*8),encInfo)==e_success))
		    {
			printf("INFO: Done\n");
			if((encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success))
			{
			    printf("INFO: Done\n");
			    if((encode_secret_file_size((encInfo->size_secret_file),encInfo)==e_success))
			    {
				printf("INFO: Done\n");
				if((encode_secret_file_data(encInfo)==e_success))
				{
				    printf("INFO: Done\n");
				    if((copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success))
				    {
					printf("INFO: Done\nINFO: ## Encoding Done Successfully ##\n");
				    }
				    else
				    {
					printf("Copying remaining data is failed\n");
				    }
				}
				else
				{
				    printf("Secret file encoding is failed\n");
				}
			    }
			    else
			    {
				printf("Secret file extension encoding size is failed\n");
			    }
			}
			else
			{
			    printf("Secret file extension is failed\n");
			}
		    }
		    else
		    {
			printf("secret file extension size if failed\n");
		    }
		}
		else
		{
		    printf("Magic string is failed\n");
		}
	    }
	    else
	    {
		printf("Copy header is failed\n");
	    }
	}
	else
	{
	    printf("Capacity is not in the limit\n");
	}
    }
    else
    {
	printf("Error in opening the files\n");
    }
}
/*checking the image capacity and secret file size */
Status check_capacity(EncodeInfo *encInfo)
{
    /* Checking the capacity of the source file which is .bmp and the secret file size 
      to check whether the secret file can be encoded within the .bmp file */
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    int len1=encInfo->size_secret_file;
    int len2=strlen(MAGIC_STRING);
    /* If the file size is lesser than image capacity return e_succes else return e_failure */
    if(encInfo->image_capacity>((54)+(len2*8)+(4*8)+(4*8)+(4*8)+(len1*8)))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }

}
/*getting secret file size to check the capacity 
  whether the file can be embedded in the image*/
uint get_file_size(FILE *fptr)
{
    if(fptr==NULL)
    {
	printf("Error unable to open file\n");
    }
    else
    {
	fseek(fptr,0,SEEK_END);
	return ftell(fptr);
    }
}

/* Initial 54 bytes is for header files so that 
  we are copying the header files from source to destination */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    rewind(fptr_src_image);
    char buffer[54];
    fread(buffer, 54, 1,fptr_src_image);
    fwrite(buffer,54, 1,fptr_dest_image);
    return e_success;
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //  printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    printf("INFO: Opened %s\n",encInfo->src_image_fname);
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    // Secret file
    printf("INFO: Opened %s\n",encInfo->secret_fname);
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    printf("INFO: Opened %s\n",encInfo->stego_image_fname);
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
    printf("INFO: Done\n");
}
/* encoding the magic string to each bytes of lsb in 
 * the output file from the input file which is beautiful.bmp
 * The magic string acts as a key to check whether the secret data
 * is encoded in the image file */

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    int len=strlen(MAGIC_STRING);
    char arr[8];
    for(int i=0;i<len;i++)
    {
	fread(arr,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(magic_string[i],arr);
	fwrite(arr,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* encoding the secret file extension  
 * file extensions maybe of .txt or .sh etc so that
 * we are storing the extension in the output image file according
 * to the user input */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extension\n",encInfo->secret_fname);
    int len=strlen(file_extn);
    char arr[8];
    for(int i=0;i<len;i++)
    {
	fread(arr,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(file_extn[i],arr);
	fwrite(arr,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
/* encoding the secret file data
 * in the output image file */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Data\n",encInfo->secret_fname);
    char secret_data[MAX_SECRET_BUF_SIZE],arr[8];
    /* encoding the secret data in the image file by opening the secret data 
     * in a file pointer and storing the data in an array */
    FILE *fptr=fopen(encInfo->secret_fname,"r");
    fscanf(fptr," %[^\n]",secret_data);
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
	fread(arr,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(secret_data[i],arr);
	fwrite(arr,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
} 
/*logic to encoding the bytes to lsb in the output file*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int val=0;
    for(int i=7;i>=0;i--)
    {
	val=((((unsigned)data>>i))&1);
	image_buffer[7-i]=(val|(image_buffer[7-i]&(~1)));
    }
    return e_success;
}
/* encoing the extension size to the output which is an integer value */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File extension size\n",encInfo->secret_fname);
    char arr[32];
    fread(arr,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size,arr);
    fwrite(arr,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/*encoding the secret file size to the output file*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Size\n",encInfo->secret_fname); 
    int len=file_size-1;
    char arr[32];
    fread(arr,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,arr);
    fwrite(arr,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/*logic to encoding the size to lsb of the output file*/ 
Status encode_size_to_lsb(int data, char *image_buffer)
{
    int val=0;
    for(int i=31;i>=0;i--)
    {
	val=((((unsigned)data>>i))&1);
	image_buffer[31-i]=(val|(image_buffer[31-i]&(~1)));
    }
    return e_success;
}
/*Copying the remaining bytes until the input file reaches the EOF to the output file*/
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    char ch;
    while(fread(&ch,1,1,fptr_src))
    {
	fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}





















