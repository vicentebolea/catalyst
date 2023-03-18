/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include "catalyst_conduit_abi.h"

#include <conduit.h>
#include <conduit_blueprint.h>

// Also bring in compatibility symbols.
#include "catalyst_conduit_abi_internal.c"

#define f(rettype, name, args, arglist)                                                            \
  rettype catalyst_##name args { return name arglist; }

// conduit.h
f(void, conduit_about, (conduit_node * node), (node))

// conduit_datatype.h
f(int, conduit_datatype_sizeof_index_t, (), ())

f(conduit_index_t, conduit_datatype_id, (const conduit_datatype* dt), (dt))
f(char*, conduit_datatype_name, (const conduit_datatype* dt), (dt))
f(void, conduit_datatype_name_destroy, (char* name), (name))

#define f_dt_prop(name) f(conduit_index_t, name, (const conduit_datatype* dt), (dt))

f_dt_prop(conduit_datatype_number_of_elements)
f_dt_prop(conduit_datatype_offset)
f_dt_prop(conduit_datatype_stride)
f_dt_prop(conduit_datatype_element_bytes)
f_dt_prop(conduit_datatype_endianness)
f(conduit_index_t, conduit_datatype_element_index, (const conduit_datatype* dt, conduit_index_t idx), (dt, idx))

#define f_dt_query(name) f(int, name, (const conduit_datatype* dt), (dt))

f_dt_query(conduit_datatype_is_empty)
f_dt_query(conduit_datatype_is_object)
f_dt_query(conduit_datatype_is_list)

f_dt_query(conduit_datatype_is_number)
f_dt_query(conduit_datatype_is_floating_point)
f_dt_query(conduit_datatype_is_integer)
f_dt_query(conduit_datatype_is_signed_integer)
f_dt_query(conduit_datatype_is_unsigned_integer)

f_dt_query(conduit_datatype_is_int8)
f_dt_query(conduit_datatype_is_int16)
f_dt_query(conduit_datatype_is_int32)
f_dt_query(conduit_datatype_is_int64)

f_dt_query(conduit_datatype_is_uint8)
f_dt_query(conduit_datatype_is_uint16)
f_dt_query(conduit_datatype_is_uint32)
f_dt_query(conduit_datatype_is_uint64)

f_dt_query(conduit_datatype_is_float32)
f_dt_query(conduit_datatype_is_float64)

f_dt_query(conduit_datatype_is_char)
f_dt_query(conduit_datatype_is_short)
f_dt_query(conduit_datatype_is_int)
f_dt_query(conduit_datatype_is_long)

f_dt_query(conduit_datatype_is_unsigned_char)
f_dt_query(conduit_datatype_is_unsigned_short)
f_dt_query(conduit_datatype_is_unsigned_int)
f_dt_query(conduit_datatype_is_unsigned_long)

f_dt_query(conduit_datatype_is_float)
f_dt_query(conduit_datatype_is_double)

f_dt_query(conduit_datatype_is_string)
f_dt_query(conduit_datatype_is_char8_str)

f_dt_query(conduit_datatype_is_little_endian)
f_dt_query(conduit_datatype_is_big_endian)
f_dt_query(conduit_datatype_endianness_matches_machine)

// conduit_node.h
f(conduit_node*, conduit_node_create, (), ())
f(void, conduit_node_destroy, (conduit_node* node), (node))

f(conduit_node*, conduit_node_fetch, (conduit_node* node, const char* path), (node, path))
f(conduit_node*, conduit_node_fetch_existing, (conduit_node* node, const char* path), (node, path))
f(conduit_node*, conduit_node_append, (conduit_node* node), (node))
f(conduit_node*, conduit_node_add_child, (conduit_node* node, const char* name), (node, name))
f(conduit_node*, conduit_node_child, (conduit_node* node, conduit_index_t idx), (node, idx))
f(conduit_node*, conduit_node_child_by_name, (conduit_node* node, const char* name), (node, name))

f(conduit_index_t, conduit_node_number_of_children, (conduit_node* node), (node))
f(conduit_index_t, conduit_node_number_of_elements, (conduit_node* node), (node))

