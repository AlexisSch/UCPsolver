/** 
 * @class VariableMaster
 ** Objects from this class stores the information about a master variable.
*/

#ifndef VARIABLEMASTER_H
#define VARIABLEMASTER_H


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




class VariableMaster
{

    public : 
    
    VariableMaster( SCIP_VAR* column_variable, ProductionPlan* production_plan );
    ~VariableMaster();

    SCIP_VAR* get_variable_pointer();
    ProductionPlan* get_production_plan();


    private :

    ProductionPlan* p_production_plan;
    SCIP_VAR* p_variable;

    char* p_variable_name;



};



#endif