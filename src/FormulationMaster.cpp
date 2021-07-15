/* Implements the class FormulationMaster */

/* standart includes */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"
#include "FormulationMaster.h"
#include "VariableMaster.h"
#include "ProductionPlan.h"

/* namespace */
using namespace std;

/** constructor */
FormulationMaster::FormulationMaster( InstanceUCP* instance, SCIP* scip_master)
{
    p_instance = instance;
    p_scip_master = scip_master;


    // creating the constraints
    ostringstream current_cons_name;

    // convexity constraint
    m_convexity_constraint = new SCIP_CONS;

    current_cons_name.str("");
    current_cons_name << "convexity_constraint";
    SCIPcreateConsBasicLinear(p_scip_master, 
            &m_convexity_constraint,            /* constraint pointer */ 
            current_cons_name.str().c_str(),    /* constraint name */
            0,                                  /* number of variable added */
            nullptr,                            /* array of variable */
            nullptr,                            /* array of coefficient */
            1 ,    /* LHS */
            1 );                                 /* RHS */

    SCIPaddCons( p_scip_master, m_convexity_constraint );



    // Complicating constraints
    int number_time_step( p_instance->get_time_steps_number() );
    m_complicating_constraints.resize( number_time_step );
    vector<int> demand( p_instance->get_demand() );
    for( int i_time_step = 0; i_time_step < number_time_step; i_time_step ++)
    {
        SCIP_CONS* demand_constraint_t;

        current_cons_name.str("");
        current_cons_name << "demand_constraint_" << i_time_step;
        SCIPcreateConsBasicLinear(p_scip_master, 
                &demand_constraint_t,               /* constraint   */ 
                current_cons_name.str().c_str(),    /* constraint name */
                0,                                  /* number of variable added */
                nullptr,                            /* array of variable */
                nullptr,                            /* array of coefficient */
                demand[i_time_step] ,               /* LHS */
                + SCIPinfinity( p_scip_master ) );  /* RHS */

        SCIPaddCons( p_scip_master, demand_constraint_t );
        m_complicating_constraints[i_time_step] = demand_constraint_t;
    }

}




FormulationMaster::~FormulationMaster()
{}


void FormulationMaster::addColumn( ProductionPlan* production_plan)
{
    // creating the column variable

    string column_name = "column_" + to_string(m_vector_columns.size()); 
    
    SCIP_VAR* p_variable;
    // we create the variable
    SCIPcreateVarBasic(  p_scip_master,
        &p_variable,                            // pointer 
        column_name.c_str(),                            // name
        0.,                                     // lowerbound
        +SCIPinfinity(p_scip_master),            // upperbound
        production_plan->get_cost(),          // coeff in obj function
        SCIP_VARTYPE_CONTINUOUS);                   // type
    
    SCIPaddVar(p_scip_master, p_variable);


    VariableMaster current_column( p_variable, production_plan );

    m_vector_columns.push_back( &current_column );
     
    // add column to convexity constraint
    SCIPaddCoefLinear(p_scip_master,
                m_convexity_constraint,
                p_variable,  /* variable to add */
                1.);                               /* coefficient */        
    
    // add column to demand constraints
    int number_time_step( p_instance->get_time_steps_number() );
    vector<vector<double>> quantity_plan = production_plan->get_quantity_plan();
    for( int i_time_step = 0; i_time_step < number_time_step; i_time_step++ )
    {
        int quantity_plan_t(0);
        for( int i_unit = 0; i_unit < p_instance->get_units_number(); i_unit++ )
        {
            quantity_plan_t += quantity_plan[i_unit][i_time_step];
        }
        SCIPaddCoefLinear(p_scip_master,
            m_complicating_constraints[i_time_step],
            p_variable,  /* variable to add */
            quantity_plan_t);                                    /* coefficient */
    }
    
    return;
}


vector<int> FormulationMaster::get_dual_sol()
{
    vector<int> dual_sol;
    int number_time_steps = p_instance->get_time_steps_number();
    dual_sol.resize(number_time_steps);
    for(int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++)
    {
        int dual_sol_t( SCIPgetDualsolLinear(p_scip_master, m_complicating_constraints[i_time_step] )); 
        cout << "Dual cost for the demand constraint " << i_time_step << " : " << dual_sol_t << "\n";
        dual_sol[i_time_step] = dual_sol_t;
    }
}



