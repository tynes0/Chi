#pragma once

#ifndef _CHI_
#define _CHI_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

//  #### todolist ####
// fix assertions
// update for clang
// ignore space func for ctype functions

#ifndef CHI_API
#define CHI_API // may be change
#endif // CHI_API

#if defined(__GNUC__) || defined(__clang__)
#define CHI_CHECK_RETURN __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#define CHI_CHECK_RETURN _Check_return_
#else 
#define CHI_CHECK_RETURN
#endif

#if defined(_MSC_VER)
#define _CHI_MSVC
#endif // _MSC_VER

#if defined(_CHI_MSVC)
#if defined(__CUDACC__) || defined(__INTEL_COMPILER)
#define _CHI_PRAGMA(PRAGMA) __pragma(PRAGMA)
#else
#define _CHI_PRAGMA(PRAGMA) _Pragma(#PRAGMA)
#endif // defined(__CUDACC__) || defined(__INTEL_COMPILER)
#else // defined(_CHI_MSVC)
#define _CHI_PRAGMA(PRAGMA)
#endif // defined(_CHI_MSVC)

#if defined(_DEBUG)
#define _CHI_DEBUG 1
#endif 

#if defined (__cplusplus)
#define _CHI_CPP
#endif // defined (__cplusplus)

#ifdef _CHI_CPP
#define _CHI_HEADER_BEGIN       _CHI_PRAGMA(warning(push)) _CHI_PRAGMA(warning(disable : 4190)) extern "C" {
#define _CHI_HEADER_END         } _CHI_PRAGMA(warning(pop))
#else // !_CHI_CPPp
#define _CHI_HEADER_BEGIN
#define _CHI_HEADER_END
#endif // _CHI_CPP

_CHI_HEADER_BEGIN

#ifdef _CHI_DEBUG
/**
 * @brief Shows an assertion message and terminates the program.
 *
 * @param message The assertion message.
 * @param file The file where the assertion failed.
 * @param line The line number where the assertion failed.
 */
CHI_API __declspec(noreturn) void _chi_assert_show_message(const char* message, const char* file, int line);

/**
 * @brief Asserts that the given expression is true. If not, shows a message and terminates the program.
 *
 * @param expression The expression to evaluate.
 * @param message The message to display if the assertion fails.
 */
#define chi_assert(expression, message)                                 \
    do                                                                  \
    {                                                                   \
        if (!(expression))                                              \
        {                                                               \
            _chi_assert_show_message(message, __FILE__, __LINE__);      \
        }                                                               \
    } while (0)
#else
/**
 * @brief Asserts that the given expression is true. Does nothing in release mode.
 *
 * @param expression The expression to evaluate.
 * @param message The message to display if the assertion fails.
 */
#define chi_assert(expression, message)
#endif // _CHI_DEBUG

 /* main structs begin */

 /**
  * @brief Represents a dynamically allocated string.
  */
typedef struct _chi_string chi_string;

/**
 * @brief Represents a view into a string with a pointer to the data and its size.
 */
struct _chi_string_view
{
    const char* data; /**< Pointer to the data */
    const size_t size; /**< Size of the view */
};

/**
 * @brief Alias for the _chi_string_view structure.
 */
typedef struct _chi_string_view chi_string_view;

/* main structs end */

/**
 * @brief Represents an invalid or not found position for chi functions with size_t return type.
 */
static const size_t chi_npos = ((size_t)-1);

/**
 * @brief Represents a null `chi_string` with zero size, capacity and null data.
 */
static const chi_string* chi_snull;

/**
 * @brief Represents a null `chi_string_view` with zero size and null data.
 */
static const chi_string_view chi_svnull = { 0, 0 };

/* helper macros begin */

/**
 * @brief Creates an empty `chi_string`.
 *
 * @return Returns a `chi_string` instance.
 */
#define chichi() chi_create_empty(0)

/**
 * @brief Creates a `chi_string` from a C-string.
 *
 * @param data The C-string to create the `chi_string` from.
 * @return Returns a `chi_string` instance.
 */
#define CHI_S(data) chi_create(data)

/**
 * @brief Creates a `chi_string_view` from a C-string.
 *
 * @param data The C-string to create the `chi_string_view` from.
 * @return Returns a `chi_string_view` instance.
 */
#define CHI_SV(data) chi_sv_create(data)

/**
 * @brief Returns a null `chi_string`.
 *
 * @return Returns a `chi_string` instance representing a null string.
 */
#define CHI_SNULL chi_snull

 /**
  * @brief Creates a null `chi_string`.
  *
  * @return Returns a `chi_string` instance representing a null string.
  */
#define CHI_CSNULL chi_create(NULL)

/**
 * @brief Represents a null `chi_string_view`.
 */
#define CHI_SVNULL chi_svnull

/**
 * @brief Format specifier for printing a `chi_string` or `chi_string_view` with `printf`.
 */
#define CHI_FMT "%.*s"

/**
 * @brief Arguments for printing a `chi_string` with `printf`.
 *
 * @param chi_s The `chi_string` instance.
 * @return Returns the size and data of the `chi_string` for use with `printf`.
 */
#define CHI_ARG(chi_s) (unsigned int)chi_size(chi_s), chi_get(chi_s)

/**
 * @brief Arguments for printing a `chi_string_view` with `printf`.
 *
 * @param chi_s The `chi_string_view` instance.
 * @return Returns the size and data of the `chi_string_view` for use with `printf`.
 */
#define CHI_SV_ARG(chi_s) (unsigned int)chi_sv_size(chi_s), chi_sv_get(chi_s)

/**
 * @brief Accesses the character at the specified index in a `chi_string`.
 *
 * @param chi The `chi_string` instance.
 * @param idx The index of the character to access.
 * @return Returns a reference to the character at the specified index.
 */
#define CHI_AT(chi, idx) (*chi_at(chi, idx))

/**
 * @brief Accesses the first character in a `chi_string`.
 *
 * @param chi The `chi_string` instance.
 * @return Returns a reference to the first character.
 */
#define CHI_FRONT(chi) (*chi_front(chi))

/**
 * @brief Accesses the last character in a `chi_string`.
 *
 * @param chi The `chi_string` instance.
 * @return Returns a reference to the last character.
 */
#define CHI_BACK(chi) (*chi_back(chi))

                  
/* helper macros end */

/**
 * @brief Calculates a new capacity for a `chi_string` based on the old and new capacities.
 * Note: This is a non-standard helper function designed to manage memory efficiently.
 * 
 * @param old_cap The current capacity of the `chi_string`.
 * @param new_cap The requested new capacity.
 * @return Returns the calculated new capacity, which is usually larger than the requested new capacity to minimize reallocations.
 */
CHI_API CHI_CHECK_RETURN size_t _chi_calculate_capacity(size_t old_cap, size_t new_cap);

/**
 * @brief Computes a hash value for a null-terminated C string.
 * Note: This function can be used to generate a hash for strings to be used in hash tables or for other hashing purposes.
 * 
 * @param str The null-terminated C string to be hashed.
 * @return Returns the hash value of the string.
 */
CHI_API CHI_CHECK_RETURN size_t chi_hash_s(const char* str);

/**
 * @brief Enables or disables the check for n parameters in chi functions.
 * Note: By default, the check is disabled.
 * 
 * @param enable A boolean value to enable (true) or disable (false) the check.
 */
CHI_API void chi_enable_check_n(bool enable);

/* CHI STRING BEGIN */

