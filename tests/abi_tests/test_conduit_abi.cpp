/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

// XXX(c++14): use `_t` and `_v`-suffixed type queries.

#include <conduit.h>
#include <conduit_blueprint.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <tuple>
#include <type_traits>

#define abi_require(cond, desc)                                                                    \
  do                                                                                               \
  {                                                                                                \
    if (!(cond))                                                                                   \
    {                                                                                              \
      fprintf(stderr, "invalid '" #cond "': " desc "\n");                                          \
      ret = EXIT_FAILURE;                                                                          \
    }                                                                                              \
  } while (false)

template <typename F>
struct funcinfo;

template <typename Return, typename... Args>
struct funcinfo<Return(Args...)>
{
  using return_t = Return;
  using argument_t = std::tuple<Args...>;
};

static int test_conduit_bitwidth_types()
{
  int ret = EXIT_SUCCESS;

#define CHECK_CONDUIT_TYPEDEF(conduit, c)                                                          \
  static_assert(std::is_same<conduit, c>::value, #conduit " must be the same as '" #c "'")

  CHECK_CONDUIT_TYPEDEF(conduit_unsigned_char, unsigned char);
  CHECK_CONDUIT_TYPEDEF(conduit_unsigned_short, unsigned short);
  CHECK_CONDUIT_TYPEDEF(conduit_unsigned_int, unsigned int);
  CHECK_CONDUIT_TYPEDEF(conduit_unsigned_long, unsigned long);
  CHECK_CONDUIT_TYPEDEF(conduit_unsigned_long_long, unsigned long long);

  CHECK_CONDUIT_TYPEDEF(conduit_signed_char, signed char);
  CHECK_CONDUIT_TYPEDEF(conduit_signed_short, signed short);
  CHECK_CONDUIT_TYPEDEF(conduit_signed_int, signed int);
  CHECK_CONDUIT_TYPEDEF(conduit_signed_long, signed long);
  CHECK_CONDUIT_TYPEDEF(conduit_signed_long_long, signed long long);

  CHECK_CONDUIT_TYPEDEF(conduit_float, float);
  CHECK_CONDUIT_TYPEDEF(conduit_double, double);
  // CHECK_CONDUIT_TYPEDEF(conduit_long_double, long double);

  // CHECK_CONDUIT_TYPEDEF(conduit_char, char);
  CHECK_CONDUIT_TYPEDEF(conduit_short, short);
  CHECK_CONDUIT_TYPEDEF(conduit_int, int);
  CHECK_CONDUIT_TYPEDEF(conduit_long, long);
  CHECK_CONDUIT_TYPEDEF(conduit_long_long, long long);

#undef CHECK_CONDUIT_TYPEDEF

  abi_require(CONDUIT_EMPTY_ID == 0, "CONDUIT_EMPTY_ID must be 0");
  abi_require(CONDUIT_OBJECT_ID == 1, "CONDUIT_OBJECT_ID must be 1");
  abi_require(CONDUIT_LIST_ID == 2, "CONDUIT_LIST_ID must be 2");
  abi_require(CONDUIT_INT8_ID == 3, "CONDUIT_INT8_ID must be 3");
  abi_require(CONDUIT_INT16_ID == 4, "CONDUIT_INT16_ID must be 4");
  abi_require(CONDUIT_INT32_ID == 5, "CONDUIT_INT32_ID must be 5");
  abi_require(CONDUIT_INT64_ID == 6, "CONDUIT_INT64_ID must be 6");
  abi_require(CONDUIT_UINT8_ID == 7, "CONDUIT_UINT8_ID must be 7");
  abi_require(CONDUIT_UINT16_ID == 8, "CONDUIT_UINT16_ID must be 8");
  abi_require(CONDUIT_UINT32_ID == 9, "CONDUIT_UINT32_ID must be 9");
  abi_require(CONDUIT_UINT64_ID == 10, "CONDUIT_UINT64_ID must be 10");
  abi_require(CONDUIT_FLOAT32_ID == 11, "CONDUIT_FLOAT32_ID must be 11");
  abi_require(CONDUIT_FLOAT64_ID == 12, "CONDUIT_FLOAT64_ID must be 12");
  abi_require(CONDUIT_CHAR8_STR_ID == 13, "CONDUIT_CHAR8_STR_ID must be 13");

  abi_require(sizeof(conduit_int8) == 1, "conduit_int8 must be 1 byte");
  static_assert(std::is_integral<conduit_int8>::value, "conduit_int8 must be integral");
  static_assert(std::is_signed<conduit_int8>::value, "conduit_int8 must be signed");

  abi_require(sizeof(conduit_int16) == 2, "conduit_int16 must be 2 byte");
  static_assert(std::is_integral<conduit_int16>::value, "conduit_int16 must be integral");
  static_assert(std::is_signed<conduit_int16>::value, "conduit_int16 must be signed");

  abi_require(sizeof(conduit_int32) == 4, "conduit_int32 must be 4 byte");
  static_assert(std::is_integral<conduit_int32>::value, "conduit_int32 must be integral");
  static_assert(std::is_signed<conduit_int32>::value, "conduit_int32 must be signed");

  abi_require(sizeof(conduit_int64) == 8, "conduit_int64 must be 8 byte");
  static_assert(std::is_integral<conduit_int64>::value, "conduit_int64 must be integral");
  static_assert(std::is_signed<conduit_int64>::value, "conduit_int64 must be signed");

  abi_require(sizeof(conduit_uint8) == 1, "conduit_uint8 must be 1 byte");
  static_assert(std::is_integral<conduit_uint8>::value, "conduit_uint8 must be integral");
  static_assert(std::is_unsigned<conduit_uint8>::value, "conduit_uint8 must be unsigned");

  abi_require(sizeof(conduit_uint16) == 2, "conduit_uint16 must be 2 byte");
  static_assert(std::is_integral<conduit_uint16>::value, "conduit_uint16 must be integral");
  static_assert(std::is_unsigned<conduit_uint16>::value, "conduit_uint16 must be unsigned");

  abi_require(sizeof(conduit_uint32) == 4, "conduit_uint32 must be 4 byte");
  static_assert(std::is_integral<conduit_uint32>::value, "conduit_uint32 must be integral");
  static_assert(std::is_unsigned<conduit_uint32>::value, "conduit_uint32 must be unsigned");

  abi_require(sizeof(conduit_uint64) == 8, "conduit_uint64 must be 8 byte");
  static_assert(std::is_integral<conduit_uint64>::value, "conduit_uint64 must be integral");
  static_assert(std::is_unsigned<conduit_uint64>::value, "conduit_uint64 must be unsigned");

  abi_require(sizeof(conduit_float32) == 4, "conduit_float32 must be 4 byte");
  static_assert(
    std::is_floating_point<conduit_float32>::value, "conduit_float32 must be floating point");

  abi_require(sizeof(conduit_float64) == 8, "conduit_float64 must be 8 byte");
  static_assert(
    std::is_floating_point<conduit_float64>::value, "conduit_float64 must be floating point");

  static_assert(std::is_integral<conduit_index_t>::value, "conduit_index_t must be integral");
  static_assert(std::is_signed<conduit_index_t>::value, "conduit_index_t must be signed");
  abi_require(sizeof(conduit_index_t) == 8, "conduit_index_t must be 64-bits");

  return ret;
}

static int test_conduit_endianness_abi()
{
  int ret = EXIT_SUCCESS;

  abi_require(CONDUIT_ENDIANNESS_DEFAULT_ID == 0, "CONDUIT_ENDIANNESS_DEFAULT_ID must be 0");
  abi_require(CONDUIT_ENDIANNESS_BIG_ID == 1, "CONDUIT_ENDIANNESS_BIG_ID must be 1");
  abi_require(CONDUIT_ENDIANNESS_LITTLE_ID == 2, "CONDUIT_ENDIANNESS_LITTLE_ID must be 2");

  return ret;
}

// From https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
template <typename, typename = void>
struct is_type_complete
{
  static const bool value = false;
};

// XXX(c++17): use `std::void_t`
template <typename...>
using void_t = void;

template <typename T>
struct is_type_complete<T, void_t<decltype(sizeof(T))>>
{
  static const bool value = true;
};

static int test_conduit_datatype_abi()
{
  int ret = EXIT_SUCCESS;

  // Only add assertions to this list; removal or modification of any
  // checks constitutes an ABI break except:
  //
  // - where noted (XXX comments)
  // - changing a `void` return to a real type
  // - adding `const` qualification to parameter pointers
  // - removing `const` qualification from return pointers

  // conduit_datatype_sizeof_index_t
  {
    using func_types = funcinfo<decltype(conduit_datatype_sizeof_index_t)>;
    abi_require(
      &conduit_datatype_sizeof_index_t, "conduit_datatype_sizeof_index_t must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_datatype_sizeof_index_t must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 0,
      "conduit_datatype_sizeof_index_t must take no arguments");
  }

  static_assert(!is_type_complete<conduit_datatype>::value, "conduit_datatype must be incomplete");

  // conduit_datatype_id
  {
    using func_types = funcinfo<decltype(conduit_datatype_id)>;
    abi_require(&conduit_datatype_id, "conduit_datatype_id must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_datatype_id must return conduit_index_t");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_datatype_id must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,
                    const conduit_datatype*>::value,
      "conduit_datatype_id should take a const conduit_datatype*");
  }

  // conduit_datatype_name
  {
    using func_types = funcinfo<decltype(conduit_datatype_name)>;
    abi_require(&conduit_datatype_name, "conduit_datatype_name must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_datatype_name must return catalyst_status");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_datatype_name must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,
                    const conduit_datatype*>::value,
      "conduit_datatype_name should take a const conduit_datatype*");
  }

  // conduit_datatype_name_destroy
  {
    using func_types = funcinfo<decltype(conduit_datatype_name_destroy)>;
    abi_require(&conduit_datatype_name_destroy, "conduit_datatype_name_destroy must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_datatype_destroy must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_datatype_destroy must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type, char*>::value,
      "conduit_datatype_destroy should take a char*");
  }

  // datatype query functions

#define CHECK_CONDUIT_DATATYPE_QUERY(rettype, func)                                                \
  do                                                                                               \
  {                                                                                                \
    using func_types = funcinfo<decltype(func)>;                                                   \
    abi_require(&(func), #func " must be defined");                                                \
    static_assert(                                                                                 \
      std::is_same<func_types::return_t, rettype>::value, #func " must return " #rettype);         \
    abi_require(                                                                                   \
      std::tuple_size<func_types::argument_t>::value == 1, #func " must take one argument");       \
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,                \
                    const conduit_datatype*>::value,                                               \
      #func " should take a const conduit_datatype*");                                             \
  } while (false)

  CHECK_CONDUIT_DATATYPE_QUERY(conduit_index_t, conduit_datatype_number_of_elements);
  CHECK_CONDUIT_DATATYPE_QUERY(conduit_index_t, conduit_datatype_number_of_elements);
  CHECK_CONDUIT_DATATYPE_QUERY(conduit_index_t, conduit_datatype_offset);
  CHECK_CONDUIT_DATATYPE_QUERY(conduit_index_t, conduit_datatype_stride);
  CHECK_CONDUIT_DATATYPE_QUERY(conduit_index_t, conduit_datatype_element_bytes);
  CHECK_CONDUIT_DATATYPE_QUERY(conduit_index_t, conduit_datatype_endianness);

  // conduit_datatype_element_index
  {
    using func_types = funcinfo<decltype(conduit_datatype_element_index)>;
    abi_require(&conduit_datatype_element_index, "conduit_datatype_element_index must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_datatype_element_index must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_datatype_element_index must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,
                    const conduit_datatype*>::value,
      "conduit_datatype_element_index should take a const conduit_datatype*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_index_t>::value,
      "conduit_datatype_element_index should take a conduit_index_t");
  }

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_empty);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_object);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_list);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_number);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_floating_point);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_integer);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_signed_integer);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_unsigned_integer);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_int8);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_int16);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_int32);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_int64);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_uint8);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_uint16);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_uint32);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_uint64);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_float32);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_float64);

  // CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_index_t);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_char);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_short);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_int);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_long);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_unsigned_char);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_unsigned_short);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_unsigned_int);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_unsigned_long);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_float);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_double);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_string);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_char8_str);

  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_little_endian);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_is_big_endian);
  CHECK_CONDUIT_DATATYPE_QUERY(int, conduit_datatype_endianness_matches_machine);

