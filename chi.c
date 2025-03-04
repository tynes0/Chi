#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "chi.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#ifdef max
#undef max
#endif // max

#ifdef min
#undef min
#endif // min

#define alloc_assert(block)                             chi_assert(block != NULL, "Allocation failed!")
#define data_assert(block)                              chi_assert(block != NULL, "NULL data!")
#define chi_str_assert(block)                           chi_assert(block != NULL, "NULL chi_string!")

#define ignore_spaces_or_return(data, action)           do { if(s_ignore_space) { if(!isspace(data)) { action; } } else { action; } } while(0)

#define CHECK_NULL(ptr)                                 do { if(ptr == NULL) { return NULL; } } while(0)
#define CHECK_NULL2(ptr, retval)                        do { if(ptr == NULL) { return retval; } } while(0)
#define CHECK_N(ptr, n)                                 do { if(g_check_n) { size_t len = strlen(ptr); if(len < n) { n = len; } } } while(0) // n should't be const!! 
#define CHECK_BEGIN_AND_END(size, begin, end)           do { chi_assert(begin <= end && begin < size, "range is not verified!"); if(size < end) { end = size; } } while(0) // end shouldn't be const

#define to_chi_str_impl(buffer_size, format, value)     do { chi_string* result = chi_create_empty(buffer_size); snprintf(result->data, result->size, format, value); return result; } while(0)

#define equal_null_check(lhs, rhs, check)               do { if (check) { if (lhs == NULL) { if (rhs == NULL) { return true; } return false; } if (rhs == NULL) {return false; } } } while(0)
#define equal_data_check(left_data, right_data)         do { if(left_data != right_data) { return false; } } while(0)

#define vformat_from(from, chi_str)                     do { if(s_no_format_item_count) { s_no_format_item_count--; } else { va_list arg_list; va_start(arg_list, from); _chi_vformat(chi_str, arg_list); va_end(arg_list); } } while(0)

/* structures begin */
struct _chi_string
{
    char* data;
    size_t size;
    size_t capacity;
};

struct _chi_arena
{
    void** data;
    size_t count;
    size_t capacity;
    bool is_active;
    bool* is_chi;
};

typedef struct
{
    chi_arena* arenas;
    size_t count;
    size_t capacity;
}arena_array;
/* structures end */

/* const - static - extern variables begin */
#if defined(_WIN64)
static const size_t _hash_offset_basis = 14695981039346656037ull;
static const size_t _hash_prime = 1099511628211ull;
#else // !defined(_WIN64)
static const size_t _hash_offset_basis = 2166136261u;
static const size_t _hash_prime = 16777619u;
#endif // defined(_WIN64)

static const size_t MIN_ARENA_CAP = 32;
static const size_t MIN_ARENA_ARRAY_CAP = 8;

static const chi_string s_chi_snull = { .data = NULL, .size = 0, .capacity = 0 };
const chi_string* chi_snull = &s_chi_snull;

extern bool g_check_n = false;
extern CHI_BOOL_FORMAT_ g_bool_format = CHI_BOOL_FORMAT_DECIMAL;

static bool s_rand_initialized = false;
static uint32_t s_rand_seed = 0;

static bool s_ignore_space = false;

static uint32_t s_no_format_item_count = 0;

static chi_arena s_arena = { 0 };
static arena_array s_arena_array = { 0 };
static arena_array s_user_arenas = { 0 };
/* const - static - extern variables end */

/* UTILS BEGIN */

static chi_string* alloc_a_chi_str()
{
    chi_string* result = ((chi_string*)malloc(sizeof(chi_string)));
    alloc_assert(result);
    return result;
}

static char* alloc_str_ptr(size_t size)
{
    char* result = ((char*)malloc(size + 1));
    alloc_assert(result);
    return result;
    
}

static char* realloc_str_ptr(char* block, size_t size)
{
    char* result = ((char*)realloc(block, size + 1));
    alloc_assert(result);
    return result;
}

static void zero_memory(void* block, size_t size)
{
    memset(block, 0, size);
}

static void copy_to_chi(chi_string* chi, const void* block)
{
    memcpy(chi->data, block, chi->size + 1);
}

static void copy_from_chi(chi_string* chi, void* block)
{
    memcpy(block, (chi)->data, (chi)->size + 1);
}

size_t CHI_CHECK_RETURN _chi_calculate_capacity(size_t old_cap, size_t new_cap)
{
    if (old_cap > new_cap)
        return old_cap;
    if (old_cap == 0 || old_cap == 1)
        old_cap = 2;
    while (new_cap > old_cap)
        old_cap = (old_cap * 3 / 2);
    return old_cap;
}

_CHI_PRAGMA(warning(push))
_CHI_PRAGMA(warning(disable : 6011))
static void _add_ptr_to_arena(chi_arena* arena, void* ptr, bool is_chi)
{
    if (arena == NULL)
        return;
    if (!arena->is_active)
        return;
    if (arena->data == NULL)
    {
        arena->data = (void**)malloc(sizeof(void*) * MIN_ARENA_CAP);
        arena->is_chi = (bool*)malloc(sizeof(bool) * MIN_ARENA_CAP);
        chi_assert(arena->data && arena->is_chi, "Chi initialize failed!");
        arena->count = 0;
        arena->capacity = MIN_ARENA_CAP;
    }
    if (arena->count == arena->capacity)
    {
        size_t new_cap = _chi_calculate_capacity(arena->capacity, arena->capacity + 1);
        void** temp = (void**)malloc(sizeof(void*) * new_cap);
        bool* tempb = (bool*)malloc(sizeof(bool) * new_cap);
        chi_assert(temp && tempb, "Allocation error!");
        memcpy(temp, arena->data, sizeof(void*) * arena->count);
        memcpy(tempb, arena->is_chi, sizeof(bool) * arena->count);
        free(arena->data);
        free(arena->is_chi);
        arena->data = temp;
        arena->is_chi = tempb;
        arena->capacity = new_cap;
    }
    arena->data[arena->count] = ptr;
    arena->is_chi[arena->count] = is_chi;
    arena->count++;
}
_CHI_PRAGMA(warning(pop))

// todo decrease capacity
static bool _remove_ptr_from_arena(chi_arena* arena, const void* ptr, bool* is_chi_result)
{
    if (arena == NULL || arena->data == NULL)
        return false;
    if (!arena->is_active)
        return false;
    chi_assert(arena->data, "Unexpexted NULL data! Implamentation error!");
    for (size_t i = 0; i < arena->count; ++i)
    {
        if (arena->data[i] == ptr)
        {
            if (is_chi_result != NULL)
                *is_chi_result = arena->is_chi[i];
            if (i != arena->count - 1)
            {
                arena->data[i] = arena->data[arena->count - 1];
                arena->is_chi[i] = arena->is_chi[arena->count - 1];
            }
            arena->data[arena->count - 1] = NULL;
            arena->is_chi[arena->count - 1] = false;
            arena->count--;
            return true;
        }
    }
    return false;
}

static chi_arena* _top_arena(arena_array array)
{
    return array.count == 0 ? NULL : &array.arenas[array.count - 1];
}

static void _arena_push(chi_arena* arena, void* ptr, bool is_chi)
{
    if (!s_arena.is_active)
        s_arena.is_active = true;
    _add_ptr_to_arena(&s_arena, ptr, is_chi);
    _add_ptr_to_arena(arena, ptr, is_chi);
}

static bool _arena_pop(chi_arena* arena, void* ptr, bool* is_chi_result)
{
    _remove_ptr_from_arena(&s_arena, ptr, NULL);
    bool result = _remove_ptr_from_arena(arena, ptr, is_chi_result);
    return (arena == &s_arena) ? true : result;
}

static void _top_arena_push(void* ptr, bool is_chi)
{
    _arena_push(_top_arena(s_arena_array), ptr, is_chi);
}

static void _top_arena_pop(void* ptr)
{ 
    _arena_pop(_top_arena(s_arena_array), ptr, NULL);
}

static void _arena_free_inner_data(chi_arena* arena)
{
    if (arena == NULL)
        return;
    if (!arena->is_active)
        return;

    for (size_t i = 0; i < arena->count; ++i)
    {
        if (arena->is_chi[i])
        {
            chi_arena_free(arena, (chi_string*)arena->data[i]);
        }
        else
        {
            _arena_pop(arena, arena->data[i], NULL);
            free(arena->data[i]);
        }
    }

    arena->is_active = false;
    arena->count = 0;
}

_CHI_PRAGMA(warning(push))
_CHI_PRAGMA(warning(disable : 6385))
static chi_arena* _arena_array_push(arena_array* array)
{
    chi_assert(array != NULL, "NULL arena_array!");

    if (array->arenas == NULL)
    {
        array->arenas = (chi_arena*)malloc(sizeof(chi_arena) * MIN_ARENA_ARRAY_CAP);
        chi_assert(array->arenas, "Chi initialize failed!");
        zero_memory(array->arenas, sizeof(chi_arena) * MIN_ARENA_ARRAY_CAP);
        array->count = 0;
        array->capacity = MIN_ARENA_ARRAY_CAP;
    }

    if (array->count == array->capacity)
    {
        size_t new_cap = _chi_calculate_capacity(array->capacity, array->capacity + 1);
        chi_arena* temp = (chi_arena*)malloc(sizeof(chi_arena) * new_cap);
        alloc_assert(temp);
        zero_memory(temp, sizeof(chi_arena) * new_cap);
        memcpy(temp, array->arenas, sizeof(chi_arena*) * array->count);
        free(array->arenas);
        array->arenas = temp;
        array->capacity = new_cap;
    }

    for (size_t i = 0; i < array->capacity; ++i)
    {
        if (!array->arenas[i].is_active)
        {
            array->arenas[i].capacity = MIN_ARENA_CAP;
            array->arenas[i].count = 0;
            array->arenas[i].data = NULL;
            array->arenas[i].is_chi = NULL;
            array->arenas[i].is_active = true;
            array->count++;
            return &array->arenas[i];
        }
    }

    return NULL;
}
_CHI_PRAGMA(warning(pop))

