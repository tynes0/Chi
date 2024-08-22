#pragma once
#ifndef _CHI_IO_
#define _CHI_IO_

#include "chi.h"

#include <stdio.h>

_CHI_HEADER_BEGIN

/* CHI STRING BEGIN */

/**
 * @brief Reads up to `n` characters from the given file into the chi_string.
 *
 * @param chi_str The string to read into.
 * @param file The file to read from.
 * @param n The maximum number of characters to read.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_read_n(chi_string* chi_str, FILE* file, int n);

/**
 * @brief Reads a line from the given file into the chi_string.
 *
 * @param chi_str The string to read into.
 * @param file The file to read from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_read(chi_string* chi_str, FILE* file);

/**
 * @brief Reads the content of the specified file path into the chi_string.
 *
 * @param chi_str The string to read into.
 * @param filepath The path to the file to read from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_read_from_filepath(chi_string* chi_str, const char* filepath);

/**
 * @brief Reads up to `n` characters from the given file into the end of the chi_string.
 *
 * @param chi_str The string to read into.
 * @param file The file to read from.
 * @param n The maximum number of characters to read.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_read_to_end_n(chi_string* chi_str, FILE* file, int n);

/**
 * @brief Reads a line from the given file into the end of the chi_string.
 *
 * @param chi_str The string to read into.
 * @param file The file to read from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_read_to_end(chi_string* chi_str, FILE* file);

/**
 * @brief Writes the contents of the chi_string to the specified file.
 *
 * @param chi_str The string to write from.
 * @param file The file to write to.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_write(const chi_string* chi_str, FILE* file);

/**
 * @brief Writes the contents of the chi_string to the specified file path.
 *
 * @param chi_str The string to write from.
 * @param filepath The path to the file to write to.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_write_fp(const chi_string* chi_str, const char* filepath);

/**
 * @brief Appends the contents of the chi_string to the specified file path.
 *
 * This function appends the data contained in the given `chi_string` to a file specified by the `filepath`.
 * If the file does not exist, it will be created.
 *
 * @param chi_str The chi_string containing the data to append.
 * @param filepath The path to the file where the data will be appended.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_app_to(const chi_string* chi_str, const char* filepath);

/**
 * @brief Reads up to `n` characters from standard input into the chi_string.
 *
 * @param chi_str The string to read into.
 * @param n The maximum number of characters to read.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_scan_n(chi_string* chi_str, int n);

/**
 * @brief Reads a line from standard input into the chi_string.
 *
 * @param chi_str The string to read into.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_scan(chi_string* chi_str);

/**
 * @brief Reads up to `n` characters from standard input into the end of the chi_string.
 *
 * @param chi_str The string to read into.
 * @param n The maximum number of characters to read.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_scan_to_end_n(chi_string* chi_str, int n);

/**
 * @brief Reads a line from standard input into the end of the chi_string.
 *
 * @param chi_str The string to read into.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_scan_to_end(chi_string* chi_str);

/**
 * @brief Writes the contents of the chi_string to standard output.
 *
 * @param chi_str The string to write from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_print(const chi_string* chi_str);

/**
 * @brief Writes the contents of the chi_string to standard output followed by a newline.
 *
 * @param chi_str The string to write from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_println(const chi_string* chi_str);

/**
 * @brief Writes the contents of the chi_string to standard error.
 *
 * @param chi_str The string to write from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_printerr(const chi_string* chi_str);



/* CHI STRING END */

/* CHI STRING VIEW BEGIN */

/**
 * @brief Writes the contents of the chi_string_view to the specified file.
 *
 * @param sv The string view to write from.
 * @param file The file to write to.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_sv_writef(chi_string_view sv, FILE* file);

/**
 * @brief Writes the contents of the chi_string_view to the specified file path.
 *
 * @param sv The string view to write from.
 * @param filepath The path to the file to write to.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_sv_writefp(chi_string_view sv, const char* filepath);

/**
 * @brief Writes the contents of the chi_string_view to standard output.
 *
 * @param sv The string view to write from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_sv_print(chi_string_view sv);

/**
 * @brief Writes the contents of the chi_string_view to standard output followed by a newline.
 *
 * @param sv The string view to write from.
 * @return true or false indicating success or failure.
 */
CHI_API bool chi_sv_println(chi_string_view sv);

/* CHI STRING VIEW END */

_CHI_HEADER_END

#endif // !_CHI_IO_