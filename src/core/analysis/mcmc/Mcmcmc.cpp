#include "Mcmcmc.h"
#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "RlUserInterface.h"
#include "RbConstants.h"
#include "RbException.h"

#include <iostream>
#include <vector>
#include <cmath>

#ifdef RB_MPI
#include <mpi.h>
#endif

using namespace RevBayesCore;

Mcmcmc::Mcmcmc(const Model& m, const RbVector<Move> &mv, const RbVector<Monitor> &mn, std::string sT, size_t nc, size_t si, double dt) : MonteCarloSampler( ),
    numChains(nc),
    scheduleType(sT),
    currentGeneration(0),
    swapInterval(si),
    activeChainIndex( 0 ),
    delta( dt ),
    generation( 0 ),
    numAttemptedSwaps( 0 ),
    numAcceptedSwaps( 0 )
{
    std::cout << "\npid " << pid << " start std ctor\n\n";
    
    // only use a many processes as we have chains
    if (numChains < numProcesses)
    {
        numProcesses = numChains;
    }
    
    // initialize container sizes
    chains.resize(numChains);
    chainsPerProcess.resize(numProcesses);
    chainValues.resize(numChains, 0.0);
    chainHeats.resize(numChains, 0.0);
    processPerChain.resize(numChains);
    
    // assign chains to processors, instantiate Mcmc objects
    baseChain = new Mcmc(m, mv, mn);
    for (size_t i = 0, j = 0; i < numChains; i++, j++)
    {
        // all chains know heat-order and chain-processor schedules
        heatRanks.push_back(i);
        if (j >= numProcesses)
        {
            j = j % numProcesses;
        }
        
        chainsPerProcess[j].push_back(i);
        processPerChain[i] = j;
        
        // add chain to pid's chain vector (smaller memory footprint)
        if (j == pid)
        {
            
            // get chain heat
            double b = computeBeta(delta,i);
            
            // create chains
            Mcmc* oneChain = new Mcmc( *baseChain );
            oneChain->setScheduleType( scheduleType );
            oneChain->setChainActive( i == 0 );
            oneChain->setChainPosteriorHeat( b );
            oneChain->setChainIndex( i );
            oneChain->setActive( i == 0);
            chains[i] = oneChain;
        }
        else
        {
            chains[i] = NULL;
        }
    }

    std::cout << "\npid " << pid << " end std ctor\n\n";
    
}

Mcmcmc::Mcmcmc(const Mcmcmc &m) : MonteCarloSampler(m)
{
    
    delta               = m.delta;
    numChains           = m.numChains;
    numProcesses        = m.numProcesses;
    heatRanks           = m.heatRanks;
    swapInterval        = m.swapInterval;
    activeChainIndex    = m.activeChainIndex;
    scheduleType        = m.scheduleType;
    
    numAttemptedSwaps   = m.numAttemptedSwaps;
    numAcceptedSwaps    = m.numAcceptedSwaps;
    generation          = m.generation;
    
    chainsPerProcess.clear();
    for (size_t i = 0; i < m.chainsPerProcess.size(); i++)
    {
        chainsPerProcess.push_back(m.chainsPerProcess[i]);
    }
    
    chains.clear();
    chains.resize(numChains, NULL);
//    for (size_t i = 0; i < m.chainsPerProcess[pid].size(); i++)
    for (size_t i = 0; i < m.chainsPerProcess[pid].size(); i++)
    {
//        size_t k    = m.chainsPerProcess[pid][i];
        size_t k    = m.chainsPerProcess[pid][i];
        std::cout << "\n" << pid << " copy ctor " << i << " " << chains[k] << "\n\n";
        if (m.chains[k] != NULL) {
            chains[k]   = m.chains[k]->clone();
            std::cout << "\n" << pid << " moves " <<  chains[k]->getMoves().size() << "\n";
            std::cout << "\n" << pid << " monitors " << chains[k]->getMonitors().size() << "\n";
        }
    }
    
    chainValues         = m.chainValues;
    chainHeats          = m.chainHeats;
    processPerChain     = m.processPerChain;
    
    currentGeneration   = m.currentGeneration;
    baseChain           = m.baseChain->clone();
    
}