static void _arena_array_pop(arena_array* array)
{
    array->count--;
    _arena_free_inner_data(&array->arenas[array->count]);
}

static size_t _arena_array_get_index(arena_array* array, const chi_arena* arena, bool check_valid)
{
    size_t index = chi_npos;
    if (array == NULL || arena == NULL || array->count == 0)
        return index;

    if (check_valid)
    {
        for (size_t i = 0; i < array->count; ++i)
        {
            if (&array->arenas[i] == arena)
            {
                index = i;
                break;
            }
        }
    }
    else
    {
        index = arena - array->arenas;
    }
    return index;
}

static void _arena_array_remove(arena_array* array, chi_arena* arena, bool check_valid)
{
    if (!arena->is_active)
        return;

    size_t index = _arena_array_get_index(array, arena, check_valid);
    if (index == chi_npos)
        return;

    array->count--;
    _arena_free_inner_data(&array->arenas[index]);
}

static void _arena_array_cleanup(arena_array* array, bool free_array_itself, bool free_inner_data)
{
    if (array == NULL)
        return;

    if (array->arenas == NULL)
    {
        if (free_array_itself)
        {
            free(array);
            array = NULL;
        }
        return;
    }

    for (size_t i = 0; i < array->count; ++i)
    {
        if (array->arenas[i].data != NULL)
        {
            if (free_inner_data)
                _arena_free_inner_data(&array->arenas[i]);

            free(array->arenas[i].data);
            free(array->arenas[i].is_chi);
            array->arenas[i].data = NULL;
            array->arenas[i].is_chi = NULL;
        }
    }

    free(array->arenas);
    array->arenas = NULL;

    if (free_array_itself)
    {
        free(array);
        array = NULL;
    }
    else
    {
        array->count = 0;
        array->capacity = 0;
    }
}

static chi_string* _chi_arena_create_base(chi_arena* arena, size_t size)
{
    chi_string* result = alloc_a_chi_str();
    _arena_push(arena, result, true);
    result->data = NULL;
    if (size == 0)
    {
        result->size = result->capacity = 0;
        return result;
    }
    chi_reserve(result, _chi_calculate_capacity(0, size));
    result->size = size;
    return result;
}

static void _chi_arena_free_base(chi_arena* arena, void* data)
{
    data_assert(data);
    bool is_chi = false;
    _arena_pop(arena, data, &is_chi);
    if (is_chi)
    {
        chi_string* chi_str = (chi_string*)data;
        if (chi_str->data != NULL)
        {
            free(chi_str->data);
            chi_str->data = NULL;
        }
    }
    
    free(data);
    data = NULL;
}

static void _chi_arena_free_ptr_base(chi_arena* arena, void* ptr)
{
    data_assert(ptr);
    _arena_pop(arena, ptr, NULL);
    free(ptr);
    ptr = NULL;
}

static chi_string* _chi_create_base(size_t size)
{
    chi_string* result = alloc_a_chi_str();
    _top_arena_push(result, true);
    result->data = NULL;
    if (size == 0)
    {
        result->size = result->capacity = 0;
        return result;
    }
    chi_reserve(result, _chi_calculate_capacity(0, size));
    result->size = size;
    return result;
}

static chi_string* _chi_insert_data(chi_string* chi_str, size_t idx, const char* data, size_t data_length)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(data, chi_str);
    chi_assert(idx <= chi_str->size, "index is out of the range!");

    if (data_length == 0)
        return chi_str;

    size_t new_size = chi_str->size + data_length;
    if (new_size > chi_str->capacity)
    {
        size_t new_capacity = _chi_calculate_capacity(chi_str->capacity, new_size);
        char* new_data = alloc_str_ptr(new_capacity);

        memcpy(new_data, chi_str->data, idx);
        memcpy(new_data + idx, data, data_length);
        memcpy(new_data + idx + data_length, chi_str->data + idx, chi_str->size - idx);

        free(chi_str->data);

        chi_str->data = new_data;
        chi_str->capacity = new_capacity;
    }
    else
    {
        memmove(chi_str->data + idx + data_length, chi_str->data + idx, chi_str->size - idx);
        memcpy(chi_str->data + idx, data, data_length);
    }

    chi_str->size = new_size;
    return chi_str;
}

static chi_string* _chi_append_data(chi_string* chi_str, const char* data, size_t data_length)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(data, chi_str);
    if (chi_str->data == NULL)
    {
        chi_reserve(chi_str, _chi_calculate_capacity(0, data_length));
        if (chi_str->data == NULL)
            return chi_str;
        chi_str->size = data_length;
        copy_to_chi(chi_str, data);
        return chi_str;
    }
    if (chi_str->capacity < chi_str->size + data_length)
        chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, chi_str->size + data_length));
    memcpy(chi_str->data + chi_str->size, data, data_length + 1);
    chi_str->size += data_length;
    return chi_str;
}

static chi_string* _chi_erase_data(chi_string* chi_str, size_t begin, size_t end)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    size_t removed_part_len = end - begin;
    if (removed_part_len == chi_str->size)
    {
        zero_memory(chi_str->data, chi_str->size);
        chi_str->size = 0;
        return chi_str;
    }
    memmove(chi_str->data + begin, chi_str->data + end, chi_str->size - end);
    chi_str->size -= removed_part_len;
    return chi_str;
}

static chi_string* _chi_reset_data(chi_string* chi_str, const char* data, size_t size)
{
    CHECK_NULL(chi_str);
    if (data == NULL)
    {
        if (chi_str->data == NULL)
            return chi_str;
        free(chi_str->data);
        chi_str->data = NULL;
        chi_str->size = chi_str->capacity = 0;
        return chi_str;
    }
    if (chi_str->data != NULL)
        chi_clear(chi_str);
    if (chi_str->capacity < size)
        chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, size));
    if (chi_str->data == NULL)
        return chi_str;
    chi_str->size = size;
    chi_str->data[chi_str->size] = 0;
    copy_to_chi(chi_str, data);
    return chi_str;
}

static chi_string* _chi_fill_data(chi_string* chi_str, size_t offset, size_t length, char ch)
{
    CHECK_NULL(chi_str);
    if (chi_str->data == NULL || offset >= chi_str->size)
        return chi_str;
    if (offset + length > chi_str->size)
        length = chi_str->size - offset;
    memset(chi_str->data + offset, ch, length);
    return chi_str;
}

_CHI_PRAGMA(warning(push))
_CHI_PRAGMA(warning(disable : 6386)) // capacity is always larger than size!
static chi_string* _chi_remove_data(chi_string* chi_str, const char* delim, size_t delim_len) // make optimized version of this
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(delim, chi_str);
    chi_assert(delim_len > 0, "delimiter length must be greater than zero");

    if (chi_str->data == NULL || chi_str->size == 0)
        return chi_str;

    char* temp = alloc_str_ptr(chi_str->capacity);
    size_t idx = 0;

    for (size_t i = 0; i < chi_str->size; ++i)
    {
        if (i <= chi_str->size - delim_len && memcmp(chi_str->data + i, delim, delim_len) == 0)
            i += delim_len - 1;
        else
            temp[idx++] = chi_str->data[i];
    }

    free(chi_str->data);
    chi_str->size = idx;
    chi_str->data = temp;
    chi_str->data[idx] = 0;
    return chi_str;
}
_CHI_PRAGMA(warning(pop))

_CHI_PRAGMA(warning(push))
_CHI_PRAGMA(warning(disable : 6386))
static chi_string* _chi_replace_data(chi_string* chi_str, size_t begin, size_t end, const char* old_value, size_t old_value_size, const char* new_value, size_t new_value_size)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(old_value, chi_str);
    CHECK_NULL2(new_value, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    chi_assert(old_value_size > 0, "old_value length must be greater than zero");

    if (chi_str->data == NULL || chi_str->size == 0)
        return chi_str;

    if (old_value_size == new_value_size)
    {
        for (size_t i = 0; i <= chi_str->size - old_value_size; ++i)
        {
            if (memcmp(chi_str->data + i, old_value, old_value_size) == 0)
            {
                memcpy(chi_str->data + i, new_value, new_value_size);
                i += new_value_size - 1;
            }
        }
    }
    else
    {
        size_t new_capacity = _chi_calculate_capacity(chi_str->capacity, chi_str->size + (new_value_size - old_value_size));
        char* temp = alloc_str_ptr(new_capacity);
        size_t idx = begin;
        if (begin > 0)
            memcpy(temp, chi_str->data, begin - 0);
        for (size_t i = begin; i <= end; ++i)
        {
            if (memcmp(chi_str->data + i, old_value, old_value_size) == 0)
            {
                memcpy(temp + idx, new_value, new_value_size);
                idx += new_value_size;
                i += old_value_size;
            }
            else
            {
                temp[idx++] = chi_str->data[i];
            }
        }
        if (chi_str->size > end)
            memcpy(temp + idx, chi_str->data + end, chi_str->size - end);
        free(chi_str->data);
        chi_str->size = idx + (chi_str->size - end);
        chi_str->capacity = new_capacity;
        chi_str->data = temp;
        chi_str->data[chi_str->size] = 0;
    }

    return chi_str;
}
_CHI_PRAGMA(warning(pop))

