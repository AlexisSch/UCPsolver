/** Implement the class ObjPricerUCP */


/* standart includes */
#include <vector>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "ObjPricerUCP.h"
#include "FormulationMaster.h"
#include "FormulationPricer.h"
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
        SCIP_CALL( SCIPgetTransformedCons(scip, 
                p_formulation_master->m_complicating_constraints[i_time_step],
                &(p_formulation_master->m_complicating_constraints)[i_time_step]) );
    }

    SCIP_CALL( SCIPgetTransformedCons(scip, 
            p_formulation_master->m_convexity_constraint, 
            &p_formulation_master->m_convexity_constraint) );

    int number_columns( p_formulation_master->m_vector_columns.size());
    for( int i_column = 0; i_column < number_columns; i_column ++)
    {
        SCIP_VAR* current_var( p_formulation_master->m_vector_columns[i_column]->get_variable_pointer() );
        SCIP_CALL( SCIPgetTransformedVar(scip, 
            current_var, 
            &current_var) );
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
    ucp_pricing(scip);

    return SCIP_OKAY;
} 



void ObjPricerUCP::ucp_pricing(SCIP* scip)
{
    m_list_RMP_opt.push_back( SCIPgetPrimalbound( scip ) );

    // get the reduced costs
    int number_time_steps( p_instance_ucp->get_time_steps_number());
    vector< SCIP_Real > reduced_cost_demand;
    reduced_cost_demand.resize( number_time_steps );
    SCIP_CONS* current_constraint(0);

    for(int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++ ) 
    {
        current_constraint =  p_formulation_master->m_complicating_constraints[i_time_step] ;
        reduced_cost_demand[i_time_step] = SCIPgetDualsolLinear( scip, current_constraint );
        // cerr << "the value of the demand constraint " << i_time_step << " is : " << reduced_cost_demand[i_time_step] << endl;
    }
    current_constraint = p_formulation_master->m_convexity_constraint;
    SCIP_Real reduced_cost_convexity( SCIPgetDualsolLinear( scip, current_constraint ) );
    // cerr << "the value of convexity demand is : " << reduced_cost_convexity << endl;


    //  create and solve the pricing problem with the reduced values
    SCIP* scip_pricer(0);
    SCIPcreate( &scip_pricer );
    SCIPincludeDefaultPlugins( scip_pricer );
    SCIPcreateProb(scip_pricer, "UCP_PRICER_PROBLEM", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_pricer, "display/verblevel", 0);
    FormulationPricer *formulation_pricer = new FormulationPricer( p_instance_ucp, scip_pricer, reduced_cost_demand );
    SCIPsolve( scip_pricer );
    // SCIPprintBestSol(scip_pricer, NULL, FALSE) ;

    // if a plan is found, create and add the column, else, do nothing, which will make the column generation stop
    SCIP_Real optimal_value(SCIPgetPrimalbound( scip_pricer ) );
    if( optimal_value < reduced_cost_convexity -0.0001 )
    {

        // create the plan
        ProductionPlan* new_plan = new ProductionPlan( p_instance_ucp, formulation_pricer );
        new_plan->computeCost();

        // create the scip variable
        string column_name = "column_" + to_string(p_formulation_master->m_vector_columns.size()); 
        SCIP_VAR* p_variable;

        SCIPcreateVar(  scip,
            &p_variable,                            // pointer 
            column_name.c_str(),                            // name
            0.,                                     // lowerbound
            +SCIPinfinity(scip),            // upperbound
            new_plan->get_cost(),          // coeff in obj function
            SCIP_VARTYPE_CONTINUOUS,
            false, false, NULL, NULL, NULL, NULL, NULL);
    
        SCIPaddPricedVar(scip, p_variable, 1.);

        // create the master variable
        VariableMaster* new_column = new VariableMaster( p_variable, new_plan );
        p_formulation_master->addColumn( new_column );
    }
    
}