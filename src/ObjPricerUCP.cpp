/** Implement the class ObjPricerUCP */


/* standart includes */
#include <vector>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "ObjPricerUCP.h"
#include "FormulationMaster.h"
#include "InstanceUCP.h"


/* namespace */

using namespace std;
using namespace scip;


/** constructor */
ObjPricerUCP::ObjPricerUCP(
    SCIP* scip_master,       /**< SCIP pointer */
    const char* name,               /**< name of pricer */
    FormulationMaster* formulation_master,
    InstanceUCP* instance_ucp
):
    ObjPricer(scip_master, name, "Pricer", 0, TRUE)
{
    p_formulation_master = formulation_master;
    p_instance_ucp = instance_ucp;
}


/** destructor */
ObjPricerUCP::~ObjPricerUCP()
{}


/** initialization method of variable pricer (called after problem was transformed)
 *
 *  Because SCIP transformes the original problem in preprocessing, we need to get the references to
 *  the variables and constraints in the transformed problem from the references in the original
 *  problem.
 */
SCIP_DECL_PRICERINIT(ObjPricerUCP::scip_init)
{
    int number_time_steps( p_instance_ucp->get_time_steps_number());
    for(int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++ ) 
    {
        SCIP_CONS* current_constraint( p_formulation_master->m_complicating_constraints[i_time_step]);
        SCIPgetTransformedCons(scip_, current_constraint, &current_constraint);
    }

    return SCIP_OKAY;
}


/** Pricing of additional variables if LP is feasible.
 *
 *  - get the values of the dual variables you need
 *  - construct the reduced-cost arc lengths from these values
 *  - find the shortest admissible tour with respect to these lengths
 *  - if this tour has negative reduced cost, add it to the LP
 *
 *  possible return values for *result:
 *  - SCIP_SUCCESS    : at least one improving variable was found, or it is ensured that no such variable exists
 *  - SCIP_DIDNOTRUN  : the pricing process was aborted by the pricer, there is no guarantee that the current LP solution is optimal
 */
SCIP_DECL_PRICERREDCOST(ObjPricerUCP::scip_redcost)
{
    SCIPdebugMsg(scip, "call scip_redcost ...\n");

    /* set result pointer, see above */
    *result = SCIP_SUCCESS;

    /* call pricing routine */
    ucp_pricing();

    return SCIP_OKAY;
} 



void ObjPricerUCP::ucp_pricing()
{
    cerr << "Let's take a look at the reduced costs : " << endl;
    int number_time_steps( p_instance_ucp->get_time_steps_number());
    int value_dual_cost(0);
    for(int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++ ) 
    {
        SCIP_CONS* current_constraint( p_formulation_master->m_complicating_constraints[i_time_step]);
        value_dual_cost = SCIPgetDualsolLinear( scip_, current_constraint );
        cerr << "the value of the constraint " << i_time_step << " is : " << value_dual_cost << endl;
    }
    cerr << "see you, pricer" << endl;
}