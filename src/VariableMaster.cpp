
/* Implement the class ProductionPlan */


/* standart includes */
#include <vector>
#include <iostream>


/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "VariableMaster.h"
#include "ProductionPlan.h"

using namespace std; 



VariableMaster::VariableMaster( SCIP_VAR* column_variable, ProductionPlan* production_plan )
{
    p_variable = column_variable;
    p_production_plan = production_plan;
}


VariableMaster::~VariableMaster()
{}


SCIP_VAR* VariableMaster::get_variable_pointer()
{
    return( p_variable );
}

ProductionPlan* VariableMaster::get_production_plan()
{
    return( p_production_plan );
}











