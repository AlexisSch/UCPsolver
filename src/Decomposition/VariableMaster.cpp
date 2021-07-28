/**
 * @file VariableMaster.cpp
 * 
 * 
*/


//** Includes

//* Standart
#include <vector>
#include <iostream>


//* SCIP
#include <scip/scipdefplugins.h>
#include <scip/scip.h>


//* User

// general
#include "DataClasses/ProductionPlan.h"

// Decomposition
#include "Decomposition/VariableMaster.h"



//** Namespaces
using namespace std;



VariableMaster::VariableMaster( SCIP_VAR* column_variable, ProductionPlan* production_plan )
{
    p_variable = column_variable;
    p_production_plan = production_plan;
}



SCIP_VAR* VariableMaster::get_variable_pointer()
{
    return( p_variable );
}

ProductionPlan* VariableMaster::get_production_plan()
{
    return( p_production_plan );
}