f(void, conduit_node_reset, (conduit_node *node), (node))
f(void, conduit_node_move, (conduit_node *node_a, conduit_node *node_b), (node_a, node_b))
f(void, conduit_node_swap, (conduit_node *node_a, conduit_node *node_b), (node_a, node_b))

f(void, conduit_node_remove_path, (conduit_node* node, const char* path), (node, path))
f(void, conduit_node_remove_child, (conduit_node* node, conduit_index_t idx), (node, idx))
f(void, conduit_node_remove_child_by_name, (conduit_node* node, const char* name), (node, name))
f(char*, conduit_node_name, (const conduit_node* node), (node))
f(char*, conduit_node_path, (const conduit_node* node), (node))
f(int, conduit_node_has_child, (const conduit_node* node, const char* name), (node, name))
f(int, conduit_node_has_path, (const conduit_node* node, const char* path), (node, path))
f(void, conduit_node_rename_child, (conduit_node* node, const char* current_name, const char* new_name), (node, current_name, new_name))

f(int, conduit_node_is_root, (conduit_node* node), (node))
f(int, conduit_node_is_data_external, (const conduit_node* node), (node))

f(conduit_node*, conduit_node_parent, (conduit_node* node), (node))

f(conduit_index_t, conduit_node_total_strided_bytes, (const conduit_node* node), (node))
f(conduit_index_t, conduit_node_total_bytes_compact, (const conduit_node* node), (node))
f(conduit_index_t, conduit_node_total_bytes_allocated, (const conduit_node* node), (node))

f(int, conduit_node_is_compact, (const conduit_node* node), (node))

f(int, conduit_node_is_contiguous, (const conduit_node* node), (node))
f(int, conduit_node_contiguous_with_node, (const conduit_node* node, const conduit_node* other), (node, other))
f(int, conduit_node_contiguous_with_address, (const conduit_node* node, void* address), (node, address))

f(int, conduit_node_diff, (const conduit_node* node, const conduit_node* other, conduit_node* info, conduit_float64 epsilon), (node, other, info, epsilon))
f(int, conduit_node_diff_compatible, (const conduit_node* node, const conduit_node* other, conduit_node* info, conduit_float64 epsilon), (node, other, info, epsilon))
f(int, conduit_node_compatible, (const conduit_node* node, const conduit_node* other), (node, other))

f(void, conduit_node_info, (const conduit_node* node, conduit_node* nres), (node, nres))
f(void, conduit_node_print, (conduit_node* node), (node))
f(void, conduit_node_print_detailed, (conduit_node* node), (node))

f(void, conduit_node_compact_to, (const conduit_node* node, conduit_node* nres), (node, nres))

f(void, conduit_node_update, (conduit_node* node, const conduit_node* other), (node, other))
f(void, conduit_node_update_compatible, (conduit_node* node, const conduit_node* other), (node, other))
f(void, conduit_node_update_external, (conduit_node* node, conduit_node* other), (node, other))

f(void, conduit_node_parse, (conduit_node* node, const char* schema, const char* protocol), (node, schema, protocol))

f(void, conduit_node_generate, (conduit_node* node, const char* schema, const char* protocol, void* data), (node, schema, protocol, data))
f(void, conduit_node_generate_external, (conduit_node* node, const char* schema, const char* protocol, void* data), (node, schema, protocol, data))

f(void, conduit_node_save, (conduit_node* node, const char* path, const char* protocol), (node, path, protocol))
f(void, conduit_node_load, (conduit_node* node, const char* path, const char* protocol), (node, path, protocol))

f(char*, conduit_node_to_json, (const conduit_node* node), (node))
f(char*, conduit_node_to_json_with_options, (const conduit_node* node, const conduit_node* opts), (node, opts))

f(char*, conduit_node_to_yaml, (const conduit_node* node), (node))
f(char*, conduit_node_to_yaml_with_options, (const conduit_node* node, const conduit_node* opts), (node, opts))

f(char*, conduit_node_to_string, (const conduit_node* node), (node))
f(char*, conduit_node_to_string_with_options, (const conduit_node* node, const conduit_node* opts), (node, opts))

