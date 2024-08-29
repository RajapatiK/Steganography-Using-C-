#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "decode.h"

int main(int argc,char *argv[])
{
    if((argv[1]==NULL)||(argv[2]==NULL))
    {
	printf("Encoding: ./a.out -e <.bmp_file> <.Secret_file> [output file]\n");
	printf("Decoding: ./a.out -d <.bmp_file> [output file]\n");
    }
    else
    {
	int ret=check_operation_type(argv);
    /*ret =>e_encode
      call Encode function
      ret=>e_decode
      call Decode function
      ret=>e_unsupported
      print error part*/
    if(ret==e_encode)
    {
	EncodeInfo encinfo;
	if((read_and_validate_encode_args(argv,&encinfo)) == e_failure)
	{
	    printf("INFO: Validation part Error\n");
	}
	else 
	{
	    printf("INFO: Encoding\n");
	    do_encoding(argv,&encinfo);
	    check_capacity(&encinfo);
	}
    }
    if(ret==e_decode)
    {
	DecodeInfo decodeinfo;
	if((read_and_validate_decode_args(argv,&decodeinfo)==e_failure))
	{
	    printf("INFO: Decoding Validation part error\n");
	}
	else
	{
	    printf("INFO: ## Decoding Procedure Started ##\n");
	    do_decoding(&decodeinfo);
	}
    }
    if(ret==e_unsupported)
    {
	printf("Error\n");
    }
    }

}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1],"-e")==0)
    {
	return e_encode;
    }
    if(strcmp(argv[1],"-d")==0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}


