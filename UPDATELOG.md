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
* Update 1.10.0: Added chi_erase and chi_erase_from functions.
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
* Update 1.20.2: chi_str_array_free has been added.
* Update 1.21.0: Instead of string_list and string_list_array, there is now pointer_list and pointer_list_array. Functions that use these structures have been updated according to the change. (Update 1.21.1)
* Update 1.21.1: The implementation functions _add_string_to_list and _remove_str_from_list have been removed and replaced with _add_ptr_to_list and _remove_ptr_from_list. The _top_list function has been updated to take a pointer_list_array parameter and return a pointer_list.
* Update 1.21.2: Now not only chi_strings but also all pointers created in chi are freed with chi_cleanup or chi_begin_scope - chi_end_scope.
* Update 1.21.3: chi_str_array_free function has been updated. If the pointer we delete with this function is in our pointer list, it is also removed from the list.
* Update 1.22.0: To make it easier for those who want to examine the implementation code, the defined macros are separated into lines according to their types. (Update note: The updates coming with 1.22.xx consist of minor fixes.)
* Update 1.22.1: The use of CHECK_N has been removed from unnecessary places and moved to the correct places.
* Update 1.22.2: The return value of the _remove_ptr_from_list function has been changed from void to bool.
* Update 1.22.3: The comment lines containing the macros that were intended to be defined for pointer_list were deleted because a different implementation method was used.
* Update 1.23.0: Added chi_ignore_freeing, added chi_add_to_the_free_list functions.
* Update 1.24.0: chi_strings can now be constants. Added chi_set_constant_state and chi_is_const.
* Update 1.24.1: Updated chi_front and chi_back.
* Update 1.24.2: Updated chi_at, chi_begin and chi_end.
* Update 1.24.3: Added chi_cat, chi_cbegin and chi_cend.
* Update 1.24.4: Updated CHI_AT, CHI_FRONT and CHI_BACK.
* Update 1.24.5: Added CHI_BEGIN AND CHI_END.
* Update 1.24.6: Fixed chi_cbegin warning error.
* Update 1.24.7: chi_set_constant_state removed and chi_add_const, chi_remove_const, chi_set_const_key added. (chi_strings will now be made constant with a specific key and cannot be made non-constant without entering that key.)
* Update 2.0.0: Version 2.0 update: (2.0.1 to 2.5.2)
* Update 2.0.1: CHI_S - CHI_SV removed.
* Update 2.0.2: CHI_CSNULL removed.
* Update 2.0.3: CHI_BEGIN - CHI_END removed.
* Update 2.0.4: CHI_BEGIN - CHI_END removed.
* Update 2.0.5: chi_make_chi_n - chi_make_chi_n_c removed. (In fact, chi_make_chi and chi_make_chi_c now do the same thing.)
* Update 2.1.0: Removed chi string locking with keys. (chi_add_const - chi_remove_const - chi_is_const - chi_reset_const_key)
* Update 2.1.1: Fixed CHI_AT, CHI_FRONT and CHI_BACK.
* Update 2.2.0: chi_replace_ip added.
* Update 2.2.1: chi_sv_create renamed to chi_sv. chi_sv_create_n renamed to chi_sv_n. chi_sv_create_from_chi_s renamed to chi_sv_from_chi_s.
* Update 2.3.0: New sv count functions has been added. (chi_sv_count_ip, chi_sv_count, chi_sv_count_s_ip, chi_sv_count_s, chi_sv_count_cs_ip, chi_sv_count_cs, chi_sv_count_sv_ip, chi_sv_count_sv, chi_sv_count_if_ip, chi_sv_count_if)
* Update 2.3.1: New sv predicate algorithm functions has been added. (chi_sv_all_of_ip, chi_sv_all_of, chi_sv_any_of_ip, chi_sv_any_of_ip, chi_sv_none_of_ip, chi_sv_none_of)
* Update 2.3.2: chi_sv_sample_ip and chi_sv_sample functions added.
* Update 2.3.3: New sv classification functions has been added. (chi_sv_islower_ip, chi_sv_islower, chi_sv_isupper_ip, chi_sv_isupper, chi_sv_isalnum_ip, chi_sv_isalnum, chi_sv_isalpha_ip, chi_sv_isalpha, chi_sv_isdigit_ip, chi_sv_isdigit, chi_sv_isprintable_ip, chi_sv_isprintable, chi_sv_isspace_ip, chi_sv_isspace)
* Update 2.4.0: chi_format has been added. chi_string generating functions (chi_create... , chi_make...) can now take arguments. The arguments will be written in the string as {i} (int). For example: chi_string_create("Hello {i} world{c} {s}", 100, '!', "This is a message");
* Update 2.4.1: chi_set_next_items_no_format has been added. With this function, the formatting process will be ignored for the number entered. Note: Since the purpose of the chi_format function is to perform direct formatting, it cannot be ignored.
* Update 2.5.0: chi_snull fixed.
* Update 2.5.1: All replace functions fixed.
* Update 2.5.2: chi_count_... implementations has changed. This allows chi_sv_count_... functions to use the same implementation.
* Update 2.5.3: FIX TYPO.
* Update 2.5.4: Added missing doxygen format function description comments.
* Update 2.6.0: Some adjustments have been made to the code in preparation for chi_arena.
* Update 2.6.1: Macro usages that can be functionalized have been converted into functions.
* Update 2.6.2: chi_use_arena, chi_unuse_arena, chi_arena_is_using, chi_arena_create_str, chi_arena_free, chi_arena_create_ptr functions has been added, Now you can use arenas for local usage.
* Update 2.6.3: chi_arena_load and chi_arena_create_empty_str functions has been added.
* Update 2.6.4: chi_substring_to function has been added.
* Update 2.6.5: The ptr-chi_string distinction in the implementation has been removed. There is no need to use separate arenas for both anymore. It will be possible to track ptr or chi_string in a single arena.
* Update 2.6.6: Some implementation issues have been fixed. Some unnecessary codes have been removed.
* Update 2.6.7: The part containing variables and structures in the chi.c file has been made more organized.