f(char*, conduit_node_to_summary_string, (const conduit_node* node), (node))
f(char*, conduit_node_to_summary_string_with_options, (const conduit_node* node, const conduit_node* opts), (node, opts))

f(void, conduit_node_set_node, (conduit_node* node, conduit_node* data), (node, data))
f(void, conduit_node_set_path_node, (conduit_node* node, const char* path, conduit_node* data), (node, path, data))
f(void, conduit_node_set_external_node, (conduit_node* node, conduit_node* data), (node, data))
f(void, conduit_node_set_path_external_node, (conduit_node* node, const char* path, conduit_node* data), (node, path, data))

f(void, conduit_node_set_char8_str, (conduit_node* node, const char* value), (node, value))
f(void, conduit_node_set_path_char8_str, (conduit_node* node, const char* path, const char* value), (node, path, value))
f(void, conduit_node_set_external_char8_str, (conduit_node* node, char* value), (node, value))
f(void, conduit_node_set_path_external_char8_str, (conduit_node* node, const char* path, char* value), (node, path, value))

#define f_node_scalars(call)                                                                       \
  call(int8, conduit_int8)                                                                         \
  call(int16, conduit_int16)                                                                       \
  call(int32, conduit_int32)                                                                       \
  call(int64, conduit_int64)                                                                       \
  call(uint8, conduit_uint8)                                                                       \
  call(uint16, conduit_uint16)                                                                     \
  call(uint32, conduit_uint32)                                                                     \
  call(uint64, conduit_uint64)                                                                     \
  call(float32, conduit_float32)                                                                   \
  call(float64, conduit_float64)                                                                   \
  call(char, char)                                                                                 \
  call(short, short)                                                                               \
  call(int, int)                                                                                   \
  call(long, long)                                                                                 \
  call(signed_char, signed char)                                                                   \
  call(signed_short, signed short)                                                                 \
  call(signed_int, signed int)                                                                     \
  call(signed_long, signed long)                                                                   \
  call(unsigned_char, unsigned char)                                                               \
  call(unsigned_short, unsigned short)                                                             \
  call(unsigned_int, unsigned int)                                                                 \
  call(unsigned_long, unsigned long)                                                               \
  call(float, float)                                                                               \
  call(double, double)

