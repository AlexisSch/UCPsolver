/** 
 * @class VariableMaster
 ** Objects from this class stores the information about a master variable.
*/

#ifndef VARIABLEMASTER_H
#define VARIABLEMASTER_H

/* standart includes */
#include <vector>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "ProductionPlan.h"




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