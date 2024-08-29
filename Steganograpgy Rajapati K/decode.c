#include <stdio.h>
#include "decode.h"
#include <string.h>
#include "types.h"
#include "common.h"


/* Fuunction Definitions */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decodeinfo)
{
    /*argv[2]=>.bmp
      true=>store argv[2] in src_image_fname
      failure=>return e_failure*/
    if(argv[2]==NULL)
    {
	printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
    }
    else if(strstr(argv[2],".bmp"))
    {
	decodeinfo->src_image_fname=argv[2];
    }
    else
    {
	printf("Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
	return e_failure;
    }
    /*argv[3]=>if not Null storing the argument to the secret_fname*/ 
    if(argv[3]!=NULL)
    {
	strcpy(decodeinfo->secret_fname, argv[3]);
    }
    /*Else creating a name as default and storing it in the file name*/
    else
    { 
	strcpy(decodeinfo->secret_fname, "default");	
    }
    return e_success;
}
/*Decoding operation*/
/*FUnction Calls*/
Status do_decoding(DecodeInfo *decodeinfo)
{
    if(file_open(decodeinfo)==e_success)
    {
	printf("INFO: Done\n");
	printf("INFO: Decoding Magic string signature\n");
	if(decode_magic_string(decodeinfo->fptr_src_image)==e_success)
	{
	    printf("INFO: Done\n");
	    if(decode_secret_file_extn_size((32),decodeinfo)==e_success)
	    {
		printf("INFO: Done\n");
		if(decode_secret_file_extn(decodeinfo->fptr_src_image,decodeinfo->extn_secret_file,decodeinfo)==e_success)
		{
		    printf("INFO: Done\n");
		    if(decode_secret_file_size(decodeinfo->fptr_src_image, decodeinfo)==e_success)
		    {
			printf("INFO: Done\n");
			if(decode_secret_data(decodeinfo->fptr_src_image, decodeinfo)==e_success)
			{
			    printf("INFO: Done\nINFO: ## Decoding Done Successfully ##\n");
			}
			else
			{
			    printf("Decoding secret message failed\n");
			}
		    }
		    else
		    {
			printf("Decoding file size failed\n");
		    }
		}
		else
		{
		    printf("Decoding the extension failed\n");
		}
	    }
	    else
	    {
		printf("Decoding the extension size failed\n");
	    }
	}
	else
	{
	    printf("Decoding magic string failed\n");
	}
    }
    else
    {
	printf("Unable to open the files\n");
    }
}

/* Decoding the magic string */
Status decode_magic_string(FILE *fptr_src_image)
{
    /*Initial 54 bytes is for the header files so that we are decoding the bytes 
      from 55th byte for the magic string which was encoded first*/
    static int count=0;
    fseek(fptr_src_image, 54, SEEK_SET);
    char key[10];
    printf("Enter the Magic string : ");
    scanf("%s",key);
    char arr[8],buffer[5];
    int i=0;
    for(int i=0;i<5;i++)
    {
	fread(arr,8,1,fptr_src_image);
	decode_byte_from_lsb(arr,&buffer[i]);
    }
    buffer[5]='\0';
    /*Checking whether the magic string is matching or not 
      and if not return e_failure */
    if(strcmp(key,buffer)==0)
    {
	return e_success;
    }
    else
    {
	count++;
	if(count!=3)
	{
	    printf("Wrong key\nTry again\n");
	    decode_magic_string(fptr_src_image);
	}
	/*If the user enters the magic string incorrectly for  three times it will return e_failure*/
	else
	{
	    printf("Magic string is not matching\n");
	    return e_failure;
	}
    }
}

/* Decoding the secret file extension */  
Status decode_secret_file_extn(FILE *fptr_src_image,char *extn_secret_file,DecodeInfo *decodeinfo)
{
    printf("INFO: Decoding Output File Extensions\n");
    /*decoding the secret file extension which was ecnoded after the extension size */
    int size=decodeinfo->secret_file_extn_size;
    char arr[8],arr1[5];
    for(int i=0;i<size/8;i++)
    {
	fread(arr,8,1,fptr_src_image);
	decode_byte_from_lsb(arr,&arr1[i]);
    }
    arr1[size/8]='\0';
    /*After decoding the extension concatinating the filename with extension*/ 
    strcat(decodeinfo->secret_fname,arr1);
    /*Opening the file in a file pointer to craete that file with extension */ 
    printf("Output File not mentioned. Creating decoded file as %s\n",decodeinfo->secret_fname);
    decodeinfo->fptr_secret_file=fopen(decodeinfo->secret_fname,"w");
    if(decodeinfo->fptr_secret_file==NULL)
    {
	printf("Unable to open the file\n");
    }
    return e_success;
}
/*Decoding the secret data*/
Status decode_secret_data(FILE *fptr_src_image, DecodeInfo *decodeinfo)
{
    /*decoding the secret data and storing it in a file*/
    printf("INFO: Decoding Secret Data\n");
    char arr[8],buffer[decodeinfo->file_size];
    for(int i=0;i<decodeinfo->file_size-1;i++)
    {
	fread(arr,8,1,fptr_src_image);
	decode_byte_from_lsb(arr,&buffer[i]);
    }
    buffer[decodeinfo->file_size]='\0';
    /*Storing each characters in the output file*/
    fwrite(buffer,1,decodeinfo->file_size-1,decodeinfo->fptr_secret_file);
    return e_success;
}
/*Logic to decoding the character bytes from lsb*/
Status decode_byte_from_lsb(char *arr,char *buffer)
{
    char ch=0,n;
    for(int i=0;i<8;i++)
    {
	n=arr[i]&1;
	ch=n<<(7-i)|ch;
    }
    *buffer=ch;
}
/*Decoding the secret file extension size*/
Status decode_secret_file_extn_size(int size, DecodeInfo *decodeinfo)
{
    /*After the magic string which encoded was size of the the extension 
      which is an integer value so we are fetcing 32 bytes lsb's*/ 
    printf("INFO: Decoding Output File Extension Size\n");
    char arr[size];
    fread(arr,size,1,decodeinfo->fptr_src_image);
    decode_size_from_lsb(arr,&decodeinfo->secret_file_extn_size);
    return e_success;
}
/*Decoding secret file size*/
Status decode_secret_file_size(FILE *fptr_src_image, DecodeInfo *decodeinfo)
{
    /*Decoding the secret file size which was encoded after the extension which is an integer value */ 
    printf("INFO: Decoding Secret File Size\n"); 
    char arr[32];
    fread(arr,32,1,fptr_src_image);
    decode_size_from_lsb(arr,&decodeinfo->file_size);
    return e_success;
}
/*logic to decoding the integer bytes from lsb*/
Status decode_size_from_lsb(char *arr,int *num)
{
    int size=0;
    for(int i=0;i<32;i++)
    {
	int num1=arr[i]&1;
	size=num1<<(31-i)|size;
    }
    *num=size;
}
/*Opening the source image in a file pointer*/
Status file_open(DecodeInfo *decodeinfo)
{
    //Source image
    printf("INFO: Opening required files\n");
    decodeinfo->fptr_src_image = fopen(decodeinfo->src_image_fname,"r");
    printf("Opened %s\n",decodeinfo->src_image_fname);
    //Error Handling
    if(decodeinfo->fptr_src_image==NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR: unable to open the file %s\n",decodeinfo->src_image_fname);
	return e_failure;
    }
    //No failure return e_success;
    return e_success;
}





























