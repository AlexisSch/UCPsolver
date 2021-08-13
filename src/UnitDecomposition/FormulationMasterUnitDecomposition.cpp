/**
 * 
 *  Implements the class FormulationMasterUnitDecompositionUnitDecomposition 
*/





//** Includes

//* Standart
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


//* SCIP
#include <scip/scipdefplugins.h>
#include <scip/scip.h>


//* User

// general
#include "DataClasses/InstanceUCP.h"
#include "DataClasses/ProductionPlan.h"

// Decomposition
#include "Decomposition/FormulationMaster.h"
#include "Decomposition/VariableMaster.h"

// Unit Decomposition
#include "UnitDecomposition/FormulationMasterUnitDecomposition.h"

//** Relaxation
using namespace std;




FormulationMasterUnitDecomposition::FormulationMasterUnitDecomposition( InstanceUCP* instance, SCIP* scip_master):
    FormulationMaster(instance, scip_master)
{

    create_constraints();

    create_and_add_first_columns();

}




FormulationMasterUnitDecomposition::~FormulationMasterUnitDecomposition()
{
}



void FormulationMasterUnitDecomposition::create_constraints()
{
    // creating the constraints
    ostringstream current_cons_name;

    // convexity constraint
    m_convexity_constraint = new SCIP_CONS;

    current_cons_name.str("");
    current_cons_name << "convexity_constraint";
    SCIPcreateConsLinear( m_scip_master, &m_convexity_constraint, current_cons_name.str().c_str(), 0, NULL, NULL,
			    1.0,   // lhs 
			    1.0,   // rhs 
			    true,  /* initial */
			    false, /* separate */
			    true,  /* enforce */
			    true,  /* check */
			    true,  /* propagate */
			    false, /* local */
			    true,  /* modifiable */
			    false, /* dynamic */
			    false, /* removable */
			    false  /* stickingatnode */ );
    SCIPaddCons( m_scip_master, m_convexity_constraint );


    // Demand constraints
    int number_time_step( m_instance_ucp->get_time_steps_number() );
    m_complicating_constraints.resize( number_time_step );
    vector<int> demand( m_instance_ucp->get_demand() );
    for( int i_time_step = 0; i_time_step < number_time_step; i_time_step ++)
    {
        SCIP_CONS* demand_constraint_t;

        current_cons_name.str("");
        current_cons_name << "demand_constraint_" << i_time_step;
        SCIPcreateConsLinear( m_scip_master, &demand_constraint_t, current_cons_name.str().c_str(), 0, NULL, NULL,
			    demand[i_time_step],   // lhs 
			    + SCIPinfinity( m_scip_master ),   // rhs 
			    true,  /* initial */
			    false, /* separate */
			    true,  /* enforce */
			    true,  /* check */
			    true,  /* propagate */
			    false, /* local */
			    true,  /* modifiable */
			    false, /* dynamic */
			    false, /* removable */
			    false  /* stickingatnode */ );
        SCIPaddCons( m_scip_master, demand_constraint_t );
        m_complicating_constraints[i_time_step] = demand_constraint_t;
    }

}



void FormulationMasterUnitDecomposition::create_and_add_first_columns()
{
    int number_of_units( m_instance_ucp->get_units_number() );
    int number_of_time_steps( m_instance_ucp->get_time_steps_number() );
    vector<int> initial_state( m_instance_ucp->get_initial_state());
    vector<int> maximum_production( m_instance_ucp->get_production_max());

    // Create an plan with every units working at every time steps
    vector< vector < double > > up_down_plan;
    vector< vector < double > > switch_plan;
    vector< vector < double > > quantity_plan;
    up_down_plan.resize(number_of_units);
    switch_plan.resize(number_of_units);
    quantity_plan.resize(number_of_units);

    for( int i_unit = 0; i_unit < number_of_units; i_unit ++)
    {
        up_down_plan[i_unit].resize(number_of_time_steps);
        quantity_plan[i_unit].resize(number_of_time_steps);

        for( int i_time_step = 0; i_time_step < number_of_time_steps; i_time_step++)
        {
            up_down_plan[i_unit][i_time_step] = 1.;
            quantity_plan[i_unit][i_time_step] = maximum_production[i_unit];
        }   

        switch_plan[i_unit].resize(number_of_time_steps, 0);        
        if( initial_state[i_unit] == 0 )
        {
            switch_plan[i_unit][0] = 1;
        }

    }
    ProductionPlan* new_plan = new ProductionPlan( m_instance_ucp, up_down_plan, switch_plan, quantity_plan );
    new_plan->compute_cost();

    // create and add the column
    string column_name = "column_0" ; 
    SCIP_VAR* p_variable;
    SCIPcreateVar(  m_scip_master,
        &p_variable,                            // pointer 
        column_name.c_str(),                            // name
        0.,                                     // lowerbound
        +SCIPinfinity( m_scip_master),            // upperbound
        new_plan->get_cost(),          // coeff in obj function
        SCIP_VARTYPE_CONTINUOUS,
        true, false, NULL, NULL, NULL, NULL, NULL);
    SCIPaddVar(m_scip_master, p_variable);
    VariableMaster* first_column = new VariableMaster( p_variable, new_plan);
    add_column( first_column );

}



void FormulationMasterUnitDecomposition::add_column( VariableMaster* variable_to_add )
{
    
    SCIP_VAR* p_variable = variable_to_add->get_variable_pointer();

    // add column to convexity constraint
    SCIPaddCoefLinear(m_scip_master,
                m_convexity_constraint,
                p_variable,  /* variable to add */
                1.);         /* coefficient */        
    
    // add column to demand constraints
    int number_time_step( m_instance_ucp->get_time_steps_number() );
    vector<vector<double>> quantity_plan = variable_to_add->get_production_plan()->get_quantity_plan();
    for( int i_time_step = 0; i_time_step < number_time_step; i_time_step++ )
    {
        double quantity_plan_t(0);
        for( int i_unit = 0; i_unit < m_instance_ucp->get_units_number(); i_unit++ )
        {
            quantity_plan_t += quantity_plan[i_unit][i_time_step];
        }
        SCIPaddCoefLinear(m_scip_master,
            m_complicating_constraints[i_time_step],
            p_variable,  /* variable to add */
            quantity_plan_t);                                    /* coefficient */
    }

    m_vector_columns.push_back( variable_to_add );


    return;
}




SCIP_CONS** FormulationMasterUnitDecomposition::get_convexity_constraint()
{
    return( &m_convexity_constraint );
}


SCIP_CONS** FormulationMasterUnitDecomposition::get_complicating_constraints(int i_time_step)
{
    return( &m_complicating_constraints[i_time_step] );
}