static size_t _chi_find_pattern(const char* data, size_t len, size_t offset, const char* pattern, size_t pattern_len, bool reverse)
{
    CHECK_NULL2(data, chi_npos);
    CHECK_NULL2(pattern, chi_npos);

    if (offset >= len || pattern_len == 0)
        return chi_npos;

    if (reverse)
    {
        if (offset + pattern_len > len)
            return chi_npos;

        for (size_t i = len - pattern_len; i >= offset; --i)
        {
            if (memcmp(data + i, pattern, pattern_len) == 0)
                return i;
            if (i == 0) // secure underflow 
                break;
        }
    }
    else
    {
        if (offset + pattern_len > len)
            return chi_npos;

        for (size_t i = offset; i <= len - pattern_len; ++i)
        {
            if (memcmp(data + i, pattern, pattern_len) == 0)
                return i;
        }
    }

    return chi_npos;
}

static size_t _chi_find_first_last_of_not_of(const char* data, size_t len, size_t offset, const char* delims, bool reverse, bool not_of)
{
    CHECK_NULL2(delims, chi_npos);
    if (offset >= len)
        return chi_npos;
    size_t delim_count = strlen(delims);
    if (delim_count == 0 || len == 0)
        return chi_npos;

    if (reverse)
    {
        for (size_t i = len; i > offset; --i)
        {
            bool found = false;
            for (size_t j = 0; j < delim_count; ++j)
            {
                if ((delims[j] == data[i - 1]) != not_of)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                return i - 1;
            if (i == 0) // secure underflow 
                break; 
        }
    }
    else
    {
        for (size_t i = offset; i < len; ++i)
        {
            bool found = false;
            for (size_t j = 0; j < delim_count; ++j)
            {
                if ((delims[j] == data[i]) != not_of)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                return i;
        }
    }

    return chi_npos;
}

static size_t _chi_count_data(const char* str, size_t size, const char* data, size_t length, size_t begin, size_t end)
{
    data_assert(str);
    data_assert(data);
    CHECK_BEGIN_AND_END(size, begin, end);
    size_t counter = 0;
    if (length > end - begin)
        return counter;
    for (size_t i = begin; i < end - length; ++i)
    {
        if (memcmp(str + i, data, length) == 0)
        {
            ++counter;
            i += length - 1;
        }
    }
    return counter;
}

_CHI_PRAGMA(warning(push))
_CHI_PRAGMA(warning(disable : 6386)) // asserted
static chi_string** _chi_split_data(const chi_string* chi_str, const char* delim, size_t delim_length, size_t* size)
{
    chi_str_assert(chi_str);
    chi_assert(delim != NULL, "NULL delimeter!");
    chi_assert(size != NULL, "NULL size!");
    *size = _chi_count_data(chi_str->data, chi_str->size, delim, delim_length, 0, chi_str->size) + 1;
    chi_string** result = (chi_string**)malloc(sizeof(chi_string*) * (*size));
    alloc_assert(result);
    _top_arena_push(result, false);
    size_t offset = 0;
    size_t last_location = 0;
    size_t index = 0;

    while ((offset = _chi_find_pattern(chi_str->data, chi_str->size, last_location, delim, delim_length, false)) != chi_npos)
    {
        chi_assert(index < *size, "Buffer overflow detected! Implamentation error.");
        result[index++] = chi_substring(chi_str, last_location, offset - last_location);
        ++offset;
        last_location = offset;
    }
    result[index++] = chi_substring(chi_str, last_location, chi_str->size - last_location);
    if (index < *size)
        *size = index;
    return result;
}
_CHI_PRAGMA(warning(pop))

_CHI_PRAGMA(warning(push))
_CHI_PRAGMA(warning(disable : 6386)) // asserted
static chi_string** _chi_arena_split_data(chi_arena* arena, const chi_string* chi_str, const char* delim, size_t delim_length, size_t* size)
{
    chi_str_assert(chi_str);
    chi_assert(delim != NULL, "NULL delimeter!");
    chi_assert(size != NULL, "NULL size!");
    *size = _chi_count_data(chi_str->data, chi_str->size, delim, delim_length, 0, chi_str->size) + 1;
    chi_string** result = (chi_string**)malloc(sizeof(chi_string*) * (*size));
    alloc_assert(result);
    _arena_push(arena, result, false);
    size_t offset = 0;
    size_t last_location = 0;
    size_t index = 0;

    while ((offset = _chi_find_pattern(chi_str->data, chi_str->size, last_location, delim, delim_length, false)) != chi_npos)
    {
        chi_assert(index < *size, "Buffer overflow detected! Implamentation error.");
        result[index++] = chi_arena_substring(arena, chi_str, last_location, offset - last_location);
        ++offset;
        last_location = offset;
    }
    result[index++] = chi_arena_substring(arena, chi_str, last_location, chi_str->size - last_location);
    if (index < *size)
        *size = index;
    return result;
}
_CHI_PRAGMA(warning(pop))

static bool _chi_equal_data(const char* lhs, const char* rhs, size_t ls, size_t rs, bool check)
{
    equal_null_check(lhs, rhs, check);
    equal_data_check(ls, rs);
    return (memcmp(lhs, rhs, ls) == 0);
}

static bool _chi_equal_data_ignorecase(const char* lhs, const char* rhs, size_t ls, size_t rs, bool check)
{
    equal_null_check(lhs, rhs, check);
    equal_data_check(ls, rs);

    char left, right;
    for (size_t i = 0; i < ls; i++)
    {
        left = 'A' <= lhs[i] && lhs[i] <= 'Z' ? lhs[i] + 32 : lhs[i];
        right = 'A' <= rhs[i] && rhs[i] <= 'Z' ? rhs[i] + 32 : rhs[i];
        equal_data_check(left, right);
    }
    return true;
}

static size_t _hash(size_t val, const unsigned char* const first, const size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        val ^= (size_t)(first[i]);
        val *= _hash_prime;
    }
    return val;
}

static void _chi_check_rand_seed()
{
    if (!s_rand_initialized) 
    { 
        s_rand_seed = (uint32_t)time(NULL); 
        s_rand_initialized = true; 
    }
}

static uint32_t _chi_random()
{
    _chi_check_rand_seed();

    s_rand_seed = (1664525 * s_rand_seed + 1013904223) & 0xFFFFFFFF;
    return s_rand_seed;
}

static uint32_t _chi_random_in_range(uint32_t min, uint32_t max)
{
    chi_assert(max > min, "max should be bigger than min!");
    return (_chi_random() % (max - min)) + min;
}

static void _chi_vformat(chi_string* chi_str, va_list arg_list)
{
    if (arg_list == NULL)
        return;
    chi_begin_scope();
    size_t index = 0, scope_end = 0;

    while (index < chi_str->size)
    {
        index = chi_find_c(chi_str, scope_end, '{');
        if (index == chi_npos || index == chi_str->size - 1)
            goto function_end;
        if (chi_str->data[index] + 1 == '{')
        {
            index += 1;
            continue;
        }

        scope_end = chi_find_c(chi_str, index, '}');
        if (scope_end == chi_npos)
            goto function_end;

        chi_string_view view = chi_sv_n(chi_str->data + index + 1, scope_end - index + -1);
        if (view.size == 0)
            continue;

        if (chi_sv_equal_s(view, "c"))
        {
            char v = va_arg(arg_list, char);
            chi_replace_ip(chi_str, index, scope_end + 1, &v);
        }
        else if (chi_sv_equal_s(view, "i"))
        {
            int v = va_arg(arg_list, int);
            chi_string* str = chi_to_string_i(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "b"))
        {
            bool v = va_arg(arg_list, bool);
            if(g_bool_format == CHI_BOOL_FORMAT_TEXT)
                chi_replace_ip(chi_str, index, scope_end + 1, (v == true) ? "true" : "false");
            else
                chi_replace_ip(chi_str, index, scope_end + 1, (v == true) ? "1" : "0");
        }
        else if (chi_sv_equal_s(view, "f"))
        {
            float v = va_arg(arg_list, float);
            chi_string* str = chi_to_string_f(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "d"))
        {
            double v = va_arg(arg_list, double);
            chi_string* str = chi_to_string_d(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "l"))
        {
            long v = va_arg(arg_list, long);
            chi_string* str = chi_to_string_l(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "ll"))
        {
            long long v = va_arg(arg_list, long long);
            chi_string* str = chi_to_string_ll(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "uc"))
        {
            unsigned char v = va_arg(arg_list, unsigned char);
            chi_string* str = chi_to_string_ul((unsigned long)v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "u"))
        {
            unsigned v = va_arg(arg_list, unsigned);
            chi_string* str = chi_to_string_ul((unsigned long)v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "ul"))
        {
            unsigned long v = va_arg(arg_list, unsigned long);
            chi_string* str = chi_to_string_ul(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "ull"))
        {
            unsigned long long v = va_arg(arg_list, unsigned long long);
            chi_string* str = chi_to_string_ull(v);
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "sz"))
        {
            size_t v = va_arg(arg_list, size_t);
#ifdef _WIN64
            chi_string* str = chi_to_string_ull(v);
#else // _WIN64
            chi_string* str = chi_to_string_ul(v);
#endif // _WIN64
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "p"))
        {
            intptr_t* v = va_arg(arg_list, intptr_t*);
            chi_string* str = chi_to_string_ull((unsigned long long)v); // think about that
            chi_replace_ip(chi_str, index, scope_end + 1, str->data);
        }
        else if (chi_sv_equal_s(view, "s"))
        {
            const char* v = va_arg(arg_list, const char*);
            chi_replace_ip(chi_str, index, scope_end + 1, v);
        }
        else if (chi_sv_equal_s(view, "cs"))
        {
            chi_string* v = va_arg(arg_list, chi_string*);
            if (v == NULL)
                continue;
            chi_replace_ip(chi_str, index, scope_end + 1, v->data);
        }
        else if (chi_sv_equal_s(view, "csv"))
        {
            chi_string_view v = va_arg(arg_list, chi_string_view);
            chi_replace_ip(chi_str, index, scope_end + 1, v.data);
        }
    }
    function_end:;
    chi_end_scope();
}