/**
 * @brief Creates a new chi_string instance with the given data.
 * 
 * @param data The data to initialize the string with.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create(const char* data);

/**
 * @brief Creates a new chi_string instance with the given data and size.
 * 
 * @param data The data to initialize the string with.
 * @param size The size of the data.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create_n(const char* data, size_t size);

/**
 * @brief Creates a new empty chi_string instance with the specified size.
 * 
 * @param size The size of the string to create.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create_empty(size_t size);

/**
 * @brief Creates a new empty chi_string instance with the specified capacity.
 *
 * @param capacity The capacity of the string to create.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create_with_capacity(size_t capacity);

/**
 * @brief Creates a new chi_string instance filled with the specified character.
 * 
 * @param ch The character to fill the string with.
 * @param size The size of the string to create.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create_and_fill(char ch, size_t size);

/**
 * @brief Creates a new chi_string instance by copying an existing chi_string.
 * 
 * @param chi_str The chi_string to copy.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create_from_chi_s(const chi_string* chi_str);

/**
 * @brief Creates a new chi_string instance from a chi_string_view.
 * 
 * @param chi_sv The chi_string_view to copy from.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_create_from_chi_sv(chi_string_view chi_sv);

/**
 * @brief Wraps an existing character array in a chi_string.
 * Note: This function may be unsafe if the data is not properly managed.
 * 
 * @param data The character array to wrap.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_make_chi(char* data);

/**
 * @brief Wraps an existing character array in a chi_string with a specified size.
 * Note: This function may be unsafe if the data is not properly managed.
 * 
 * @param data The character array to wrap.
 * @param n The size of the character array.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_make_chi_n(char* data, size_t n);

/**
 * @brief Wraps an existing character array in a chi_string with a specified capacity.
 * Note: This function may be unsafe if the data is not properly managed.
 * 
 * @param data The character array to wrap.
 * @param capacity The capacity of the character array.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_make_chi_c(char* data, size_t capacity);

/**
 * @brief Wraps an existing character array in a chi_string with a specified size and capacity.
 * Note: This function may be unsafe if the data is not properly managed.
 * 
 * @param data The character array to wrap.
 * @param n The size of the character array.
 * @param capacity The capacity of the character array.
 * @return A pointer to the newly created chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_make_chi_n_c(char* data, size_t n, size_t capacity);

/**
 * @brief Begins a scope for chi_string operations.
 */
CHI_API void chi_begin_scope();

/**
 * @brief Ends a scope for chi_string operations.
 */
CHI_API void chi_end_scope();

/**
 * @brief Cleans up resources used by the chi_string library.
 */
CHI_API void chi_cleanup();

/**
 * @brief Frees a chi_string instance.
 * 
 * @param chi_str The chi_string to free.
 */
CHI_API void chi_free(chi_string* chi_str);

/**
 * @brief Frees an array of chi_string pointers.
 *
 * This function frees each individual `chi_string` in the provided array and then
 * frees the array itself. After freeing, the pointer to the array is set to `NULL`
 * to prevent dangling pointers.
 *
 * @param chi_str_array The array of `chi_string*` to be freed.
 * @param size The number of elements in the `chi_string` array.
 */
CHI_API void chi_str_array_free(chi_string** chi_str_array, size_t size);

/**
 * @brief Clears the contents of a chi_string instance.
 * 
 * @param chi_str The chi_string to clear.
 */
CHI_API void chi_clear(chi_string* chi_str);

/**
 * @brief Releases the internal character array of a chi_string.
 * 
 * @param chi_str The chi_string to release.
 */
CHI_API char* chi_release(chi_string* chi_str);

/**
 * @brief Resizes a chi_string instance to the specified size.
 * 
 * @param chi_str The chi_string to resize.
 * @param new_size The new size of the chi_string.
 */
CHI_API void chi_resize(chi_string* chi_str, size_t new_size);

/**
 * @brief Reserves capacity for a chi_string instance.
 * 
 * @param chi_str The chi_string to reserve capacity for.
 * @param new_cap The new capacity of the chi_string.
 */
CHI_API void chi_reserve(chi_string* chi_str, size_t new_cap);

/**
 * @brief Shrinks the capacity of a chi_string instance to fit its size.
 * 
 * @param chi_str The chi_string to shrink.
 */
CHI_API void chi_shrink_to_fit(chi_string* chi_str);

/**
 * @brief Resets a chi_string instance with new data.
 * 
 * @param chi_str The chi_string to reset.
 * @param data The new data for the chi_string.
 * @return A pointer to the reset chi_string.
 */
CHI_API chi_string* chi_reset(chi_string* chi_str, const char* data);

/**
 * @brief Resets a chi_string instance with new data and size.
 * 
 * @param chi_str The chi_string to reset.
 * @param data The new data for the chi_string.
 * @param n The size of the new data.
 * @return A pointer to the reset chi_string.
 */
CHI_API chi_string* chi_reset_n(chi_string* chi_str, const char* data, size_t size);

/**
 * @brief Resets the contents of a chi_string to match another chi_string.
 *
 * This function resets the contents of the destination chi_string (`chi_str`) to match
 * the data and size of the source chi_string (`data`). The destination chi_string is modified
 * in place. If the source chi_string is NULL, the function asserts with an error message.
 *
 * @param chi_str The chi_string to be reset.
 * @param data The chi_string whose contents will be copied to `chi_str`.
 * @return A pointer to the modified `chi_str`.
 */
CHI_API chi_string* chi_reset_cs(chi_string* chi_str, const chi_string* data);

/**
 * @brief Resets a chi_string instance with another chi_string.
 * 
 * @param chi_str The chi_string to reset.
 * @param chi_string_view to copy from.
 * @return A pointer to the reset chi_string.
 */
CHI_API chi_string* chi_reset_from_sv(chi_string* chi_str, chi_string_view sv);

/**
 * @brief Retrieves the character array of a chi_string instance.
 * 
 * @param chi_str The chi_string to get the data of.
 * @return A pointer to the character array.
 */
CHI_API CHI_CHECK_RETURN char* chi_get(chi_string* chi_str);

/**
 * @brief Returns the underlying C string (null-terminated) from a chi_string.
 *
 * This function returns the internal null-terminated C string stored within a chi_string.
 * If the chi_string is NULL, the function asserts with an error message.
 *
 * @param chi_str The chi_string from which to retrieve the C string.
 * @return A pointer to the internal null-terminated C string.
 */
CHI_API CHI_CHECK_RETURN const char* chi_cstr(const chi_string* chi_str);

/**
 * @brief Accesses the character at the specified index.
 * Note: It is the responsibility of the calling function to ensure that the `idx` parameter is valid.
 * 
 * @param chi_str The `chi_string` from which the character will be accessed.
 * @param idx The 0-based index of the character.
 * @return Returns the address of the character at the specified index; if the index is invalid, undefined behavior may occur.

 */
CHI_API char* chi_at(chi_string* chi_str, size_t idx);

/**
 * @brief Returns the address of the character at the beginning of the `chi_string`.
 * Note: If the `chi_string` is empty, the returned address may be invalid.
 * 
 * @param chi_str The `chi_string` whose beginning is to be returned.
 * @return Returns the address of the character at the beginning of the `chi_string`.
 */
CHI_API char* chi_begin(chi_string* chi_str);

/**
 * @brief Returns the address of the character at the beginning of the `chi_string`.
 * Note: If the `chi_string` is empty, the returned address may be invalid.
 * 
 * @param chi_str The `chi_string` whose beginning is to be returned.
 * @return Returns the address of the character at the beginning of the `chi_string`.
 */
CHI_API char* chi_end(chi_string* chi_str);

/**
 * @brief Appends a character array to a chi_string instance.
 * 
 * @param chi_str The chi_string to append to.
 * @param data The character array to append.
 * @return A pointer to the chi_string after appending.
 */
CHI_API chi_string* chi_append(chi_string* chi_str, const char* data);

/**
 * @brief Appends a character to a chi_string instance.
 * 
 * @param chi_str The chi_string to append to.
 * @param ch The character to append.
 * @return A pointer to the chi_string after appending.
 */
CHI_API chi_string* chi_append_c(chi_string* chi_str, char data);

/**
 * @brief Appends a character array with specified size to a chi_string instance.
 *
 * @param chi_str The chi_string to append to.
 * @param data The character array to append.
 * @param n The size of the character array.
 * @return A pointer to the chi_string after appending.
 */
CHI_API chi_string* chi_append_n(chi_string* chi_str, const char* data, size_t n);

/**
 * @brief Appends another chi_string to a chi_string instance.
 * 
 * @param chi_str The chi_string to append to.
 * @param other The chi_string to append.
 * @return A pointer to the chi_string after appending.
 */
CHI_API chi_string* chi_append_cs(chi_string* lhs, const chi_string* rhs);

/**
 * @brief Appends chi_string_view to a chi_string instance.
 * 
 * @param chi_str The chi_string to append to.
 * @param The chi_string_view to append.
 * @return A pointer to the chi_string after appending.
 */
CHI_API chi_string* chi_append_sv(chi_string* lhs, chi_string_view rhs);

/**
 * @brief Appends a character to a chi_string instance.
 * 
 * @param chi_str The chi_string to append to.
 * @param ch The character to append.
 * @return A pointer to the chi_string after appending.
 */
CHI_API chi_string* chi_push_back(chi_string* chi_str, char data);

