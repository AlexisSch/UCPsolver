/**
 * @file FormulationMaster.cpp
 *  Implements the class FormulationMaster 
 * 
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



//** Namespaces
using namespace std;



FormulationMaster::FormulationMaster( InstanceUCP* instance, SCIP* scip_master)
{
    m_instance_ucp = instance;
    m_scip_master = scip_master;
}



ProductionPlan* FormulationMaster::get_production_plan_from_solution()
{

    int number_columns( m_vector_columns.size());
    int number_of_units( m_instance_ucp->get_units_number());
    int number_of_time_steps( m_instance_ucp->get_time_steps_number());

    SCIP_SOL *solution = SCIPgetBestSol( m_scip_master );


    // We start by creating full zeros plans
    vector< vector < double > > up_down_plan;
    vector< vector < double > > switch_plan;
    vector< vector < double > > quantity_plan;
    up_down_plan.resize(number_of_units);
    switch_plan.resize(number_of_units);
    quantity_plan.resize(number_of_units);
    for(int i_unit = 0; i_unit < number_of_units; i_unit ++)
    {
        up_down_plan[i_unit].resize(number_of_time_steps, 0);
        switch_plan[i_unit].resize(number_of_time_steps, 0);
        quantity_plan[i_unit].resize(number_of_time_steps, 0);
    }

    // for each column, we add its plan value to the base plan, times it's coefficient
    for(int i_column = 0; i_column < number_columns; i_column ++)
    {
        // get the plan with all the corresponding informations, and the coefficient
        VariableMaster* current_variable = m_vector_columns[i_column];
        ProductionPlan* current_plan = current_variable->get_production_plan();
        vector< vector< double > > current_up_down_plan( current_plan->get_up_down_plan() );
        vector< vector< double > > current_switch_plan( current_plan->get_switch_plan() );
        vector< vector< double > > current_quantity_plan( current_plan->get_quantity_plan() );
        SCIP_Real coefficient_column( SCIPgetSolVal( m_scip_master, solution, current_variable->get_variable_pointer()));

        // add the value to the main plan
        for( int i_unit = 0; i_unit < number_of_units; i_unit ++)
        {
            for( int i_time_step = 0; i_time_step < number_of_time_steps; i_time_step++ )
            {
                up_down_plan[i_unit][i_time_step] += current_up_down_plan[i_unit][i_time_step]*coefficient_column;
                switch_plan[i_unit][i_time_step] += current_switch_plan[i_unit][i_time_step]*coefficient_column;
                quantity_plan[i_unit][i_time_step] += current_quantity_plan[i_unit][i_time_step]*coefficient_column; 
            }
        }

    }
 
    // we can now create the plan
    ProductionPlan* production_plan = new ProductionPlan( m_instance_ucp, up_down_plan, switch_plan, quantity_plan );
    production_plan->compute_cost();

    return( production_plan );
}




SCIP* FormulationMaster::get_scip_pointer()
{
    return( m_scip_master );
}


InstanceUCP* FormulationMaster::get_instance()
{
    return( m_instance_ucp );
}


vector< VariableMaster* > FormulationMaster::get_vector_columns()
{
    return( m_vector_columns );
}




