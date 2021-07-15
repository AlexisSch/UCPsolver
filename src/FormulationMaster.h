/** 
 * @class FormulationMaster
 ** An object from this class has the variable and constraints defining a Dantzig Wolfe reformulation.
 *  todo : improve the printing to get it with cout ?
*/

#ifndef FormulationMaster_H
#define FormulationMaster_H


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
#include "VariableMaster.h"

/* namespace */



class FormulationMaster
{

    public:
    
    FormulationMaster( InstanceUCP* instance, SCIP* scip_master);
    ~FormulationMaster();

    void addColumn( ProductionPlan* production_plan );

    SCIP* get_scip_pointer();

    std::vector<int> get_dual_sol();

    SCIP_CONS* m_convexity_constraint;
    std::vector< SCIP_CONS* > m_complicating_constraints;
    
    private:
    
    InstanceUCP *p_instance;
    SCIP *p_scip_master;
    std::vector< VariableMaster* > m_vector_columns;

    // SCIP_CONS* m_convexity_constraint;
    // std::vector< SCIP_CONS* > m_complicating_constraints;


};



#endif