Mcmcmc::~Mcmcmc(void)
{
    for (size_t i = 0; i < chains.size(); i++)
    {
        if (chains[i] != NULL)
        {
            delete chains[i];
        }
    }
    chains.clear();
    delete baseChain;
}


void Mcmcmc::initialize(void)
{
    
}


double Mcmcmc::computeBeta(double d, size_t idx)
{

    return 1.0 / (1.0+delta*idx);
}


Mcmcmc* Mcmcmc::clone(void) const
{
    return new Mcmcmc(*this);
}


double Mcmcmc::getModelLnProbability( void )
{
    synchronizeValues();
    
    for (size_t i=0; i<chains.size(); i++)
    {
        if ( chainHeats[i] == 1.0 )
        {
            return chainValues[i];
        }
    }
    
    return RbConstants::Double::neginf;
}

std::string Mcmcmc::getStrategyDescription( void ) const
{
    std::string description = "";
    std::stringstream stream;
    stream << "The MCMCMC simulator runs 1 cold chain and " << (numChains-1) << " heated chains.\n";
//    stream << chains[ chainsPerProcess[pid][0] ]->getStrategyDescription();
    stream << chains[ chainsPerProcess[pid][0] ]->getStrategyDescription();
    description = stream.str();
    
    return description;
}


void Mcmcmc::initializeSampler( bool priorOnly )
{
    
    // initialize each chain
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        chains[ chainsPerProcess[pid][i] ]->initializeSampler( priorOnly );
    }
    
}


void Mcmcmc::monitor(unsigned long g)
{
    
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        if (chains[ chainsPerProcess[pid][i] ]->isChainActive() )
        {
            chains[ chainsPerProcess[pid][i] ]->monitor(g);
        }
    }
    
}

void Mcmcmc::nextCycle(bool advanceCycle)
{
    
    // run each chain for this process
    for (size_t j = 0; j < chainsPerProcess[pid].size(); j++)
    {
        // get chain index from job vector
        size_t chainIdx = chainsPerProcess[pid][j];
        
        // advance chain j by a single cycle
        chains[chainIdx]->nextCycle( advanceCycle );
        
    } // loop over chains for this process
    
    if ( advanceCycle == true )
    {
        // advance gen counter
        ++currentGeneration;
    }
    
    if ( currentGeneration % swapInterval == 0 )
    {
#ifdef RB_MPI
        // wait until all chains complete
        MPI::COMM_WORLD.Barrier();
#endif
        
        // perform chain swap
        swapChains();
    }
    
}

void Mcmcmc::printOperatorSummary(void) const
{
    for (size_t i = 0; i < numChains; i++)
    {
        if (chains[i] != NULL)
        {
            chains[i]->printOperatorSummary();
        }
    }
}


void Mcmcmc::reset( void )
{
    
    // reset counters
    numAcceptedSwaps = 0;
    numAttemptedSwaps = 0;
    
//    /* Reset the monitors */
//    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
//    {
//        RbVector<Monitor>& monitors = chains[ chainsPerProcess[pid][i] ]->getMonitors();
//        for (size_t i=0; i<monitors.size(); i++)
//        {
//            monitors[i].reset();
//        }
//    }
    
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        chains[ chainsPerProcess[pid][i] ]->reset();
    }

    
}


/**
 * Set the heat of the likelihood of the current chain.
 * This heat is used in posterior posterior MCMC algorithms to
 * heat the likelihood
 * The heat is passed to the moves for the accept-reject mechanism.
 */
void Mcmcmc::setLikelihoodHeat(double h)
{
    
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        if (chains[ chainsPerProcess[pid][i] ] != NULL)
        {
            chains[ chainsPerProcess[pid][i] ]->setLikelihoodHeat( h );
        }
    }
    
}

