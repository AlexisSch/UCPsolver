
/** 
 * @class ObjPricerUCPMyDecomposition.h
 ** This class is needed by SCIP to make a column generation.
 * an object of the class will have to be created before the 
*/

#ifndef ObjPricerUCPUMyDecomposition_H
#define ObjPricerUCPUMyDecomposition_H

//** Includes

//* Standart
#include <vector>


//* SCIP
#include "objscip/objpricer.h"
#include "scip/pub_var.h"
#include <scip/scip.h>


//* User 

// general
#include "DataClasses/InstanceUCP.h"

// Decomposition
#include "Decomposition/FormulationPricer.h"

// Unit Decomposition 2
#include "UMyDecomposition/FormulationMasterUMyDecomposition.h"
#include "UMyDecomposition/FormulationPricerUMyDecomposition.h"

//** Namespace 
using namespace scip;

class ObjPricerUCPUMyDecomposition : public ObjPricer
{

    public:

    //* class functions

    /**
     * constructor
     */
    ObjPricerUCPUMyDecomposition(
        SCIP* scip_master,                  /**< SCIP pointer */
        const char* name,                   /**< name of pricer */
        FormulationMasterUMyDecomposition* formulation_master,
        InstanceUCP* instance_ucp
    );

    /**
     * destructor
     */
    virtual ~ObjPricerUCPUMyDecomposition();



    //* interface (?) functions recquired by scip

    /** 
     * SCIP calls this function to generate new variables
     * the pricing takes place here.
     *  possible return values for *result:
     *  - SCIP_SUCCESS    : at least one improving variable was found, or it is ensured that no such variable exists
     *  - SCIP_DIDNOTRUN  : the pricing process was aborted by the pricer, there is no guarantee that the current LP solution is optimal
     */
    virtual SCIP_DECL_PRICERREDCOST(scip_redcost);

    /** 
     * SCIP transforms the constraints of the problem during the presolving (?)
     * we need to get the right adresses  of the transformed constraints to get their reduced costs  
    */
    virtual SCIP_DECL_PRICERINIT(scip_init);
    
    
    //* user functions

    /**
     * performs the pricing, adding the variables recquired to the master problem.
     * if no variables are added, the column generation will stop.
     */
    SCIP_RETCODE ucp_pricing(SCIP* scip);


    private:

    FormulationMasterUMyDecomposition* m_formulation_master;
    InstanceUCP* m_instance_ucp;
};


#endif