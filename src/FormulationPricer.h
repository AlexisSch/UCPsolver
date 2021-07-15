/** 
 * @class FormulationPricer
 ** An object from this class has the variable and constraints defining a UCP problem.
 *  todo : improve the printing to get it with cout ?
*/

#ifndef FormulationPricer_H
#define FormulationPricer_H


/* standart includes */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"

/* namespace */

class FormulationPricer
{

    public : 

    FormulationPricer(InstanceUCP *instance, SCIP *scip);
    SCIP_RETCODE create_variables();
    SCIP_RETCODE create_constraints();


    SCIP* get_scip_pointer();
    

    std::vector< std::vector< SCIP_VAR* >> get_variable_u();
    std::vector< std::vector< SCIP_VAR* >> get_variable_x();
    std::vector< std::vector< SCIP_VAR* >> get_variable_p();

    std::vector< std::vector< SCIP_CONS* >> m_constraint_min_up_time;

    private:

    SCIP *p_scip;
    InstanceUCP *p_ucp_instance;

    std::vector< std::vector< SCIP_VAR* >> m_variable_u;
    std::vector< std::vector< SCIP_VAR* >> m_variable_x;
    std::vector< std::vector< SCIP_VAR* >> m_variable_p;

    std::vector< SCIP_CONS* > m_constraint_demand;
    std::vector< std::vector< SCIP_CONS* >> m_constraint_production;
    // std::vector< std::vector< SCIP_CONS* >> m_constraint_min_up_time;
    std::vector< std::vector< SCIP_CONS* >> m_constraint_min_down_time;
    std::vector< std::vector< SCIP_CONS* >> m_constraint_startup;
     

};

#endif