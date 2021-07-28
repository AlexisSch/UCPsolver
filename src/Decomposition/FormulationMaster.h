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
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"
#include "VariableMaster.h"

/* namespace */



class FormulationMaster
{

    public:
    
        /* constructor */
        FormulationMaster( InstanceUCP* instance, SCIP* scip_master);

        /* destructor */
        virtual ~FormulationMaster()
        {}

        /* goes over the columns selected to get the current best plan */
        ProductionPlan* get_production_plan_from_solution();

        /* adds a column to the restricted master problem . Pure virtual function */
        virtual void add_column( VariableMaster* variable_to_add ) = 0;



        /* gets */

        SCIP* get_scip_pointer();
        InstanceUCP* get_instance();

        std::vector< VariableMaster* > get_vector_columns();



    protected:


        InstanceUCP *m_instance;
        SCIP *m_scip_master;

        //* variables
        std::vector< VariableMaster* > m_vector_columns;

        //* constraints : defined in derived class

};



#endif