void Mcmcmc::setNumberOfProcesses(size_t n)
{
    MonteCarloSampler::setNumberOfProcesses(n);    
    
    std::cout << pid << " Mcmcmc::setNumberOfProcesses() " << n << "\n";
    // @MJL: Note to self. The ctor assumes numProcesses==1, so all chains are assigned to that processor.
    // After cloning all chains across processors, you will then want to thin out the chains as needed.
    // This should behave much like the old Mcmcmc ctor code, except it cannot assume a fresh object state.
    
    // only use a many processes as we have chains
    if (numChains < numProcesses)
    {
        numProcesses = numChains;
    }
        
    // initialize container sizes
    for (size_t i = 0; i < chains.size(); i++)
    {
        if (chains[i] != NULL)
            delete chains[i];
    }

    chains.clear();
    chainsPerProcess.clear();
    chainValues.clear();
    chainHeats.clear();
    processPerChain.clear();
    
    chains.resize(numChains);
    chainsPerProcess.resize(numProcesses);
    chainValues.resize(numChains, 0.0);
    chainHeats.resize(numChains, 0.0);
    processPerChain.resize(numChains);
    
    std::cout << pid << " Mcmcmc::setNumberOfProcesses() rm old chains, resize, etc\n";
    
    // assign chains to processors, instantiate Mcmc objects
    for (size_t i = 0, j = 0; i < numChains; i++, j++)
    {
        // all chains know heat-order and chain-processor schedules
        heatRanks.push_back(i);
        if (j >= numProcesses)
        {
            j = j % numProcesses;
        }
        
        chainsPerProcess[j].push_back(i);
        processPerChain[i] = j;
        
        // add chain to pid's chain vector (smaller memory footprint)
        if (j == pid)
        {
            // get chain heat
            double b = computeBeta(delta, i);
            
            // create chains
            Mcmc* oneChain = new Mcmc( *baseChain );
            oneChain->setScheduleType( scheduleType );
            oneChain->setChainActive( i == 0 );
            oneChain->setChainPosteriorHeat( b );
            oneChain->setChainIndex( i );
            oneChain->setActive( i == 0);
            chains[i] = oneChain;
        }
        else
        {
            chains[i] = NULL;
        }
    }
    
    std::cout << pid << " Mcmcmc::setNumberOfProcesses() done\n";
    
}


void Mcmcmc::setReplicateIndex(size_t index)
{
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        chains[ chainsPerProcess[pid][i] ]->setReplicateIndex(index);
    }
}


void Mcmcmc::setStoneIndex(size_t index)
{
    
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        chains[ chainsPerProcess[pid][i] ]->setStoneIndex(index);
    }
}



void Mcmcmc::startMonitors( void )
{
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        std::cout << "\n" << pid << " Mcmcmc::startMonitors(void) " << i << " " << pid << " " << chainsPerProcess[pid][i] << " " << chains[ chainsPerProcess[pid][i] ] << "\n";
    }
    
    std::cout << "\n" << pid << " Mcmcmc::startMonitors(void) start\n";
    std::cout << "\n" << pid << " Mcmcmc::startMonitors(void) pid " << pid << " - " << activePID << "\n";
    // Monitor
    for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
    {
        std::cout << "\n" << pid << " Mcmcmc::startMonitors(void) inside " << chainsPerProcess[pid][i] << "\n";
        std::cout << "\n" << pid << " Mcmcmc::startMonitors(void) inside " << chains[ chainsPerProcess[pid][i] ] << "\n";
        chains[ chainsPerProcess[pid][i] ]->startMonitors();
    }
    std::cout << "\n" << pid << " Mcmcmc::startMonitors(void) end\n";
    
}


void Mcmcmc::startMonitors(size_t numCycles)
{
    
     std::cout << "\n" << pid << " Mcmcmc::startMonitors(numCycles) start\n";
    // Monitor
    for (size_t i = 0; i < chains.size(); i++)
    {
        
        chains[ chainsPerProcess[pid][i] ]->startMonitors( numCycles );
        
        // monitor chain activeIndex only
        if (chains[ chainsPerProcess[pid][i] ]->isChainActive() )
        {
            chains[ chainsPerProcess[pid][i] ]->monitor(0);
        }
        
    }
    std::cout << "\n" << pid << " Mcmcmc::endMonitors(numCycles) start\n";
    
}


