#include <cmath>
#include <iostream>

#include "DistributionCauchy.h"
#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "ScaleBactrianCauchyProposal.h"
#include "SimpleProposal.h"
#include "StochasticNode.h"

namespace RevBayesCore { class DagNode; }

using namespace RevBayesCore;

/** 
 * Constructor
 *
 * Here we simply allocate and initialize the Proposal object.
 */
ScaleBactrianCauchyProposal::ScaleBactrianCauchyProposal( StochasticNode<double> *n, double l, double p) : SimpleProposal<double>( n, p ),
    storedValue( 0.0 ),
    lambda( l )
{
    
}


/**
 * The cleanProposal function may be called to clean up memory allocations after AbstractMove
 * decides whether to accept, reject, etc. the proposed value.
 *
 */
void ScaleBactrianCauchyProposal::cleanProposal( void )
{
    ; // do nothing
}

/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the proposal. 
 */
ScaleBactrianCauchyProposal* ScaleBactrianCauchyProposal::clone( void ) const 
{
    
    return new ScaleBactrianCauchyProposal( *this );
}


/**
 * Get Proposals' name of object 
 *
 * \return The Proposals' name.
 */
const std::string& ScaleBactrianCauchyProposal::getProposalName( void ) const 
{
    static std::string name = "ScalingBactrianCauchy";
    
    return name;
}


double ScaleBactrianCauchyProposal::getProposalTuningParameter( void ) const
{
    return lambda;
}


/** 
 * Perform the proposal.
 *
 * A scaling Proposal draws a random uniform number u ~ unif (-0.5,0.5)
 * and scales the current vale by a scaling factor
 * sf = exp( lambda * u )
 * where lambda is the tuning parameter of the Proposal to influence the size of the proposals.
 *
 * \return The hastings ratio.
 */
double ScaleBactrianCauchyProposal::propose( double &val )
{
    
    // Get random number generator
    RandomNumberGenerator* rng     = GLOBAL_RNG;
    
    // copy value
    storedValue = val;
    
    double u = rng->uniform01();
//    double delta = RbStatistics::Cauchy::rv(*GLOBAL_RNG) * RbConstants::BACT_SD;
    double delta = RbStatistics::Cauchy::rv(*GLOBAL_RNG) * std::sqrt(1 - 0.98*0.98);
    
    // See Yang and Rodriguez (2013) SI eqns 19 and 20
    // Currently hard-coding m = 0.95
    if ( u < 0.5 ) {
        delta += 0.98;
    } else {
        delta -= 0.98;
    }
    
    double scaling_factor = std::exp( lambda * delta );
    val *= scaling_factor;
    
    // compute the Hastings ratio
//    double lnHastingsratio = log( scaling_factor );
    double lnHastingsratio = lambda * delta;
    
    return lnHastingsratio;
}


/**
 *
 */
void ScaleBactrianCauchyProposal::prepareProposal( void )
{
    
}


/**
 * Print the summary of the Proposal.
 *
 * The summary just contains the current value of the tuning parameter.
 * It is printed to the stream that it passed in.
 *
 * \param[in]     o     The stream to which we print the summary.
 */
void ScaleBactrianCauchyProposal::printParameterSummary(std::ostream &o, bool name_only) const 
{
    
    o << "lambda = ";
    if (name_only == false)
    {
        o << lambda;
    }
    
}


/**
 * Reject the Proposal.
 *
 * Since the Proposal stores the previous value and it is the only place
 * where complex undo operations are known/implement, we need to revert
 * the value of the variable/DAG-node to its original value.
 */
void ScaleBactrianCauchyProposal::undoProposal( void ) 
{
    // swap current value and stored value
    variable->setValue( new double(storedValue) );
    
}


/**
 * Swap the current variable for a new one.
 *
 * \param[in]     oldN     The old variable that needs to be replaced.
 * \param[in]     newN     The new RevVariable.
 */
void ScaleBactrianCauchyProposal::swapNodeInternal(DagNode *oldN, DagNode *newN)
{
    
    variable = static_cast<StochasticNode<double>* >(newN) ;
    
}


void ScaleBactrianCauchyProposal::setProposalTuningParameter(double tp)
{
    lambda = tp;
}


/**
 * Tune the Proposal to accept the desired acceptance ratio.
 *
 * The acceptance ratio for this Proposal should be around 0.44.
 * If it is too large, then we increase the proposal size,
 * and if it is too small, then we decrease the proposal size.
 */
void ScaleBactrianCauchyProposal::tune( double rate ) 
{
    
    double p = this->targetAcceptanceRate;
    if ( rate > p )
    {
        lambda *= (1.0 + ((rate-p)/(1.0 - p)) );
    }
    else
    {
        lambda /= (2.0 - rate/p);
    }
    
    if ( lambda > 1 ) {
        lambda = fmin(10000, lambda);
    } else {
        lambda = fmax(1/10000, lambda);
    }
    
}

