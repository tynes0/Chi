#define _CRT_SECURE_NO_WARNINGS
#include "chi_io.h"

#include <string.h>

#define alloc_a_chi_str()           (chi_string*)malloc(sizeof(chi_string)) 
#define alloc_str_ptr(size)         (char*)malloc(size + 1)
#define copy_to_chi(chi, block)     memcpy((chi)->data, block, (chi)->size + 1)

#define file_assert(block)          chi_assert(block != NULL, "NULL FILE!");
#define chi_str_assert(block)       chi_assert(block != NULL, "NULL chi_string!")
#define alloc_assert(block)         chi_assert(block != NULL, "Allocation failed!")
#define data_assert(block)          chi_assert(block != NULL, "NULL data!")

#define if_null_write_empty(data, file)         do { if(data == NULL) { return _write_data("", 1, file); } } while(0)
#define if_null_write_empty_fp(data, fp, om)    do { if(data == NULL) { return _write_data_fp("", 1, fp, om); } } while(0)
#define CHECK_NULL(ptr, retval)                 do { if(ptr == NULL) { return retval; } } while(0)
#define CHECK_N(ptr, n)                         do { if(g_check_n) { size_t len = strlen(ptr); if(len < n) { n = len; } } } while(0)

#define MAX_BUFFER_LEN 8192

extern bool g_check_n;

struct _chi_string
{
    char* data;
    size_t size;
    size_t capacity;
};

/* helpers begin */

bool _write_data(const char* str, size_t size, FILE* file)
{
    if (fwrite(str, 1, size, file) < size)
        return false;
    return true;
}

bool _write_data_fp(const char* str, size_t size, const char* filepath, const char* openmode)
{
    FILE* file = fopen(filepath, openmode);
    file_assert(file);
    bool result = _write_data(str, size, file);
    fclose(file);
    return result;
}

bool _put_newline()
{
    if (fputc('\n', stdout) == EOF)
        return false;
    return true;
}

/* helpers end */

bool chi_read_n(chi_string* chi_str, FILE* file, int n)
{
    file_assert(file);
    chi_str_assert(chi_str);
    if (n > chi_str->capacity)
        chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, n));
    if (fgets(chi_str->data, n, file) != NULL)
    {
        size_t length = 0;
        while (chi_str->data[length] != '\0' && chi_str->data[length] != '\n' && length < n)
            length++;

        chi_str->size = length;
        chi_str->data[length] = 0;
        return true;
    }
    return false;
}

bool chi_read(chi_string* chi_str, FILE* file)
{
    return chi_read_n(chi_str, file, MAX_BUFFER_LEN);
}

bool chi_read_from_filepath(chi_string* chi_str, const char* filepath)
{
    chi_assert(filepath != NULL, "NULL filepath!");
    chi_str_assert(chi_str);
    FILE* file = fopen(filepath, "rb+");
    file_assert(file);
    bool result = chi_read(chi_str, file);
    fclose(file);
    return result;
}

bool chi_read_to_end_n(chi_string* chi_str, FILE* file, int n)
{
    file_assert(file);
    chi_str_assert(chi_str);
    if (n + chi_str->size > chi_str->capacity)
        chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, n + chi_str->size));
    if (fgets(chi_str->data + chi_str->size, n, file) != NULL)
    {
        size_t length = chi_str->size;
        while (chi_str->data[length] != '\0' && chi_str->data[length] != '\n' && length < n + chi_str->size)
            length++;

        chi_str->size = length;
        chi_str->data[length] = 0;
        return true;
    }
    return false;
}

bool chi_read_to_end(chi_string* chi_str, FILE* file)
{
    return chi_read_to_end_n(chi_str, file, MAX_BUFFER_LEN);
}

bool chi_write(const chi_string* chi_str, FILE* file)
{
    chi_str_assert(chi_str);
    file_assert(file);
    if_null_write_empty(chi_str->data, file);
    return _write_data(chi_str->data, chi_str->size, file);
}

bool chi_write_fp(const chi_string* chi_str, const char* filepath)
{
    chi_str_assert(chi_str);
    chi_assert(filepath != NULL, "NULL filepath!");
    if_null_write_empty_fp(chi_str->data, filepath, "w+");
    return _write_data_fp(chi_str->data, chi_str->size, filepath, "w+");
}

CHI_API bool chi_app_to(const chi_string* chi_str, const char* filepath)
{
    chi_str_assert(chi_str);
    chi_assert(filepath != NULL, "NULL filepath!");
    if_null_write_empty_fp(chi_str->data, filepath, "a+");
    return _write_data_fp(chi_str->data, chi_str->size, filepath, "a+");
}

bool chi_scan_n(chi_string* chi_str, int n)
{
    return chi_read_n(chi_str, stdin, n);
}

bool chi_scan(chi_string* chi_str)
{
    return chi_read(chi_str, stdin);
}

bool chi_scan_to_end_n(chi_string* chi_str, int n)
{
    return chi_read_to_end_n(chi_str, stdin, n);
}

bool chi_scan_to_end(chi_string* chi_str)
{
    return chi_read_to_end(chi_str, stdin);
}

bool chi_print(const chi_string* chi_str)
{
    return chi_write(chi_str, stdout);
}

bool chi_println(const chi_string* chi_str)
{
    bool success = chi_write(chi_str, stdout);
    if (!success)
        return success;
    return _put_newline();
}

CHI_API bool chi_printerr(const chi_string* chi_str)
{
    return chi_write(chi_str, stderr);
}

bool chi_sv_writef(chi_string_view sv, FILE* file)
{
    file_assert(file);
    return _write_data(sv.data, sv.size, file);
}

bool chi_sv_writefp(chi_string_view sv, const char* filepath)
{
    chi_assert(filepath != NULL, "NULL filepath!");
    return _write_data_fp(sv.data, sv.size, filepath, "w+");
}

bool chi_sv_print(chi_string_view sv)
{
    return _write_data(sv.data, sv.size, stdout);
}

bool chi_sv_println(chi_string_view sv)
{
    bool success = _write_data(sv.data, sv.size, stdout);
    if (!success)
        return success;
    return _put_newline();
}

CHI_API bool chi_sv_printerr(chi_string_view sv)
{
    return _write_data(sv.data, sv.size, stderr);
}