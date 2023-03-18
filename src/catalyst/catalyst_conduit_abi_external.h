/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#ifndef catalyst_conduit_abi_h
#define catalyst_conduit_abi_h

#include <catalyst_export.h>

#include <conduit.h>

#ifdef __cplusplus
extern "C"
{
#endif

  // conduit.h
  CATALYST_EXPORT void catalyst_conduit_about(conduit_node* node);

  // conduit_datatype.h
  CATALYST_EXPORT int catalyst_conduit_datatype_sizeof_index_t();

  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_id(const conduit_datatype* dt);
  CATALYST_EXPORT char* catalyst_conduit_datatype_name(const conduit_datatype* dt);
  CATALYST_EXPORT void catalyst_conduit_datatype_name_destroy(char* name);

  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_number_of_elements(
    const conduit_datatype* dt);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_offset(const conduit_datatype* dt);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_stride(const conduit_datatype* dt);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_element_bytes(
    const conduit_datatype* dt);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_endianness(const conduit_datatype* dt);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_datatype_element_index(
    const conduit_datatype* dt, conduit_index_t idx);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_empty(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_object(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_list(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_number(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_floating_point(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_integer(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_signed_integer(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_unsigned_integer(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_int8(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_int16(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_int32(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_int64(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_uint8(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_uint16(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_uint32(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_uint64(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_float32(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_float64(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_char(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_short(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_int(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_long(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_unsigned_char(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_unsigned_short(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_unsigned_int(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_unsigned_long(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_float(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_double(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_string(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_char8_str(const conduit_datatype* dt);

  CATALYST_EXPORT int catalyst_conduit_datatype_is_little_endian(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_is_big_endian(const conduit_datatype* dt);
  CATALYST_EXPORT int catalyst_conduit_datatype_endianness_matches_machine(
    const conduit_datatype* dt);

  // conduit_node.h
  CATALYST_EXPORT conduit_node* catalyst_conduit_node_create();
  CATALYST_EXPORT void catalyst_conduit_node_destroy(conduit_node* node);

  CATALYST_EXPORT conduit_node* catalyst_conduit_node_fetch(conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_node* catalyst_conduit_node_fetch_existing(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_node* catalyst_conduit_node_append(conduit_node* node);
  CATALYST_EXPORT conduit_node* catalyst_conduit_node_add_child(
    conduit_node* node, const char* name);
  CATALYST_EXPORT conduit_node* catalyst_conduit_node_child(
    conduit_node* node, conduit_index_t idx);
  CATALYST_EXPORT conduit_node* catalyst_conduit_node_child_by_name(
    conduit_node* node, const char* name);

  CATALYST_EXPORT conduit_index_t catalyst_conduit_node_number_of_children(conduit_node* node);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_node_number_of_elements(conduit_node* node);

  CATALYST_EXPORT void catalyst_conduit_node_reset(conduit_node* node);
  CATALYST_EXPORT void catalyst_conduit_node_move(conduit_node* node_a, conduit_node* node_b);
  CATALYST_EXPORT void catalyst_conduit_node_swap(conduit_node* node_a, conduit_node* node_b);

  CATALYST_EXPORT void catalyst_conduit_node_remove_path(conduit_node* node, const char* path);
  CATALYST_EXPORT void catalyst_conduit_node_remove_child(conduit_node* node, conduit_index_t idx);
  CATALYST_EXPORT void catalyst_conduit_node_remove_child_by_name(
    conduit_node* node, const char* name);
  CATALYST_EXPORT char* catalyst_conduit_node_name(const conduit_node* node);
  CATALYST_EXPORT char* catalyst_conduit_node_path(const conduit_node* node);
  CATALYST_EXPORT int catalyst_conduit_node_has_child(const conduit_node* node, const char* name);
  CATALYST_EXPORT int catalyst_conduit_node_has_path(const conduit_node* node, const char* path);
  CATALYST_EXPORT void catalyst_conduit_node_rename_child(
    conduit_node* node, const char* current_name, const char* new_name);

  CATALYST_EXPORT int catalyst_conduit_node_is_root(conduit_node* node);
  CATALYST_EXPORT int catalyst_conduit_node_is_data_external(const conduit_node* node);

  CATALYST_EXPORT conduit_node* catalyst_conduit_node_parent(conduit_node* node);

  CATALYST_EXPORT conduit_index_t catalyst_conduit_node_total_strided_bytes(
    const conduit_node* node);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_node_total_bytes_compact(
    const conduit_node* node);
  CATALYST_EXPORT conduit_index_t catalyst_conduit_node_total_bytes_allocated(
    const conduit_node* node);

  CATALYST_EXPORT int catalyst_conduit_node_is_compact(const conduit_node* node);

  CATALYST_EXPORT int catalyst_conduit_node_is_contiguous(const conduit_node* node);
  CATALYST_EXPORT int catalyst_conduit_node_contiguous_with_node(
    const conduit_node* node, const conduit_node* other);
  CATALYST_EXPORT int catalyst_conduit_node_contiguous_with_address(
    const conduit_node* node, void* address);

  CATALYST_EXPORT int catalyst_conduit_node_diff(const conduit_node* node,
    const conduit_node* other, conduit_node* info, conduit_float64 epsilon);
  CATALYST_EXPORT int catalyst_conduit_node_diff_compatible(const conduit_node* node,
    const conduit_node* other, conduit_node* info, conduit_float64 epsilon);
  CATALYST_EXPORT int catalyst_conduit_node_compatible(
    const conduit_node* node, const conduit_node* other);

  CATALYST_EXPORT void catalyst_conduit_node_info(const conduit_node* node, conduit_node* nres);
  CATALYST_EXPORT void catalyst_conduit_node_print(conduit_node* node);
  CATALYST_EXPORT void catalyst_conduit_node_print_detailed(conduit_node* node);

  CATALYST_EXPORT void catalyst_conduit_node_compact_to(
    const conduit_node* node, conduit_node* nres);

  CATALYST_EXPORT void catalyst_conduit_node_update(conduit_node* node, const conduit_node* other);
  CATALYST_EXPORT void catalyst_conduit_node_update_compatible(
    conduit_node* node, const conduit_node* other);
  CATALYST_EXPORT void catalyst_conduit_node_update_external(
    conduit_node* node, conduit_node* other);

  CATALYST_EXPORT void catalyst_conduit_node_parse(
    conduit_node* node, const char* schema, const char* protocol);

  CATALYST_EXPORT void catalyst_conduit_node_generate(
    conduit_node* node, const char* schema, const char* protocol, void* data);
  CATALYST_EXPORT void catalyst_conduit_node_generate_external(
    conduit_node* node, const char* schema, const char* protocol, void* data);

  CATALYST_EXPORT void catalyst_conduit_node_save(
    conduit_node* node, const char* path, const char* protocol);
  CATALYST_EXPORT void catalyst_conduit_node_load(
    conduit_node* node, const char* path, const char* protocol);

  CATALYST_EXPORT char* catalyst_conduit_node_to_json(const conduit_node* node);
  CATALYST_EXPORT char* catalyst_conduit_node_to_json_with_options(
    const conduit_node* node, const conduit_node* opts);

  CATALYST_EXPORT char* catalyst_conduit_node_to_yaml(const conduit_node* node);
  CATALYST_EXPORT char* catalyst_conduit_node_to_yaml_with_options(
    const conduit_node* node, const conduit_node* opts);

  CATALYST_EXPORT char* catalyst_conduit_node_to_string(const conduit_node* node);
  CATALYST_EXPORT char* catalyst_conduit_node_to_string_with_options(
    const conduit_node* node, const conduit_node* opts);

  CATALYST_EXPORT char* catalyst_conduit_node_to_summary_string(const conduit_node* node);
  CATALYST_EXPORT char* catalyst_conduit_node_to_summary_string_with_options(
    const conduit_node* node, const conduit_node* opts);

  CATALYST_EXPORT void catalyst_conduit_node_set_node(conduit_node* node, conduit_node* data);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_node(
    conduit_node* node, const char* path, conduit_node* data);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_node(
    conduit_node* node, conduit_node* data);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_node(
    conduit_node* node, const char* path, conduit_node* data);

  CATALYST_EXPORT void catalyst_conduit_node_set_char8_str(conduit_node* node, const char* value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_char8_str(
    conduit_node* node, const char* path, const char* value);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_char8_str(
    conduit_node* node, char* value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_char8_str(
    conduit_node* node, const char* path, char* value);

  CATALYST_EXPORT void catalyst_conduit_node_set_int8(conduit_node* node, conduit_int8 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_int16(conduit_node* node, conduit_int16 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_int32(conduit_node* node, conduit_int32 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_int64(conduit_node* node, conduit_int64 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint8(conduit_node* node, conduit_uint8 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint16(conduit_node* node, conduit_uint16 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint32(conduit_node* node, conduit_uint32 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint64(conduit_node* node, conduit_uint64 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_float32(conduit_node* node, conduit_float32 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_float64(conduit_node* node, conduit_float64 value);

  CATALYST_EXPORT void catalyst_conduit_node_set_int8_ptr(
    conduit_node* node, conduit_int8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_int8_ptr_detailed(conduit_node* node,
    conduit_int8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_int16_ptr(
    conduit_node* node, conduit_int16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_int16_ptr_detailed(conduit_node* node,
    conduit_int16* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_int32_ptr(
    conduit_node* node, conduit_int32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_int32_ptr_detailed(conduit_node* node,
    conduit_int32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_int64_ptr(
    conduit_node* node, conduit_int64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_int64_ptr_detailed(conduit_node* node,
    conduit_int64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint8_ptr(
    conduit_node* node, conduit_uint8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint8_ptr_detailed(conduit_node* node,
    conduit_uint8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint16_ptr(
    conduit_node* node, conduit_uint16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint16_ptr_detailed(conduit_node* node,
    conduit_uint16* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint32_ptr(
    conduit_node* node, conduit_uint32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint32_ptr_detailed(conduit_node* node,
    conduit_uint32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint64_ptr(
    conduit_node* node, conduit_uint64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_uint64_ptr_detailed(conduit_node* node,
    conduit_uint64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_float32_ptr(
    conduit_node* node, conduit_float32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_float32_ptr_detailed(conduit_node* node,
    conduit_float32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_float64_ptr(
    conduit_node* node, conduit_float64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_float64_ptr_detailed(conduit_node* node,
    conduit_float64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_path_int8(
    conduit_node* node, const char* path, conduit_int8 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int16(
    conduit_node* node, const char* path, conduit_int16 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int32(
    conduit_node* node, const char* path, conduit_int32 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int64(
    conduit_node* node, const char* path, conduit_int64 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint8(
    conduit_node* node, const char* path, conduit_uint8 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint16(
    conduit_node* node, const char* path, conduit_uint16 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint32(
    conduit_node* node, const char* path, conduit_uint32 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint64(
    conduit_node* node, const char* path, conduit_uint64 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float32(
    conduit_node* node, const char* path, conduit_float32 value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float64(
    conduit_node* node, const char* path, conduit_float64 value);

  CATALYST_EXPORT void catalyst_conduit_node_set_path_int8_ptr(
    conduit_node* node, const char* path, conduit_int8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int8_ptr_detailed(conduit_node* node,
    const char* path, conduit_int8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int16_ptr(
    conduit_node* node, const char* path, conduit_int16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int16_ptr_detailed(conduit_node* node,
    const char* path, conduit_int16* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int32_ptr(
    conduit_node* node, const char* path, conduit_int32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int32_ptr_detailed(conduit_node* node,
    const char* path, conduit_int32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int64_ptr(
    conduit_node* node, const char* path, conduit_int64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int64_ptr_detailed(conduit_node* node,
    const char* path, conduit_int64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint8_ptr(
    conduit_node* node, const char* path, conduit_uint8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint8_ptr_detailed(conduit_node* node,
    const char* path, conduit_uint8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint16_ptr(
    conduit_node* node, const char* path, conduit_uint16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint16_ptr_detailed(conduit_node* node,
    const char* path, conduit_uint16* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint32_ptr(
    conduit_node* node, const char* path, conduit_uint32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint32_ptr_detailed(conduit_node* node,
    const char* path, conduit_uint32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint64_ptr(
    conduit_node* node, const char* path, conduit_uint64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_uint64_ptr_detailed(conduit_node* node,
    const char* path, conduit_uint64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float32_ptr(
    conduit_node* node, const char* path, conduit_float32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float32_ptr_detailed(conduit_node* node,
    const char* path, conduit_float32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float64_ptr(
    conduit_node* node, const char* path, conduit_float64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float64_ptr_detailed(conduit_node* node,
    const char* path, conduit_float64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_external_int8_ptr(
    conduit_node* node, conduit_int8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int8_ptr_detailed(conduit_node* node,
    conduit_int8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int16_ptr(
    conduit_node* node, conduit_int16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int16_ptr_detailed(conduit_node* node,
    conduit_int16* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int32_ptr(
    conduit_node* node, conduit_int32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int32_ptr_detailed(conduit_node* node,
    conduit_int32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int64_ptr(
    conduit_node* node, conduit_int64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int64_ptr_detailed(conduit_node* node,
    conduit_int64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint8_ptr(
    conduit_node* node, conduit_uint8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint8_ptr_detailed(conduit_node* node,
    conduit_uint8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint16_ptr(
    conduit_node* node, conduit_uint16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint16_ptr_detailed(conduit_node* node,
    conduit_uint16* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint32_ptr(
    conduit_node* node, conduit_uint32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint32_ptr_detailed(conduit_node* node,
    conduit_uint32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint64_ptr(
    conduit_node* node, conduit_uint64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_uint64_ptr_detailed(conduit_node* node,
    conduit_uint64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_float32_ptr(
    conduit_node* node, conduit_float32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_float32_ptr_detailed(conduit_node* node,
    conduit_float32* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_float64_ptr(
    conduit_node* node, conduit_float64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_float64_ptr_detailed(conduit_node* node,
    conduit_float64* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int8_ptr(
    conduit_node* node, const char* path, conduit_int8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int8_ptr_detailed(conduit_node* node,
    const char* path, conduit_int8* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int16_ptr(
    conduit_node* node, const char* path, conduit_int16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int16_ptr_detailed(
    conduit_node* node, const char* path, conduit_int16* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int32_ptr(
    conduit_node* node, const char* path, conduit_int32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int32_ptr_detailed(
    conduit_node* node, const char* path, conduit_int32* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int64_ptr(
    conduit_node* node, const char* path, conduit_int64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int64_ptr_detailed(
    conduit_node* node, const char* path, conduit_int64* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint8_ptr(
    conduit_node* node, const char* path, conduit_uint8* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint8_ptr_detailed(
    conduit_node* node, const char* path, conduit_uint8* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint16_ptr(
    conduit_node* node, const char* path, conduit_uint16* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint16_ptr_detailed(
    conduit_node* node, const char* path, conduit_uint16* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint32_ptr(
    conduit_node* node, const char* path, conduit_uint32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint32_ptr_detailed(
    conduit_node* node, const char* path, conduit_uint32* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint64_ptr(
    conduit_node* node, const char* path, conduit_uint64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_uint64_ptr_detailed(
    conduit_node* node, const char* path, conduit_uint64* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_float32_ptr(
    conduit_node* node, const char* path, conduit_float32* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_float32_ptr_detailed(
    conduit_node* node, const char* path, conduit_float32* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_float64_ptr(
    conduit_node* node, const char* path, conduit_float64* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_float64_ptr_detailed(
    conduit_node* node, const char* path, conduit_float64* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_char(conduit_node* node, char value);
  CATALYST_EXPORT void catalyst_conduit_node_set_short(conduit_node* node, short value);
  CATALYST_EXPORT void catalyst_conduit_node_set_int(conduit_node* node, int value);
  CATALYST_EXPORT void catalyst_conduit_node_set_long(conduit_node* node, long value);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_char(conduit_node* node, signed char value);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_short(
    conduit_node* node, signed short value);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_int(conduit_node* node, signed int value);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_long(conduit_node* node, signed long value);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_char(
    conduit_node* node, unsigned char value);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_short(
    conduit_node* node, unsigned short value);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_int(
    conduit_node* node, unsigned int value);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_long(
    conduit_node* node, unsigned long value);
  CATALYST_EXPORT void catalyst_conduit_node_set_float(conduit_node* node, float value);
  CATALYST_EXPORT void catalyst_conduit_node_set_double(conduit_node* node, double value);

  CATALYST_EXPORT void catalyst_conduit_node_set_char_ptr(
    conduit_node* node, char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_char_ptr_detailed(conduit_node* node, char* data,
    conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_short_ptr(
    conduit_node* node, short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_short_ptr_detailed(conduit_node* node, short* data,
    conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_int_ptr(
    conduit_node* node, int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_int_ptr_detailed(conduit_node* node, int* data,
    conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_long_ptr(
    conduit_node* node, long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_long_ptr_detailed(conduit_node* node, long* data,
    conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_char_ptr(
    conduit_node* node, signed char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_char_ptr_detailed(conduit_node* node,
    signed char* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_short_ptr(
    conduit_node* node, signed short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_short_ptr_detailed(conduit_node* node,
    signed short* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_int_ptr(
    conduit_node* node, signed int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_int_ptr_detailed(conduit_node* node,
    signed int* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_long_ptr(
    conduit_node* node, signed long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_signed_long_ptr_detailed(conduit_node* node,
    signed long* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_char_ptr(
    conduit_node* node, unsigned char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_char_ptr_detailed(conduit_node* node,
    unsigned char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_short_ptr(
    conduit_node* node, unsigned short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_short_ptr_detailed(conduit_node* node,
    unsigned short* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_int_ptr(
    conduit_node* node, unsigned int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_int_ptr_detailed(conduit_node* node,
    unsigned int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_long_ptr(
    conduit_node* node, unsigned long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_unsigned_long_ptr_detailed(conduit_node* node,
    unsigned long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_float_ptr(
    conduit_node* node, float* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_float_ptr_detailed(conduit_node* node, float* data,
    conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_double_ptr(
    conduit_node* node, double* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_double_ptr_detailed(conduit_node* node,
    double* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_path_char(
    conduit_node* node, const char* path, char value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_short(
    conduit_node* node, const char* path, short value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int(
    conduit_node* node, const char* path, int value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_long(
    conduit_node* node, const char* path, long value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_char(
    conduit_node* node, const char* path, signed char value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_short(
    conduit_node* node, const char* path, signed short value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_int(
    conduit_node* node, const char* path, signed int value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_long(
    conduit_node* node, const char* path, signed long value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_char(
    conduit_node* node, const char* path, unsigned char value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_short(
    conduit_node* node, const char* path, unsigned short value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_int(
    conduit_node* node, const char* path, unsigned int value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_long(
    conduit_node* node, const char* path, unsigned long value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float(
    conduit_node* node, const char* path, float value);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_double(
    conduit_node* node, const char* path, double value);

  CATALYST_EXPORT void catalyst_conduit_node_set_path_char_ptr(
    conduit_node* node, const char* path, char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_char_ptr_detailed(conduit_node* node,
    const char* path, char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_short_ptr(
    conduit_node* node, const char* path, short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_short_ptr_detailed(conduit_node* node,
    const char* path, short* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int_ptr(
    conduit_node* node, const char* path, int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_int_ptr_detailed(conduit_node* node,
    const char* path, int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_long_ptr(
    conduit_node* node, const char* path, long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_long_ptr_detailed(conduit_node* node,
    const char* path, long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_char_ptr(
    conduit_node* node, const char* path, signed char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_char_ptr_detailed(conduit_node* node,
    const char* path, signed char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_short_ptr(
    conduit_node* node, const char* path, signed short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_short_ptr_detailed(conduit_node* node,
    const char* path, signed short* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_int_ptr(
    conduit_node* node, const char* path, signed int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_int_ptr_detailed(conduit_node* node,
    const char* path, signed int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_long_ptr(
    conduit_node* node, const char* path, signed long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_signed_long_ptr_detailed(conduit_node* node,
    const char* path, signed long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_char_ptr(
    conduit_node* node, const char* path, unsigned char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_char_ptr_detailed(conduit_node* node,
    const char* path, unsigned char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_short_ptr(
    conduit_node* node, const char* path, unsigned short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_short_ptr_detailed(
    conduit_node* node, const char* path, unsigned short* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_int_ptr(
    conduit_node* node, const char* path, unsigned int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_int_ptr_detailed(conduit_node* node,
    const char* path, unsigned int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_long_ptr(
    conduit_node* node, const char* path, unsigned long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_unsigned_long_ptr_detailed(conduit_node* node,
    const char* path, unsigned long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float_ptr(
    conduit_node* node, const char* path, float* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_float_ptr_detailed(conduit_node* node,
    const char* path, float* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_double_ptr(
    conduit_node* node, const char* path, double* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_double_ptr_detailed(conduit_node* node,
    const char* path, double* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_external_char_ptr(
    conduit_node* node, char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_char_ptr_detailed(conduit_node* node,
    char* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_short_ptr(
    conduit_node* node, short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_short_ptr_detailed(conduit_node* node,
    short* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int_ptr(
    conduit_node* node, int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_int_ptr_detailed(conduit_node* node,
    int* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_long_ptr(
    conduit_node* node, long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_long_ptr_detailed(conduit_node* node,
    long* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_char_ptr(
    conduit_node* node, signed char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_char_ptr_detailed(
    conduit_node* node, signed char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_short_ptr(
    conduit_node* node, signed short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_short_ptr_detailed(
    conduit_node* node, signed short* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_int_ptr(
    conduit_node* node, signed int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_int_ptr_detailed(
    conduit_node* node, signed int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_long_ptr(
    conduit_node* node, signed long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_signed_long_ptr_detailed(
    conduit_node* node, signed long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_char_ptr(
    conduit_node* node, unsigned char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_char_ptr_detailed(
    conduit_node* node, unsigned char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_short_ptr(
    conduit_node* node, unsigned short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_short_ptr_detailed(
    conduit_node* node, unsigned short* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_int_ptr(
    conduit_node* node, unsigned int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_int_ptr_detailed(
    conduit_node* node, unsigned int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_long_ptr(
    conduit_node* node, unsigned long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_unsigned_long_ptr_detailed(
    conduit_node* node, unsigned long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_float_ptr(
    conduit_node* node, float* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_float_ptr_detailed(conduit_node* node,
    float* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_double_ptr(
    conduit_node* node, double* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_external_double_ptr_detailed(conduit_node* node,
    double* data, conduit_index_t num_elements, conduit_index_t offset, conduit_index_t stride,
    conduit_index_t element_bytes, conduit_index_t endianness);

  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_char_ptr(
    conduit_node* node, const char* path, char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_char_ptr_detailed(conduit_node* node,
    const char* path, char* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_short_ptr(
    conduit_node* node, const char* path, short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_short_ptr_detailed(
    conduit_node* node, const char* path, short* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int_ptr(
    conduit_node* node, const char* path, int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_int_ptr_detailed(conduit_node* node,
    const char* path, int* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_long_ptr(
    conduit_node* node, const char* path, long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_long_ptr_detailed(conduit_node* node,
    const char* path, long* data, conduit_index_t num_elements, conduit_index_t offset,
    conduit_index_t stride, conduit_index_t element_bytes, conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_char_ptr(
    conduit_node* node, const char* path, signed char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_char_ptr_detailed(
    conduit_node* node, const char* path, signed char* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_short_ptr(
    conduit_node* node, const char* path, signed short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_short_ptr_detailed(
    conduit_node* node, const char* path, signed short* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_int_ptr(
    conduit_node* node, const char* path, signed int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_int_ptr_detailed(
    conduit_node* node, const char* path, signed int* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_long_ptr(
    conduit_node* node, const char* path, signed long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_signed_long_ptr_detailed(
    conduit_node* node, const char* path, signed long* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_char_ptr(
    conduit_node* node, const char* path, unsigned char* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_char_ptr_detailed(
    conduit_node* node, const char* path, unsigned char* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_short_ptr(
    conduit_node* node, const char* path, unsigned short* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_short_ptr_detailed(
    conduit_node* node, const char* path, unsigned short* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_int_ptr(
    conduit_node* node, const char* path, unsigned int* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_int_ptr_detailed(
    conduit_node* node, const char* path, unsigned int* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_long_ptr(
    conduit_node* node, const char* path, unsigned long* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_unsigned_long_ptr_detailed(
    conduit_node* node, const char* path, unsigned long* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_float_ptr(
    conduit_node* node, const char* path, float* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_float_ptr_detailed(
    conduit_node* node, const char* path, float* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_double_ptr(
    conduit_node* node, const char* path, double* data, conduit_index_t num_elements);
  CATALYST_EXPORT void catalyst_conduit_node_set_path_external_double_ptr_detailed(
    conduit_node* node, const char* path, double* data, conduit_index_t num_elements,
    conduit_index_t offset, conduit_index_t stride, conduit_index_t element_bytes,
    conduit_index_t endianness);

  CATALYST_EXPORT void* catalyst_conduit_node_data_ptr(conduit_node* node);
  CATALYST_EXPORT void* catalyst_conduit_node_element_ptr(conduit_node* node, conduit_index_t idx);

  CATALYST_EXPORT void* catalyst_conduit_node_fetch_path_data_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT void* catalyst_conduit_node_fetch_path_element_ptr(
    conduit_node* node, const char* path, conduit_index_t idx);

  CATALYST_EXPORT char* catalyst_conduit_node_as_char8_str(conduit_node* node);
  CATALYST_EXPORT char* catalyst_conduit_node_fetch_path_as_char8_str(
    conduit_node* node, const char* path);

  CATALYST_EXPORT conduit_int8 catalyst_conduit_node_as_int8(conduit_node* node);
  CATALYST_EXPORT conduit_int16 catalyst_conduit_node_as_int16(conduit_node* node);
  CATALYST_EXPORT conduit_int32 catalyst_conduit_node_as_int32(conduit_node* node);
  CATALYST_EXPORT conduit_int64 catalyst_conduit_node_as_int64(conduit_node* node);
  CATALYST_EXPORT conduit_uint8 catalyst_conduit_node_as_uint8(conduit_node* node);
  CATALYST_EXPORT conduit_uint16 catalyst_conduit_node_as_uint16(conduit_node* node);
  CATALYST_EXPORT conduit_uint32 catalyst_conduit_node_as_uint32(conduit_node* node);
  CATALYST_EXPORT conduit_uint64 catalyst_conduit_node_as_uint64(conduit_node* node);
  CATALYST_EXPORT conduit_float32 catalyst_conduit_node_as_float32(conduit_node* node);
  CATALYST_EXPORT conduit_float64 catalyst_conduit_node_as_float64(conduit_node* node);

  CATALYST_EXPORT conduit_int8* catalyst_conduit_node_as_int8_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_int16* catalyst_conduit_node_as_int16_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_int32* catalyst_conduit_node_as_int32_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_int64* catalyst_conduit_node_as_int64_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_uint8* catalyst_conduit_node_as_uint8_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_uint16* catalyst_conduit_node_as_uint16_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_uint32* catalyst_conduit_node_as_uint32_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_uint64* catalyst_conduit_node_as_uint64_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_float32* catalyst_conduit_node_as_float32_ptr(conduit_node* node);
  CATALYST_EXPORT conduit_float64* catalyst_conduit_node_as_float64_ptr(conduit_node* node);

  CATALYST_EXPORT conduit_int8 catalyst_conduit_node_fetch_path_as_int8(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_int16 catalyst_conduit_node_fetch_path_as_int16(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_int32 catalyst_conduit_node_fetch_path_as_int32(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_int64 catalyst_conduit_node_fetch_path_as_int64(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint8 catalyst_conduit_node_fetch_path_as_uint8(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint16 catalyst_conduit_node_fetch_path_as_uint16(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint32 catalyst_conduit_node_fetch_path_as_uint32(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint64 catalyst_conduit_node_fetch_path_as_uint64(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_float32 catalyst_conduit_node_fetch_path_as_float32(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_float64 catalyst_conduit_node_fetch_path_as_float64(
    conduit_node* node, const char* path);

  CATALYST_EXPORT conduit_int8* catalyst_conduit_node_fetch_path_as_int8_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_int16* catalyst_conduit_node_fetch_path_as_int16_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_int32* catalyst_conduit_node_fetch_path_as_int32_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_int64* catalyst_conduit_node_fetch_path_as_int64_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint8* catalyst_conduit_node_fetch_path_as_uint8_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint16* catalyst_conduit_node_fetch_path_as_uint16_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint32* catalyst_conduit_node_fetch_path_as_uint32_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_uint64* catalyst_conduit_node_fetch_path_as_uint64_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_float32* catalyst_conduit_node_fetch_path_as_float32_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT conduit_float64* catalyst_conduit_node_fetch_path_as_float64_ptr(
    conduit_node* node, const char* path);

  CATALYST_EXPORT char catalyst_conduit_node_as_char(conduit_node* node);
  CATALYST_EXPORT short catalyst_conduit_node_as_short(conduit_node* node);
  CATALYST_EXPORT int catalyst_conduit_node_as_int(conduit_node* node);
  CATALYST_EXPORT long catalyst_conduit_node_as_long(conduit_node* node);
  CATALYST_EXPORT signed char catalyst_conduit_node_as_signed_char(conduit_node* node);
  CATALYST_EXPORT signed short catalyst_conduit_node_as_signed_short(conduit_node* node);
  CATALYST_EXPORT signed int catalyst_conduit_node_as_signed_int(conduit_node* node);
  CATALYST_EXPORT signed long catalyst_conduit_node_as_signed_long(conduit_node* node);
  CATALYST_EXPORT unsigned char catalyst_conduit_node_as_unsigned_char(conduit_node* node);
  CATALYST_EXPORT unsigned short catalyst_conduit_node_as_unsigned_short(conduit_node* node);
  CATALYST_EXPORT unsigned int catalyst_conduit_node_as_unsigned_int(conduit_node* node);
  CATALYST_EXPORT unsigned long catalyst_conduit_node_as_unsigned_long(conduit_node* node);
  CATALYST_EXPORT float catalyst_conduit_node_as_float(conduit_node* node);
  CATALYST_EXPORT double catalyst_conduit_node_as_double(conduit_node* node);

  CATALYST_EXPORT char* catalyst_conduit_node_as_char_ptr(conduit_node* node);
  CATALYST_EXPORT short* catalyst_conduit_node_as_short_ptr(conduit_node* node);
  CATALYST_EXPORT int* catalyst_conduit_node_as_int_ptr(conduit_node* node);
  CATALYST_EXPORT long* catalyst_conduit_node_as_long_ptr(conduit_node* node);
  CATALYST_EXPORT signed char* catalyst_conduit_node_as_signed_char_ptr(conduit_node* node);
  CATALYST_EXPORT signed short* catalyst_conduit_node_as_signed_short_ptr(conduit_node* node);
  CATALYST_EXPORT signed int* catalyst_conduit_node_as_signed_int_ptr(conduit_node* node);
  CATALYST_EXPORT signed long* catalyst_conduit_node_as_signed_long_ptr(conduit_node* node);
  CATALYST_EXPORT unsigned char* catalyst_conduit_node_as_unsigned_char_ptr(conduit_node* node);
  CATALYST_EXPORT unsigned short* catalyst_conduit_node_as_unsigned_short_ptr(conduit_node* node);
  CATALYST_EXPORT unsigned int* catalyst_conduit_node_as_unsigned_int_ptr(conduit_node* node);
  CATALYST_EXPORT unsigned long* catalyst_conduit_node_as_unsigned_long_ptr(conduit_node* node);
  CATALYST_EXPORT float* catalyst_conduit_node_as_float_ptr(conduit_node* node);
  CATALYST_EXPORT double* catalyst_conduit_node_as_double_ptr(conduit_node* node);

  CATALYST_EXPORT char catalyst_conduit_node_fetch_path_as_char(
    conduit_node* node, const char* path);
  CATALYST_EXPORT short catalyst_conduit_node_fetch_path_as_short(
    conduit_node* node, const char* path);
  CATALYST_EXPORT int catalyst_conduit_node_fetch_path_as_int(conduit_node* node, const char* path);
  CATALYST_EXPORT long catalyst_conduit_node_fetch_path_as_long(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed char catalyst_conduit_node_fetch_path_as_signed_char(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed short catalyst_conduit_node_fetch_path_as_signed_short(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed int catalyst_conduit_node_fetch_path_as_signed_int(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed long catalyst_conduit_node_fetch_path_as_signed_long(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned char catalyst_conduit_node_fetch_path_as_unsigned_char(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned short catalyst_conduit_node_fetch_path_as_unsigned_short(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned int catalyst_conduit_node_fetch_path_as_unsigned_int(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned long catalyst_conduit_node_fetch_path_as_unsigned_long(
    conduit_node* node, const char* path);
  CATALYST_EXPORT float catalyst_conduit_node_fetch_path_as_float(
    conduit_node* node, const char* path);
  CATALYST_EXPORT double catalyst_conduit_node_fetch_path_as_double(
    conduit_node* node, const char* path);

  CATALYST_EXPORT char* catalyst_conduit_node_fetch_path_as_char_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT short* catalyst_conduit_node_fetch_path_as_short_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT int* catalyst_conduit_node_fetch_path_as_int_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT long* catalyst_conduit_node_fetch_path_as_long_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed char* catalyst_conduit_node_fetch_path_as_signed_char_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed short* catalyst_conduit_node_fetch_path_as_signed_short_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed int* catalyst_conduit_node_fetch_path_as_signed_int_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT signed long* catalyst_conduit_node_fetch_path_as_signed_long_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned char* catalyst_conduit_node_fetch_path_as_unsigned_char_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned short* catalyst_conduit_node_fetch_path_as_unsigned_short_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned int* catalyst_conduit_node_fetch_path_as_unsigned_int_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT unsigned long* catalyst_conduit_node_fetch_path_as_unsigned_long_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT float* catalyst_conduit_node_fetch_path_as_float_ptr(
    conduit_node* node, const char* path);
  CATALYST_EXPORT double* catalyst_conduit_node_fetch_path_as_double_ptr(
    conduit_node* node, const char* path);

  CATALYST_EXPORT const conduit_datatype* catalyst_conduit_node_dtype(const conduit_node* node);

  // conduit_utils.h
  CATALYST_EXPORT void catalyst_conduit_utils_set_info_handler(
    void (*on_info)(const char*, const char*, int));
  CATALYST_EXPORT void catalyst_conduit_utils_set_warning_handler(
    void (*on_warning)(const char*, const char*, int));
  CATALYST_EXPORT void catalyst_conduit_utils_set_error_handler(
    void (*on_error)(const char*, const char*, int));

  // conduit_blueprint.h
  CATALYST_EXPORT void catalyst_conduit_blueprint_about(conduit_node* node);
  CATALYST_EXPORT int catalyst_conduit_blueprint_verify(
    const char* protocol, const conduit_node* node, conduit_node* info);

  // conduit_blueprint_mcarray.h
  CATALYST_EXPORT int catalyst_conduit_blueprint_mcarray_verify(
    const conduit_node* node, conduit_node* info);
  CATALYST_EXPORT int catalyst_conduit_blueprint_mcarray_verify_sub_protocol(
    const char* protocol, const conduit_node* node, conduit_node* info);
  CATALYST_EXPORT int catalyst_conduit_blueprint_mcarray_is_interleaved(const conduit_node* node);
  CATALYST_EXPORT int catalyst_conduit_blueprint_mcarray_to_contiguous(
    const conduit_node* node, conduit_node* dest);
  CATALYST_EXPORT int catalyst_conduit_blueprint_mcarray_to_interleaved(
    const conduit_node* node, conduit_node* dest);

  // conduit_blueprint_mesh.h
  CATALYST_EXPORT int catalyst_conduit_blueprint_mesh_verify(
    const conduit_node* node, conduit_node* info);
  CATALYST_EXPORT int catalyst_conduit_blueprint_mesh_verify_sub_protocol(
    const char* protocol, const conduit_node* node, conduit_node* info);
  CATALYST_EXPORT void catalyst_conduit_blueprint_mesh_generate_index(const conduit_node* mesh,
    const char* ref_path, conduit_index_t num_domains, conduit_node* index_out);
  CATALYST_EXPORT void catalyst_conduit_blueprint_mesh_partition(
    const conduit_node* mesh, const conduit_node* options, conduit_node* output);
  CATALYST_EXPORT void catalyst_conduit_blueprint_mesh_flatten(
    const conduit_node* mesh, const conduit_node* options, conduit_node* output);

  // conduit_blueprint_table.h
  CATALYST_EXPORT int catalyst_conduit_blueprint_table_verify(
    const conduit_node* node, conduit_node* info);
  CATALYST_EXPORT int catalyst_conduit_blueprint_table_verify_sub_protocol(
    const char* protocol, const conduit_node* node, conduit_node* info);

#ifdef __cplusplus
}
#endif

#endif
