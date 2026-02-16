cmake_minimum_required(VERSION 3.13)

execute_process(
  COMMAND "${nm}" "${library}"
  OUTPUT_VARIABLE out
  ERROR_VARIABLE err
  RESULT_VARIABLE res
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_STRIP_TRAILING_WHITESPACE)

if (res)
  message(FATAL_ERROR
    "Failed to extract symbols: ${res}")
endif ()

set(symbol_names)

string(REPLACE "\n" ";" symbol_lines "${out}")
foreach (symbol_line IN LISTS symbol_lines)
  string(REGEX REPLACE " +" ";" symbol_line "${symbol_line}")
  list(GET symbol_line 1 type)

  # Ignore internal symbols as well as weak
  # and undefined symbols.
  #   lowercase - private
  #   U - undefined
  #   V - weak objects
  #   W - weak functions
  if (type MATCHES "[a-zUVW]")
    continue ()
  endif ()

  list(GET symbol_line 2 name)
  list(APPEND symbol_names "${name}")
endforeach ()

if (NOT symbol_names)
  message(FATAL_ERROR
    "No public symbols found in ${library}")
endif ()

list(SORT symbol_names)
list(REMOVE_DUPLICATES symbol_names)

# Read the list of expected symbols.
file(STRINGS "${symbols}" expected)
list(REMOVE_ITEM expected "")
list(SORT expected)

# Add extra symbols for fortran
if(fortran_symbols)
  file(STRINGS "${fortran_symbols}" expected_fortran)
  list(REMOVE_ITEM expected_fortran "")
  list(SORT expected_fortran)

  if (ifort_mangle)
    list(TRANSFORM expected_fortran REPLACE "^__" "")
    list(TRANSFORM expected_fortran REPLACE "_MOD_" "_mp_")
    list(TRANSFORM expected_fortran APPEND "_")
    # extra symbol added only by ifort
     list(APPEND expected "catalyst_conduit._")
  endif ()

  list(APPEND expected "${expected_fortran}")
endif()

if (apple_mangle)
  list(TRANSFORM expected PREPEND "_")
endif ()

# extra symbols added only by intel compilers C/C++
if (icc_mangle OR icx_mangle)
  list(APPEND expected
    "__bss_start"
    "_edata"
    "_end")
endif ()
set(debug_types
  Debug)
if (icc_mangle AND build_config IN_LIST debug_types)
  list(APPEND expected
    ".debug_info_seg")
endif ()

# Check for unexpected symbols (in library but not expected)
set(unexpected "${symbol_names}")
list(REMOVE_ITEM unexpected ${expected})

# Filter unexpected symbols to only those that look like internal leaks.
# Compiler-generated symbols (RTTI, Fortran module internals, etc.) are ignored.
# We only care about symbols containing "catalyst" or "conduit" which would
# indicate accidentally exported internal functions.
# Exclude Fortran module symbols (_mp_ for ifort, _MOD_ for gfortran) as these
# are legitimate public API symbols that vary by compiler.
set(leaked)
foreach(sym IN LISTS unexpected)
  string(TOLOWER "${sym}" sym_lower)
  # Skip Fortran module symbols (public API, not leaks)
  if(sym_lower MATCHES "(_mp_|_mod_)")
    continue()
  endif()
  if(sym_lower MATCHES "(catalyst|conduit)")
    list(APPEND leaked "${sym}")
  endif()
endforeach()

if(leaked)
  string(REPLACE ";" ", " leaked_text "${leaked}")
  message(SEND_ERROR
    "Leaked internal symbols found: ${leaked_text}")
endif()

# Check for missing symbols (in expected but not in library)
set(missing "${expected}")
list(REMOVE_ITEM missing ${symbol_names})

if (missing)
  string(REPLACE ";" ", " missing_text "${missing}")
  message(SEND_ERROR
    "Missing symbols not found: ${missing_text}")
endif ()
