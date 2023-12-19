# interface library to hold platform flags
add_library(catalyst_python_flags INTERFACE)
set(platform_flags
    # Apple flags.
    # fixes warning "ld: warning: -undefined dynamic_lookup may not work with chained fixups"
    # for Xcode < 14.3
    "$<$<PLATFORM_ID:Darwin>:LINKER:-no_fixup_chains>"
)
target_link_options(catalyst_python_flags
  INTERFACE
     ${platform_flags})
