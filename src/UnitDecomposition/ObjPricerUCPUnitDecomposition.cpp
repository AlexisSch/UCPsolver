/** 
 * @file
 * Implement the class ObjPricerUCPUnitDecomposition 
*/


//** Includes

//* Standart
#include <vector>


//* SCIP
#include "objscip/objpricer.h"
#include "scip/pub_var.h"
#include <scip/scip.h>


//* User 

// general
#include "DataClasses/InstanceUCP.h"

// Decomposition
#include "Decomposition/FormulationPricer.h"

// Unit Decomposition
#include "UnitDecomposition/FormulationMasterUnitDecomposition.h"
#include "UnitDecomposition/FormulationPricerUnitDecomposition.h"
#include "UnitDecomposition/ObjPricerUCPUnitDecomposition.h"


//** Namespaces

using namespace std;
using namespace scip;


/** constructor */
ObjPricerUCPUnitDecomposition::ObjPricerUCPUnitDecomposition(
    SCIP* scip_master,       /**< SCIP pointer */
    const char* name,               /**< name of pricer */
    FormulationMasterUnitDecomposition* formulation_master,
    InstanceUCP* instance_ucp
):
    ObjPricer(scip_master, name, "Pricer", 0, TRUE)
{
    m_formulation_master = formulation_master;
    m_instance_ucp = instance_ucp;
}


/** destructor */
ObjPricerUCPUnitDecomposition::~ObjPricerUCPUnitDecomposition()
{}


/** initialization method of variable pricer (called after problem was transformed)
 *
 *  Because SCIP transformes the original problem in preprocessing, we need to get the references to
 *  the variables and constraints in the transformed problem from the references in the original
 *  problem.
 */
SCIP_DECL_PRICERINIT(ObjPricerUCPUnitDecomposition::scip_init)
{
    
    int number_time_steps( m_instance_ucp->get_time_steps_number());
    for(int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++ ) 
    {
        SCIP_CALL( SCIPgetTransformedCons( scip, 
                *m_formulation_master->get_complicating_constraints(i_time_step),
                m_formulation_master->get_complicating_constraints(i_time_step) ) );
    } 

    SCIP_CALL( SCIPgetTransformedCons(scip, 
            *m_formulation_master->get_convexity_constraint(), 
            m_formulation_master->get_convexity_constraint() ) );

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
SCIP_DECL_PRICERREDCOST(ObjPricerUCPUnitDecomposition::scip_redcost)
{
    SCIPdebugMsg(scip, "call scip_redcost ...\n");

    /* set result pointer, see above */
    *result = SCIP_SUCCESS;

    /* call pricing routine */
    ucp_pricing(scip);

    return SCIP_OKAY;
} 



void ObjPricerUCPUnitDecomposition::ucp_pricing(SCIP* scip)
{
    m_list_RMP_opt.push_back( SCIPgetPrimalbound( scip ) );

    //* get the reduced costs
    int number_time_steps( m_instance_ucp->get_time_steps_number());
    vector< SCIP_Real > reduced_cost_demand;
    reduced_cost_demand.resize( number_time_steps );
    SCIP_CONS* current_constraint(0);

    for(int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++ ) 
    {
        current_constraint =  *m_formulation_master->get_complicating_constraints(i_time_step) ;
        reduced_cost_demand[i_time_step] = SCIPgetDualsolLinear( scip, current_constraint );
    }
    current_constraint = *(m_formulation_master->get_convexity_constraint());
    SCIP_Real reduced_cost_convexity( SCIPgetDualsolLinear( scip, current_constraint ) );


    //*  create and solve the pricing problem with the reduced values
    SCIP* scip_pricer(0);
    SCIPcreate( &scip_pricer );
    SCIPincludeDefaultPlugins( scip_pricer );
    SCIPcreateProb(scip_pricer, "UCP_PRICER_PROBLEM", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_pricer, "display/verblevel", 0);
    FormulationPricerUnitDecomposition formulation_pricer( m_instance_ucp, scip_pricer, reduced_cost_demand );
    SCIPsolve( scip_pricer );
    // SCIPprintBestSol(scip_pricer, NULL, FALSE) ;

    //* if a plan is found, create and add the column, else, do nothing, which will make the column generation stop
    SCIP_Real optimal_value(SCIPgetPrimalbound( scip_pricer ) );
    if( optimal_value < reduced_cost_convexity -0.0001 )
    {
        ProductionPlan* new_plan = formulation_pricer.get_production_plan_from_solution();
        m_formulation_master->add_column( new_plan, false );     
    }
    
}









