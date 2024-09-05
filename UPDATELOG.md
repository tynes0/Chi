### UPDATELOG

* Update 1.0.0: Project Setup
* Update 1.1.0: Added sv descriptions, _CHI_HEADER_ macros added to chi_io.
* Update 1.2.0: Algorithm functions has been updated and chi_sample & chi_sample_ip functions has been added.
* Update 1.3.0: Fixed warning C4267 and added CHI_CHECK_RETURN to chi_sample - chi_sample_ip.
* Update 1.4.0: chi_io update -> read and scan functions changed and optimized. 
* Update 1.4.1: chi_io update -> MAX_BUFFER_LEN for read and scan functions, increased from 1024 to 8192.
* Update 1.5.0: to_upper - to_lower functions has been added.
* Update 1.6.0: chi_errno_t removed - 1
* Update 1.6.1: chi_errno_t removed - 2 - finished.
* Update 1.7.0: Fixed an issue where chi create functions could not be added to a string list in some cases.
* Update 1.8.0: CHI_SNULL edited. CHI_CSNULL added, chi_null() added.
* Update 1.9.0: Fixed a bug in the chi_create functions that caused the size of the generated chi_string to be 0.
* Update 1.10.0: Added chi_erase and chi_erase_from functions
* Update 1.11.0: Fixed a bug that caused the size to be incorrect in the chi_reset function. 
* Update 1.12.0: Added chi_cstr, added chi_reset_cs.
* Update 1.13.0: Added ctype functions for chi_strings.
* Update 1.14.0: chi_null() function removed, chi_snull static constant added.
* Update 1.15.0: chi_create_with_capacity() and chi_printerr functions added.
* Update 1.15.1: chi_printerr return type has been fixed.
* Update 1.15.2: chi_sv_printerr() function has been added.
* Update 1.16.0: Added chi_ignore_spaces function to ignore whitespace characters for some ctype functions.
* Update 1.17.0: chi_switch_cases and chi_switch_cases_ip has been added.
* Update 1.18.0: Fixed some pragmas that were specific to msvc compilers. Note that: Will be fixed for more compilers in the future.
* Update 1.18.1: Fixed an error that occurred when the size was 0 in the chi_create_n function.
* Update 1.18.2: Fixed typos in the _add_string_to_list implementation function.
* Update 1.18.3: The assertions of null chi_strings in the write functions were replaced with empty writes. Unnecessary assertions were removed.
* Update 1.19.0: chi_count_s_ip & chi_count_s functions has been added.
* Update 1.19.1: chi_count_cs_ip & chi_count_cs functions has been added.
* Update 1.19.2: chi_count_sv_ip & chi_count_sv functions has been added.
* Update 1.20.0: chi_split function has been added.
* Update 1.20.1: chi_split_c, chi_split_s, chi_split_cs, chi_split_sv functions has been added.