/* UTILS END */

#ifdef _CHI_DEBUG
__declspec(noreturn) void _chi_assert_show_message(const char* message, const char* file, int line)
{
    fprintf(stderr, "Chi assertion failed: %s, File: %s, Line: %d\n", message, file, line);
    abort();
}
#endif // _CHI_DEBUG

size_t CHI_CHECK_RETURN chi_hash_s(const char* str)
{
    data_assert(str);
    return _hash(_hash_offset_basis, (const unsigned char*)(str), strlen(str));
}

void chi_enable_check_n(bool enable)
{
    g_check_n = enable;
}

CHI_API void chi_ignore_spaces(bool ignore)
{
    s_ignore_space = ignore;
}

CHI_API void chi_set_next_items_no_format(uint32_t count)
{
    s_no_format_item_count = count;
}

CHI_API void chi_set_bool_format(CHI_BOOL_FORMAT_ format)
{
    switch (format)
    {
    case CHI_BOOL_FORMAT_DECIMAL:
    case CHI_BOOL_FORMAT_TEXT:
        g_bool_format = format;
        break;
    default:
        chi_assert(false, "Invalid CHI_BOOL_FORMAT_!");
    }
}

CHI_API void chi_ignore_freeing(void* block)
{
    _top_arena_pop(block);
}

