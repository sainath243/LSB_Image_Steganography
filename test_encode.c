#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if (argc == 1)
    {
        printf("\033[1;31m❌ Usage:\033[0m\n");
        printf("  To encode : \033[1;33m./a.out -e <input.bmp> <secret.txt> <optional_output.bmp>\033[0m\n");
        printf("  To decode : \033[1;33m./a.out -d <stego_image.bmp> <optional_output_file>\033[0m\n");
        return e_failure;
    }

    if (strcmp(argv[1], "-e") == 0)
    {
        printf("\033[1;34m📦 Selected Encoding Mode\033[0m\n");

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("\033[1;32m✅ Validation Successful\033[0m\n");

            if (do_encoding(&encInfo) == e_success)
            {
                printf("\n\033[1;32m🎉 Encoding completed successfully! ✅\033[0m\n");
            }
            else
            {
                printf("\033[1;31m❌ Encoding failed!\033[0m\n");
            }
        }
        else
        {
            printf("\033[1;31m❌ Validation failed! Please check your arguments.\033[0m\n");
        }
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        printf("\033[1;34m📦 Selected Decoding Mode\033[0m\n");

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("\033[1;32m✅ Validation Successful\033[0m\n");

            if (do_decoding(&decInfo) == e_success)
            {
                printf("\n\033[1;32m🎉 Decoding completed successfully! ✅\033[0m\n");
            }
            else
            {
                printf("\033[1;31m❌ Decoding failed!\033[0m\n");
            }
        }
        else
        {
            printf("\033[1;31m❌ Validation failed! Please check your arguments.\033[0m\n");
        }
    }
    else
    {
        printf("\033[1;31m❌ Invalid option!\033[0m\n");
        printf("  Use \033[1;33m-e\033[0m for encoding or \033[1;33m-d\033[0m for decoding.\n");
        return e_failure;
    }

    return e_success;
}
