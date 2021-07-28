/** 
 * @class FormulationPricer
 ** 
*/

#ifndef FormulationPricer_H
#define FormulationPricer_H


/* standart includes */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

/* SCIP includes */
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"

/* namespace */

class FormulationPricer
{

    public : 

        //* constructor
        FormulationPricer(InstanceUCP *instance, SCIP *scip );

        //* virtual destructor
        virtual ~FormulationPricer()
        {}

        //* Pure virtual functions
        virtual SCIP_RETCODE create_variables( std::vector<SCIP_Real> reduced_costs_demand ) = 0;
        virtual SCIP_RETCODE create_constraints() = 0;

        //* gets
        SCIP* get_scip_pointer();
    

    protected:

        SCIP *m_scip;
        InstanceUCP *m_instance;

        //* variables : defined in derived classes

        //* constraints : defined in dervied classes

};

#endif