CHI_API void chi_add_to_the_free_list(void* block, bool is_chi)
{
    _top_arena_push(block, is_chi);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create(const char* data, ...)
{
    CHECK_NULL2(data, _chi_create_base(0));
    chi_string* result = _chi_create_base(strlen(data));
    copy_to_chi(result, data);
    vformat_from(data, result);
    return result;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create_n(const char* data, size_t size, ...)
{
    CHECK_NULL2(data, _chi_create_base(0));
    if (size == 0)
        return _chi_create_base(0);
    CHECK_N(data, size);
    chi_string* result = _chi_create_base(size);
    copy_to_chi(result, data);
    result->data[result->size] = 0;
    vformat_from(size, result);
    return result;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create_empty(size_t size)
{
    return _chi_create_base(size);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create_with_capacity(size_t capacity)
{
    chi_string* result = alloc_a_chi_str();
    _top_arena_push(result, true);
    result->data = NULL;
    result->size = 0;
    if (capacity == 0)
    {
        result->capacity = 0;
        return result;
    }
    if (capacity < 2)
        capacity = 2;
    chi_reserve(result, capacity);
    return result;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create_and_fill(char ch, size_t size)
{
    chi_string* result = _chi_create_base(size);
    memset(result->data, ch, size);
    result->data[result->size] = 0;
    _top_arena_push(result, true);
    return result;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create_from_chi_s(const chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    return chi_create_n(chi_str->data, chi_str->size);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_create_from_chi_sv(const chi_string_view chi_sv)
{
    return chi_create_n(chi_sv.data, chi_sv.size);
}

CHI_API CHI_CHECK_RETURN chi_string* chichi()
{
    return chi_create_empty(0);
}

CHI_CHECK_RETURN chi_string* chi_make_chi(char* data, size_t n, ...)
{
    data_assert(data);
    chi_string* result = alloc_a_chi_str();
    result->data = data;
    result->size = result->capacity = n;
    result->data[n] = 0;
    _top_arena_push(result, true);
    vformat_from(n, result);
    return result;
}

CHI_CHECK_RETURN chi_string* chi_make_chi_c(char* data, size_t n, size_t capacity, ...)
{
    data_assert(data);
    chi_string* result = alloc_a_chi_str();
    result->data = data;
    result->size = n;
    result->data[n] = 0;
    result->capacity = capacity;
    _top_arena_push(result, true);
    vformat_from(capacity, result);
    return result;
}

CHI_API CHI_CHECK_RETURN void* chi_create_ptr(size_t size)
{
    return chi_arena_create_ptr(_top_arena(s_arena_array), size);
}

CHI_API CHI_CHECK_RETURN void chi_free_ptr(void* ptr)
{
    chi_arena_free(_top_arena(s_arena_array), ptr);
}

void chi_begin_scope()
{
    _arena_array_push(&s_arena_array);
}

// TODO decrease cap
void chi_end_scope()
{
    chi_assert(s_arena_array.count > 0, "chi_end_scope() called on empty stack!");
    _arena_array_pop(&s_arena_array);
}

CHI_API void chi_cleanup()
{
    _arena_free_inner_data(&s_arena);
    s_arena.is_active = true; // always active

    _arena_array_cleanup(&s_arena_array, false, false);
    _arena_array_cleanup(&s_user_arenas, false, false);
}

CHI_API void chi_free(chi_string* chi_str)
{
    _chi_arena_free_base(_top_arena(s_arena_array), chi_str);
}

CHI_API void chi_str_array_free(chi_string** chi_str_array, size_t size)
{
    while (size--) // If the size is incorrect, undefined behavior occurs
        chi_free(chi_str_array[size]);
    _top_arena_pop(chi_str_array); // Note that: If it is a pointer produced by Chi functions and found in lists, it is deleted from the lists.
    free(chi_str_array);
    chi_str_array = NULL;
}

CHI_API void chi_clear(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    if (chi_str->data == NULL)
        return;
    zero_memory(chi_str->data, chi_str->size);
    chi_str->size = 0;
}

char* chi_release(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    char* data = chi_str->data;
    chi_str->data = NULL;
    chi_free(chi_str);
    return data;
}

CHI_API void chi_resize(chi_string* chi_str, size_t new_size)
{
    chi_str_assert(chi_str);
    if (new_size == 0)
    {
        chi_clear(chi_str);
        return;
    }

    if (new_size > chi_str->capacity)
    {
        chi_reserve(chi_str, _chi_calculate_capacity(chi_str->capacity, new_size));
        chi_str->size = new_size;
        chi_str->data[new_size] = 0;
        return;
    }
    chi_str->data[new_size] = 0;
    chi_str->size = new_size;
}

CHI_API void chi_reserve(chi_string* chi_str, size_t new_cap)
{
    chi_str_assert(chi_str);

    if (chi_str->data == NULL)
    {
        chi_str->data = alloc_str_ptr(new_cap);
        chi_str->capacity = new_cap;
        chi_str->size = 0;
        return;
    }

    if (chi_str->capacity >= new_cap)
        return;

    char* new_data = realloc_str_ptr(chi_str->data, new_cap);
    chi_str->data = new_data;
    chi_str->capacity = new_cap;
}


CHI_API void chi_shrink_to_fit(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data);
    if (chi_str->size == chi_str->capacity)
        return;

    char* new_data = alloc_str_ptr(chi_str->size);
    copy_from_chi(chi_str, new_data);
    free(chi_str->data);
    chi_str->data = new_data;
    chi_str->capacity = chi_str->size;
}

CHI_API chi_string* chi_reset(chi_string* chi_str, const char* data)
{
    return _chi_reset_data(chi_str, data, data != NULL ? strlen(data) : 0);
}

CHI_API chi_string* chi_reset_n(chi_string* chi_str, const char* data, size_t n)
{
    CHECK_N(data, n);
    return _chi_reset_data(chi_str, data, n);
}

CHI_API chi_string* chi_reset_cs(chi_string* chi_str, const chi_string* data)
{
    data_assert(data);
    return _chi_reset_data(chi_str, data->data, data->size);
}

chi_string* chi_reset_from_sv(chi_string* chi_str, const chi_string_view sv)
{
    return _chi_reset_data(chi_str, sv.data, sv.size);
}

CHI_CHECK_RETURN char* chi_get(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->data;
}

CHI_API CHI_CHECK_RETURN const char* chi_cstr(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->data;
}

CHI_API char* chi_at(chi_string* chi_str, size_t idx)
{
    chi_str_assert(chi_str);
    chi_assert(idx < chi_str->size, "Index subscript out of the range!");
    return (chi_str->data + idx);
}

CHI_API const char* chi_cat(const chi_string* chi_str, size_t idx)
{
    chi_str_assert(chi_str);
    chi_assert(idx < chi_str->size, "Index subscript out of the range!");
    return (chi_str->data + idx);
}

CHI_API char* chi_begin(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->data;
}

CHI_API const char* chi_cbegin(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->data;
}

CHI_API char* chi_end(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->data + chi_str->size;
}

CHI_API const char* chi_cend(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->data + chi_str->size;
}

CHI_API chi_string* chi_append_c(chi_string* chi_str, char data)
{
    return _chi_append_data(chi_str, &data, 1);
}

chi_string* chi_append_n(chi_string* chi_str, const char* data, size_t n)
{
    CHECK_N(data, n);
    return _chi_append_data(chi_str, data, n);
}

CHI_API chi_string* chi_append(chi_string* chi_str, const char* data)
{
    CHECK_NULL2(data, chi_str);
    return _chi_append_data(chi_str, data, strlen(data));
}

CHI_API chi_string* chi_append_cs(chi_string* lhs, const chi_string* rhs)
{
    CHECK_NULL(rhs);
    return _chi_append_data(lhs, rhs->data, rhs->size);
}

CHI_API chi_string* chi_append_sv(chi_string* lhs, const chi_string_view rhs)
{
    return _chi_append_data(lhs, rhs.data, rhs.size);
}

chi_string* chi_push_back(chi_string* chi_str, char data)
{
    return _chi_append_data(chi_str, &data, 1);
}

chi_string* chi_pop_back(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    chi_assert(!chi_is_empty(chi_str), "chi_pop_back() call with empty chi_string!");
    chi_str->data[--chi_str->size] = 0;
    return chi_str;
}

chi_string* chi_insert_c(chi_string* chi_str, size_t idx, char data)
{
    return _chi_insert_data(chi_str, idx, &data, 1);
}

chi_string* chi_insert_n(chi_string* chi_str, size_t idx, const char* data, size_t n)
{
    CHECK_N(data, n);
    return _chi_insert_data(chi_str, idx, data, n);
}

chi_string* chi_insert(chi_string* chi_str, size_t idx, const char* data)
{
    return _chi_insert_data(chi_str, idx, data, strlen(data));
}

chi_string* chi_insert_cs(chi_string* chi_str, size_t idx, const chi_string* data)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(data, chi_str);
    return _chi_insert_data(chi_str, idx, data->data, data->size);
}

chi_string* chi_insert_sv(chi_string* chi_str, size_t idx, chi_string_view data)
{
    CHECK_NULL(chi_str);
    return _chi_insert_data(chi_str, idx, data.data, data.size);
}

chi_string* chi_erase(chi_string* chi_str, size_t begin, size_t end)
{
    return _chi_erase_data(chi_str, begin, end);
}

chi_string* chi_erase_from(chi_string* chi_str, size_t offset)
{
    return _chi_erase_data(chi_str, offset, chi_str->size);
}

CHI_CHECK_RETURN size_t chi_size(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->size;
}

CHI_CHECK_RETURN size_t chi_length(const chi_string* chi_str)
{
    CHECK_NULL2(chi_str, 0);
    return chi_str->size;
}

CHI_CHECK_RETURN size_t chi_capacity(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_str->capacity;
}

CHI_CHECK_RETURN char chi_front(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    chi_assert(0 != chi_str->size, "chi_front() call with empty chi_string!");
    return chi_str->data[0];
}

CHI_CHECK_RETURN char chi_back(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    chi_assert(0 != chi_str->size, "chi_back() call with empty chi_string!");
    return chi_str->data[chi_str->size - 1];
}

CHI_CHECK_RETURN bool chi_equal_s(const chi_string* lhs, const char* rhs)
{
    chi_str_assert(lhs);
    equal_null_check(lhs->data, rhs, true);
    return _chi_equal_data(lhs->data, rhs, lhs->size, strlen(rhs), false);
}

CHI_CHECK_RETURN bool chi_equal_sv(const chi_string* lhs, chi_string_view rhs)
{
    chi_str_assert(lhs);
    return _chi_equal_data(lhs->data, rhs.data, lhs->size, rhs.size, true);
}

CHI_CHECK_RETURN bool chi_equal(const chi_string* lhs, const chi_string* rhs)
{
    chi_str_assert(lhs);
    chi_str_assert(rhs);
    return _chi_equal_data(lhs->data, rhs->data, lhs->size, rhs->size, true);
}

CHI_CHECK_RETURN bool chi_equal_ignorecase_s(const chi_string* lhs, const char* rhs)
{
    chi_str_assert(lhs);
    equal_null_check(lhs->data, rhs, true);
    return _chi_equal_data_ignorecase(lhs->data, rhs, lhs->size, strlen(rhs), false);
}

CHI_CHECK_RETURN bool chi_equal_ignorecase_sv(const chi_string* lhs, chi_string_view rhs)
{
    chi_str_assert(lhs);
    return _chi_equal_data_ignorecase(lhs->data, rhs.data, lhs->size, rhs.size, true);
}

CHI_CHECK_RETURN bool chi_equal_ignorecase(const chi_string* lhs, const chi_string* rhs)
{
    chi_str_assert(lhs);
    chi_str_assert(rhs);
    return _chi_equal_data_ignorecase(lhs->data, rhs->data, lhs->size, rhs->size, true);
}

CHI_CHECK_RETURN bool chi_starts_with(const chi_string* chi_str, char ch)
{
    chi_str_assert(chi_str);
    return (chi_str->data[0] == ch);
}

CHI_CHECK_RETURN bool chi_ends_with(const chi_string* chi_str, char ch)
{
    chi_str_assert(chi_str);
    return (chi_str->data[chi_str->size - 1] == ch);
}

CHI_CHECK_RETURN bool chi_is_empty(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(chi_str->data, true);
    return (chi_str->size == 0);
}

CHI_API chi_string* chi_fill(chi_string* chi_str, char ch)
{
    return _chi_fill_data(chi_str, 0, chi_str->size, ch);
}

CHI_API chi_string* chi_fill_n(chi_string* chi_str, size_t offset, size_t length, char ch)
{
    return _chi_fill_data(chi_str, offset, length, ch);
}

CHI_API chi_string* chi_copy(chi_string* chi_str, const chi_string* src_cs)
{
    if (src_cs == NULL)
    {
        CHECK_NULL(chi_str);
        chi_free(chi_str);
        return NULL;
    }
    if (chi_str == NULL)
    {
        chi_str = chi_create(src_cs->data);
        return chi_str;
    }
    chi_reset_n(chi_str, src_cs->data, src_cs->size);
    return chi_str;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_substring(const chi_string* chi_str, size_t offset, size_t length)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(chi_str->data, NULL);
    chi_assert(offset < chi_str->size, "offset is out of the range!");

    // same with CHECK_N but always enable
    if (offset + length > chi_str->size)
        length = chi_str->size - offset;

    return chi_create_n(chi_str->data + offset, length);
}

CHI_API CHI_CHECK_RETURN chi_string** chi_split(const chi_string* chi_str, size_t* size)
{
    return _chi_split_data(chi_str, " ", 1, size);
}

CHI_API CHI_CHECK_RETURN chi_string** chi_split_c(const chi_string* chi_str, char delim, size_t* size)
{
    return _chi_split_data(chi_str, &delim, 1, size);
}

CHI_API CHI_CHECK_RETURN chi_string** chi_split_s(const chi_string* chi_str, const char* delim, size_t* size)
{
    data_assert(delim);
    return _chi_split_data(chi_str, delim, strlen(delim), size);
}

CHI_API CHI_CHECK_RETURN chi_string** chi_split_cs(const chi_string* chi_str, const chi_string* delim, size_t* size)
{
    data_assert(delim);
    return _chi_split_data(chi_str, delim->data, delim->size, size);
}

CHI_API CHI_CHECK_RETURN chi_string** chi_split_sv(const chi_string* chi_str, chi_string_view delim, size_t* size)
{
    return _chi_split_data(chi_str, delim.data, delim.size, size);
}

chi_string* chi_remove_repated(chi_string* chi_str)
{
#pragma warning(push)
#pragma warning(disable : 6386) // capacity is always larger than size!
    CHECK_NULL(chi_str);
    if (chi_str->data == NULL || chi_str->size == 0)
        return chi_str;
    char* temp = alloc_str_ptr(chi_str->capacity);
    size_t idx = 0;
    unsigned char ascii[256] = { 0 };
    for (size_t i = 0; i < chi_str->size; ++i)
    {
        if (ascii[chi_str->data[i]] == 0)
        {
            temp[idx++] = chi_str->data[i];
            ascii[chi_str->data[i]] = 1;
        }
    }
    free(chi_str->data);
    chi_str->size = idx;
    chi_str->data = temp;
    chi_str->data[idx] = 0;
    return chi_str;
#pragma warning(pop)
}

chi_string* chi_remove_c(chi_string* chi_str, char delim)
{
    return _chi_remove_data(chi_str, &delim, 1);
}

chi_string* chi_remove_n(chi_string* chi_str, const char* delim, size_t n)
{
    CHECK_N(delim, n);
    return _chi_remove_data(chi_str, delim, n);
}

chi_string* chi_remove(chi_string* chi_str, const char* delim)
{
    CHECK_NULL2(delim, chi_str);
    return _chi_remove_data(chi_str, delim, strlen(delim));
}

chi_string* chi_remove_cs(chi_string* chi_str, const chi_string* delim)
{
    CHECK_NULL2(delim, chi_str);
    return _chi_remove_data(chi_str, delim->data, delim->size);
}

chi_string* chi_remove_sv(chi_string* chi_str, chi_string_view delim)
{
    return _chi_remove_data(chi_str, delim.data, delim.size);
}

CHI_API chi_string* chi_trim_left(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    size_t counter = 0;
    while (isspace(chi_str->data[counter]))
        counter++;
    if (counter == chi_str->size)
    {
        chi_clear(chi_str);
        return chi_str;
    }
    memmove(chi_str->data, chi_str->data + counter, chi_str->size - counter);
    chi_str->size -= counter;
    return chi_str;
}

CHI_API chi_string* chi_trim_right(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    size_t idx = chi_str->size - 1;
    while (isspace(chi_str->data[idx]))
        idx--;
    if (idx == 0)
    {
        chi_clear(chi_str);
        return chi_str;
    }
    chi_str->size = idx + 1;
    chi_str->data[chi_str->size] = 0;
    return chi_str;
}

CHI_API chi_string* chi_trim(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    return chi_trim_left(chi_trim_right(chi_str));
}

CHI_API chi_string* chi_replace_ip(chi_string* chi_str, size_t begin, size_t end, const char* new_value)
{
    CHECK_NULL2(new_value, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    chi_string_view view = chi_sv_n(chi_str->data + begin, end - begin);
    return _chi_replace_data(chi_str, begin, end, view.data, view.size, new_value, strlen(new_value));
}

chi_string* chi_replace_c(chi_string* chi_str, char old_value, char new_value)
{
    chi_str_assert(chi_str);
    return _chi_replace_data(chi_str, 0, chi_str->size, &old_value, 1, &new_value, 1);
}

chi_string* chi_replace(chi_string* chi_str, const char* old_value, const char* new_value)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(old_value, chi_str);
    CHECK_NULL2(new_value, chi_str);
    return _chi_replace_data(chi_str, 0, chi_str->size, old_value, strlen(old_value), new_value, strlen(new_value));
}

chi_string* chi_replace_sv(chi_string* chi_str, chi_string_view old_value, chi_string_view new_value)
{
    chi_str_assert(chi_str);
    return _chi_replace_data(chi_str, 0, chi_str->size, old_value.data, old_value.size, new_value.data, new_value.size);
}

CHI_CHECK_RETURN size_t chi_find_c(const chi_string* chi_str, size_t offset, char delim)
{
    chi_str_assert(chi_str);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, &delim, 1, false);
}

CHI_CHECK_RETURN size_t chi_find(const chi_string* chi_str, size_t offset, const char* delim)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(delim, chi_npos);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, delim, strlen(delim), false);
}

CHI_CHECK_RETURN size_t chi_find_cs(const chi_string* chi_str, size_t offset, const chi_string* delim)
{
    chi_str_assert(chi_str);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, delim->data, delim->size, false);
}

CHI_CHECK_RETURN size_t chi_find_sv(const chi_string* chi_str, size_t offset, chi_string_view delim)
{
    chi_str_assert(chi_str);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, delim.data, delim.size, false);
}

