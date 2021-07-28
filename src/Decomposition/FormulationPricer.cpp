/** 
 * @file
 * Implements the class FormulationPricer 
 * 
*/

// standart includes

// SCIP includes 
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

// user includes 
#include "InstanceUCP.h"
#include "FormulationPricer.h"

// namespace
using namespace std;




FormulationPricer::FormulationPricer(InstanceUCP *instance, SCIP *scip ): 
    m_scip(scip), m_instance(instance)
{
}


SCIP* FormulationPricer::get_scip_pointer()
{
    return( m_scip );
}

