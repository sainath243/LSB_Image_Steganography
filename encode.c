#include <stdio.h>
#include <string.h>
#include "encode.h"

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define CYAN "\033[1;36m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

Status pointer_pos(FILE *fptr_src, FILE *fptr_dest)
{
    if (ftell(fptr_src) == ftell(fptr_dest))
    {
        printf(CYAN "Both offset are pointed at : %ld\n" RESET, ftell(fptr_src));
        return e_success;
    }
    printf(YELLOW "Position of offset in src file is : %ld\n" RESET, ftell(fptr_src));
    printf(YELLOW "Position of offset in stego file is : %ld\n" RESET, ftell(fptr_dest));
    return e_failure;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i, n = 7;
    for (i = 0; i <= 7; i++)
    {
        int mask = 1 << n;
        int get = data & mask;
        image_buffer[i] = image_buffer[i] & ~1;
        get = (get >> n);
        image_buffer[i] = image_buffer[i] | get;
        n--;
    }

    return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer)
{
    int i, n = 31;
    for (i = 0; i < 32; i++)
    {
        int mask = 1 << n;
        int get = data & mask;
        image_buffer[i] = image_buffer[i] & ~1;
        get = (get >> n);
        image_buffer[i] = image_buffer[i] | get;
        n--;
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    printf(CYAN "Enter the Magic string : " RESET);
    scanf("%s", encInfo->magic);

    encInfo->magic_str_len = strlen(encInfo->magic);
    printf(GREEN "Length is : %d\n" RESET, encInfo->magic_str_len);

    int sizeof_magic_len = sizeof(encInfo->magic_str_len);
    printf(GREEN "sizeof Magic string is : %d\n" RESET, sizeof_magic_len);

    char *secret_ext = strstr(encInfo->secret_fname, ".");
    for (int i = 0; i < MAX_FILE_SUFFIX; i++)
    {
        encInfo->extn_secret_file[i] = secret_ext[i];
    }
    printf(GREEN "Extension of Secret file: %s\n" RESET, encInfo->extn_secret_file);

    encInfo->ext_size = strlen(secret_ext);
    printf(GREEN "Length of secret extension is: %d\n" RESET, encInfo->ext_size);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf(GREEN "Size of Secret data : %d\n" RESET, encInfo->size_secret_file);
    printf(GREEN "Size of Secret file : %ld\n" RESET, sizeof(encInfo->size_secret_file));

    int file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf(GREEN "Size Of Source image : %d\n" RESET, file_size);

    int bytes_encode;
    bytes_encode = sizeof_magic_len + encInfo->magic_str_len + encInfo->ext_size + encInfo->size_secret_file + sizeof(encInfo->size_secret_file) + sizeof(encInfo->ext_size);
    printf(CYAN "Capacity checking : %d\n" RESET, bytes_encode * 8);

    if (bytes_encode <= file_size - 54)
    {
        fseek(encInfo->fptr_src_image, 0, SEEK_SET);
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    printf(CYAN "width = %u\n" RESET, width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf(CYAN "height = %u\n" RESET, height);

    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "❌ ERROR: Unable to open file %s\n" RESET, encInfo->stego_image_fname);
        return e_failure;
    }

    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_magic_string_len(int len, EncodeInfo *encInfo)
{
    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);

    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    for (int i = 0; i < (encInfo->magic_str_len); i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_len(int len, EncodeInfo *encInfo)
{
    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);

    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    for (int i = 0; i < (encInfo->ext_size); i++)
    {
        char buffer[8];
        fread(buffer, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    pointer_pos(encInfo->fptr_src_image, encInfo->fptr_stego_image);

    char ch;
    while (fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        char buffer[8];
        if (fread(buffer, 1, 8, encInfo->fptr_src_image) < 8)
        {
            return e_failure;
        }
        encode_byte_to_lsb(ch, buffer);
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    pointer_pos(fptr_src, fptr_dest);
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        printf(GREEN "✅ .bmp file is present\n" RESET);
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf(RED "❌ .bmp file is not present\n" RESET);
        return e_failure;
    }

    if (strstr(argv[3], ".txt") != NULL || strstr(argv[3], ".c") != NULL || strstr(argv[3], ".bmp") != NULL || strstr(argv[3], ".csv") != NULL)
    {
        printf(GREEN "✅ %s file is present\n" RESET, argv[3]);
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf(RED "❌ %s is not present\n" RESET, argv[3]);
        return e_failure;
    }

    if (strstr(argv[4], ".bmp") != NULL)
    {
        printf(GREEN "✅ %s is present\n" RESET, argv[4]);
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        strcpy(encInfo->stego_image_fname, "stego.bmp");
    }
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    fread(buffer, 1, 54, fptr_src_image);
    fwrite(buffer, 1, 54, fptr_dest_image);
    pointer_pos(fptr_src_image, fptr_dest_image);

    if (ftell(fptr_dest_image) == 54)
        return e_success;
    else
        return e_failure;
}

Status do_encoding(EncodeInfo *encInfo)
{
    open_files(encInfo);
    int res = check_capacity(encInfo);
    if (res == e_failure)
    {
        printf(RED "❌ Check capacity failed!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Check capacity is success!\n" RESET);

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf(RED "❌ BMP header not copied!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ BMP header copied successfully\n" RESET);

    if (encode_magic_string_len(encInfo->magic_str_len, encInfo) == e_failure)
    {
        printf(RED "❌ Magic string length not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Magic string length copied successfully\n" RESET);

    if (encode_magic_string(encInfo->magic, encInfo) == e_failure)
    {
        printf(RED "❌ Magic string data not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Magic string data copied successfully\n" RESET);

    if (encode_secret_file_extn_len(encInfo->ext_size, encInfo) == e_failure)
    {
        printf(RED "❌ Secret file extension length not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Secret file extension length copied successfully\n" RESET);

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf(RED "❌ Secret file extension not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Secret file extension copied successfully\n" RESET);

    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf(RED "❌ Secret file size not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Secret file size copied successfully\n" RESET);

    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf(RED "❌ Secret file data not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Secret file data copied successfully\n" RESET);

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf(RED "❌ Remaining image data not encoded!\n" RESET);
        return e_failure;
    }
    printf(GREEN "✅ Remaining image data copied successfully\n" RESET);

    printf(GREEN "🎉 Encoding completed successfully! 🔐\n" RESET);

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}
