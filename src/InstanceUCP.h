/** 
 * @class InstanceUCP
 ** An object from this class stores all the data about a UCP problem in one place.
 *  todo : improve the printing to get it with cout ?
*/

#ifndef INSTANCEUCP_H
#define INSTANCEUCP_H

/* standart includes */
#include <vector>

/* SCIP includes */

/* user includes */


class InstanceUCP
{

    public :

    InstanceUCP( char *filename);
    int read_data_from_file(char *filename);
    void print_instance();

    /* gets */

    int get_units_number();
    int get_time_steps_number();
    std::vector<int> get_costs_fixed();
    std::vector<int> get_costs_startup();
    std::vector<int> get_costs_proportionnal();
    std::vector<int> get_production_min();
    std::vector<int> get_production_max();
    std::vector<int> get_demand();
    std::vector<int> get_min_uptime();
    std::vector<int> get_min_downtime();
    std::vector<int> get_initial_state();



    private :

    int m_units_number;
    int m_time_steps_number;
    std::vector<int> m_costs_fixed;
    std::vector<int> m_costs_startup;
    std::vector<int> m_costs_proportionnal;
    std::vector<int> m_demand;
    std::vector<int> m_production_min;
    std::vector<int> m_production_max;
    std::vector<int> m_min_uptime;
    std::vector<int> m_min_downtime;
    std::vector<int> m_initial_state;

};

#endif