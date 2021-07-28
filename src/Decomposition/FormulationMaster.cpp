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
    m_instance = instance;
    m_scip_master = scip_master;
}




ProductionPlan* FormulationMaster::get_production_plan_from_solution()
{

    int number_columns( m_vector_columns.size());
    
    // create empty plan
    ProductionPlan* production_plan = new ProductionPlan( m_instance );
    production_plan->empty_plan();

    // now add the columns values to the plan
    SCIP_SOL *solution = SCIPgetBestSol( m_scip_master );

    for(int i_column = 0; i_column < number_columns; i_column ++)
    {
        // get the plan
        VariableMaster* current_variable = m_vector_columns[i_column];
        ProductionPlan* current_plan = current_variable->get_production_plan();
        
        // add the value to the new plan
        SCIP_Real coefficient_column( SCIPgetSolVal( m_scip_master, solution, current_variable->get_variable_pointer()));
        production_plan->add_column_values( current_plan, coefficient_column);
        production_plan->computeCost();
    }

    return( production_plan);
}




SCIP* FormulationMaster::get_scip_pointer()
{
    return( m_scip_master );
}


InstanceUCP* FormulationMaster::get_instance()
{
    return( m_instance );
}


vector< VariableMaster* > FormulationMaster::get_vector_columns()
{
    return( m_vector_columns );
}




