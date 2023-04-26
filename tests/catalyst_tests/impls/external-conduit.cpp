/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>
#include <catalyst_stub.h>

#include "catalyst_impl_external_conduit.h"

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_initialize_external_conduit(const conduit_node* params)
{
  return catalyst_stub_initialize(params);
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_execute_external_conduit(const conduit_node* params)
{
  return catalyst_stub_execute(params);
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_finalize_external_conduit(const conduit_node* params)
{
  return catalyst_stub_finalize(params);
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_about_external_conduit(conduit_node* params)
{
  return catalyst_stub_about(params);
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_results_external_conduit(conduit_node* params)
{
  return catalyst_stub_results(params);
}
