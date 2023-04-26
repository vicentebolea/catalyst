/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include "catalyst_conduit_abi.h"

#include <catalyst_export.h>

// Typo'd mangled symbol. Provide compatibility.
CATALYST_EXPORT void* catalyst_conduit_node_fetch_node_data_ptr(
  conduit_node* node, const char* path)
{
  return conduit_node_fetch_path_data_ptr(node, path);
}
