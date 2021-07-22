/** 
 * @file main
 ** We pilote the solving of a UCP problem from here.
 * todo : complete the classes (need a destructor for each of them !!!) 
 * todo : add a function for data entry and out 
 * todo : add a class for constraints / cvariable ?
 * todo : understand what namespace is, errors catching ? 
 * todo : improve commentaries / documentation. get the same thing everywhere.
 * check pierre's code + code from scip
 * todo : put on github
 * put the same names everywhere. for instance_ucp;..
 * change the class productionplan, it has two use at the same time, circular dependance
 * todo : WHEN U HAVE FINISHED THE FIRST COLUMN GENERATION, CLEAN DIS CODE !!! 
 * create the destructors and the gets, use smard pointers, add commentaries, etc.
*/


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
#include "FormulationPricer.h"
#include "FormulationMaster.h"
#include "ObjPricerUCP.h"
#include "FormulationCompact.h"
#include "FormulationLinearRelaxation.h"
#include "VariableMaster.h"
#include "ProductionPlan.h"

/* namespace */
using namespace std;




/** main function */
int main(
    int argc,
    char** argv
    )
{
    
    /* the user must enter one or two paths when executing. We have to check */

    char* entry_data_filename;
    char* exit_data_filename;
    bool must_write_data_out(false);
    
    // todo like classify.c
    if(argc < 2)
    {
        cerr << "ERROR : no file provided" << endl;    
        return(1);
    }   
    else if (argc == 2)
    {
        entry_data_filename = argv[1];
    } 
    else if (argc == 3)
    {
        entry_data_filename = argv[1];
        exit_data_filename = argv[2];
        must_write_data_out = true;
    }
    else
    {
        cerr << "Warning, too many arguments provided... \nusing first as entry and second as out, rest ignored" << endl;
        entry_data_filename = argv[1];
        exit_data_filename = argv[2];
        must_write_data_out = true;
    }


    try
    {



        // lets get the informations about the instance in the dedicated object
        InstanceUCP *instance = new InstanceUCP( entry_data_filename );
        instance->print_instance();




        /* COMPACT RESOLUTION */


        SCIP* scip_compact(0);
        SCIPcreate( &scip_compact );
        SCIPincludeDefaultPlugins( scip_compact );
        SCIPcreateProb(scip_compact, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip_compact, "display/verblevel", 3);
        
        FormulationCompact *formulation_compact = new FormulationCompact( instance, scip_compact );
        SCIPsolve( scip_compact );
        ProductionPlan production_plan_compact( instance, formulation_compact);
        production_plan_compact.show();


        /* LINEAR RELAXATION */


        SCIP* scip_lr(0);
        SCIPcreate( &scip_lr );
        SCIPincludeDefaultPlugins( scip_lr );
        SCIPcreateProb(scip_lr, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip_lr, "display/verblevel", 3);
        
        FormulationLinearRelaxation *formulation_lr = new FormulationLinearRelaxation( instance, scip_lr );
        SCIPsolve( scip_lr );
        ProductionPlan production_plan_linear_relaxation( instance, formulation_lr);
        production_plan_linear_relaxation.show();



        /* COLUMN GENERATION */

        
        // Create the master problem
        SCIP* scip_master(0);
        SCIPcreate( &scip_master );
        SCIPincludeDefaultPlugins( scip_master );
        SCIPcreateProb(scip_master, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip_master, "display/verblevel", 1);
        FormulationMaster *formulation_master = new FormulationMaster( instance, scip_master );

        // create and add first column, with the worst plan, but working 8)
        ProductionPlan* first_production_plan = new ProductionPlan( instance );
        first_production_plan->computeCost();
        string column_name = "column_0" ; 
        SCIP_VAR* p_variable;
        SCIPcreateVar(  scip_master,
            &p_variable,                            // pointer 
            column_name.c_str(),                            // name
            0.,                                     // lowerbound
            +SCIPinfinity( scip_master),            // upperbound
            first_production_plan->get_cost(),          // coeff in obj function
            SCIP_VARTYPE_CONTINUOUS,
            true, false, NULL, NULL, NULL, NULL, NULL);
        SCIPaddVar(scip_master, p_variable);
        VariableMaster* first_column = new VariableMaster( p_variable, first_production_plan);
        formulation_master->addColumn( first_column );


        // Create and add the pricer

        static const char* PRICER_NAME = "Pricer_UCP";

        ObjPricerUCP *pricer_ucp = new ObjPricerUCP( scip_master, PRICER_NAME, 
            formulation_master, 
            instance
        );
        
        SCIPincludeObjPricer(scip_master, pricer_ucp, true);
        SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME));


        // solving
        SCIPsolve( scip_master );

        SCIPprintBestSol(scip_master, NULL, FALSE) ;

        ProductionPlan* production_plan_column_generation = formulation_master->get_production_plan_from_solution();
        production_plan_column_generation->show();

    }
    catch(const std::exception& e)
    {
        std::cerr << "error : " <<  e.what() << '\n';
    }
    

    cout << "Program ends here. Everything seems to have worked." << endl;
    return 0;
}