#define f_node_accesses(typename, type)                                                            \
  f(void, conduit_node_set_##typename, (conduit_node* node, type value), (node, value))            \
  f(void, conduit_node_set_##typename##_ptr,                                                       \
    (conduit_node* node, type* value, conduit_index_t num_elements), (node, value, num_elements))  \
  f(void, conduit_node_set_##typename##_ptr_detailed,                                              \
    (conduit_node* node, type* value, conduit_index_t num_elements, conduit_index_t offset,        \
     conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness),           \
    (node, value, num_elements, offset, stride, element_bytes, endianness))                        \
  f(void, conduit_node_set_path_##typename,                                                        \
    (conduit_node* node, const char* path, type value),                                            \
    (node, path, value))                                                                           \
  f(void, conduit_node_set_path_##typename##_ptr,                                                  \
    (conduit_node* node, const char* path, type* value, conduit_index_t num_elements),             \
    (node, path, value, num_elements))                                                             \
  f(void, conduit_node_set_path_##typename##_ptr_detailed,                                         \
    (conduit_node* node, const char* path, type* value, conduit_index_t num_elements,              \
     conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,                \
     conduit_index_t endianness),                                                                  \
    (node, path, value, num_elements, offset, stride, element_bytes, endianness))                  \
  f(void, conduit_node_set_external_##typename##_ptr,                                              \
    (conduit_node* node, type* value, conduit_index_t num_elements),                               \
    (node, value, num_elements))                                                                   \
  f(void, conduit_node_set_external_##typename##_ptr_detailed,                                     \
    (conduit_node* node, type* value, conduit_index_t num_elements, conduit_index_t offset,        \
     conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness),           \
    (node, value, num_elements, offset, stride, element_bytes, endianness))                        \
  f(void, conduit_node_set_path_external_##typename##_ptr,                                         \
    (conduit_node* node, const char* path, type* value, conduit_index_t num_elements),             \
    (node, path, value, num_elements))                                                             \
  f(void, conduit_node_set_path_external_##typename##_ptr_detailed,                                \
    (conduit_node* node, const char* path, type* value, conduit_index_t num_elements,              \
     conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,                \
     conduit_index_t endianness),                                                                  \
     (node, path, value, num_elements, offset, stride, element_bytes, endianness))                 \
  f(type, conduit_node_as_##typename, (conduit_node* node), (node))                                \
  f(type*, conduit_node_as_##typename##_ptr, (conduit_node* node), (node))                         \
  f(type, conduit_node_fetch_path_as_##typename, (conduit_node* node, const char* path),           \
    (node, path))                                                                                  \
  f(type*, conduit_node_fetch_path_as_##typename##_ptr, (conduit_node* node, const char* path),    \
    (node, path))

f_node_scalars(f_node_accesses)

f(void*, conduit_node_data_ptr, (conduit_node* node), (node))
f(void*, conduit_node_element_ptr, (conduit_node* node, conduit_index_t idx), (node, idx))

f(void*, conduit_node_fetch_path_data_ptr, (conduit_node* node, const char* path), (node, path))
f(void*, conduit_node_fetch_path_element_ptr, (conduit_node* node, const char* path, conduit_index_t idx), (node, path, idx))

f(char*, conduit_node_as_char8_str, (conduit_node* node), (node))
f(char*, conduit_node_fetch_path_as_char8_str, (conduit_node* node, const char* path), (node, path))

f(const conduit_datatype*, conduit_node_dtype, (const conduit_node* node), (node))

// conduit_utils.h
f(void, conduit_utils_set_info_handler, (void(*on_info) (const char* , const char* , int)), (on_info))
f(void, conduit_utils_set_warning_handler, (void(*on_warning) (const char* , const char* , int)), (on_warning))
f(void, conduit_utils_set_error_handler, (void(*on_error) (const char* , const char* , int)), (on_error))

// conduit_blueprint.h
f(void, conduit_blueprint_about, (conduit_node* node), (node))
f(int, conduit_blueprint_verify, (const char* protocol, const conduit_node* node, conduit_node* info), (protocol, node, info))

// conduit_blueprint_mcarray.h
f(int, conduit_blueprint_mcarray_verify, (const conduit_node* node, conduit_node* info), (node, info))
f(int, conduit_blueprint_mcarray_verify_sub_protocol, (const char* protocol, const conduit_node* node, conduit_node* info), (protocol, node, info))
f(int, conduit_blueprint_mcarray_is_interleaved, (const conduit_node* node), (node))
f(int, conduit_blueprint_mcarray_to_contiguous, (const conduit_node* node, conduit_node* dest), (node, dest))
f(int, conduit_blueprint_mcarray_to_interleaved, (const conduit_node* node, conduit_node* dest), (node, dest))

// conduit_blueprint_mesh.h
f(int, conduit_blueprint_mesh_verify, (const conduit_node* node, conduit_node* info), (node, info))
f(int, conduit_blueprint_mesh_verify_sub_protocol, (const char* protocol, const conduit_node* node, conduit_node* info), (protocol, node, info))
f(void, conduit_blueprint_mesh_generate_index, (const conduit_node* mesh, const char* ref_path, conduit_index_t num_domains, conduit_node* index_out), (mesh, ref_path, num_domains, index_out))
f(void, conduit_blueprint_mesh_partition, (const conduit_node* mesh, const conduit_node* options, conduit_node* output), (mesh, options, output))
f(void, conduit_blueprint_mesh_flatten, (const conduit_node* mesh, const conduit_node* options, conduit_node* output), (mesh, options, output))

// conduit_blueprint_table.h
f(int, conduit_blueprint_table_verify, (const conduit_node* node, conduit_node* info), (node, info))
f(int, conduit_blueprint_table_verify_sub_protocol, (const char* protocol, const conduit_node* node, conduit_node* info), (protocol, node, info))
