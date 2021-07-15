#ifndef SOLVERUCP_H
#define SOLVERUCP_H




/* standart includes */

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "FormulationMaster.h"



class SolverUCP{

    public :
    SolverUCP( SCIP *scip, FormulationPricer *formulation );
    SCIP_RETCODE solve();
    void showSolution();

    private :

    SCIP* p_scip;
    FormulationPricer* p_formulation;
    // * maybe add some other parameters usefull for solving ?

};

#endif