void Mcmcmc::synchronizeValues(void)
{
    std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() start\n";
    
    // synchronize chain values
    double results[numChains];
    for (size_t j = 0; j < numChains; j++)
    {
        results[j] = 0.0;
    }
    for (size_t j = 0; j < chainsPerProcess[pid].size(); j++)
    {
        size_t k = chainsPerProcess[pid][j];
        results[k] = chains[k]->getModelLnProbability();
        std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() " << k << " " << j << " " << results[k] << "\n";
    }
    
    std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() get probs\n";
    std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() collect\n";
    std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() activePID " << activePID << "\n";
    std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() processActive " << processActive << "\n";

#ifdef RB_MPI
    if (activePID != pid)
    {
        std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() Sending!\n";
        MPI::COMM_WORLD.Send(&results, int(numChains), MPI::DOUBLE, activePID, 0);
        std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() Sent!\n";
    }
#endif
    MPI::COMM_WORLD.Barrier();
    
//    if ( processActive == true )
    if (activePID == pid)
    {
#ifdef RB_MPI
        for (size_t i = 0; i < numProcesses; i++)
//            for (size_t i = 0; i < chainsPerProcess[pid].size(); i++)
        {
            // ignore self
            if (pid == i)
            {
                std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() skip -- do not Recv from self\n";
                continue;
            }
            
            double tmp_results[numChains];
            for (size_t j = 0; j < numChains; j++)
            {
                tmp_results[j] = 0.0;
            }
            std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() Receiving from " << i << "...\n";
            MPI::COMM_WORLD.Recv(&tmp_results, int(numChains), MPI::DOUBLE, i, 0);
            std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() Received from " << i << " !\n";
            for (size_t j = 0; j < numChains; j++)
            {
                std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() received values from " << i << " " << j << " " << tmp_results[j] << "\n";
            }
            for (size_t j = 0; j < chainsPerProcess[i].size(); j++)
            {
                size_t k = chainsPerProcess[i][j];
                results[k] = tmp_results[k];
                std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() " << j << " " << k << " " << results[k] << "\n";
            }
        }
#endif
        for (size_t i = 0; i < chainValues.size(); i++)
        {
            chainValues[i] = results[i];
            std::cout << "\n" << pid << " Mcmcmc::chainValues[ " << i << " ] " << chainValues[i] << "\n";
        }
    }
    MPI::COMM_WORLD.Barrier();
    
    std::cout << "\n" << pid << " Mcmcmc::synchronizeValues() end\n";
    
}

void Mcmcmc::synchronizeHeats(void)
{
    
    // synchronize heat values
    double heats[numChains];
    for (size_t j = 0; j < numChains; j++)
    {
        heats[j] = 0.0;
    }
    for (size_t j = 0; j < chainsPerProcess[pid].size(); j++)
    {
        size_t k = chainsPerProcess[pid][j];
        if (chains[k] != NULL)
        {
            heats[k] = chains[k]->getChainPosteriorHeat();
        }
    }
    
    
    std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() collect\n";
    std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() activePID " << activePID << "\n";
    std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() processActive " << processActive << "\n";
#ifdef RB_MPI
    // share the heats accross processes
    if (activePID != pid)
    {
        std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() Sending! " << processActive << "\n";
        MPI::COMM_WORLD.Send(&heats, numChains, MPI::DOUBLE, activePID, 0);
        std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() Sent!\n";
    }
#endif
    MPI::COMM_WORLD.Barrier();
    
    if ( activePID == pid )
    {
#ifdef RB_MPI
        for (size_t i = 0; i < numProcesses; i++)
        {
            if (pid == i)
                continue;
            
            double tmp_heats[numChains];
            for (size_t j = 0; j < numChains; j++)
            {
                tmp_heats[j] = 0.0;
            }
            std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() pre-Recv  " << i << "\n";
            MPI::COMM_WORLD.Recv(&tmp_heats, numChains, MPI::DOUBLE, i, 0);
            std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() post-Recv " << i << "\n";
            for (size_t j = 0; j < chainsPerProcess[i].size(); j++)
            {
                size_t k = chainsPerProcess[i][j];
                std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() get heats " << i << " " << j << " " << k << "\n";
                heats[k] = tmp_heats[k];
            }
        }
#endif
        for (size_t i = 0; i < chainValues.size(); i++)
        {
            chainHeats[i] = heats[i];
            std::cout << pid << " Mcmcmc::chainHeats[ " << i << " ] " << chainHeats[i] << "\n";
        }
    }
    MPI::COMM_WORLD.Barrier();
    std::cout << "\n" << pid << " Mcmcmc::synchronizeHeats() end\n";
}


