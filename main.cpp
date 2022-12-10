// OpenMP header
#include <omp.h>
// MPI header
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>


#include "market.h"
#include "player.h"
#include "activity.h"
#include "strategies.h"

#define DEFAULT_NUM_ITERATIONS 10000
#define DEFAULT_DATA_PATH "sim.dat"
#define DEFAULT_ITEM_NAME "Coal"


/*
Struct to contain the command-line arguments given to the program.
*/
struct Parameters {
    int numIterations = DEFAULT_NUM_ITERATIONS;
    // dataPath determines where to save the binary simulation output
    // compiler gives a warning if not cast explicitly
    std::string dataPath = DEFAULT_DATA_PATH;
    std::string itemName = DEFAULT_ITEM_NAME;
};



/*
Initializes the command-line arguments and places them into the Parameters
struct
*/
void initParams(int argc, char* const* argv, Parameters *params) {
    int c;
    while ((c = getopt(argc, argv, "i:o:u:")) != -1) {
        switch (c) {
            case 'i':
                params->numIterations = atoi(optarg);
                break;
            case 'o':
                params->dataPath = optarg;
                break;
            case 'u':
                params->itemName = optarg;
                break;
        }
    }
}

int main(int argc, char* const* argv) {
    srand(123);

    Parameters params = Parameters{};
    initParams(argc, argv, &params);

    int turnDuration = 500;

    Market market = Market();

    std::vector<Agent> agents = {};

    int defaultImpatience = 75;
    int defaultProfitMotive = 20;

    thread_local std::random_device rd;  //Will be used to obtain a seed for the random number engine
    thread_local std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> impatienceDistr(10, 75);
    std::uniform_int_distribution<> profitDistr(20, 60);

    // initialize agents
    for (int i = 0; i < 10; i++) {
        Agent agent = Agent(turnDuration, impatienceDistr(gen), profitDistr(gen));
        agent.activity = Activities::MineRunite();
        agents.push_back(agent);
    }

    for (int i = 0; i < 10; i++) {
        Agent agent = Agent(turnDuration, impatienceDistr(gen), profitDistr(gen));
        agent.activity = Activities::MineCoal();
        agents.push_back(agent);
    }

    for (int i = 0; i < 10; i++) {
        Agent agent = Agent(turnDuration, impatienceDistr(gen), profitDistr(gen));
        agent.activity = Activities::SmithRuniteBar();
        agents.push_back(agent);
    }

    for (int i = 0; i < 10; i++) {
        Agent agent = Agent(turnDuration, impatienceDistr(gen), profitDistr(gen));
        agent.activity = Activities::SmithRunePlatebody();
        agents.push_back(agent);
    }

    for (int i = 0; i < 10; i++) {
        Agent agent = Agent(turnDuration, impatienceDistr(gen), profitDistr(gen));
        agent.activity = Activities::CraftNatureRunes();
        agents.push_back(agent);
    }

    for (int i = 0; i < 20; i++) {
        Agent agent = Agent(turnDuration, impatienceDistr(gen), profitDistr(gen));
        agent.activity = Activities::AlchRunePlatebody();
        agents.push_back(agent);
    }

    // randomize agent trade order
    std::random_shuffle(agents.begin(), agents.end());
    
    std::ofstream wf(params.dataPath, std::ios::out | std::ios::binary);
    if (!wf) {
        std::cout << "Cannot open file!" << std::endl;
        return 1;
    }

    std::pair<int, int> margin;

    // empty shock trade ID
    TradeID shockID = TradeID(false, "null", std::make_pair(0, 0));


    for (int i = 0; i < params.numIterations; i++) {

        // write to a file
        for (Agent &agent : agents) {
            agent.act(market);
        }

        std::random_shuffle(agents.begin(), agents.end());

        std::cout << i << std::endl;
        market.print_market(false);
        std::cout << std::endl;
        std::cout << activity_profit(market, Activities::SmithRuniteBar(), turnDuration) << std::endl;
        std::cout << std::endl;

        margin = market.get_unit_prices(params.itemName);



        // ignore pre-equilibrium pricing
        if (i <= 5000) {
            continue;
        }

        // // model a shock at 7500 iterations
        // if (i == 7500) {
        //     shockID = market.create_sell_trade(8000, "Runite Ore", 1000);
        // } else if (i > 7500) {
        //     Trade shock = market.check_trade(shockID);
        //     if (shock.status == TradeStatus::SUCCESS) {
        //         market.cancel_trade(shockID);
        //     }
        // }
        
        // write to the file
        wf.write(reinterpret_cast<const char*>(&i), sizeof(int32_t));
        wf.write(reinterpret_cast<const char*>(&(margin.first)), sizeof(int32_t));
        wf.write(reinterpret_cast<const char*>(&(margin.second)), sizeof(int32_t));
    }

    wf.close();
    if(!wf.good()) {
      std::cout << "Error occurred at writing time!" << std::endl;
      return 1;
   }


}