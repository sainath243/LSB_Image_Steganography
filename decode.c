#include <stdio.h>
#include <string.h>
#include <unistd.h> // for sleep()
#include "decode.h"
#include "types.h"

// Define color macros
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define BLUE    "\033[1;34m"
#define YELLOW  "\033[1;33m"

/*
 * decode_byte_from_lsb -
 * Reads 8 LSBs from image buffer and reconstructs 1 character
 */
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    int n = 7;
    char result = 0;

    for (int i = 0; i < 8; i++)
    {
        int bit = image_buffer[i] & 1; // Extract LSB
        bit <<= n;                     // Shift to correct position
        result |= bit;                 // Build result
        n--;
    }

    *data = result;
    return e_success;
}

/*
 * decode_size_from_lsb -
 * Reads 32 LSBs and reconstructs an integer (size)
 */
Status decode_size_from_lsb(int *data, char *image_buffer)
{
    int n = 31, result = 0;

    for (int i = 0; i < 32; i++)
    {
        int bit = image_buffer[i] & 1;
        bit <<= n;
        result |= bit;
        n--;
    }

    *data = result;
    return e_success;
}

/*
 * decode_open_files -
 * Opens stego image file for reading and sets offset at 54
 */
Status decode_open_files(DecodeInfo *decInfo)
{
    printf(BLUE "\n🔓 You have chosen DECODING mode.\n" RESET);
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, decInfo->stego_image_fname);
        return d_failure;
    }

    fseek(decInfo->fptr_stego_image, 0, SEEK_SET);
    printf(YELLOW "Position of offset in stego file is : %ld\n" RESET, ftell(decInfo->fptr_stego_image));

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    printf(YELLOW "Position of offset in stego file is : %ld\n" RESET, ftell(decInfo->fptr_stego_image));

    printf(GREEN "✅ File opened successfully!\n" RESET);
    return d_success;
}

/*
 * decode_magic_string_len -
 * Reads magic string length (32 bits)
 */
Status decode_magic_string_len(int *len, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(len, buffer);
    decInfo->magic_str_len = *len;
    printf(BLUE "Magic string length is : %d\n" RESET, decInfo->magic_str_len);
    sleep(1);
    return e_success;
}

/*
 * decode_magic_string -
 * Reads hidden magic string from image
 */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    for (int i = 0; i < decInfo->magic_str_len; i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&decInfo->decoded_magic[i], buffer);
    }

    decInfo->decoded_magic[decInfo->magic_str_len] = '\0';
    printf(GREEN "✅ Magic string decoded successfully: %s\n" RESET, decInfo->decoded_magic);
    sleep(1);
    return d_success;
}

/*
 * decode_secret_file_extn_len -
 * Reads length of secret file extension (32 bits)
 */
Status decode_secret_file_extn_len(int *len, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(len, buffer);
    decInfo->de_ext_size = *len;
    printf(BLUE "Extension length is : %d\n" RESET, decInfo->de_ext_size);
    sleep(1);
    return e_success;
}

/*
 * decode_secret_file_extn -
 * Reads extension characters and prepares output file
 */
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo)
{
    char output[50];

    for (int i = 0; i < decInfo->de_ext_size; i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&output[i], buffer);
    }

    output[decInfo->de_ext_size] = '\0';
    strcpy(decInfo->secret_fname, output);

    printf(GREEN "✅ Secret file extension decoded: %s\n" RESET, decInfo->secret_fname);

    strcat(decInfo->output_fname, decInfo->secret_fname);
    decInfo->fptr_output_file = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, decInfo->output_fname);
        return d_failure;
    }

    printf(GREEN "✅ Output file created: %s\n" RESET, decInfo->output_fname);
    sleep(1);
    return d_success;
}

/*
 * decode_secret_file_size -
 * Reads 32 bits for size of hidden secret file
 */
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(&file_size, buffer);
    decInfo->size_secret_file = file_size;
    printf(BLUE "Secret file size is : %d bytes\n" RESET, decInfo->size_secret_file);
    sleep(1);
    return e_success;
}

/*
 * decode_secret_file_data -
 * Reads secret file bytes and writes to output file
 */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    printf(YELLOW "\n🕵️ Decoding hidden data...\n" RESET);
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch, buffer);
        fputc(ch, decInfo->fptr_output_file);
    }

    printf(GREEN "\n✅ Secret file data decoded successfully!\n" RESET);
    sleep(1);
    return d_success;
}

/*
 * do_decoding -
 * Wrapper function, calls all decoding steps in order
 */
Status do_decoding(DecodeInfo *decInfo)
{
    if (decode_open_files(decInfo) == d_failure) return d_failure;
    if (decode_magic_string_len(&decInfo->magic_str_len, decInfo) == d_failure) return d_failure;
    if (decode_magic_string(decInfo->decoded_magic, decInfo) == d_failure) return d_failure;
    if (decode_secret_file_extn_len(&decInfo->de_ext_size, decInfo) == d_failure) return d_failure;
    if (decode_secret_file_extn(decInfo->secret_fname, decInfo) == d_failure) return d_failure;
    if (decode_secret_file_size(decInfo->size_secret_file, decInfo) == d_failure) return d_failure;
    if (decode_secret_file_data(decInfo) == d_failure) return d_failure;

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output_file);

    //printf(GREEN "\n🎉 Decoding completed successfully! ✅\n" RESET);
    return d_success;
}

/*
 * read_and_validate_decode_args -
 * Validates input arguments for decoding
 * Ensures BMP file and sets default output name
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char str[20];
    char def_fname[20] = "Output";

    // Check for BMP input
    if (strstr(argv[2], ".bmp") != NULL)
    {
        printf(GREEN "✅ .bmp file detected: %s\n" RESET, argv[2]);
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(RED "❌ Error: Input file is not a .bmp image!\n" RESET);
        return d_failure;
    }

    // If output filename not provided
    if (argv[3] == NULL)
    {
        argv[3] = def_fname;
    }

    strcpy(str, argv[3]);

    // Remove extension from name
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
        {
            str[i] = '\0';
        }
    }

    strcpy(decInfo->output_fname, str);
    printf(BLUE "Output file name set to : %s\n" RESET, decInfo->output_fname);
    return d_success;
}