// MJL: allow swapChains to take a swap function -- e.g. pairwise swap for 1..n-1
void Mcmcmc::swapChains(void)
{
    size_t numChains = chains.size();
    
    // exit if there is only one chain
    if (numChains < 2)
    {
        return;
    }
    MPI::COMM_WORLD.Barrier();
    
    // send all chain values to pid 0
    synchronizeValues();
    std::cout << pid << " Mcmcmc::swapChains synchronizeValues\n";
    
#ifdef RB_MPI
    // wait until all chains complete
    MPI::COMM_WORLD.Barrier();
#endif
    
    // send all chain heats to pid 0
    synchronizeHeats();
    std::cout << pid << " Mcmcmc::swapChains synchronizeHeats\n";
    
#ifdef RB_MPI
    // wait until all chains complete
    MPI::COMM_WORLD.Barrier();
#endif
    // swap chains
//    swapNeighborChains();
    swapRandomChains();
    std::cout << pid << " Mcmcmc::swapChains swapRandomChains\n";
    
#ifdef RB_MPI
    // wait until all chains complete
    MPI::COMM_WORLD.Barrier();
#endif

}


void Mcmcmc::swapNeighborChains(void)
{
    
    size_t numAccepted = 0;
    double lnProposalRatio = 0.0;
    
    //for (size_t i = 1; i < numChains; i++)
    for (size_t i = numChains-1; i > 0; i--)
    {
        // swap?
        bool accept = false;
        // swap adjacent chains
        size_t j = 0;
        size_t k = 0;
        
        if (processActive == true)
        {
            ++numAttemptedSwaps;
            
            j = heatRanks[i-1];
            k = heatRanks[i];
            
            // compute exchange ratio
            double bj = chainHeats[j];
            double bk = chainHeats[k];
            double lnPj = chainValues[j];
            double lnPk = chainValues[k];
            double lnR = bj * (lnPk - lnPj) + bk * (lnPj - lnPk) + lnProposalRatio;
            
            // determine whether we accept or reject the chain swap
            double u = GLOBAL_RNG->uniform01();
            if (lnR >= 0)
            {
                accept = true;
            }
            else if (lnR < -100)
            {
                accept = false;
            }
            else if (u < exp(lnR))
            {
                accept = true;
            }
            else
            {
                accept = false;
            }
            
            if (accept == true)
            {
                numAccepted++;
            }
            
            // on accept, swap beta values and active chains
            if (accept)
            {
                
                //size_t tmpIdx = j;
                heatRanks[i-1] = k;
                heatRanks[i] = j;
                
                // swap active chain
                if (activeChainIndex == j)
                {
                    activeChainIndex = k;
                }
                else if (activeChainIndex == k)
                {
                    activeChainIndex = j;
                }
                
            }
        }
        
        if (accept)
        {
            updateChainState(j);
            updateChainState(k);
            
            ++numAcceptedSwaps;
        }
    }
    
}


