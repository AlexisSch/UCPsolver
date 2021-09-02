/** 
 * @class FormulationPricerMyDecomposition
*/

#ifndef FormulationPricerMyDecomposition_H
#define FormulationPricerMyDecomposition_H

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
#include "Decomposition/FormulationPricer.h"

// MyDecomposition



/* namespace */

class FormulationPricerMyDecomposition : public FormulationPricer
{

    public : 


        //* class related function 

        /**
         * Constructor
         */
        FormulationPricerMyDecomposition(InstanceUCP *instance, 
            SCIP *scip, ... // reduced costs and unit number
        );

        /** Destructor
         * 
         */
        ~FormulationPricerMyDecomposition()
        {};


        //* solving related functions

        /**
         * create the variable
         */
        SCIP_RETCODE create_variables();

        /**
         * create the constraints
         */
        SCIP_RETCODE create_constraints();

        /**
         * create the production plan corresponding to the solution
        */
        ProductionPlan* get_production_plan_from_solution();
        

        //* gets
        
        int get_unit_number();



    private:

        //* variables

        //* contraintes
        
        //* reduced costs

        //* block information
        int m_unit_number;

};

#endif