/**
 * @brief Removes the last character from the `chi_string`.
 * Note: If the `chi_string` is empty, this function will not modify the string.
 * 
 * @param chi_str The `chi_string` from which the last character will be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_pop_back(chi_string* chi_str);

/**
 * @brief Inserts a single character into the `chi_string` at the specified index.
 *
 * @param chi_str The `chi_string` instance to modify.
 * @param idx The index at which to insert the character.
 * @param data The character to insert.
 * @return Returns the modified `chi_string` with the character inserted.
 */
CHI_API chi_string* chi_insert_c(chi_string* chi_str, size_t idx, char data);

/**
 * @brief Inserts a specified number of characters from a C-string into the `chi_string` at the specified index.
 *
 * @param chi_str The `chi_string` instance to modify.
 * @param idx The index at which to insert the characters.
 * @param data The C-string from which characters will be inserted.
 * @param n The number of characters to insert from the C-string.
 * @return Returns the modified `chi_string` with the characters inserted.
 */
CHI_API chi_string* chi_insert_n(chi_string* chi_str, size_t idx, const char* data, size_t n);

/**
 * @brief Inserts a null-terminated C-string into the `chi_string` at the specified index.
 *
 * @param chi_str The `chi_string` instance to modify.
 * @param idx The index at which to insert the C-string.
 * @param data The null-terminated C-string to insert.
 * @return Returns the modified `chi_string` with the C-string inserted.
 */
CHI_API chi_string* chi_insert(chi_string* chi_str, size_t idx, const char* data);

/**
 * @brief Inserts another `chi_string` into the `chi_string` at the specified index.
 *
 * @param chi_str The `chi_string` instance to modify.
 * @param idx The index at which to insert the `chi_string`.
 * @param data The `chi_string` to insert.
 * @return Returns the modified `chi_string` with the `chi_string` inserted.
 */
CHI_API chi_string* chi_insert_cs(chi_string* chi_str, size_t idx, const chi_string* data);

/**
 * @brief Inserts a `chi_string_view` into the `chi_string` at the specified index.
 *
 * @param chi_str The `chi_string` instance to modify.
 * @param idx The index at which to insert the `chi_string_view`.
 * @param data The `chi_string_view` to insert.
 * @return Returns the modified `chi_string` with the `chi_string_view` inserted.
 */
CHI_API chi_string* chi_insert_sv(chi_string* chi_str, size_t idx, chi_string_view data);

/**
 * @brief Erases a portion of the chi_string.
 *
 * This function erases the characters within the specified range [begin, end) from the chi_string.
 * If the range is invalid or the string is empty, it asserts with an error message. The size of
 * the chi_string is adjusted accordingly after the removal of the specified portion.
 *
 * @param chi_str The chi_string to erase a portion from.
 * @param begin The starting position of the range to erase.
 * @param end The ending position of the range to erase (exclusive).
 * @return A pointer to the modified chi_string.
 */
