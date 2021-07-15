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
#include "SolverUCP.h"
#include "ProductionPlan.h"
#include "VariableMaster.h"
#include "FormulationMaster.h"
#include "ObjPricerUCP.h"

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

        // we get a first productionplan, bad but working
        ProductionPlan production_plan( instance );
        production_plan.computeCost();
        production_plan.show();
        
        // Create the master problem, add the columns 
        SCIP* scip_master(0);
        SCIPcreate( &scip_master );
        SCIPincludeDefaultPlugins( scip_master );
        SCIPcreateProbBasic(scip_master, "Master Problem");
        SCIPsetIntParam(scip_master, "display/verblevel", 3);
        FormulationMaster *formulation_master = new FormulationMaster( instance, scip_master );

        formulation_master->addColumn( &production_plan );

        // Adding the pricer

        static const char* PRICER_NAME = "Pricer_UCP";

        ObjPricerUCP *pricer_ucp = new ObjPricerUCP( scip_master, PRICER_NAME, formulation_master, instance );
        
        SCIPincludeObjPricer(scip_master, pricer_ucp, true);
        SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME));


        // solving
        SCIPsolve( scip_master );


        SCIPprintBestSol(scip_master, NULL, FALSE) ;


    }
    catch(const std::exception& e)
    {
        std::cerr << "error : " <<  e.what() << '\n';
    }
    

    cout << "Program ends here. Everything seems to have worked." << endl;
    return 0;
}