void Mcmcmc::swapRandomChains(void)
{
    
    double lnProposalRatio = 0.0;
    
    // randomly pick the indices of two chains
    int j = 0;
    int k = 0;
    
    // swap?
    bool accept = false;
    
    if ( pid == activePID )
    {
        j = int(GLOBAL_RNG->uniform01() * numChains);
        if (numChains > 1)
        {
            do {
                k = int(GLOBAL_RNG->uniform01() * numChains);
            }
            while(j == k);
        }
        
        ++numAttemptedSwaps;
            
        // compute exchange ratio
        double bj = chainHeats[j];
        double bk = chainHeats[k];
        double lnPj = chainValues[j];
        double lnPk = chainValues[k];
        double lnR = bj * (lnPk - lnPj) + bk * (lnPj - lnPk) + lnProposalRatio;
            
        // determine whether we accept or reject the chain swap
        double u = GLOBAL_RNG->uniform01();
        if (lnR >= 0)
        {
            accept = true;
        }
        else if (lnR < -100)
        {
            accept = false;
        }
        else if (u < exp(lnR))
        {
            accept = true;
        }
        else
        {
            accept = false;
        }
        
        
        // on accept, swap beta values and active chains
        if (accept == true )
        {
            
            std::cout << pid << " swap chains " << j << " " << k << "\n";
            // swap active chain
            if (activeChainIndex == j)
            {
                activeChainIndex = k;
            }
            else if (activeChainIndex == k)
            {
                activeChainIndex = j;
            }
            
            chainHeats[j] = bk;
            chainHeats[k] = bj;
            size_t tmp = heatRanks[j];
            heatRanks[j] = heatRanks[k];
            heatRanks[k] = tmp;
            
            ++numAcceptedSwaps;
        }
        
        
    }

#ifdef RB_MPI
    std::cout << pid << " pre-Bcast-j " << activePID << " " << j << " " << k << " " << "\n";
    MPI::COMM_WORLD.Bcast(&j, 1, MPI_INT, activePID);
    MPI::COMM_WORLD.Barrier();
    std::cout << pid << " pre-Bcast-k " << activePID << " " << j << " " << k << " " << "\n";
    MPI::COMM_WORLD.Bcast(&k, 1, MPI_INT, activePID);
    MPI::COMM_WORLD.Barrier();
#endif
    
    
    // update the chains accross processes
    // this is necessary because only process 0 does the swap
    // all the other processes need to be told that there was a swap
    updateChainState(j);
    updateChainState(k);
    
}


void Mcmcmc::tune( void )
{
    
    double rate = numAcceptedSwaps / double(numAttemptedSwaps);
    
    if ( rate > 0.44 )
    {
        delta *= (1.0 + ((rate-0.44)/0.56) );
    }
    else
    {
        delta /= (2.0 - rate/0.44 );
    }
    
}


void Mcmcmc::updateChainState(size_t j)
{
    
    std::cout << pid << " Mcmcmc::updateChainState start\n";
    
#ifdef RB_MPI
    // update heat
    if (pid == activePID)
    {
        MPI::COMM_WORLD.Send(&chainHeats[j], 1, MPI::DOUBLE, (int)processPerChain[j], 0);
    }
    if (pid == processPerChain[j])
    {
        MPI::COMM_WORLD.Recv(&chainHeats[j], 1, MPI::DOUBLE, (int)activePID, 0);
    }
    MPI::COMM_WORLD.Barrier();
#endif
    
    if (pid == processPerChain[j])
    {
        chains[j]->setChainPosteriorHeat(chainHeats[j]);
    }
    // update active state
    bool tf = activeChainIndex == j;
    
#ifdef RB_MPI
    if (pid == activePID)
    {
        MPI::COMM_WORLD.Send(&tf, 1, MPI::BOOL, (int)processPerChain[j], 0);
    }
    if (pid == processPerChain[j])
    {
        MPI::COMM_WORLD.Recv(&tf, 1, MPI::BOOL, (int)activePID, 0);
    }
    MPI::COMM_WORLD.Barrier();
#endif
    
    if (pid == processPerChain[j])
    {
        chains[j]->setChainActive(tf);
    }
    std::cout << pid << " Mcmcmc::updateChainState end\n";
    
}

