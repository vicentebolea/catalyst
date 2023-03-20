set(fmt_headers
    conduit_fmt/chrono.h
    conduit_fmt/color.h
    conduit_fmt/compile.h
    conduit_fmt/conduit_fmt.h
    conduit_fmt/core.h
    conduit_fmt/format-inl.h
    conduit_fmt/format.h
    conduit_fmt/locale.h
    conduit_fmt/os.h
    conduit_fmt/ostream.h
    conduit_fmt/posix.h
    conduit_fmt/printf.h
    conduit_fmt/ranges.h
    )

add_library(catalyst_conduit_fmt INTERFACE
  ${fmt_headers})
target_include_directories(catalyst_conduit_fmt
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
set_property(TARGET catalyst_conduit_fmt PROPERTY
  EXPORT_NAME conduit_fmt)
add_library(catalyst::conduit_fmt ALIAS catalyst_conduit_fmt)
