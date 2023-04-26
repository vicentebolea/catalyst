catalyst_implementation(
  TARGET  catalyst-double
  NAME    double
  SOURCES double.cpp)

# Mock up Catalyst targets with different Conduit states.
add_library(catalyst_with_external_conduit INTERFACE)
target_link_libraries(catalyst_with_external_conduit INTERFACE catalyst::catalyst)
set_property(TARGET catalyst_with_external_conduit PROPERTY CATALYST_WITH_EXTERNAL_CONDUIT 1)

add_library(catalyst_with_internal_conduit INTERFACE)
target_link_libraries(catalyst_with_internal_conduit INTERFACE catalyst::catalyst)
set_property(TARGET catalyst_with_internal_conduit PROPERTY CATALYST_WITH_EXTERNAL_CONDUIT 0)

catalyst_implementation(
  TARGET  catalyst-external-conduit
  NAME    external_conduit
  SOURCES external-conduit.cpp
  CATALYST_TARGET catalyst_with_external_conduit)

catalyst_implementation(
  TARGET  catalyst-internal-conduit
  NAME    internal_conduit
  SOURCES internal-conduit.cpp
  CATALYST_TARGET catalyst_with_internal_conduit)
