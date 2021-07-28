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



    // Complicating constraints
    int number_time_step( m_instance->get_time_steps_number() );
    m_complicating_constraints.resize( number_time_step );
    vector<int> demand( m_instance->get_demand() );
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




FormulationMasterUnitDecomposition::~FormulationMasterUnitDecomposition()
{
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
    int number_time_step( m_instance->get_time_steps_number() );
    vector<vector<double>> quantity_plan = variable_to_add->get_production_plan()->get_quantity_plan();
    for( int i_time_step = 0; i_time_step < number_time_step; i_time_step++ )
    {
        double quantity_plan_t(0);
        for( int i_unit = 0; i_unit < m_instance->get_units_number(); i_unit++ )
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





