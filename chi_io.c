#define _CRT_SECURE_NO_WARNINGS
#include "chi_io.h"

#include <string.h>

#define alloc_a_chi_str() (chi_string*)malloc(sizeof(chi_string)) 
#define alloc_str_ptr(size) (char*)malloc(size + 1)
#define copy_to_chi(chi, block) memcpy((chi)->data, block, (chi)->size + 1)

#define CHECK_NULL(ptr, retval) do { if(ptr == NULL) { return retval; } } while(0)
#define CHECK_N(ptr, n) do { if(s_check_n) { size_t len = strlen(ptr); if(len < n) { n = len; } } } while(0)

#define MAX_BUFFER_LEN 8192

extern bool s_check_n;

struct _chi_string
{
    char* data;
    size_t size;
    size_t capacity;
};

/* helpers begin */

chi_errno_t _write_data(const char* str, size_t size, FILE* file)
{
    if (fwrite(str, 1, size, file) < size)
        return CHI_ERR_FILE_WRITE;
    return CHI_ERR_NO_ERR;
}

chi_errno_t _write_data_fp(const char* str, size_t size, const char* filepath, const char* openmode)
{
    FILE* file = fopen(filepath, openmode);
    CHECK_NULL(file, CHI_ERR_NULL_FILE);
    chi_errno_t result = _write_data(str, size, file);
    fclose(file);
    return result;
}

chi_errno_t _put_newline()
{
    if (fputc('\n', stdout) == EOF)
        return CHI_ERR_FILE_WRITE;
    return CHI_ERR_NO_ERR;
}

/* helpers end */

chi_errno_t chi_read_n(chi_string* chi_str, FILE* file, int n)
{
    CHECK_NULL(file, CHI_ERR_NULL_FILE);
    CHECK_NULL(chi_str, CHI_ERR_NULL_CHI_STR);
    if (n > chi_str->capacity)
    {
        chi_errno_t err;
        if ((err = chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, n))) != CHI_ERR_NO_ERR)
            return err;
    }
    if (fgets(chi_str->data, n, file) != NULL)
    {
        size_t length = 0;
        while (chi_str->data[length] != '\0' && chi_str->data[length] != '\n' && length < n)
            length++;

        chi_str->size = length;
        chi_str->data[length] = 0;
        return CHI_ERR_NO_ERR;
    }
    return CHI_ERR_FILE_READ;
}

chi_errno_t chi_read(chi_string* chi_str, FILE* file)
{
    return chi_read_n(chi_str, file, MAX_BUFFER_LEN);
}

chi_errno_t chi_read_from_filepath(chi_string* chi_str, const char* filepath)
{
    CHECK_NULL(chi_str, CHI_ERR_NULL_CHI_STR);
    CHECK_NULL(filepath, CHI_ERR_NULL_FILE);
    FILE* file = fopen(filepath, "rb+");
    if (file == NULL)
        return CHI_ERR_NULL_FILE;
    chi_errno_t result = chi_read(chi_str, file);
    fclose(file);
    return result;
}

chi_errno_t chi_read_to_end_n(chi_string* chi_str, FILE* file, int n)
{
    CHECK_NULL(file, CHI_ERR_NULL_FILE);
    CHECK_NULL(chi_str, CHI_ERR_NULL_CHI_STR);
    if (n + chi_str->size > chi_str->capacity)
    {
        chi_errno_t err;
        if ((err = chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, n + chi_str->size))) != CHI_ERR_NO_ERR)
            return err;
    }
    if (fgets(chi_str->data + chi_str->size, n, file) != NULL)
    {
        size_t length = chi_str->size;
        while (chi_str->data[length] != '\0' && chi_str->data[length] != '\n' && length < n + chi_str->size)
            length++;

        chi_str->size = length;
        chi_str->data[length] = 0;
        return CHI_ERR_NO_ERR;
    }
    return CHI_ERR_FILE_READ;
}

chi_errno_t chi_read_to_end(chi_string* chi_str, FILE* file)
{
    return chi_read_to_end_n(chi_str, file, MAX_BUFFER_LEN);
}

chi_errno_t chi_write(const chi_string* chi_str, FILE* file)
{
    CHECK_NULL(chi_str, CHI_ERR_NULL_CHI_STR);
    CHECK_NULL(chi_str->data, CHI_ERR_NULL_DATA);
    CHECK_NULL(file, CHI_ERR_NULL_FILE);
    return _write_data(chi_str->data, chi_str->size, file);
}

chi_errno_t chi_write_fp(const chi_string* chi_str, const char* filepath)
{
    CHECK_NULL(chi_str, CHI_ERR_NULL_CHI_STR);
    CHECK_NULL(chi_str->data, CHI_ERR_NULL_DATA);
    CHECK_NULL(filepath, CHI_ERR_NULL_PARAMETER);
    return _write_data_fp(chi_str->data, chi_str->size, filepath, "w+");
 }

CHI_API chi_errno_t chi_app_to(const chi_string* chi_str, const char* filepath)
{
    CHECK_NULL(chi_str, CHI_ERR_NULL_CHI_STR);
    CHECK_NULL(chi_str->data, CHI_ERR_NULL_DATA);
    CHECK_NULL(filepath, CHI_ERR_NULL_PARAMETER);
    return _write_data_fp(chi_str->data, chi_str->size, filepath, "a+");
}

chi_errno_t chi_scan_n(chi_string* chi_str, int n)
{
    return chi_read_n(chi_str, stdin, n);
}

chi_errno_t chi_scan(chi_string* chi_str)
{
    return chi_read(chi_str, stdin);
}

chi_errno_t chi_scan_to_end_n(chi_string* chi_str, int n)
{
    return chi_read_to_end_n(chi_str, stdin, n);
}

chi_errno_t chi_scan_to_end(chi_string* chi_str)
{
    return chi_read_to_end(chi_str, stdin);
}

chi_errno_t chi_print(const chi_string* chi_str)
{
    return chi_write(chi_str, stdout);
}

chi_errno_t chi_println(const chi_string* chi_str)
{
    chi_errno_t err = chi_write(chi_str, stdout);
    if (err != CHI_ERR_NO_ERR)
        return err;
    return _put_newline();
}

chi_errno_t chi_sv_writef(chi_string_view sv, FILE* file)
{
    CHECK_NULL(file, CHI_ERR_NULL_FILE);
    return _write_data(sv.data, sv.size, file);
}

chi_errno_t chi_sv_writefp(chi_string_view sv, const char* filepath)
{
    CHECK_NULL(filepath, CHI_ERR_NULL_PARAMETER);
    return _write_data_fp(sv.data, sv.size, filepath, "w+");
}

chi_errno_t chi_sv_print(chi_string_view sv)
{
    CHECK_NULL(sv.data, CHI_ERR_NULL_DATA);
    return _write_data(sv.data, sv.size, stdout);
}

chi_errno_t chi_sv_println(chi_string_view sv)
{
    CHECK_NULL(sv.data, CHI_ERR_NULL_DATA);
    chi_errno_t err = _write_data(sv.data, sv.size, stdout);
    if (err != CHI_ERR_NO_ERR)
        return err;
    return _put_newline();
}