#undef CHECK_CONDUIT_DATATYPE_QUERY

  return ret;
}

// NOLINTNEXTLINE(readability-function-size)
static int test_conduit_node_abi()
{
  int ret = EXIT_SUCCESS;

  // Only add assertions to this list; removal or modification of any
  // checks constitutes an ABI break except:
  //
  // - where noted (XXX comments)
  // - changing a `void` return to a real type
  // - adding `const` qualification to parameter pointers
  // - removing `const` qualification from return pointers

  static_assert(!is_type_complete<conduit_node>::value, "conduit_node must be incomplete");

  // conduit_node_create
  {
    using func_types = funcinfo<decltype(conduit_node_create)>;
    abi_require(&conduit_node_create, "conduit_node_create must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_create must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 0,
      "conduit_node_create must take one argument");
  }

  // conduit_node_destroy
  {
    using func_types = funcinfo<decltype(conduit_node_destroy)>;
    abi_require(&conduit_node_destroy, "conduit_node_destroy must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_destroy must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_destroy must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_destroy should take a conduit_node*");
  }

  // conduit_node_fetch
  {
    using func_types = funcinfo<decltype(conduit_node_fetch)>;
    abi_require(&conduit_node_fetch, "conduit_node_fetch must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_fetch must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_fetch must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_fetch should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_fetch should take a const char*");
  }

  // conduit_node_fetch_existing
  {
    using func_types = funcinfo<decltype(conduit_node_fetch_existing)>;
    abi_require(&conduit_node_fetch_existing, "conduit_node_fetch_existing must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_fetch_existing must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_fetch_existing must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_fetch_existing should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_fetch_existing should take a const char*");
  }

  // conduit_node_append
  {
    using func_types = funcinfo<decltype(conduit_node_append)>;
    abi_require(&conduit_node_append, "conduit_node_append must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_append must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_append must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_append should take a conduit_node*");
  }

  // conduit_node_add_child
  {
    using func_types = funcinfo<decltype(conduit_node_add_child)>;
    abi_require(&conduit_node_add_child, "conduit_node_add_child must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_add_child must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_add_child must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_add_child should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_add_child should take a const char*");
  }

  // conduit_node_child
  {
    using func_types = funcinfo<decltype(conduit_node_child)>;
    abi_require(&conduit_node_child, "conduit_node_child must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_child must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_child must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_child should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_index_t>::value,
      "conduit_node_child should take a conduit_index_t");
  }

  // conduit_node_child_by_name
  {
    using func_types = funcinfo<decltype(conduit_node_child_by_name)>;
    abi_require(&conduit_node_child_by_name, "conduit_node_child_by_name must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_child_by_name must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_child_by_name must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_child_by_name should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_child_by_name should take a const char*");
  }

  // conduit_node_number_of_children
  {
    using func_types = funcinfo<decltype(conduit_node_number_of_children)>;
    abi_require(
      &conduit_node_number_of_children, "conduit_node_number_of_children must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_node_number_of_children must return conduit_index_t");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_number_of_children must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_number_of_children should take a conduit_node*");
  }

  // conduit_node_number_of_elements
  {
    using func_types = funcinfo<decltype(conduit_node_number_of_elements)>;
    abi_require(
      &conduit_node_number_of_elements, "conduit_node_number_of_elements must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_node_number_of_elements must return conduit_index_t");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_number_of_elements must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_number_of_elements should take a conduit_node*");
  }

  // conduit_node_reset
  {
    using func_types = funcinfo<decltype(conduit_node_reset)>;
    abi_require(&conduit_node_reset, "conduit_node_reset must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_reset must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_reset must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_reset should take a conduit_node*");
  }

  // conduit_node_move
  {
    using func_types = funcinfo<decltype(conduit_node_move)>;
    abi_require(&conduit_node_move, "conduit_node_move must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_move must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_move must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_move should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_move should take a conduit_node*");
  }

  // conduit_node_swap
  {
    using func_types = funcinfo<decltype(conduit_node_swap)>;
    abi_require(&conduit_node_swap, "conduit_node_swap must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_swap must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_swap must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_swap should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_swap should take a conduit_node*");
  }

  // conduit_node_remove_path
  {
    using func_types = funcinfo<decltype(conduit_node_remove_path)>;
    abi_require(&conduit_node_remove_path, "conduit_node_remove_path must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_remove_path must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_remove_path must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_remove_path should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_remove_path should take a const char*");
  }

  // conduit_node_remove_child
  {
    using func_types = funcinfo<decltype(conduit_node_remove_child)>;
    abi_require(&conduit_node_remove_child, "conduit_node_remove_child must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_remove_child must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_remove_child must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_remove_child should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_index_t>::value,
      "conduit_node_remove_child should take a conduit_index_t");
  }

  // conduit_node_remove_child_by_name
  {
    using func_types = funcinfo<decltype(conduit_node_remove_child_by_name)>;
    abi_require(
      &conduit_node_remove_child_by_name, "conduit_node_remove_child_by_name must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_remove_child_by_name must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_remove_child_by_name must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_remove_child_by_name should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_remove_child_by_name should take a const char*");
  }

  // conduit_node_name
  {
    using func_types = funcinfo<decltype(conduit_node_name)>;
    abi_require(&conduit_node_name, "conduit_node_name must be defined");
    static_assert(
      std::is_same<func_types::return_t, char*>::value, "conduit_node_name must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_name must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_name should take a const conduit_node*");
  }

  // conduit_node_path
  {
    using func_types = funcinfo<decltype(conduit_node_path)>;
    abi_require(&conduit_node_path, "conduit_node_path must be defined");
    static_assert(
      std::is_same<func_types::return_t, char*>::value, "conduit_node_path must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_path must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_path should take a const conduit_node*");
  }

  // conduit_node_has_child
  {
    using func_types = funcinfo<decltype(conduit_node_has_child)>;
    abi_require(&conduit_node_has_child, "conduit_node_has_child must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_has_child must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_has_child must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_has_child should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_has_child should take a const char*");
  }

  // conduit_node_has_path
  {
    using func_types = funcinfo<decltype(conduit_node_has_path)>;
    abi_require(&conduit_node_has_path, "conduit_node_has_path must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_has_path must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_has_path must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_has_path should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_has_path should take a const char*");
  }

  // conduit_node_rename_child
  {
    using func_types = funcinfo<decltype(conduit_node_rename_child)>;
    abi_require(&conduit_node_rename_child, "conduit_node_rename_child must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_rename_child must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_rename_child must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_rename_child should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_rename_child should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_rename_child should take a const char*");
  }

  // conduit_node_is_root
  {
    using func_types = funcinfo<decltype(conduit_node_is_root)>;
    abi_require(&conduit_node_is_root, "conduit_node_is_root must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_is_root must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_is_root must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_is_root should take a conduit_node*");
  }

  // conduit_node_is_data_external
  {
    using func_types = funcinfo<decltype(conduit_node_is_data_external)>;
    abi_require(&conduit_node_is_data_external, "conduit_node_is_data_external must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_node_is_data_external must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_is_data_external must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_is_data_external should take a const conduit_node*");
  }

  // conduit_node_parent
  {
    using func_types = funcinfo<decltype(conduit_node_parent)>;
    abi_require(&conduit_node_parent, "conduit_node_parent must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_node*>::value,
      "conduit_node_parent must return conduit_node*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_parent must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_parent should take a conduit_node*");
  }

  // conduit_node_total_strided_bytes
  {
    using func_types = funcinfo<decltype(conduit_node_total_strided_bytes)>;
    abi_require(
      &conduit_node_total_strided_bytes, "conduit_node_total_strided_bytes must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_node_total_strided_bytes must return conduit_index_t");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_total_strided_bytes must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_total_strided_bytes should take a const conduit_node*");
  }

  // conduit_node_total_bytes_compact
  {
    using func_types = funcinfo<decltype(conduit_node_total_bytes_compact)>;
    abi_require(
      &conduit_node_total_bytes_compact, "conduit_node_total_bytes_compact must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_node_total_bytes_compact must return conduit_index_t");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_total_bytes_compact must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_total_bytes_compact should take a const conduit_node*");
  }

  // conduit_node_total_bytes_allocated
  {
    using func_types = funcinfo<decltype(conduit_node_total_bytes_allocated)>;
    abi_require(
      &conduit_node_total_bytes_allocated, "conduit_node_total_bytes_allocated must be defined");
    static_assert(std::is_same<func_types::return_t, conduit_index_t>::value,
      "conduit_node_total_bytes_allocated must return conduit_index_t");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_total_bytes_allocated must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_total_bytes_allocated should take a const conduit_node*");
  }

  // conduit_node_is_compact
  {
    using func_types = funcinfo<decltype(conduit_node_is_compact)>;
    abi_require(&conduit_node_is_compact, "conduit_node_is_compact must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_is_compact must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_is_compact must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_is_compact should take a const conduit_node*");
  }

  // conduit_node_compatible
  {
    using func_types = funcinfo<decltype(conduit_node_compatible)>;
    abi_require(&conduit_node_compatible, "conduit_node_compatible must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_compatible must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_compatible must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_compatible should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_compatible should take a const conduit_node*");
  }

  // conduit_node_is_contiguous
  {
    using func_types = funcinfo<decltype(conduit_node_is_contiguous)>;
    abi_require(&conduit_node_is_contiguous, "conduit_node_is_contiguous must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_is_contiguous must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_is_contiguous must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_is_contiguous should take a const conduit_node*");
  }

  // conduit_node_contiguous_with_node
  {
    using func_types = funcinfo<decltype(conduit_node_contiguous_with_node)>;
    abi_require(
      &conduit_node_contiguous_with_node, "conduit_node_contiguous_with_node must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_node_contiguous_with_node must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_contiguous_with_node must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_contiguous_with_node should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_contiguous_with_node should take a const conduit_node*");
  }

  // conduit_node_contiguous_with_address
  {
    using func_types = funcinfo<decltype(conduit_node_contiguous_with_address)>;
    abi_require(&conduit_node_contiguous_with_address,
      "conduit_node_contiguous_with_address must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_node_contiguous_with_address must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_contiguous_with_address must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_contiguous_with_address should take a const conduit_node*");
    static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, void*>::value,
      "conduit_node_contiguous_with_address should take a void*");
  }

  // conduit_node_diff
  {
    using func_types = funcinfo<decltype(conduit_node_diff)>;
    abi_require(&conduit_node_diff, "conduit_node_diff must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_diff must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 4,
      "conduit_node_diff must take four arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_diff should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_diff should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_diff should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_float64>::value,
      "conduit_node_diff should take a conduit_float64");
  }

  // conduit_node_diff_compatible
  {
    using func_types = funcinfo<decltype(conduit_node_diff_compatible)>;
    abi_require(&conduit_node_diff_compatible, "conduit_node_diff_compatible must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_node_diff_compatible must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 4,
      "conduit_node_diff_compatible must take four arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_diff_compatible should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_diff_compatible should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_diff_compatible should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_float64>::value,
      "conduit_node_diff_compatible should take a conduit_float64");
  }

  // conduit_node_compatible
  {
    using func_types = funcinfo<decltype(conduit_node_compatible)>;
    abi_require(&conduit_node_compatible, "conduit_node_compatible must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_node_compatible must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_compatible must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_compatible should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_compatible should take a const conduit_node*");
  }

  // conduit_node_info
  {
    using func_types = funcinfo<decltype(conduit_node_info)>;
    abi_require(&conduit_node_info, "conduit_node_info must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_info must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_info must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_info should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_info should take a conduit_node*");
  }

  // conduit_node_print
  {
    using func_types = funcinfo<decltype(conduit_node_print)>;
    abi_require(&conduit_node_print, "conduit_node_print must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_print must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_print must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_print should take a conduit_node*");
  }

  // conduit_node_print_detailed
  {
    using func_types = funcinfo<decltype(conduit_node_print_detailed)>;
    abi_require(&conduit_node_print_detailed, "conduit_node_print_detailed must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_print_detailed must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_print_detailed must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_print_detailed should take a conduit_node*");
  }

  // conduit_node_compact_to
  {
    using func_types = funcinfo<decltype(conduit_node_compact_to)>;
    abi_require(&conduit_node_compact_to, "conduit_node_compact_to must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_compact_to must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_compact_to must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_compact_to should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_compact_to should take a conduit_node*");
  }

  // conduit_node_update
  {
    using func_types = funcinfo<decltype(conduit_node_update)>;
    abi_require(&conduit_node_update, "conduit_node_update must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_update must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_update must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_update should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_update should take a const conduit_node*");
  }

  // conduit_node_update_compatible
  {
    using func_types = funcinfo<decltype(conduit_node_update_compatible)>;
    abi_require(&conduit_node_update_compatible, "conduit_node_update_compatible must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_update_compatible must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_update_compatible must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_update_compatible should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_update_compatible should take a const conduit_node*");
  }

  // conduit_node_update_external
  {
    using func_types = funcinfo<decltype(conduit_node_update_external)>;
    abi_require(&conduit_node_update_external, "conduit_node_update_external must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_update_external must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_update_external must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_update_external should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_update_external should take a conduit_node*");
  }

  // conduit_node_parse
  {
    using func_types = funcinfo<decltype(conduit_node_parse)>;
    abi_require(&conduit_node_parse, "conduit_node_parse must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_parse must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_parse must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_parse should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_parse should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_parse should take a const char*");
  }

  // conduit_node_generate
  {
    using func_types = funcinfo<decltype(conduit_node_generate)>;
    abi_require(&conduit_node_generate, "conduit_node_generate must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_generate must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 4,
      "conduit_node_generate must take four arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_generate should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_generate should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_generate should take a const char*");
    static_assert(std::is_same<std::tuple_element<3, func_types::argument_t>::type, void*>::value,
      "conduit_node_generate should take a void*");
  }

  // conduit_node_generate_external
  {
    using func_types = funcinfo<decltype(conduit_node_generate_external)>;
    abi_require(&conduit_node_generate_external, "conduit_node_generate_external must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_generate_external must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 4,
      "conduit_node_generate_external must take four arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_generate_external should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_generate_external should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_generate_external should take a const char*");
    static_assert(std::is_same<std::tuple_element<3, func_types::argument_t>::type, void*>::value,
      "conduit_node_generate_external should take a void*");
  }

  // conduit_node_save
  {
    using func_types = funcinfo<decltype(conduit_node_save)>;
    abi_require(&conduit_node_save, "conduit_node_save must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_save must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_save must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_save should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_save should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_save should take a const char*");
  }

  // conduit_node_load
  {
    using func_types = funcinfo<decltype(conduit_node_load)>;
    abi_require(&conduit_node_load, "conduit_node_load must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_load must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_load must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_load should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_load should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_load should take a const char*");
  }

  // conduit_node_set_node
  {
    using func_types = funcinfo<decltype(conduit_node_set_node)>;
    abi_require(&conduit_node_set_node, "conduit_node_set_node must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_node_set_node must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_set_node must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_node should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_node should take a conduit_node*");
  }

  // conduit_node_set_path_node
  {
    using func_types = funcinfo<decltype(conduit_node_set_path_node)>;
    abi_require(&conduit_node_set_path_node, "conduit_node_set_path_node must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_path_node must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_set_path_node must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_path_node should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_set_path_node should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_path_node should take a conduit_node*");
  }

  // conduit_node_set_external_node
  {
    using func_types = funcinfo<decltype(conduit_node_set_external_node)>;
    abi_require(&conduit_node_set_external_node, "conduit_node_set_external_node must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_external_node must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_set_external_node must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_external_node should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_external_node should take a conduit_node*");
  }

  // conduit_node_set_path_external_node
  {
    using func_types = funcinfo<decltype(conduit_node_set_path_external_node)>;
    abi_require(
      &conduit_node_set_path_external_node, "conduit_node_set_path_external_node must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_path_external_node must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_set_path_external_node must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_path_external_node should take a conduit_external_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_set_path_external_node should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_path_external_node should take a conduit_external_node*");
  }

  // conduit_node_set_char8_str
  {
    using func_types = funcinfo<decltype(conduit_node_set_char8_str)>;
    abi_require(&conduit_node_set_char8_str, "conduit_node_set_char8_str must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_char8_str must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_set_char8_str must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_char8_str should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_set_char8_str should take a const char*");
  }

  // conduit_node_set_path_char8_str
  {
    using func_types = funcinfo<decltype(conduit_node_set_path_char8_str)>;
    abi_require(
      &conduit_node_set_path_char8_str, "conduit_node_set_path_char8_str must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_path_char8_str must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_set_path_char8_str must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_path_char8_str should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_set_path_char8_str should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, const char*>::value,
      "conduit_node_set_path_char8_str should take a const char*");
  }

  // conduit_node_set_external_char8_str
  {
    using func_types = funcinfo<decltype(conduit_node_set_external_char8_str)>;
    abi_require(
      &conduit_node_set_external_char8_str, "conduit_node_set_external_char8_str must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_external_char8_str must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_set_external_char8_str must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_external_char8_str should take a conduit_node*");
    static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, char*>::value,
      "conduit_node_set_external_char8_str should take a char*");
  }

  // conduit_node_set_path_external_char8_str
  {
    using func_types = funcinfo<decltype(conduit_node_set_path_external_char8_str)>;
    abi_require(&conduit_node_set_path_external_char8_str,
      "conduit_node_set_path_external_char8_str must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_node_set_path_external_char8_str must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_set_path_external_char8_str must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_set_path_external_char8_str should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_set_path_external_char8_str should take a const char*");
    static_assert(std::is_same<std::tuple_element<2, func_types::argument_t>::type, char*>::value,
      "conduit_node_set_path_external_char8_str should take a char*");
  }

  // conduit_node_to_json
  {
    using func_types = funcinfo<decltype(conduit_node_to_json)>;
    abi_require(&conduit_node_to_json, "conduit_node_to_json must be defined");
    static_assert(
      std::is_same<func_types::return_t, char*>::value, "conduit_node_to_json must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_to_json must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_json should take a const conduit_node*");
  }

  // conduit_node_to_json_with_options
  {
    using func_types = funcinfo<decltype(conduit_node_to_json_with_options)>;
    abi_require(
      &conduit_node_to_json_with_options, "conduit_node_to_json_with_options must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_to_json_with_options must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_to_json_with_options must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_json_with_options should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_json_with_options should take a const conduit_node*");
  }

  // conduit_node_to_string
  {
    using func_types = funcinfo<decltype(conduit_node_to_string)>;
    abi_require(&conduit_node_to_string, "conduit_node_to_string must be defined");
    static_assert(
      std::is_same<func_types::return_t, char*>::value, "conduit_node_to_string must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_to_string must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_string should take a const conduit_node*");
  }

  // conduit_node_to_string_with_options
  {
    using func_types = funcinfo<decltype(conduit_node_to_string_with_options)>;
    abi_require(
      &conduit_node_to_string_with_options, "conduit_node_to_string_with_options must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_to_string_with_options must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_to_string_with_options must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_string_with_options should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_string_with_options should take a const conduit_node*");
  }

  // conduit_node_to_summary_string
  {
    using func_types = funcinfo<decltype(conduit_node_to_summary_string)>;
    abi_require(&conduit_node_to_summary_string, "conduit_node_to_summary_string must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_to_summary_string must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_to_summary_string must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_summary_string should take a const conduit_node*");
  }

  // conduit_node_to_summary_string_with_options
  {
    using func_types = funcinfo<decltype(conduit_node_to_summary_string_with_options)>;
    abi_require(&conduit_node_to_summary_string_with_options,
      "conduit_node_to_summary_string_with_options must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_to_summary_string_with_options must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_to_summary_string_with_options must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_summary_string_with_options should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_summary_string_with_options should take a const conduit_node*");
  }

  // conduit_node_to_yaml
  {
    using func_types = funcinfo<decltype(conduit_node_to_yaml)>;
    abi_require(&conduit_node_to_yaml, "conduit_node_to_yaml must be defined");
    static_assert(
      std::is_same<func_types::return_t, char*>::value, "conduit_node_to_yaml must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_to_yaml must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_yaml should take a const conduit_node*");
  }

  // conduit_node_to_yaml_with_options
  {
    using func_types = funcinfo<decltype(conduit_node_to_yaml_with_options)>;
    abi_require(
      &conduit_node_to_yaml_with_options, "conduit_node_to_yaml_with_options must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_to_yaml_with_options must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_to_yaml_with_options must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_yaml_with_options should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_to_yaml_with_options should take a const conduit_node*");
  }

