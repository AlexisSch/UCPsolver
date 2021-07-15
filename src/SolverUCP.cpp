

/* standart includes */

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "FormulationPricer.h"
#include "SolverUCP.h"

/* namespace */
using namespace std;


/** constructor */
SolverUCP::SolverUCP( SCIP *scip, FormulationPricer *formulation ) : p_scip(0), p_formulation(0)
{
    p_formulation = formulation;
    p_scip = scip;
     
}

/** solves the problem */
SCIP_RETCODE SolverUCP::solve()
{
    SCIP_CALL( SCIPsolve( p_scip )) ;

    return( SCIP_OKAY );
}


/** print the solution out */
void SolverUCP::showSolution()
{
    SCIPprintBestSol(p_scip, NULL, FALSE) ;
}