// OpenMP header
#include <omp.h>
// MPI header
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
#include <unistd.h>


#include "market.h"
#include "player.h"
#include "activity.h"
#include "strategies.h"

#define DEFAULT_NUM_ITERATIONS 10000
#define DEFAULT_DATA_PATH "sim.dat"


/*
Struct to contain the command-line arguments given to the program.
*/
struct Parameters {
    int numIterations = DEFAULT_NUM_ITERATIONS;
    // dataPath determines where to save the binary simulation output
    // compiler gives a warning if not cast explicitly
    char* dataPath = (char*)DEFAULT_DATA_PATH;
};



/*
Initializes the command-line arguments and places them into the Parameters
struct
*/
void initParams(int argc, char** argv, Parameters *params) {
    int c;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
        switch (c) {
            case 'i':
                params->numIterations = atoi(optarg);
                break;
            case 'o':
                params->dataPath = optarg;
                break;
        }
    }
}

int main(int argc, char** argv) {
    srand(123);

    Parameters params = Parameters{};
    initParams(argc, argv, &params);

    Market market = Market();

    std::vector<Agent> agents = {};

    for (int i = 0; i < 3; i++) {
        Agent agent = Agent();
        agent.activity = Activities::MineRunite();
        agent.priceStrategy = PriceStrategies::Default();
        agents.push_back(agent);
    }

    for (int i = 0; i < 3; i++) {
        Agent agent = Agent();
        agent.activity = Activities::MineCoal();
        agent.priceStrategy = PriceStrategies::Default();
        agents.push_back(agent);
    }

    for (int i = 0; i < 10; i++) {
        Agent agent = Agent();
        agent.activity = Activities::SmithRuniteBar();
        agent.priceStrategy = PriceStrategies::Default();
        agents.push_back(agent);
    }

    for (int i = 0; i < 4; i++) {
        Agent agent = Agent();
        agent.activity = Activities::SmithRunePlatebody();
        agent.priceStrategy = PriceStrategies::Default();
        agents.push_back(agent);
    }

    for (int i = 0; i < 12; i++) {
        Agent agent = Agent();
        agent.activity = Activities::AlchRunePlatebody();
        agent.priceStrategy = PriceStrategies::Default();
        agents.push_back(agent);
    }

    // shuffle the agents
    std::random_shuffle(agents.begin(), agents.end());
    
    std::ofstream wf(params.dataPath, std::ios::out | std::ios::binary);
    if (!wf) {
        std::cout << "Cannot open file!" << std::endl;
        return 1;
    }

    std::pair<int, int> margin;

    for (int i = 0; i < params.numIterations; i++) {
        for (Agent &agent : agents) {
            agent.act(market);
        }
        std::cout << i << std::endl;
        market.print_market(false);
        std::cout << std::endl;

        margin = market.get_unit_prices("Runite Ore");

        if (i <= 0) {
            continue;
        }
        
        wf.write(reinterpret_cast<const char*>(&i), sizeof(int32_t));
        wf.write(reinterpret_cast<const char*>(&(margin.first)), sizeof(int32_t));
        wf.write(reinterpret_cast<const char*>(&(margin.second)), sizeof(int32_t));



        // std::cout << activity_profit(market, Activities::SmithRunePlatebody()) << std::endl;
        // std::cout << activity_profit(market, Activities::AlchRunePlatebody()) << std::endl;
    }

    wf.close();
    if(!wf.good()) {
      std::cout << "Error occurred at writing time!" << std::endl;
      return 1;
   }


}