#define CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(stem, ty)                                                \
  do                                                                                               \
  {                                                                                                \
    /* conduit_node_set_STEM */                                                                    \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_##stem)>;                              \
      abi_require(&conduit_node_set_##stem, "conduit_node_set_" #stem " must be defined");         \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_" #stem " must return void");                                            \
      abi_require(std::tuple_size<func_types::argument_t>::value == 2,                             \
        "conduit_node_set_" #stem " must take two arguments");                                     \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_" #stem " must take a conduit_node*");                                   \
      static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, ty>::value,  \
        "conduit_node_set_" #stem " must take a " #ty);                                            \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_STEM_ptr */                                                                \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_##stem##_ptr)>;                        \
      abi_require(                                                                                 \
        &conduit_node_set_##stem##_ptr, "conduit_node_set_" #stem "_ptr must be defined");         \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_" #stem "_ptr must return void");                                        \
      abi_require(std::tuple_size<func_types::argument_t>::value == 3,                             \
        "conduit_node_set_" #stem "_ptr must take three arguments");                               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_" #stem "_ptr must take a conduit_node*");                               \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_" #stem "_ptr must take a " #ty);                                        \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_" #stem "_ptr must take a conduit_index_t");                             \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_STEM_ptr_detailed */                                                       \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_##stem##_ptr_detailed)>;               \
      abi_require(&conduit_node_set_##stem##_ptr_detailed,                                         \
        "conduit_node_set_" #stem "_ptr_detailed must be defined");                                \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_" #stem "_ptr_detailed must return void");                               \
      abi_require(std::tuple_size<func_types::argument_t>::value == 7,                             \
        "conduit_node_set_" #stem "_ptr_detailed must take seven arguments");                      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_" #stem "_ptr_detailed must take a conduit_node*");                      \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_" #stem "_ptr_detailed must take a " #ty "*");                           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_" #stem "_ptr_detailed must take a conduit_index_t");                    \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_" #stem "_ptr_detailed must take a conduit_index_t");                    \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<4, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_" #stem "_ptr_detailed must take a conduit_index_t");                    \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<5, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_" #stem "_ptr_detailed must take a conduit_index_t");                    \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<6, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_" #stem "_ptr_detailed must take a conduit_index_t");                    \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_path_STEM */                                                               \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_path_##stem)>;                         \
      abi_require(                                                                                 \
        &conduit_node_set_path_##stem, "conduit_node_set_path_" #stem " must be defined");         \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_path_" #stem " must return void");                                       \
      abi_require(std::tuple_size<func_types::argument_t>::value == 3,                             \
        "conduit_node_set_path_" #stem " must take three arguments");                              \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_path_" #stem " must take a conduit_node*");                              \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_set_path_" #stem " must take a const char*");                                \
      static_assert(std::is_same<std::tuple_element<2, func_types::argument_t>::type, ty>::value,  \
        "conduit_node_set_path_" #stem " must take a " #ty);                                       \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_path_STEM_ptr */                                                           \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_path_##stem##_ptr)>;                   \
      abi_require(&conduit_node_set_path_##stem##_ptr,                                             \
        "conduit_node_set_path_" #stem "_ptr must be defined");                                    \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_path_" #stem "_ptr must return void");                                   \
      abi_require(std::tuple_size<func_types::argument_t>::value == 4,                             \
        "conduit_node_set_path_" #stem "_ptr must take four arguments");                           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_path_" #stem "_ptr must take a conduit_node*");                          \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_set_path_" #stem "_ptr must take a const char*");                            \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<2, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_path_" #stem "_ptr must take a " #ty "*");                               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_" #stem "_ptr must take a conduit_index_t");                        \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_path_STEM_ptr_detailed */                                                  \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_path_##stem##_ptr_detailed)>;          \
      abi_require(&conduit_node_set_path_##stem##_ptr_detailed,                                    \
        "conduit_node_set_path_" #stem "_ptr_detailed must be defined");                           \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_path_" #stem "_ptr_detailed must return void");                          \
      abi_require(std::tuple_size<func_types::argument_t>::value == 8,                             \
        "conduit_node_set_path_" #stem "_ptr_detailed must take eight arguments");                 \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a conduit_node*");                 \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a const char*");                   \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<2, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a " #ty "*");                      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a conduit_index_t");               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<4, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a conduit_index_t");               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<5, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a conduit_index_t");               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<6, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a conduit_index_t");               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<7, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_" #stem "_ptr_detailed must take a conduit_index_t");               \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_external_STEM_ptr */                                                       \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_external_##stem##_ptr)>;               \
      abi_require(&conduit_node_set_external_##stem##_ptr,                                         \
        "conduit_node_set_external_" #stem "_ptr must be defined");                                \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_external_" #stem "_ptr must return void");                               \
      abi_require(std::tuple_size<func_types::argument_t>::value == 3,                             \
        "conduit_node_set_external_" #stem "_ptr must take three arguments");                      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_external_" #stem "_ptr must take a conduit_node*");                      \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_external_" #stem "_ptr must take a " #ty);                               \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_external_" #stem "_ptr must take a conduit_index_t");                    \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_STEM_ptr_detailed */                                                       \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_external_##stem##_ptr_detailed)>;      \
      abi_require(&conduit_node_set_external_##stem##_ptr_detailed,                                \
        "conduit_node_set_external_" #stem "_ptr_detailed must be defined");                       \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_external_" #stem "_ptr_detailed must return void");                      \
      abi_require(std::tuple_size<func_types::argument_t>::value == 7,                             \
        "conduit_node_set_external_" #stem "_ptr_detailed must take seven arguments");             \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a conduit_node*");             \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<1, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a " #ty "*");                  \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a conduit_index_t");           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a conduit_index_t");           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<4, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a conduit_index_t");           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<5, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a conduit_index_t");           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<6, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_external_" #stem "_ptr_detailed must take a conduit_index_t");           \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_path_external_STEM_ptr */                                                  \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_path_external_##stem##_ptr)>;          \
      abi_require(&conduit_node_set_path_external_##stem##_ptr,                                    \
        "conduit_node_set_path_external_" #stem "_ptr must be defined");                           \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_path_external_" #stem "_ptr must return void");                          \
      abi_require(std::tuple_size<func_types::argument_t>::value == 4,                             \
        "conduit_node_set_path_external_" #stem "_ptr must take four arguments");                  \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_path_external_" #stem "_ptr must take a conduit_node*");                 \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_set_path_external_" #stem "_ptr must take a const char*");                   \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<2, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_path_external_" #stem "_ptr must take a " #ty "*");                      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_external_" #stem "_ptr must take a conduit_index_t");               \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_set_path_external_STEM_ptr_detailed */                                         \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_set_path_external_##stem##_ptr_detailed)>; \
      abi_require(&conduit_node_set_path_external_##stem##_ptr_detailed,                           \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must be defined");                  \
      static_assert(std::is_same<func_types::return_t, void>::value,                               \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must return void");                 \
      abi_require(std::tuple_size<func_types::argument_t>::value == 8,                             \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take eight arguments");        \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a conduit_node*");        \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a const char*");          \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<std::tuple_element<2, func_types::argument_t>::type, ty*>::value, \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a " #ty "*");             \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a conduit_index_t");      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<4, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a conduit_index_t");      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<5, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a conduit_index_t");      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<6, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a conduit_index_t");      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<7, func_types::argument_t>::type, conduit_index_t>::value, \
        "conduit_node_set_path_external_" #stem "_ptr_detailed must take a conduit_index_t");      \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_as_STEM */                                                                     \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_as_##stem)>;                               \
      abi_require(&conduit_node_as_##stem, "conduit_node_as_" #stem " must be defined");           \
      static_assert(std::is_same<func_types::return_t, ty>::value,                                 \
        "conduit_node_as_" #stem " must return " #ty);                                             \
      abi_require(std::tuple_size<func_types::argument_t>::value == 1,                             \
        "conduit_node_set_" #stem " must take one argument");                                      \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_" #stem " must take a conduit_node*");                                   \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_as_STEM_ptr */                                                                 \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_as_##stem##_ptr)>;                         \
      abi_require(&conduit_node_as_##stem##_ptr, "conduit_node_as_" #stem "_ptr must be defined"); \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<func_types::return_t, ty*>::value,                                \
        "conduit_node_as_" #stem "_ptr must return " #ty "*");                                     \
      abi_require(std::tuple_size<func_types::argument_t>::value == 1,                             \
        "conduit_node_set_" #stem "_ptr must take one argument");                                  \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_set_" #stem "_ptr must take a conduit_node*");                               \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_fetch_path_as_STEM */                                                          \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_fetch_path_as_##stem)>;                    \
      abi_require(&conduit_node_fetch_path_as_##stem,                                              \
        "conduit_node_fetch_path_as_" #stem " must be defined");                                   \
      static_assert(std::is_same<func_types::return_t, ty>::value,                                 \
        "conduit_node_fetch_path_as_" #stem " must return " #ty);                                  \
      abi_require(std::tuple_size<func_types::argument_t>::value == 2,                             \
        "conduit_node_fetch_path_as_" #stem " must take two arguments");                           \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_fetch_path_as_" #stem " must take a conduit_node*");                         \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_fetch_path_as_" #stem " must take a const char*");                           \
    }                                                                                              \
                                                                                                   \
    /* conduit_node_fetch_path_as_STEM_ptr */                                                      \
    {                                                                                              \
      using func_types = funcinfo<decltype(conduit_node_fetch_path_as_##stem##_ptr)>;              \
      abi_require(&conduit_node_fetch_path_as_##stem##_ptr,                                        \
        "conduit_node_fetch_path_as_" #stem "_ptr must be defined");                               \
      /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                                             \
      static_assert(std::is_same<func_types::return_t, ty*>::value,                                \
        "conduit_node_fetch_path_as_" #stem "_ptr must return " #ty "*");                          \
      abi_require(std::tuple_size<func_types::argument_t>::value == 2,                             \
        "conduit_node_fetch_path_as_" #stem "_ptr must take two arguments");                       \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,   \
        "conduit_node_fetch_path_as_" #stem "_ptr must take a conduit_node*");                     \
      static_assert(                                                                               \
        std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,     \
        "conduit_node_fetch_path_as_" #stem "_ptr must take a const char*");                       \
    }                                                                                              \
  } while (false)

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(int8, conduit_int8);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(int16, conduit_int16);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(int32, conduit_int32);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(int64, conduit_int64);

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(uint8, conduit_uint8);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(uint16, conduit_uint16);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(uint32, conduit_uint32);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(uint64, conduit_uint64);

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(float32, conduit_float32);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(float64, conduit_float64);

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(char, char);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(short, short);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(int, int);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(long, long);

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(signed_char, signed char);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(signed_short, signed short);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(signed_int, signed int);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(signed_long, signed long);

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(unsigned_char, unsigned char);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(unsigned_short, unsigned short);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(unsigned_int, unsigned int);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(unsigned_long, unsigned long);

  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(float, float);
  CHECK_CONDUIT_NODE_TYPE_FUNCTIONS(double, double);

#undef CHECK_CONDUIT_NODE_TYPE_FUNCTIONS

  // conduit_node_data_ptr
  {
    using func_types = funcinfo<decltype(conduit_node_data_ptr)>;
    abi_require(&conduit_node_data_ptr, "conduit_node_data_ptr must be defined");
    static_assert(
      std::is_same<func_types::return_t, void*>::value, "conduit_node_data_ptr must return void*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_data_ptr must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_data_ptr should take a conduit_node*");
  }

  // conduit_node_element_ptr
  {
    using func_types = funcinfo<decltype(conduit_node_element_ptr)>;
    abi_require(&conduit_node_element_ptr, "conduit_node_element_ptr must be defined");
    static_assert(std::is_same<func_types::return_t, void*>::value,
      "conduit_node_element_ptr must return void*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_element_ptr must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_element_ptr should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_index_t>::value,
      "conduit_node_element_ptr should take a conduit_index_t");
  }

  // conduit_node_fetch_path_data_ptr
  {
    using func_types = funcinfo<decltype(conduit_node_fetch_path_data_ptr)>;
    abi_require(
      &conduit_node_fetch_path_data_ptr, "conduit_node_fetch_path_data_ptr must be defined");
    static_assert(std::is_same<func_types::return_t, void*>::value,
      "conduit_node_fetch_path_data_ptr must return void*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_fetch_path_data_ptr must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_fetch_path_data_ptr should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_fetch_path_data_ptr should take a const char*");
  }

  // conduit_node_fetch_path_element_ptr
  {
    using func_types = funcinfo<decltype(conduit_node_fetch_path_element_ptr)>;
    abi_require(
      &conduit_node_fetch_path_element_ptr, "conduit_node_fetch_path_element_ptr must be defined");
    static_assert(std::is_same<func_types::return_t, void*>::value,
      "conduit_node_fetch_path_element_ptr must return void*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_node_fetch_path_element_ptr must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_fetch_path_element_ptr should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_fetch_path_element_ptr should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_index_t>::value,
      "conduit_node_fetch_path_element_ptr should take a conduit_index_t");
  }

  // conduit_node_as_char8_str
  {
    using func_types = funcinfo<decltype(conduit_node_as_char8_str)>;
    abi_require(&conduit_node_as_char8_str, "conduit_node_as_char8_str must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_as_char8_str must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_as_char8_str must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_as_char8_str should take a conduit_node*");
  }

  // conduit_node_fetch_path_as_char8_str
  {
    using func_types = funcinfo<decltype(conduit_node_fetch_path_as_char8_str)>;
    abi_require(&conduit_node_fetch_path_as_char8_str,
      "conduit_node_fetch_path_as_char8_str must be defined");
    static_assert(std::is_same<func_types::return_t, char*>::value,
      "conduit_node_fetch_path_as_char8_str must return char*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_node_fetch_path_as_char8_str must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_node_fetch_path_as_char8_str should take a conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_node_fetch_path_as_char8_str should take a const char*");
  }

  // conduit_node_dtype
  {
    using func_types = funcinfo<decltype(conduit_node_dtype)>;
    abi_require(&conduit_node_dtype, "conduit_node_dtype must be defined");
    static_assert(std::is_same<func_types::return_t, const conduit_datatype*>::value,
      "conduit_node_dtype must return const conduit_datatype*");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_node_dtype must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_node_dtype should take a const conduit_node*");
  }

  return ret;
}

static int test_conduit_abi()
{
  int ret = EXIT_SUCCESS;

  // conduit_about
  {
    using func_types = funcinfo<decltype(conduit_about)>;
    abi_require(&conduit_about, "conduit_about must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_about must return void");
    abi_require(
      std::tuple_size<func_types::argument_t>::value == 1, "conduit_about must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_about should take a conduit_node*");
  }

  return ret;
}

static int test_conduit_utils_abi()
{
  int ret = EXIT_SUCCESS;

  // conduit_utils_set_info_handler
  {
    using func_types = funcinfo<decltype(conduit_utils_set_info_handler)>;
    abi_require(&conduit_utils_set_info_handler, "conduit_utils_set_info_handler must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_utils_set_info_handler must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_utils_set_info_handler must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,
                    void (*)(const char*, const char*, int)>::value,
      "conduit_utils_set_info_handler should take an appropriate callback type");
  }

  // conduit_utils_set_warning_handler
  {
    using func_types = funcinfo<decltype(conduit_utils_set_warning_handler)>;
    abi_require(
      &conduit_utils_set_warning_handler, "conduit_utils_set_warning_handler must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_utils_set_warning_handler must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_utils_set_warning_handler must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,
                    void (*)(const char*, const char*, int)>::value,
      "conduit_utils_set_warning_handler should take an appropriate callback type");
  }

  // conduit_utils_set_error_handler
  {
    using func_types = funcinfo<decltype(conduit_utils_set_error_handler)>;
    abi_require(
      &conduit_utils_set_error_handler, "conduit_utils_set_error_handler must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_utils_set_error_handler must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_utils_set_error_handler must take one argument");
    static_assert(std::is_same<std::tuple_element<0, func_types::argument_t>::type,
                    void (*)(const char*, const char*, int)>::value,
      "conduit_utils_set_error_handler should take an appropriate callback type");
  }

  return ret;
}

static int test_conduit_blueprint_abi()
{
  int ret = EXIT_SUCCESS;

  // conduit_blueprint_about
  {
    using func_types = funcinfo<decltype(conduit_blueprint_about)>;
    abi_require(&conduit_blueprint_about, "conduit_blueprint_about must be defined");
    static_assert(
      std::is_same<func_types::return_t, void>::value, "conduit_blueprint_about must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_blueprint_about must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_about should take a conduit_node*");
  }

  // conduit_blueprint_verify
  {
    using func_types = funcinfo<decltype(conduit_blueprint_verify)>;
    abi_require(&conduit_blueprint_verify, "conduit_blueprint_verify must be defined");
    static_assert(
      std::is_same<func_types::return_t, int>::value, "conduit_blueprint_verify must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_blueprint_verify must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const char*>::value,
      "conduit_blueprint_verify should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_verify should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_verify should take a conduit_node*");
  }

  // conduit_blueprint_mcarray_verify
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mcarray_verify)>;
    abi_require(
      &conduit_blueprint_mcarray_verify, "conduit_blueprint_mcarray_verify must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mcarray_verify must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_blueprint_mcarray_verify must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mcarray_verify should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mcarray_verify should take a conduit_node*");
  }

  // conduit_blueprint_mcarray_verify_sub_protocol
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mcarray_verify_sub_protocol)>;
    abi_require(&conduit_blueprint_mcarray_verify_sub_protocol,
      "conduit_blueprint_mcarray_verify_sub_protocol must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mcarray_verify_sub_protocol must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_blueprint_mcarray_verify_sub_protocol must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const char*>::value,
      "conduit_blueprint_mcarray_verify_sub_protocol should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mcarray_verify_sub_protocol should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mcarray_verify_sub_protocol should take a conduit_node*");
  }

  // conduit_blueprint_mcarray_is_interleaved
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mcarray_is_interleaved)>;
    abi_require(&conduit_blueprint_mcarray_is_interleaved,
      "conduit_blueprint_mcarray_is_interleaved must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mcarray_is_interleaved must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 1,
      "conduit_blueprint_mcarray_is_interleaved must take one argument");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mcarray_is_interleaved should take a const conduit_node*");
  }

  // conduit_blueprint_mcarray_to_contiguous
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mcarray_to_contiguous)>;
    abi_require(&conduit_blueprint_mcarray_to_contiguous,
      "conduit_blueprint_mcarray_to_contiguous must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mcarray_to_contiguous must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_blueprint_mcarray_to_contiguous must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mcarray_to_contiguous should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mcarray_to_contiguous should take a conduit_node*");
  }

  // conduit_blueprint_mcarray_to_interleaved
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mcarray_to_interleaved)>;
    abi_require(&conduit_blueprint_mcarray_to_interleaved,
      "conduit_blueprint_mcarray_to_interleaved must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mcarray_to_interleaved must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_blueprint_mcarray_to_interleaved must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mcarray_to_interleaved should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mcarray_to_interleaved should take a conduit_node*");
  }

  // conduit_blueprint_mesh_verify
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mesh_verify)>;
    abi_require(&conduit_blueprint_mesh_verify, "conduit_blueprint_mesh_verify must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mesh_verify must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_blueprint_mesh_verify must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_verify should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mesh_verify should take a conduit_node*");
  }

  // conduit_blueprint_mesh_verify_sub_protocol
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mesh_verify_sub_protocol)>;
    abi_require(&conduit_blueprint_mesh_verify_sub_protocol,
      "conduit_blueprint_mesh_verify_sub_protocol must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_mesh_verify_sub_protocol must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_blueprint_mesh_verify_sub_protocol must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const char*>::value,
      "conduit_blueprint_mesh_verify_sub_protocol should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_verify_sub_protocol should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mesh_verify_sub_protocol should take a conduit_node*");
  }

  // conduit_blueprint_mesh_generate_index
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mesh_generate_index)>;
    abi_require(&conduit_blueprint_mesh_generate_index,
      "conduit_blueprint_mesh_generate_index must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_blueprint_mesh_generate_index must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 4,
      "conduit_blueprint_mesh_generate_index must take four arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_generate_index should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const char*>::value,
      "conduit_blueprint_mesh_generate_index should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_index_t>::value,
      "conduit_blueprint_mesh_generate_index should take a conduit_index_t");
    static_assert(
      std::is_same<std::tuple_element<3, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mesh_generate_index should take a conduit_node*");
  }

  // conduit_blueprint_mesh_partition
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mesh_partition)>;
    abi_require(
      &conduit_blueprint_mesh_partition, "conduit_blueprint_mesh_partition must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_blueprint_mesh_partition must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_blueprint_mesh_partition must take four arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_partition should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_partition should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mesh_partition should take a conduit_node*");
  }

  // conduit_blueprint_mesh_flatten
  {
    using func_types = funcinfo<decltype(conduit_blueprint_mesh_flatten)>;
    abi_require(&conduit_blueprint_mesh_flatten, "conduit_blueprint_mesh_flatten must be defined");
    static_assert(std::is_same<func_types::return_t, void>::value,
      "conduit_blueprint_mesh_flatten must return void");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_blueprint_mesh_flatten must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_flatten should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_mesh_flatten should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_mesh_flatten should take a conduit_node*");
  }

  // conduit_blueprint_table_verify
  {
    using func_types = funcinfo<decltype(conduit_blueprint_table_verify)>;
    abi_require(&conduit_blueprint_table_verify, "conduit_blueprint_table_verify must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_table_verify must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 2,
      "conduit_blueprint_table_verify must take two arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_table_verify should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_table_verify should take a conduit_node*");
  }

  // conduit_blueprint_table_verify_sub_protocol
  {
    using func_types = funcinfo<decltype(conduit_blueprint_table_verify_sub_protocol)>;
    abi_require(&conduit_blueprint_table_verify_sub_protocol,
      "conduit_blueprint_table_verify_sub_protocol must be defined");
    static_assert(std::is_same<func_types::return_t, int>::value,
      "conduit_blueprint_table_verify_sub_protocol must return int");
    abi_require(std::tuple_size<func_types::argument_t>::value == 3,
      "conduit_blueprint_table_verify_sub_protocol must take three arguments");
    static_assert(
      std::is_same<std::tuple_element<0, func_types::argument_t>::type, const char*>::value,
      "conduit_blueprint_table_verify_sub_protocol should take a const char*");
    static_assert(
      std::is_same<std::tuple_element<1, func_types::argument_t>::type, const conduit_node*>::value,
      "conduit_blueprint_table_verify_sub_protocol should take a const conduit_node*");
    static_assert(
      std::is_same<std::tuple_element<2, func_types::argument_t>::type, conduit_node*>::value,
      "conduit_blueprint_table_verify_sub_protocol should take a conduit_node*");
  }

  return ret;
}

int main(int /*argc*/, char* /*argv*/[])
{
  int ret = EXIT_SUCCESS;

  int test_ret = test_conduit_bitwidth_types();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  test_ret = test_conduit_endianness_abi();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  test_ret = test_conduit_datatype_abi();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  test_ret = test_conduit_node_abi();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  test_ret = test_conduit_abi();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  test_ret = test_conduit_utils_abi();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  test_ret = test_conduit_blueprint_abi();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  return ret;
}