CHI_API chi_string* chi_erase(chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Erases the characters in the chi_string starting from the specified offset to the end.
 *
 * This function erases all characters in the chi_string starting from the specified offset
 * to the end of the string. If the offset is invalid or the string is empty, it asserts with
 * an error message. The size of the chi_string is adjusted accordingly after the removal
 * of the specified portion.
 *
 * @param chi_str The chi_string to erase a portion from.
 * @param offset The position from which characters will be erased until the end of the string.
 * @return A pointer to the modified chi_string.
 */
CHI_API chi_string* chi_erase_from(chi_string* chi_str, size_t offset);

/**
 * @brief Retrieves the size of a chi_string instance.
 * Note: This function using assertion
 * 
 * @param chi_str The chi_string to get the size of.
 * @return The size of the chi_string.
 */
CHI_API CHI_CHECK_RETURN size_t chi_size(const chi_string* chi_str);

/**
 * @brief Retrieves the size of a chi_string instance.
 * Note: This function NOT using assertion
 * 
 * @param chi_str The chi_string to get the size of.
 * @return The size of the chi_string.
 */
CHI_API CHI_CHECK_RETURN size_t chi_length(const chi_string* chi_str); 

/**
 * @brief Retrieves the capacity of a chi_string instance.
 * 
 * @param chi_str The chi_string to get the capacity of.
 * @return The capacity of the chi_string.
 */
CHI_API CHI_CHECK_RETURN size_t chi_capacity(const chi_string* chi_str);

/**
 * @brief Retrieves the first character of the `chi_string`.
 * Note: If the `chi_string` is empty, the behavior is undefined. Make sure to check if the string is not empty using `chi_is_empty` before calling this function.
 * 
 * @param chi_str The `chi_string` instance to access.
 * @return Returns the address of the character at the front of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN char* chi_front(const chi_string* chi_str);

/**
 * @brief Retrieves the last character of the `chi_string`.
 * Note: If the `chi_string` is empty, the behavior is undefined. Make sure to check if the string is not empty using `chi_is_empty` before calling this function.
 * 
 * @param chi_str The `chi_string` instance to access.
 * @return Returns the address of the character at the back of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN char* chi_back(const chi_string* chi_str);

/**
 * @brief Compares the `chi_string` to a null-terminated C string for equality.
 * Note: The comparison is case-sensitive.
 * 
 * @param lhs The `chi_string` instance to compare.
 * @param rhs The null-terminated C string to compare against.
 * @return Returns `true` if the `chi_string` is equal to the C string, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_equal_s(const chi_string* lhs, const char* rhs);

/**
 * @brief Compares the `chi_string` to another `chi_string_view` for equality.
 * Note: The comparison is case-sensitive.
 * 
 * @param lhs The `chi_string` instance to compare.
 * @param rhs The `chi_string_view` to compare against.
 * @return Returns `true` if the `chi_string` is equal to the `chi_string_view`, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_equal_sv(const chi_string* lhs, chi_string_view rhs);

/**
 * @brief Checks if two chi_string instances are equal.
 * 
 * @param chi_str1 The first chi_string to compare.
 * @param chi_str2 The second chi_string to compare.
 * @return True if the chi_strings are equal, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_equal(const chi_string* lhs, const chi_string* rhs);

/**
 * @brief Compares the `chi_string` to a null-terminated C string for equality, ignoring case.
 * Note: The comparison is case-insensitive.
 * 
 * @param lhs The `chi_string` instance to compare.
 * @param rhs The null-terminated C string to compare against.
 * @return Returns `true` if the `chi_string` is equal to the C string ignoring case, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_equal_ignorecase_s(const chi_string* lhs, const char* rhs);

/**
 * @brief Compares the `chi_string` to a `chi_string_view` for equality, ignoring case.
 * Note: The comparison is case-insensitive.
 * 
 * @param lhs The `chi_string` instance to compare.
 * @param rhs The `chi_string_view` to compare against.
 * @return Returns `true` if the `chi_string` is equal to the `chi_string_view` ignoring case, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_equal_ignorecase_sv(const chi_string* lhs, chi_string_view rhs);

/**
 * @brief Compares two `chi_string` instances for equality, ignoring case.
 * Note: The comparison is case-insensitive.
 * 
 * @param lhs The first `chi_string` instance to compare.
 * @param rhs The second `chi_string` instance to compare against.
 * @return Returns `true` if the two `chi_string` instances are equal ignoring case, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_equal_ignorecase(const chi_string* lhs, const chi_string* rhs);

/**
 * @brief Checks if the `chi_string` starts with the specified character.
 *
 * @param chi_str The `chi_string` instance to check.
 * @param ch The character to check for.
 * @return Returns `true` if the `chi_string` starts with the specified character, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_starts_with(const chi_string* chi_str, char ch);

/**
 * @brief Checks if the `chi_string` ends with the specified character.
 *
 * @param chi_str The `chi_string` instance to check.
 * @param ch The character to check for.
 * @return Returns `true` if the `chi_string` ends with the specified character, `false` otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_ends_with(const chi_string* chi_str, char ch);

/**
 * @brief Checks if a chi_string instance is empty.
 * 
 * @param chi_str The chi_string to check.
 * @return True if the chi_string is empty, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_is_empty(const chi_string* chi_str);

/**
 * @brief Fills the entire `chi_string` with the specified character.
 * Note: This function modifies the content of the existing string without changing its size or capacity.
 * 
 * @param chi_str The `chi_string` to be filled.
 * @param ch The character to fill the string with.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_fill(chi_string* chi_str, char ch);

/**
 * @brief Fills a portion of the `chi_string` with the specified character.
 * Note: It is the responsibility of the calling function to ensure that `offset` and `length` parameters are valid.
 * 
 * @param chi_str The `chi_string` to be filled.
 * @param offset The starting point of the portion to be filled (0-based index).
 * @param length The length of the portion to be filled.
 * @param ch The character to fill the specified portion with.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_fill_n(chi_string* chi_str, size_t offset, size_t length, char ch);

/**
 * @brief Copies the content of one `chi_string` to another.
 * Note: The destination `chi_string` will be resized if necessary to accommodate the content of the source string.
 * 
 * @param chi_str The destination `chi_string` where the content will be copied to.
 * @param src_cs The source `chi_string` from which the content will be copied.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_copy(chi_string* chi_str, const chi_string* src_cs);

/**
 * @brief Creates a substring from the specified `chi_string`.
 * 
 * @param chi_str The original `chi_string` from which the substring will be created.
 * @param offset The starting point of the substring (0-based index).
 * @param length The length of the substring.
 * @return Returns a new `chi_string` structure if successful, otherwise returns `NULL`.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_substring(const chi_string* chi_str, size_t offset, size_t length);

/**
 * @brief Splits the chi_string by spaces.
 *
 * This function splits the provided `chi_string` by the space character (" ") and
 * returns an array of `chi_string*` containing the split substrings. The number of
 * substrings is stored in the provided `size` pointer.
 *
 * @param chi_str The `chi_string` to be split.
 * @param size Pointer to store the number of resulting substrings.
 * @return chi_string** An array of `chi_string*` containing the split parts.
 */
CHI_API CHI_CHECK_RETURN chi_string** chi_split(const chi_string* chi_str, size_t* size);

/**
 * @brief Splits the chi_string by a specified delimiter character.
 *
 * This function splits the provided `chi_string` using a single character delimiter
 * and returns an array of `chi_string*` containing the split substrings. The number of
 * substrings is stored in the provided `size` pointer.
 *
 * @param chi_str The `chi_string` to be split.
 * @param delim The character to use as the delimiter.
 * @param size Pointer to store the number of resulting substrings.
 * @return chi_string** An array of `chi_string*` containing the split parts.
 */
CHI_API CHI_CHECK_RETURN chi_string** chi_split_c(const chi_string* chi_str, char delim, size_t* size);

/**
 * @brief Splits the chi_string by a specified delimiter string.
 *
 * This function splits the provided `chi_string` using a string delimiter and
 * returns an array of `chi_string*` containing the split substrings. The number of
 * substrings is stored in the provided `size` pointer.
 *
 * @param chi_str The `chi_string` to be split.
 * @param delim The string to use as the delimiter.
 * @param size Pointer to store the number of resulting substrings.
 * @return chi_string** An array of `chi_string*` containing the split parts.
 */
CHI_API CHI_CHECK_RETURN chi_string** chi_split_s(const chi_string* chi_str, const char* delim, size_t* size);

/**
 * @brief Splits the chi_string by a chi_string delimiter.
 *
 * This function splits the provided `chi_string` using another `chi_string` as the delimiter
 * and returns an array of `chi_string*` containing the split substrings. The number of
 * substrings is stored in the provided `size` pointer.
 *
 * @param chi_str The `chi_string` to be split.
 * @param delim The `chi_string` to use as the delimiter.
 * @param size Pointer to store the number of resulting substrings.
 * @return chi_string** An array of `chi_string*` containing the split parts.
 */
CHI_API CHI_CHECK_RETURN chi_string** chi_split_cs(const chi_string* chi_str, const chi_string* delim, size_t* size);

/**
 * @brief Splits the chi_string by a chi_string_view delimiter.
 *
 * This function splits the provided `chi_string` using a `chi_string_view` as the delimiter
 * and returns an array of `chi_string*` containing the split substrings. The number of
 * substrings is stored in the provided `size` pointer.
 *
 * @param chi_str The `chi_string` to be split.
 * @param delim The `chi_string_view` to use as the delimiter.
 * @param size Pointer to store the number of resulting substrings.
 * @return chi_string** An array of `chi_string*` containing the split parts.
 */
CHI_API CHI_CHECK_RETURN chi_string** chi_split_sv(const chi_string* chi_str, chi_string_view delim, size_t* size);

/**
 * @brief Removes repeated consecutive characters from the `chi_string`.
 * Note: This function will keep only the first occurrence of each repeated character.
 * 
 * @param chi_str The `chi_string` from which repeated characters will be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_remove_repated(chi_string* chi_str);

/**
 * @brief Removes all occurrences of the specified character from the `chi_string`.
 * 
 * @param chi_str The `chi_string` from which the character will be removed.
 * @param delim The character to be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_remove_c(chi_string* chi_str, char delim);

/**
 * @brief Removes all occurrences of the specified sequence of characters from the `chi_string`.
 * 
 * @param chi_str The `chi_string` from which the sequence will be removed.
 * @param delim The sequence of characters to be removed.
 * @param n The length of the sequence to be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_remove_n(chi_string* chi_str, const char* delim, size_t n);

/**
 * @brief Removes all occurrences of the specified string from the `chi_string`.
 * 
 * @param chi_str The `chi_string` from which the string will be removed.
 * @param delim The string to be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_remove(chi_string* chi_str, const char* delim);

/**
 * @brief Removes all occurrences of the specified `chi_string` from the `chi_string`.
 * 
 * @param chi_str The `chi_string` from which the `chi_string` will be removed.
 * @param delim The `chi_string` to be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_remove_cs(chi_string* chi_str, const chi_string* delim);

/**
 * @brief Removes all occurrences of the specified `chi_string_view` from the `chi_string`.
 * 
 *
 * @param chi_str The `chi_string` from which the `chi_string_view` will be removed.
 * @param delim The `chi_string_view` to be removed.
 * @return Returns the modified `chi_string` if successful, otherwise returns `NULL`.
 */
CHI_API chi_string* chi_remove_sv(chi_string* chi_str, chi_string_view delim);

/**
 * @brief Removes leading whitespace characters from the given chi_string.
 * This function trims all leading whitespace characters from the `chi_str`.
 * If the entire string is whitespace, it will be cleared.
 *
 * @param chi_str The chi_string to be trimmed.
 * @return The trimmed chi_string.
 */
CHI_API chi_string* chi_trim_left(chi_string* chi_str);

/**
 * @brief Removes trailing whitespace characters from the given chi_string.
 * This function trims all trailing whitespace characters from the `chi_str`.
 * If the entire string is whitespace, it will be cleared.
 *
 * @param chi_str The chi_string to be trimmed.
 * @return The trimmed chi_string.
 */
CHI_API chi_string* chi_trim_right(chi_string* chi_str);

/**
 * @brief Removes both leading and trailing whitespace characters from the given chi_string.
 * This function trims all leading and trailing whitespace characters from the `chi_str`.
 * If the entire string is whitespace, it will be cleared.
 *
 * @param chi_str The chi_string to be trimmed.
 * @return The trimmed chi_string.
 */
CHI_API chi_string* chi_trim(chi_string* chi_str);

/**
 * @brief Replaces all occurrences of a character with another character in the `chi_string`.
 * 
 * @param chi_str The `chi_string` instance to modify.
 * @param old_value The character to be replaced.
 * @param new_value The character to replace with.
 * @return Returns the modified `chi_string` instance.
 */
CHI_API chi_string* chi_replace_c(chi_string* chi_str, char old_value, char new_value);

/**
 * @brief Replaces all occurrences of a substring with another substring in the `chi_string`.
 * 
 * @param chi_str The `chi_string` instance to modify.
 * @param old_value The null-terminated C string to be replaced.
 * @param new_value The null-terminated C string to replace with.
 * @return Returns the modified `chi_string` instance.
 */
CHI_API chi_string* chi_replace(chi_string* chi_str, const char* old_value, const char* new_value);

/**
 * @brief Replaces all occurrences of a `chi_string_view` with another `chi_string_view` in the `chi_string`.
 * 
 * @param chi_str The `chi_string` instance to modify.
 * @param old_value The `chi_string_view` to be replaced.
 * @param new_value The `chi_string_view` to replace with.
 * @return Returns the modified `chi_string` instance.
 */
CHI_API chi_string* chi_replace_sv(chi_string* chi_str, chi_string_view old_value, chi_string_view new_value);

/**
 * @brief Finds the first occurrence of a character in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The character to find.
 * @return Returns the index of the first occurrence of the character, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_c(const chi_string* chi_str, size_t offset, char delim);

/**
 * @brief Finds the first occurrence of a C-string in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The C-string to find.
 * @return Returns the index of the first occurrence of the C-string, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find(const chi_string* chi_str, size_t offset, const char* delim);

/**
 * @brief Finds the first occurrence of a `chi_string` in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The `chi_string` to find.
 * @return Returns the index of the first occurrence of the `chi_string`, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_cs(const chi_string* chi_str, size_t offset, const chi_string* delim);

/**
 * @brief Finds the first occurrence of a `chi_string_view` in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The `chi_string_view` to find.
 * @return Returns the index of the first occurrence of the `chi_string_view`, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_sv(const chi_string* chi_str, size_t offset, chi_string_view delim);

/**
 * @brief Finds the last occurrence of a character in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The character to find.
 * @return Returns the index of the last occurrence of the character, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_rfind_c(const chi_string* chi_str, size_t offset, char delim);

/**
 * @brief Finds the last occurrence of a C-string in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The C-string to find.
 * @return Returns the index of the last occurrence of the C-string, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_rfind(const chi_string* chi_str, size_t offset, const char* delim);

/**
 * @brief Finds the last occurrence of a `chi_string` in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The `chi_string` to find.
 * @return Returns the index of the last occurrence of the `chi_string`, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_rfind_cs(const chi_string* chi_str, size_t offset, const chi_string* delim);

/**
 * @brief Finds the last occurrence of a `chi_string_view` in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delim The `chi_string_view` to find.
 * @return Returns the index of the last occurrence of the `chi_string_view`, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_rfind_sv(const chi_string* chi_str, size_t offset, chi_string_view delim);

/**
 * @brief Finds the first occurrence of any character from a set of delimiters in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delims The set of delimiter characters to find.
 * @return Returns the index of the first occurrence of any delimiter character, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_first_of(const chi_string* chi_str, size_t offset, const char* delims);

/**
 * @brief Finds the last occurrence of any character from a set of delimiters in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delims The set of delimiter characters to find.
 * @return Returns the index of the last occurrence of any delimiter character, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_last_of(const chi_string* chi_str, size_t offset, const char* delims);

/**
 * @brief Finds the first occurrence of any character not in a set of delimiters in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delims The set of delimiter characters to avoid.
 * @return Returns the index of the first occurrence of any character not in the set of delimiters, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_first_not_of(const chi_string* chi_str, size_t offset, const char* delims);

/**
 * @brief Finds the last occurrence of any character not in a set of delimiters in the `chi_string` starting from the specified offset.
 *
 * @param chi_str The `chi_string` instance to search.
 * @param offset The starting index for the search.
 * @param delims The set of delimiter characters to avoid.
 * @return Returns the index of the last occurrence of any character not in the set of delimiters, or `chi_npos` if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_find_last_not_of(const chi_string* chi_str, size_t offset, const char* delims);

/**
 * @brief Checks if the `chi_string` contains a specific character.
 *
 * @param chi_str The `chi_string` instance to check.
 * @param delim The character to check for.
 * @return Returns true if the character is found, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_contains(const chi_string* chi_str, char delim);

/**
 * @brief Converts the `chi_string` to an integer.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the integer representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN int chi_toi(const chi_string* chi_str);

/**
 * @brief Converts the `chi_string` to a float.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the float representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN float chi_tof(const chi_string* chi_str);

/**
 * @brief Converts the `chi_string` to a double.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the double representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN double chi_tod(const chi_string* chi_str);

/**
 * @brief Converts the `chi_string` to a long.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the long representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN long chi_tol(const chi_string* chi_str);

/**
 * @brief Converts the `chi_string` to a long long.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the long long representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN long long chi_toll(const chi_string* chi_str);

/**
 * @brief Converts the `chi_string` to an unsigned long.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the unsigned long representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN unsigned long chi_toul(const chi_string* chi_str);

/**
 * @brief Converts the `chi_string` to an unsigned long long.
 *
 * @param chi_str The `chi_string` instance to convert.
 * @return Returns the unsigned long long representation of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN unsigned long long chi_toull(const chi_string* chi_str);

/**
 * @brief Converts an integer to a `chi_string`.
 *
 * @param value The integer value to convert.
 * @return Returns a new `chi_string` representing the integer.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_i(int value);

/**
 * @brief Converts a float to a `chi_string`.
 *
 * @param value The float value to convert.
 * @return Returns a new `chi_string` representing the float.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_f(float value);

/**
 * @brief Converts a double to a `chi_string`.
 *
 * @param value The double value to convert.
 * @return Returns a new `chi_string` representing the double.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_d(double value);

/**
 * @brief Converts a long to a `chi_string`.
 *
 * @param value The long value to convert.
 * @return Returns a new `chi_string` representing the long.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_l(long value);

/**
 * @brief Converts a long long to a `chi_string`.
 *
 * @param value The long long value to convert.
 * @return Returns a new `chi_string` representing the long long.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_ll(long long value);

/**
 * @brief Converts an unsigned long to a `chi_string`.
 *
 * @param value The unsigned long value to convert.
 * @return Returns a new `chi_string` representing the unsigned long.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_ul(unsigned long value);

/**
 * @brief Converts an unsigned long long to a `chi_string`.
 *
 * @param value The unsigned long long value to convert.
 * @return Returns a new `chi_string` representing the unsigned long long.
 */
CHI_API CHI_CHECK_RETURN chi_string* chi_to_string_ull(unsigned long long value);

/**
 * @brief Swaps the contents of two `chi_string` instances.
 *
 * @param lhs The first `chi_string` instance.
 * @param rhs The second `chi_string` instance.
 */
CHI_API void chi_swap(chi_string* lhs, chi_string* rhs);

/**
 * @brief Computes a hash value for the given `chi_string`.
 *
 * @param chi_str The `chi_string` instance to hash.
 * @return Returns the hash value of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN size_t chi_hash(const chi_string* chi_str);

/**
 * @brief Applies a given function to each character in the chi_string from begin to end.
 *
 * This function iterates over the characters in the chi_string from the begin index to the end index and applies the provided function to each character.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for processing.
 * @param end End index for processing.
 * @param pred Function to apply to each character.
 * @return Pointer to the processed chi_string.
 */
CHI_API chi_string* chi_for_each_ip(chi_string* chi_str, size_t begin, size_t end, void(*pred)(char*));

/**
 * @brief Applies a given function to each character in the chi_string.
 *
 * This function iterates over all characters in the chi_string and applies the provided function to each character.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param pred Function to apply to each character.
 * @return Pointer to the processed chi_string.
 */
CHI_API chi_string* chi_for_each(chi_string* chi_str, void(*pred)(char*));

/**
 * @brief Converts a range of characters in the chi_string to uppercase.
 *
 * This function iterates over the characters in the chi_string from the specified begin index to the end index
 * and converts each lowercase character to uppercase by adjusting their ASCII values.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for processing.
 * @param end End index for processing.
 * @return Pointer to the modified chi_string with the specified range of characters converted to uppercase.
 */
CHI_API chi_string* chi_to_upper_ip(chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Converts all characters in the chi_string to uppercase.
 *
 * This function iterates over all characters in the chi_string and converts each lowercase character to uppercase
 * by adjusting their ASCII values.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @return Pointer to the modified chi_string with all characters converted to uppercase.
 */
CHI_API chi_string* chi_to_upper(chi_string* chi_str);

/**
 * @brief Converts a range of characters in the chi_string to lowercase.
 *
 * This function iterates over the characters in the chi_string from the specified begin index to the end index
 * and converts each uppercase character to lowercase by adjusting their ASCII values.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for processing.
 * @param end End index for processing.
 * @return Pointer to the modified chi_string with the specified range of characters converted to lowercase.
 */
CHI_API chi_string* chi_to_lower_ip(chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Converts all characters in the chi_string to lowercase.
 *
 * This function iterates over all characters in the chi_string and converts each uppercase character to lowercase
 * by adjusting their ASCII values.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @return Pointer to the modified chi_string with all characters converted to lowercase.
 */
CHI_API chi_string* chi_to_lower(chi_string* chi_str);

/**
 * @brief Switches the case of characters in the given range of the chi_string.
 *
 * This function iterates over the characters in the specified range [begin, end)
 * and switches their cases. Lowercase characters are converted to uppercase and
 * uppercase characters are converted to lowercase.
 *
 * @param chi_str The chi_string to modify.
 * @param begin The starting index of the range.
 * @param end The ending index of the range.
 * @return chi_string* The modified chi_string with switched cases.
 */
CHI_API chi_string* chi_switch_cases_ip(chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Switches the case of all characters in the chi_string.
 *
 * This function switches the case of each character in the chi_string. Lowercase
 * characters are converted to uppercase and uppercase characters are converted to lowercase.
 *
 * @param chi_str The chi_string to modify.
 * @return chi_string* The modified chi_string with switched cases.
 */
CHI_API chi_string* chi_switch_cases(chi_string* chi_str);

/**
 * @brief Counts the occurrences of a character in the chi_string from begin to end.
 *
 * This function counts how many times the specified character appears in the chi_string from the begin index to the end index.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param ch Character to count.
 * @param begin Start index for counting.
 * @param end End index for counting.
 * @return Number of occurrences of the character.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_ip(const chi_string* chi_str, char ch, size_t begin, size_t end);

/**
 * @brief Counts the occurrences of a character in the chi_string.
 *
 * This function counts how many times the specified character appears in the entire chi_string.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param ch Character to count.
 * @return Number of occurrences of the character.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count(const chi_string* chi_str, char ch);

/**
 * @brief Counts occurrences of a substring within a range of the chi_string.
 *
 * This function counts how many times a specified substring occurs within a portion of
 * the `chi_string`, defined by the `begin` and `end` positions.
 *
 * @param chi_str The `chi_string` in which to count occurrences.
 * @param data The substring to search for.
 * @param begin The starting index of the range.
 * @param end The ending index of the range.
 * @return size_t The number of occurrences of the substring.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_s_ip(const chi_string* chi_str, const char* data, size_t begin, size_t end);

/**
 * @brief Counts occurrences of a substring within the chi_string.
 *
 * This function counts how many times a specified substring occurs within the entire
 * `chi_string`.
 *
 * @param chi_str The `chi_string` in which to count occurrences.
 * @param data The substring to search for.
 * @return size_t The number of occurrences of the substring.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_s(const chi_string* chi_str, const char* data);

/**
 * @brief Counts occurrences of another chi_string within a range of the chi_string.
 *
 * This function counts how many times a specified `chi_string` occurs within a portion
 * of another `chi_string`, defined by the `begin` and `end` positions.
 *
 * @param chi_str The `chi_string` in which to count occurrences.
 * @param data The `chi_string` to search for.
 * @param begin The starting index of the range.
 * @param end The ending index of the range.
 * @return size_t The number of occurrences of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_cs_ip(const chi_string* chi_str, const chi_string* data, size_t begin, size_t end);

/**
 * @brief Counts occurrences of another chi_string within the chi_string.
 *
 * This function counts how many times a specified `chi_string` occurs within the entire
 * `chi_string`.
 *
 * @param chi_str The `chi_string` in which to count occurrences.
 * @param data The `chi_string` to search for.
 * @return size_t The number of occurrences of the `chi_string`.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_cs(const chi_string* chi_str, const chi_string* data);

/**
 * @brief Counts occurrences of a chi_string_view within a range of the chi_string.
 *
 * This function counts how many times a specified `chi_string_view` occurs within a
 * portion of the `chi_string`, defined by the `begin` and `end` positions.
 *
 * @param chi_str The `chi_string` in which to count occurrences.
 * @param data The `chi_string_view` to search for.
 * @param begin The starting index of the range.
 * @param end The ending index of the range.
 * @return size_t The number of occurrences of the `chi_string_view`.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_sv_ip(const chi_string* chi_str, chi_string_view data, size_t begin, size_t end);

/**
 * @brief Counts occurrences of a chi_string_view within the chi_string.
 *
 * This function counts how many times a specified `chi_string_view` occurs within the
 * entire `chi_string`.
 *
 * @param chi_str The `chi_string` in which to count occurrences.
 * @param data The `chi_string_view` to search for.
 * @return size_t The number of occurrences of the `chi_string_view`.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_sv(const chi_string* chi_str, chi_string_view data);

/**
 * @brief Counts the characters in the chi_string that satisfy a predicate from begin to end.
 *
 * This function counts how many characters in the chi_string from the begin index to the end index satisfy the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for counting.
 * @param end End index for counting.
 * @param pred Predicate function to apply to each character.
 * @return Number of characters that satisfy the predicate.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_if_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char));

/**
 * @brief Counts the characters in the chi_string that satisfy a predicate.
 *
 * This function counts how many characters in the entire chi_string satisfy the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param pred Predicate function to apply to each character.
 * @return Number of characters that satisfy the predicate.
 */
CHI_API CHI_CHECK_RETURN size_t chi_count_if(const chi_string* chi_str, bool(*pred)(char));

/**
 * @brief Checks if all characters in the chi_string satisfy a predicate from begin to end.
 *
 * This function checks if all characters in the chi_string from the begin index to the end index satisfy the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for checking.
 * @param end End index for checking.
 * @param pred Predicate function to apply to each character.
 * @return True if all characters satisfy the predicate, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_all_of_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char));

/**
 * @brief Checks if all characters in the chi_string satisfy a predicate.
 *
 * This function checks if all characters in the entire chi_string satisfy the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param pred Predicate function to apply to each character.
 * @return True if all characters satisfy the predicate, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_all_of(const chi_string* chi_str, bool(*pred)(char));

/**
 * @brief Checks if any character in the chi_string satisfies a predicate from begin to end.
 *
 * This function checks if any character in the chi_string from the begin index to the end index satisfies the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for checking.
 * @param end End index for checking.
 * @param pred Predicate function to apply to each character.
 * @return True if any character satisfies the predicate, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_any_of_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char));

/**
 * @brief Checks if any character in the chi_string satisfies a predicate.
 *
 * This function checks if any character in the entire chi_string satisfies the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param pred Predicate function to apply to each character.
 * @return True if any character satisfies the predicate, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_any_of(const chi_string* chi_str, bool(*pred)(char));

/**
 * @brief Checks if no character in the chi_string satisfies a predicate from begin to end.
 *
 * This function checks if no character in the chi_string from the begin index to the end index satisfies the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for checking.
 * @param end End index for checking.
 * @param pred Predicate function to apply to each character.
 * @return True if no character satisfies the predicate, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_none_of_ip(const chi_string* chi_str, size_t begin, size_t end, bool(*pred)(char));

/**
 * @brief Checks if no character in the chi_string satisfies a predicate.
 *
 * This function checks if no character in the entire chi_string satisfies the provided predicate.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param pred Predicate function to apply to each character.
 * @return True if no character satisfies the predicate, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_none_of(const chi_string* chi_str, bool(*pred)(char));

/**
 * @brief Shuffles the characters in the chi_string from begin to end.
 *
 * This function randomly shuffles the characters in the chi_string from the begin index to the end index.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for shuffling.
 * @param end End index for shuffling.
 * @return Pointer to the shuffled chi_string.
 */
CHI_API chi_string* chi_shuffle_ip(chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Shuffles the characters in the chi_string.
 *
 * This function randomly shuffles all characters in the chi_string.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @return Pointer to the shuffled chi_string.
 */
CHI_API chi_string* chi_shuffle(chi_string* chi_str);

/**
 * @brief Reverses the characters in the chi_string from begin to end.
 *
 * This function reverses the order of the characters in the chi_string from the begin index to the end index.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @param begin Start index for reversing.
 * @param end End index for reversing.
 * @return Pointer to the reversed chi_string.
 */
CHI_API chi_string* chi_reverse_ip(chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Reverses the characters in the chi_string.
 *
 * This function reverses the order of all characters in the chi_string.
 *
 * @param chi_str Pointer to the chi_string to be processed.
 * @return Pointer to the reversed chi_string.
 */
CHI_API chi_string* chi_reverse(chi_string* chi_str);

/**
 * @brief Returns a random character from a specified range within the chi_string.
 *
 * This function returns a randomly selected character from the given chi_string
 * within the specified range [begin, end).
 *
 * @param chi_str The chi_string to sample a character from.
 * @param begin The starting position of the range.
 * @param end The ending position of the range (exclusive).
 * @return A randomly selected character from the specified range within the chi_string.
 */
CHI_API CHI_CHECK_RETURN char chi_sample_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Returns a random character from the chi_string.
 *
 * This function returns a randomly selected character from the given chi_string.
 * If the string is empty, it asserts with an error message.
 *
 * @param chi_str The chi_string to sample a character from.
 * @return A randomly selected character from the chi_string.
 */
CHI_API CHI_CHECK_RETURN char chi_sample(const chi_string* chi_str);

/**
 * @brief Sets whether to ignore space characters in certain validation functions.
 *
 * When `ignore` is set to `true`, functions like `chi_islower`, `chi_isupper`,
 * `chi_isalnum`, `chi_isalpha`, `chi_isdigit`, and their `_ip` variants will
 * disregard space characters when determining if the string meets the specified condition.
 * Note: By default, the ignore is disabled.
 *
 * @param ignore Flag to indicate whether space characters should be ignored.
 */
CHI_API void chi_ignore_spaces(bool ignore);

/**
 * @brief Checks if all characters in the specified range of the string are lowercase.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is lowercase.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are lowercase, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_islower_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are lowercase.
 *
 * This function checks if all characters in the chi_string are lowercase by
 * calling `chi_islower_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are lowercase, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_islower(const chi_string* chi_str);

/**
 * @brief Checks if all characters in the specified range of the string are uppercase.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is uppercase.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are uppercase, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isupper_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are uppercase.
 *
 * This function checks if all characters in the chi_string are uppercase by
 * calling `chi_isupper_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are uppercase, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isupper(const chi_string* chi_str);

/**
 * @brief Checks if all characters in the specified range of the string are alphanumeric.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is alphanumeric.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are alphanumeric, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isalnum_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are alphanumeric.
 *
 * This function checks if all characters in the chi_string are alphanumeric by
 * calling `chi_isalnum_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are alphanumeric, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isalnum(const chi_string* chi_str);

/**
 * @brief Checks if all characters in the specified range of the string are alphabetic.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is alphabetic.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are alphabetic, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isalpha_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are alphabetic.
 *
 * This function checks if all characters in the chi_string are alphabetic by
 * calling `chi_isalpha_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are alphabetic, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isalpha(const chi_string* chi_str);

/**
 * @brief Checks if all characters in the specified range of the string are digits.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is a digit.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are digits, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isdigit_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are digits.
 *
 * This function checks if all characters in the chi_string are digits by
 * calling `chi_isdigit_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are digits, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isdigit(const chi_string* chi_str);

/**
 * @brief Checks if all characters in the specified range of the string are printable.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is printable.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are printable, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isprintable_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are printable.
 *
 * This function checks if all characters in the chi_string are printable by
 * calling `chi_isprintable_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are printable, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isprintable(const chi_string* chi_str);

/**
 * @brief Checks if all characters in the specified range of the string are whitespace.
 *
 * This function iterates through the characters in the chi_string from the `begin`
 * to the `end` position and checks if each character is a whitespace character.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @param begin The start index of the range to check.
 * @param end The end index of the range to check.
 * @return `true` if all characters in the specified range are whitespace, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isspace_ip(const chi_string* chi_str, size_t begin, size_t end);

/**
 * @brief Checks if all characters in the chi_string are whitespace.
 *
 * This function checks if all characters in the chi_string are whitespace by
 * calling `chi_isspace_ip` with the full range of the string.
 *
 * @param chi_str Pointer to the chi_string structure.
 * @return `true` if all characters in the chi_string are whitespace, otherwise `false`.
 */
CHI_API CHI_CHECK_RETURN bool chi_isspace(const chi_string* chi_str);



/* CHI STRING END */

/* CHI STRING VIEW BEGIN */

/**
 * @brief Creates a chi_string_view from a null-terminated C string.
 *
 * This function creates a chi_string_view that references the given null-terminated C string.
 *
 * @param data Pointer to the null-terminated C string.
 * @return A chi_string_view referencing the provided C string.
 */
CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_create(const char* data);

/**
 * @brief Creates a chi_string_view from a C string with a specified size.
 *
 * This function creates a chi_string_view that references the given C string with the specified size.
 *
 * @param data Pointer to the C string.
 * @param size Size of the C string.
 * @return A chi_string_view referencing the provided C string.
 */
CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_create_n(const char* data, size_t size);

/**
 * @brief Creates a chi_string_view from a chi_string.
 *
 * This function creates a chi_string_view that references the data in the provided chi_string.
 *
 * @param chi_str Pointer to the chi_string.
 * @return A chi_string_view referencing the data in the provided chi_string.
 */
CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_create_from_chi_s(const chi_string* chi_str);

/**
 * @brief Gets the data pointer of the chi_string_view.
 *
 * This function returns the data pointer of the chi_string_view.
 *
 * @param sv The chi_string_view to get the data pointer from.
 * @return The data pointer of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN const char* chi_sv_get(chi_string_view sv);

/**
 * @brief Gets the character at a specified index in the chi_string_view.
 *
 * This function returns the character at the specified index in the chi_string_view.
 *
 * @param sv The chi_string_view to get the character from.
 * @param idx Index of the character to get.
 * @return The character at the specified index.
 */
CHI_API CHI_CHECK_RETURN char chi_sv_at(chi_string_view sv, size_t idx);

/**
 * @brief Gets the beginning pointer of the chi_string_view.
 *
 * This function returns the beginning pointer of the chi_string_view.
 *
 * @param sv The chi_string_view to get the beginning pointer from.
 * @return The beginning pointer of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN const char* chi_sv_begin(chi_string_view sv);

/**
 * @brief Gets the end pointer of the chi_string_view.
 *
 * This function returns the end pointer of the chi_string_view.
 *
 * @param sv The chi_string_view to get the end pointer from.
 * @return The end pointer of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN const char* chi_sv_end(chi_string_view sv);

/**
 * @brief Gets the size of the chi_string_view.
 *
 * This function returns the size of the chi_string_view.
 *
 * @param sv The chi_string_view to get the size of.
 * @return The size of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_size(chi_string_view sv);

/**
 * @brief Gets the length of the chi_string_view.
 *
 * This function returns the length of the chi_string_view.
 *
 * @param sv The chi_string_view to get the length of.
 * @return The length of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_length(chi_string_view sv);

/**
 * @brief Returns the first character of the chi_string_view.
 *
 * This function returns the first character of the chi_string_view. If the string view is empty,
 * it asserts with an error message.
 *
 * @param sv The chi_string_view to get the first character from.
 * @return The first character of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN char chi_sv_front(chi_string_view sv);

/**
 * @brief Returns the last character of the chi_string_view.
 *
 * This function returns the last character of the chi_string_view. If the string view is empty,
 * it asserts with an error message.
 *
 * @param sv The chi_string_view to get the last character from.
 * @return The last character of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN char chi_sv_back(chi_string_view sv);

/**
 * @brief Checks if two chi_string_view are equal.
 *
 * This function compares two chi_string_view and returns true if they are equal.
 *
 * @param lhs The first chi_string_view.
 * @param rhs The second chi_string_view.
 * @return true if the two chi_string_view are equal, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_equal(chi_string_view lhs, chi_string_view rhs);

/**
 * @brief Checks if a chi_string_view is equal to a null-terminated C string.
 *
 * This function compares a chi_string_view and a null-terminated C string and returns true if they are equal.
 *
 * @param lhs The chi_string_view.
 * @param rhs The null-terminated C string.
 * @return true if the chi_string_view and the C string are equal, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_equal_s(chi_string_view lhs, const char* rhs);

/**
 * @brief Checks if a chi_string_view is equal to a chi_string.
 *
 * This function compares a chi_string_view and a chi_string and returns true if they are equal.
 *
 * @param lhs The chi_string_view.
 * @param rhs The chi_string.
 * @return true if the chi_string_view and the chi_string are equal, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_equal_cs(chi_string_view lhs, chi_string* rhs);

/**
 * @brief Checks if two chi_string_view are equal, ignoring case.
 *
 * This function compares two chi_string_view, ignoring case, and returns true if they are equal.
 *
 * @param lhs The first chi_string_view.
 * @param rhs The second chi_string_view.
 * @return true if the two chi_string_view are equal, ignoring case, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_equal_ignorecase(chi_string_view lhs, chi_string_view rhs);

/**
 * @brief Checks if a chi_string_view is equal to a null-terminated C string, ignoring case.
 *
 * This function compares a chi_string_view and a null-terminated C string, ignoring case, and returns true if they are equal.
 *
 * @param lhs The chi_string_view.
 * @param rhs The null-terminated C string.
 * @return true if the chi_string_view and the C string are equal, ignoring case, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_equal_ignorecase_s(chi_string_view lhs, const char* rhs);

/**
 * @brief Checks if a chi_string_view is equal to a chi_string, ignoring case.
 *
 * This function compares a chi_string_view and a chi_string, ignoring case, and returns true if they are equal.
 *
 * @param lhs The chi_string_view.
 * @param rhs The chi_string.
 * @return true if the chi_string_view and the chi_string are equal, ignoring case, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_equal_ignorecase_cs(chi_string_view lhs, chi_string* rhs);

/**
 * @brief Checks if the chi_string_view starts with a given character.
 *
 * This function checks if the chi_string_view starts with the specified character.
 *
 * @param sv The chi_string_view.
 * @param ch The character to check.
 * @return true if the chi_string_view starts with the character, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_starts_with(chi_string_view sv, char ch);

/**
 * @brief Checks if the chi_string_view ends with a given character.
 *
 * This function checks if the chi_string_view ends with the specified character.
 *
 * @param sv The chi_string_view.
 * @param ch The character to check.
 * @return true if the chi_string_view ends with the character, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_ends_with(chi_string_view sv, char ch);

/**
 * @brief Checks if the chi_string_view is empty.
 *
 * This function checks if the chi_string_view is empty.
 *
 * @param sv The chi_string_view.
 * @return true if the chi_string_view is empty, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_is_empty(chi_string_view sv);

/**
 * @brief Creates a substring chi_string_view from the given chi_string_view.
 *
 * This function creates a chi_string_view that represents a substring of the given chi_string_view.
 *
 * @param sv The original chi_string_view.
 * @param offset The starting position of the substring.
 * @param length The length of the substring.
 * @return A chi_string_view representing the substring.
 */
CHI_API CHI_CHECK_RETURN chi_string_view chi_sv_substring(const chi_string_view sv, size_t offset, size_t length);

/**
 * @brief Finds the first occurrence of a character in the chi_string_view from a given offset.
 *
 * This function finds the first occurrence of the specified character in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The character to find.
 * @return The position of the first occurrence of the character, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_c(chi_string_view sv, size_t offset, char delim);

/**
 * @brief Finds the first occurrence of a null-terminated C string in the chi_string_view from a given offset.
 *
 * This function finds the first occurrence of the specified null-terminated C string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The null-terminated C string to find.
 * @return The position of the first occurrence of the C string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find(chi_string_view sv, size_t offset, const char* delim);

/**
 * @brief Finds the first occurrence of a chi_string in the chi_string_view from a given offset.
 *
 * This function finds the first occurrence of the specified chi_string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The chi_string to find.
 * @return The position of the first occurrence of the chi_string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_cs(chi_string_view sv, size_t offset, const chi_string* delim);

/**
 * @brief Finds the first occurrence of a chi_string_view in the chi_string_view from a given offset.
 *
 * This function finds the first occurrence of the specified chi_string_view in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The chi_string_view to find.
 * @return The position of the first occurrence of the chi_string_view, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_sv(chi_string_view sv, size_t offset, chi_string_view delim);

/**
 * @brief Finds the last occurrence of a character in the chi_string_view from a given offset.
 *
 * This function finds the last occurrence of the specified character in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The character to find.
 * @return The position of the last occurrence of the character, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind_c(chi_string_view sv, size_t offset, char delim);

/**
 * @brief Finds the last occurrence of a null-terminated C string in the chi_string_view from a given offset.
 *
 * This function finds the last occurrence of the specified null-terminated C string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The null-terminated C string to find.
 * @return The position of the last occurrence of the C string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind(chi_string_view sv, size_t offset, const char* delim);

/**
 * @brief Finds the last occurrence of a chi_string in the chi_string_view from a given offset.
 *
 * This function finds the last occurrence of the specified chi_string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The chi_string to find.
 * @return The position of the last occurrence of the chi_string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind_cs(chi_string_view sv, size_t offset, const chi_string* delim);

/**
 * @brief Finds the last occurrence of a chi_string_view in the chi_string_view from a given offset.
 *
 * This function finds the last occurrence of the specified chi_string_view in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delim The chi_string_view to find.
 * @return The position of the last occurrence of the chi_string_view, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_rfind_sv(chi_string_view sv, size_t offset, chi_string_view delim);

/**
 * @brief Finds the first occurrence of any character in a null-terminated C string in the chi_string_view from a given offset.
 *
 * This function finds the first occurrence of any character in the specified null-terminated C string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delims The null-terminated C string containing the characters to find.
 * @return The position of the first occurrence of any character in the C string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_first_of(chi_string_view sv, size_t offset, const char* delims);

/**
 * @brief Finds the last occurrence of any character in a null-terminated C string in the chi_string_view from a given offset.
 *
 * This function finds the last occurrence of any character in the specified null-terminated C string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delims The null-terminated C string containing the characters to find.
 * @return The position of the last occurrence of any character in the C string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_last_of(chi_string_view sv, size_t offset, const char* delims);

/**
 * @brief Finds the first occurrence of any character not in a null-terminated C string in the chi_string_view from a given offset.
 *
 * This function finds the first occurrence of any character not in the specified null-terminated C string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delims The null-terminated C string containing the characters to not find.
 * @return The position of the first occurrence of any character not in the C string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_first_not_of(chi_string_view sv, size_t offset, const char* delims);

/**
 * @brief Finds the last occurrence of any character not in a null-terminated C string in the chi_string_view from a given offset.
 *
 * This function finds the last occurrence of any character not in the specified null-terminated C string in the chi_string_view starting from the given offset.
 *
 * @param sv The chi_string_view.
 * @param offset The starting position for the search.
 * @param delims The null-terminated C string containing the characters to not find.
 * @return The position of the last occurrence of any character not in the C string, or chi_npos if not found.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_find_last_not_of(chi_string_view sv, size_t offset, const char* delims);

/**
 * @brief Checks if the chi_string_view contains a given character.
 *
 * This function checks if the chi_string_view contains the specified character.
 *
 * @param sv The chi_string_view.
 * @param delim The character to check.
 * @return true if the chi_string_view contains the character, false otherwise.
 */
CHI_API CHI_CHECK_RETURN bool chi_sv_contains(chi_string_view sv, char delim);

/**
 * @brief Converts the chi_string_view to an integer.
 *
 * This function converts the chi_string_view to an integer using the atoi function.
 *
 * @param sv The chi_string_view to convert.
 * @return The integer representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN int chi_sv_toi(chi_string_view sv);

/**
 * @brief Converts the chi_string_view to a float.
 *
 * This function converts the chi_string_view to a float using the strtof function.
 *
 * @param sv The chi_string_view to convert.
 * @return The float representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN float chi_sv_tof(chi_string_view sv);

/**
 * @brief Converts the chi_string_view to a double.
 *
 * This function converts the chi_string_view to a double using the strtod function.
 *
 * @param sv The chi_string_view to convert.
 * @return The double representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN double chi_sv_tod(chi_string_view sv);

/**
 * @brief Converts the chi_string_view to a long.
 *
 * This function converts the chi_string_view to a long using the strtol function.
 *
 * @param sv The chi_string_view to convert.
 * @return The long representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN long chi_sv_tol(chi_string_view sv);

/**
 * @brief Converts the chi_string_view to a long long.
 *
 * This function converts the chi_string_view to a long long using the strtoll function.
 *
 * @param sv The chi_string_view to convert.
 * @return The long long representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN long long chi_sv_toll(chi_string_view sv);

/**
 * @brief Converts the chi_string_view to an unsigned long.
 *
 * This function converts the chi_string_view to an unsigned long using the strtoul function.
 *
 * @param sv The chi_string_view to convert.
 * @return The unsigned long representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN unsigned long chi_sv_toul(chi_string_view sv);

/**
 * @brief Converts the chi_string_view to an unsigned long long.
 *
 * This function converts the chi_string_view to an unsigned long long using the strtoull function.
 *
 * @param sv The chi_string_view to convert.
 * @return The unsigned long long representation of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN unsigned long long chi_sv_toull(chi_string_view sv);

/**
 * @brief Computes the hash of the chi_string_view.
 *
 * This function computes the hash of the chi_string_view.
 *
 * @param sv The chi_string_view to hash.
 * @return The hash of the chi_string_view.
 */
CHI_API CHI_CHECK_RETURN size_t chi_sv_hash(chi_string_view sv);

/* CHI STRING VIEW END */


_CHI_HEADER_END

#endif // !_CHI_