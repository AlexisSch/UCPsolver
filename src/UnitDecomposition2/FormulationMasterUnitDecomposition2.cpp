/**
 * 
 *  Implements the class FormulationMasterUnitDecomposition2UnitDecomposition2 
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
#include "UnitDecomposition2/FormulationMasterUnitDecomposition2.h"

//** Relaxation
using namespace std;




FormulationMasterUnitDecomposition2::FormulationMasterUnitDecomposition2( InstanceUCP* instance, SCIP* scip_master):
    FormulationMaster(instance, scip_master)
{
    //* we create the constraints
    create_constraints();
    //* we must add the first columns (one per block) for the solving to start
    create_and_add_first_columns();
}



FormulationMasterUnitDecomposition2::~FormulationMasterUnitDecomposition2()
{
}



void FormulationMasterUnitDecomposition2::add_column( VariableMaster* variable_to_add )
{
    SCIP_VAR* p_variable = variable_to_add->get_variable_pointer();
    int block_number( variable_to_add->get_block_number() );
    // add column to convexity constraint
    SCIPaddCoefLinear(m_scip_master,
                m_convexity_constraint[block_number],
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



void FormulationMasterUnitDecomposition2::create_constraints()
{
    // creating the constraints
    ostringstream current_cons_name;

    // convexity constraints
    int number_of_units( m_instance_ucp->get_units_number() );
    m_convexity_constraint.resize( number_of_units );

    for( int i_unit = 0; i_unit < number_of_units; i_unit ++)
    {
        SCIP_CONS* convexity_constraint_i;

        current_cons_name.str("");
        current_cons_name << "convexity_constraint_" << i_unit;
        SCIPcreateConsLinear( m_scip_master, &convexity_constraint_i, current_cons_name.str().c_str(), 0, NULL, NULL,
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
        SCIPaddCons( m_scip_master, convexity_constraint_i );
        m_convexity_constraint[i_unit] = convexity_constraint_i;
    }


    // Complicating constraints
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



void FormulationMasterUnitDecomposition2::create_and_add_first_columns()
{

    int number_of_units( m_instance_ucp->get_units_number());
    int number_of_time_steps( m_instance_ucp->get_time_steps_number());
    vector<int> maximum_production( m_instance_ucp->get_production_max() );
    vector<int> initial_state( m_instance_ucp->get_initial_state() );
    ostringstream current_column_name;

    //* for each unit, we create a master variable where the unit is working at max production for each time step

    for(int i_unit = 0; i_unit < number_of_units; i_unit++)
    {
        // we start by creating the correct plan

        vector< vector < double > > up_down_plan;
        vector< vector < double > > switch_plan;
        vector< vector < double > > quantity_plan;

        up_down_plan.resize(number_of_units);
        switch_plan.resize(number_of_units);
        quantity_plan.resize(number_of_units);

        for( int i_unit_2 = 0; i_unit_2 < number_of_units; i_unit_2++ )
        {
            if( i_unit_2 == i_unit )
            {
                up_down_plan[i_unit_2].resize(number_of_time_steps, 1.);
                switch_plan[i_unit_2].resize(number_of_time_steps, 0.);
                quantity_plan[i_unit_2].resize(number_of_time_steps, maximum_production[i_unit_2]);
                
                if( initial_state[i_unit_2] == 0)
                {
                    switch_plan[i_unit_2][0] = 1;
                }
            }
            else
            {
                up_down_plan[i_unit_2].resize(number_of_time_steps, 0.);
                switch_plan[i_unit_2].resize(number_of_time_steps, 0.);
                quantity_plan[i_unit_2].resize(number_of_time_steps, 0.);
            }
        }


        // then we create the production plan and the master variable

        ProductionPlan* production_plan_block = new ProductionPlan( m_instance_ucp, up_down_plan, switch_plan, quantity_plan );
        current_column_name.str("");
        current_column_name << "column_" << m_vector_columns.size();
        SCIP_VAR* scip_variable;
        SCIPcreateVar(  m_scip_master,
            &scip_variable,                            // pointer 
            current_column_name.str().c_str(),                            // name
            0.,                                     // lowerbound
            +SCIPinfinity( m_scip_master ),            // upperbound
            production_plan_block->get_cost(),          // coeff in obj function
            SCIP_VARTYPE_CONTINUOUS,
            true, false, NULL, NULL, NULL, NULL, NULL);
        SCIPaddVar(m_scip_master, scip_variable);
        VariableMaster* new_column = new VariableMaster( scip_variable, production_plan_block);
        new_column->add_block_number( i_unit );
        add_column( new_column );

    }

}




//* gets

SCIP_CONS** FormulationMasterUnitDecomposition2::get_convexity_constraint(int number_unit)
{
    return( &m_convexity_constraint[number_unit] );
}


SCIP_CONS** FormulationMasterUnitDecomposition2::get_complicating_constraints(int number_time_step)
{
    return( &m_complicating_constraints[number_time_step] );
}