CHI_CHECK_RETURN size_t chi_rfind_c(const chi_string* chi_str, size_t offset, char delim)
{
    chi_str_assert(chi_str);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, &delim, 1, true);
}

CHI_CHECK_RETURN size_t chi_rfind(const chi_string* chi_str, size_t offset, const char* delim)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(delim, chi_npos);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, delim, strlen(delim), true);
}

CHI_API CHI_CHECK_RETURN size_t chi_rfind_cs(const chi_string* chi_str, size_t offset, const chi_string* delim)
{
    chi_str_assert(chi_str);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, delim->data, delim->size, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_rfind_sv(const chi_string* chi_str, size_t offset, chi_string_view delim)
{
    chi_str_assert(chi_str);
    return _chi_find_pattern(chi_str->data, chi_str->size, offset, delim.data, delim.size, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_find_first_of(const chi_string* chi_str, size_t offset, const char* delims)
{
    chi_str_assert(chi_str);
    return _chi_find_first_last_of_not_of(chi_str->data, chi_str->size, offset, delims, false, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_find_last_of(const chi_string* chi_str, size_t offset, const char* delims)
{
    chi_str_assert(chi_str);
    return _chi_find_first_last_of_not_of(chi_str->data, chi_str->size, offset, delims, true, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_find_first_not_of(const chi_string* chi_str, size_t offset, const char* delims)
{
    chi_str_assert(chi_str);
    return _chi_find_first_last_of_not_of(chi_str->data, chi_str->size, offset, delims, false, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_find_last_not_of(const chi_string* chi_str, size_t offset, const char* delims)
{
    chi_str_assert(chi_str);
    return _chi_find_first_last_of_not_of(chi_str->data, chi_str->size, offset, delims, true, true);
}

CHI_API CHI_CHECK_RETURN bool chi_contains(const chi_string* chi_str, char delim)
{
    return chi_find_c(chi_str, 0, delim) != chi_npos;
}

CHI_API CHI_CHECK_RETURN int chi_toi(const chi_string* chi_str)
{
    chi_str_assert(chi_str); 
    data_assert(chi_str->data); 
    return atoi(chi_str->data);
}

CHI_API CHI_CHECK_RETURN float chi_tof(const chi_string* chi_str)
{
    chi_str_assert(chi_str); 
    data_assert(chi_str->data); 
    char* end; 
    return strtof(chi_str->data, &end);
}

CHI_API CHI_CHECK_RETURN double chi_tod(const chi_string* chi_str)
{
    chi_str_assert(chi_str); 
    data_assert(chi_str->data); 
    char* end;
    return strtod(chi_str->data, &end);
}

CHI_API CHI_CHECK_RETURN long chi_tol(const chi_string* chi_str)
{
    chi_str_assert(chi_str); 
    data_assert(chi_str->data); 
    char* end; 
    return strtol(chi_str->data, &end, 10);
}

CHI_API CHI_CHECK_RETURN long long chi_toll(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data); 
    char* end; 
    return strtoll(chi_str->data, &end, 10);
}

CHI_API CHI_CHECK_RETURN unsigned long chi_toul(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data);
    char* end; 
    return strtoul(chi_str->data, &end, 10);
}

CHI_API CHI_CHECK_RETURN unsigned long long chi_toull(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data); 
    char* end; 
    return strtoull(chi_str->data, &end, 10);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_i(int value)
{
    to_chi_str_impl(12, "%d", value);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_f(float value)
{
    to_chi_str_impl(50, "%.6f", value);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_d(double value)
{
    to_chi_str_impl(50, "%.15f", value);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_l(long value)
{
    to_chi_str_impl(22, "%ld", value);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_ll(long long value)
{
    to_chi_str_impl(22, "%lld", value);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_ul(unsigned long value)
{
    to_chi_str_impl(22, "%lu", value);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_ull(unsigned long long value)
{
    to_chi_str_impl(22, "%llu", value);
}

CHI_API void chi_swap(chi_string* lhs, chi_string* rhs)
{
    chi_str_assert(lhs);
    chi_str_assert(rhs);
    chi_string temp = *lhs; 
    *lhs = *rhs; 
    *rhs = temp;
}

CHI_API CHI_CHECK_RETURN size_t chi_hash(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(chi_str->data, 0);
    return _hash(_hash_offset_basis, (const unsigned char*)(chi_str->data), chi_str->size);
}

CHI_API chi_string* chi_for_each_ip(chi_string* chi_str, size_t begin, size_t end, void(*pred)(char*))
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
        pred(&chi_str->data[i]);

    return chi_str;
}

CHI_API chi_string* chi_for_each(chi_string* chi_str, void(*pred)(char*))
{
    CHECK_NULL(chi_str);
    return chi_for_each_ip(chi_str, 0, chi_str->size, pred);
}

CHI_API chi_string* chi_to_upper_ip(chi_string* chi_str, size_t begin, size_t end)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (chi_str->data[i] >= 97 && chi_str->data[i] <= 122)
            chi_str->data[i] -= 32;

    return chi_str;
}

CHI_API chi_string* chi_to_upper(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    return chi_to_upper_ip(chi_str, 0, chi_str->size);
}

CHI_API chi_string* chi_to_lower_ip(chi_string* chi_str, size_t begin, size_t end)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (chi_str->data[i] >= 65 && chi_str->data[i] <= 90)
            chi_str->data[i] += 32;
    return chi_str;
}

CHI_API chi_string* chi_to_lower(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    return chi_to_lower_ip(chi_str, 0, chi_str->size);
}

CHI_API chi_string* chi_switch_cases_ip(chi_string* chi_str, size_t begin, size_t end)
{
    CHECK_NULL(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
    {
        if (chi_str->data[i] >= 97 && chi_str->data[i] <= 122)
            chi_str->data[i] -= 32;
        else if (chi_str->data[i] >= 65 && chi_str->data[i] <= 90)
            chi_str->data[i] += 32;
    }
    return chi_str;
}

CHI_API chi_string* chi_switch_cases(chi_string* chi_str)
{
    CHECK_NULL(chi_str);
    return chi_switch_cases_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_ip(const chi_string* chi_str, char ch, size_t begin, size_t end)
{
    return _chi_count_data(chi_str->data, chi_str->size, &ch, 1, begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_count(const chi_string* chi_str, char ch)
{
    chi_str_assert(chi_str);
    return _chi_count_data(chi_str->data, chi_str->size, &ch, 1, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_s_ip(const chi_string* chi_str, const char* data, size_t begin, size_t end)
{
    data_assert(data);
    return _chi_count_data(chi_str->data, chi_str->size, data, strlen(data), begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_s(const chi_string* chi_str, const char* data)
{
    chi_str_assert(chi_str);
    data_assert(data);
    return _chi_count_data(chi_str->data, chi_str->size, data, strlen(data), 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_cs_ip(const chi_string* chi_str, const chi_string* data, size_t begin, size_t end)
{
    data_assert(data);
    return _chi_count_data(chi_str->data, chi_str->size, data->data, data->size, begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_cs(const chi_string* chi_str, const chi_string* data)
{
    chi_str_assert(chi_str);
    data_assert(data);
    return _chi_count_data(chi_str->data, chi_str->size, data->data, data->size, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_sv_ip(const chi_string* chi_str, chi_string_view data, size_t begin, size_t end)
{
    data_assert(data.data);
    return _chi_count_data(chi_str->data, chi_str->size, data.data, data.size, begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_sv(const chi_string* chi_str, chi_string_view data)
{
    chi_str_assert(chi_str);
    data_assert(data.data);
    return _chi_count_data(chi_str->data, chi_str->size, data.data, data.size, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN size_t chi_count_if_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    size_t counter = 0;

    for (size_t i = begin; i < end; ++i)
        if (pred(chi_str->data[i]))
            counter++;
    return counter;
}

CHI_API CHI_CHECK_RETURN size_t chi_count_if(const chi_string* chi_str, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    return chi_count_if_ip(chi_str, 0, chi_str->size, pred);
}

CHI_API CHI_CHECK_RETURN bool chi_all_of_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (!pred(chi_str->data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_all_of(const chi_string* chi_str, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    return chi_all_of_ip(chi_str, 0, chi_str->size, pred);
}

CHI_API CHI_CHECK_RETURN bool chi_any_of_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (pred(chi_str->data[i]))
            return true;
    return false;
}

CHI_API CHI_CHECK_RETURN bool chi_any_of(const chi_string* chi_str, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    return chi_any_of_ip(chi_str, 0, chi_str->size, pred);
}

CHI_API CHI_CHECK_RETURN bool chi_none_of_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    data_assert(chi_str->data);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (pred(chi_str->data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_none_of(const chi_string* chi_str, bool(*pred)(char))
{
    chi_str_assert(chi_str);
    return chi_none_of_ip(chi_str, 0, chi_str->size, pred);
}

CHI_API chi_string* chi_shuffle_ip(chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, end, begin);

    for (size_t i = begin; i < end; ++i)
    {
        size_t j = _chi_random_in_range((uint32_t)begin, (uint32_t)end);
        char temp = chi_str->data[i]; 
        chi_str->data[i] = chi_str->data[j]; 
        chi_str->data[j] = temp;
    }
    return chi_str;
}

CHI_API chi_string* chi_shuffle(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_shuffle_ip(chi_str, 0, chi_str->size);
}

CHI_API chi_string* chi_reverse_ip(chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_NULL2(chi_str->data, chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);

    for (size_t i = 0; i < (end - begin) / 2; ++i)
    {
        char temp = chi_str->data[begin + i]; 
        chi_str->data[begin + i] = chi_str->data[end - i - 1]; 
        chi_str->data[end - i - 1] = temp;
    }

    return chi_str;
}

CHI_API chi_string* chi_reverse(chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_reverse_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN char chi_sample_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    return chi_str->data[_chi_random_in_range((uint32_t)begin, (uint32_t)end)];
}

CHI_API CHI_CHECK_RETURN char chi_sample(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_sample_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_islower_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!islower(chi_str->data[i]))
            ignore_spaces_or_return(chi_str->data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_islower(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_islower_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_isupper_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isupper(chi_str->data[i]))
            ignore_spaces_or_return(chi_str->data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_isupper(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_isupper_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_isalnum_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isalnum(chi_str->data[i]))
            ignore_spaces_or_return(chi_str->data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_isalnum(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_isalnum_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_isalpha_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isalpha(chi_str->data[i]))
            ignore_spaces_or_return(chi_str->data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_isalpha(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_isalpha_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_isdigit_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isdigit(chi_str->data[i]))
            ignore_spaces_or_return(chi_str->data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_isdigit(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_isdigit_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_isprintable_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isprint(chi_str->data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_isprintable(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_isprintable_ip(chi_str, 0, chi_str->size);
}

CHI_API CHI_CHECK_RETURN bool chi_isspace_ip(const chi_string* chi_str, size_t begin, size_t end)
{
    chi_str_assert(chi_str);
    CHECK_BEGIN_AND_END(chi_str->size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isspace(chi_str->data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_isspace(const chi_string* chi_str)
{
    chi_str_assert(chi_str);
    return chi_islower_ip(chi_str, 0, chi_str->size);
}

CHI_API void chi_format(chi_string* chi_str, ...)
{
    chi_str_assert(chi_str);
    
    va_list arg_list; 
    va_start(arg_list, chi_str); 
    _chi_vformat(chi_str, arg_list); 
    va_end(arg_list);
}

CHI_API CHI_CHECK_RETURN chi_arena* chi_use_arena()
{
    return _arena_array_push(&s_user_arenas);
}

CHI_API void chi_unuse_arena(chi_arena* arena)
{
    _arena_array_remove(&s_user_arenas, arena, true);
}

CHI_API CHI_CHECK_RETURN bool chi_arena_is_using(const chi_arena* arena)
{
    if (arena == NULL)
        return false;
    return arena->is_active;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_arena_create_empty_str(chi_arena* arena, size_t size)
{
    CHECK_NULL(arena);
    if (!arena->is_active)
        return NULL;
    return _chi_arena_create_base(arena, size);
}

CHI_API CHI_CHECK_RETURN chi_string* chi_arena_create_str(chi_arena* arena, const char* data, ...)
{
    CHECK_NULL(arena);
    if (!arena->is_active)
        return NULL;
    CHECK_NULL2(data, _chi_arena_create_base(arena, 0));
    chi_string* result = _chi_arena_create_base(arena, strlen(data));
    copy_to_chi(result, data);
    vformat_from(data, result);
    return result;
}

CHI_API CHI_CHECK_RETURN void* chi_arena_create_ptr(chi_arena* arena, size_t size)
{
    CHECK_NULL(arena);
    if (size == 0)
        return NULL;
    if (!arena->is_active)
        return NULL;
    void* buf = malloc(size);
    alloc_assert(buf);
    _arena_push(arena, buf, false);
    return buf;
}

CHI_API void chi_arena_free(chi_arena* arena, void* ptr_or_chi_str)
{
    _chi_arena_free_base(arena, ptr_or_chi_str);
}

CHI_API bool chi_arena_load(chi_arena* arena, void* ptr, bool is_chi)
{
    if (arena->is_active)
    {
        _top_arena_pop(ptr);
        _arena_push(arena, ptr, is_chi);
        return true;
    }
    return false;
}

CHI_API CHI_CHECK_RETURN chi_string* chi_arena_substring(chi_arena* arena, const chi_string* chi_str, size_t offset, size_t length)
{
    chi_assert(arena, "");
    CHECK_NULL(chi_str);
    CHECK_NULL(chi_str->data);
    chi_assert(offset < chi_str->size, "offset is out of the range!");

    // same with CHECK_N but always enable
    if (offset + length > chi_str->size)
        length = chi_str->size - offset;

    chi_string* dest = chi_arena_create_empty_str(arena, length);
    chi_reset_n(dest, chi_str->data + offset, length);
    return dest;
}

CHI_API CHI_CHECK_RETURN chi_string_view chi_sv(const char* data)
{
    CHECK_NULL2(data, CHI_SVNULL);
    chi_string_view view = { .data = data, .size = strlen(data) };
    return view;
}

CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_n(const char* data, size_t size)
{
    CHECK_NULL2(data, CHI_SVNULL);
    CHECK_N(data, size);
    chi_string_view view = { .data = data, .size = size };
    return view;
}

CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_from_chi_s(const chi_string* chi_str)
{
    chi_string_view view = { 0 };
    CHECK_NULL2(chi_str, view);
    return chi_sv_n(chi_str->data, chi_str->size);
}

CHI_API CHI_CHECK_RETURN const char* chi_sv_get(chi_string_view sv)
{
    return sv.data;
}

CHI_API CHI_CHECK_RETURN char chi_sv_at(chi_string_view sv, size_t idx)
{
    chi_assert(sv.size > idx, "Index out of the range!");
    return sv.data[idx];
}

CHI_API CHI_CHECK_RETURN const char* chi_sv_begin(chi_string_view sv)
{
    return sv.data;
}

CHI_API CHI_CHECK_RETURN const char* chi_sv_end(chi_string_view sv)
{
    return sv.data + sv.size;
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_size(chi_string_view sv)
{
    return sv.size;
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_length(chi_string_view sv)
{
    return sv.size;
}

CHI_API CHI_CHECK_RETURN char chi_sv_front(chi_string_view sv)
{
    chi_assert(0 != sv.size, "chi_sv_front() call with empty chi_string!");
    return sv.data[0];
}

CHI_API CHI_CHECK_RETURN char chi_sv_back(chi_string_view sv)
{
    chi_assert(0 != sv.size, "chi_sv_back() call with empty chi_string!");
    return sv.data[sv.size - 1];
}

CHI_API CHI_CHECK_RETURN bool chi_sv_equal(chi_string_view lhs, chi_string_view rhs)
{
    return _chi_equal_data(lhs.data, rhs.data, lhs.size, rhs.size, true);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_equal_s(chi_string_view lhs, const char* rhs)
{
    equal_null_check(lhs.data, rhs, true);
    return _chi_equal_data(lhs.data, rhs, lhs.size, strlen(rhs), false);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_equal_cs(chi_string_view lhs, chi_string* rhs)
{
    CHECK_NULL2(rhs, false);
    return _chi_equal_data(lhs.data, rhs->data, lhs.size, rhs->size, true);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_equal_ignorecase(chi_string_view lhs, chi_string_view rhs)
{
    return _chi_equal_data_ignorecase(lhs.data, rhs.data, lhs.size, rhs.size, true);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_equal_ignorecase_s(chi_string_view lhs, const char* rhs)
{
    equal_data_check(lhs.data, rhs);
    return _chi_equal_data_ignorecase(lhs.data, rhs, lhs.size, strlen(rhs), false);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_equal_ignorecase_cs(chi_string_view lhs, chi_string* rhs)
{
    CHECK_NULL2(rhs, false);
    return _chi_equal_data_ignorecase(lhs.data, rhs->data, lhs.size, rhs->size, true);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_starts_with(chi_string_view sv, char ch)
{
    CHECK_NULL2(sv.data, false);
    return (sv.data[0] == ch);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_ends_with(chi_string_view sv, char ch)
{
    CHECK_NULL2(sv.data, false);
    return (sv.data[sv.size - 1] == ch);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_is_empty(chi_string_view sv)
{
    CHECK_NULL2(sv.data, true);
    if (sv.size == 0)
        return true;
    return false;
}

CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_substring(chi_string_view sv, size_t offset, size_t length)
{
    chi_assert(sv.size > offset, "Offset out of the range!");
    if (offset + length > sv.size)
        length = sv.size - offset;
    return chi_sv_n(sv.data + offset, length);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_c(chi_string_view sv, size_t offset, char delim)
{
    return _chi_find_pattern(sv.data, sv.size, offset, &delim, 1, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find(chi_string_view sv, size_t offset, const char* delim)
{
    CHECK_NULL2(delim, chi_npos);
    return _chi_find_pattern(sv.data, sv.size, offset, delim, strlen(delim), false);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_cs(chi_string_view sv, size_t offset, const chi_string* delim)
{
    CHECK_NULL2(delim, chi_npos);
    return _chi_find_pattern(sv.data, sv.size, offset, delim->data, delim->size, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_sv(chi_string_view sv, size_t offset, chi_string_view delim)
{
    return _chi_find_pattern(sv.data, sv.size, offset, delim.data, delim.size, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind_c(chi_string_view sv, size_t offset, char delim)
{
    return _chi_find_pattern(sv.data, sv.size, offset, &delim, 1, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind(chi_string_view sv, size_t offset, const char* delim)
{
    CHECK_NULL2(delim, chi_npos);
    return _chi_find_pattern(sv.data, sv.size, offset, delim, strlen(delim), true);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind_cs(chi_string_view sv, size_t offset, const chi_string* delim)
{
    CHECK_NULL2(delim, chi_npos);
    return _chi_find_pattern(sv.data, sv.size, offset, delim->data, delim->size, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind_sv(chi_string_view sv, size_t offset, chi_string_view delim)
{
    return _chi_find_pattern(sv.data, sv.size, offset, delim.data, delim.size, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_first_of(chi_string_view sv, size_t offset, const char* delims)
{
    return _chi_find_first_last_of_not_of(sv.data, sv.size, offset, delims, false, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_last_of(chi_string_view sv, size_t offset, const char* delims)
{
    return _chi_find_first_last_of_not_of(sv.data, sv.size, offset, delims, true, false);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_first_not_of(chi_string_view sv, size_t offset, const char* delims)
{
    return _chi_find_first_last_of_not_of(sv.data, sv.size, offset, delims, false, true);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_find_last_not_of(chi_string_view sv, size_t offset, const char* delims)
{
    return _chi_find_first_last_of_not_of(sv.data, sv.size, offset, delims, true, true);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_contains(chi_string_view sv, char delim)
{
    return chi_sv_find_c(sv, 0, delim) != chi_npos;
}

CHI_API CHI_CHECK_RETURN int chi_sv_toi(chi_string_view sv)
{
    data_assert(sv.data); 
    return atoi(sv.data);
}

CHI_API CHI_CHECK_RETURN float chi_sv_tof(chi_string_view sv)
{
    data_assert(sv.data); 
    char* end; 
    return strtof(sv.data, &end);
}

CHI_API CHI_CHECK_RETURN double chi_sv_tod(chi_string_view sv)
{
    data_assert(sv.data); 
    char* end; 
    return strtod(sv.data, &end);
}

CHI_API CHI_CHECK_RETURN long chi_sv_tol(chi_string_view sv)
{
    data_assert(sv.data); 
    char* end; 
    return strtol(sv.data, &end, 10);
}

CHI_API CHI_CHECK_RETURN long long chi_sv_toll(chi_string_view sv)
{
    data_assert(sv.data);
    char* end; 
    return strtoll(sv.data, &end, 10);
}

CHI_API CHI_CHECK_RETURN unsigned long chi_sv_toul(chi_string_view sv)
{
    data_assert(sv.data); 
    char* end; 
    return strtoul(sv.data, &end, 10);
}

CHI_API CHI_CHECK_RETURN unsigned long long chi_sv_toull(chi_string_view sv)
{
    data_assert(sv.data); 
    char* end; 
    return strtoull(sv.data, &end, 10);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_hash(chi_string_view sv)
{
    CHECK_NULL2(sv.data, 0);
    return _hash(_hash_offset_basis, (const unsigned char*)(sv.data), sv.size);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_ip(chi_string_view sv, char ch, size_t begin, size_t end)
{
    return _chi_count_data(sv.data, sv.size, &ch, 1, begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count(chi_string_view sv, char ch)
{
    return _chi_count_data(sv.data, sv.size, &ch, 1, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_s_ip(chi_string_view sv, const char* data, size_t begin, size_t end)
{
    data_assert(data);
    return _chi_count_data(sv.data, sv.size, data, strlen(data), begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_s(chi_string_view sv, const char* data)
{
    data_assert(data);
    return _chi_count_data(sv.data, sv.size, data, strlen(data), 0, sv.size);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_cs_ip(chi_string_view sv, const chi_string* data, size_t begin, size_t end)
{
    return _chi_count_data(sv.data, sv.size, data->data, data->size, begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_cs(chi_string_view sv, const chi_string* data)
{
    return _chi_count_data(sv.data, sv.size, data->data, data->size, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_sv_ip(chi_string_view sv, chi_string_view data, size_t begin, size_t end)
{
    return _chi_count_data(sv.data, sv.size, data.data, data.size, begin, end);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_sv(chi_string_view sv, chi_string_view data)
{
    return _chi_count_data(sv.data, sv.size, data.data, data.size, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_if_ip(chi_string_view sv, size_t begin, size_t end, bool(*pred)(char))
{
    data_assert(sv.data);
    CHECK_BEGIN_AND_END(sv.size, begin, end);

    size_t counter = 0;

    for (size_t i = begin; i < end; ++i)
        if (pred(sv.data[i]))
            counter++;
    return counter;
}

CHI_API CHI_CHECK_RETURN size_t chi_sv_count_if(chi_string_view sv, bool(*pred)(char))
{
    return chi_sv_count_if_ip(sv, 0, sv.size, pred);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_all_of_ip(chi_string_view sv, size_t begin, size_t end, bool(*pred)(char))
{
    data_assert(sv.data);
    CHECK_BEGIN_AND_END(sv.size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (!pred(sv.data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_all_of(chi_string_view sv, bool(*pred)(char))
{
    return chi_sv_all_of_ip(sv, 0, sv.size, pred);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_any_of_ip(chi_string_view sv, size_t begin, size_t end, bool(*pred)(char))
{
    data_assert(sv.data);
    CHECK_BEGIN_AND_END(sv.size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (pred(sv.data[i]))
            return true;
    return false;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_any_of(chi_string_view sv, bool(*pred)(char))
{
    return chi_sv_any_of_ip(sv, 0, sv.size, pred);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_none_of_ip(chi_string_view sv, size_t begin, size_t end, bool(*pred)(char))
{
    data_assert(sv.data);
    CHECK_BEGIN_AND_END(sv.size, begin, end);

    for (size_t i = begin; i < end; ++i)
        if (pred(sv.data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_none_of(chi_string_view sv, bool(*pred)(char))
{
    return chi_sv_none_of_ip(sv, 0, sv.size, pred);
}

CHI_API CHI_CHECK_RETURN char chi_sv_sample_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    return sv.data[_chi_random_in_range((uint32_t)begin, (uint32_t)end)];
}

CHI_API CHI_CHECK_RETURN char chi_sv_sample(chi_string_view sv)
{
    return chi_sv_sample_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_islower_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!islower(sv.data[i]))
            ignore_spaces_or_return(sv.data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_islower(chi_string_view sv)
{
    return chi_sv_islower_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isupper_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isupper(sv.data[i]))
            ignore_spaces_or_return(sv.data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isupper(chi_string_view sv)
{
    return chi_sv_isupper_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isalnum_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isalnum(sv.data[i]))
            ignore_spaces_or_return(sv.data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isalnum(chi_string_view sv)
{
    return chi_sv_isalnum_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isalpha_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isalpha(sv.data[i]))
            ignore_spaces_or_return(sv.data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isalpha(chi_string_view sv)
{
    return chi_sv_isalpha_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isdigit_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isdigit(sv.data[i]))
            ignore_spaces_or_return(sv.data[i], return false);
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isdigit(chi_string_view sv)
{
    return chi_sv_isdigit_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isprintable_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isprint(sv.data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isprintable(chi_string_view sv)
{
    return chi_sv_isprintable_ip(sv, 0, sv.size);
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isspace_ip(chi_string_view sv, size_t begin, size_t end)
{
    CHECK_BEGIN_AND_END(sv.size, begin, end);
    for (size_t i = begin; i < end; ++i)
        if (!isspace(sv.data[i]))
            return false;
    return true;
}

CHI_API CHI_CHECK_RETURN bool chi_sv_isspace(chi_string_view sv)
{
    return chi_sv_islower_ip(sv, 0, sv.size);
}
