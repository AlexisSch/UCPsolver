/** 
 * @class Pricer.h
 ** Allows SCIP to make a column generation. Tell what is the pricing problem
 *  todo : get it working
*/

#ifndef ObjPricerUCPUnitDecomposition_H
#define ObjPricerUCPUnitDecomposition_H

/* standart includes */
#include <vector>

/* SCIP includes */

#include "objscip/objpricer.h"
#include "scip/pub_var.h"

/* user includes */
#include "InstanceUCP.h"
#include "FormulationMasterUnitDecomposition.h"

using namespace scip;

class ObjPricerUCPUnitDecomposition : public ObjPricer
{

    public:

    /** Constructs the pricer object with the data needed */
    ObjPricerUCPUnitDecomposition(
        SCIP* scip_master,       /**< SCIP pointer */
        const char* name,               /**< name of pricer */
        FormulationMasterUnitDecomposition* formulation_master,
        InstanceUCP* instance_ucp
    );

    /** Destructs the pricer object. */
    virtual ~ObjPricerUCPUnitDecomposition();

    /** reduced cost pricing method of variable pricer for feasible LPs */
    virtual SCIP_DECL_PRICERREDCOST(scip_redcost);

    /** initialization method of variable pricer (called after problem was transformed) */
    virtual SCIP_DECL_PRICERINIT(scip_init);

    SCIP_RETCODE SCIPpricerUCPActivate();

    /** perform pricing */
    void ucp_pricing(SCIP* scip);


    private:

    FormulationMasterUnitDecomposition* m_formulation_master;
    InstanceUCP* p_instance_ucp;
    std::vector< double > m_list_RMP_opt;

};


#endif