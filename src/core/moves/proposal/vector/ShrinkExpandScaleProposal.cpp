#include "ShrinkExpandScaleProposal.h"

#include <cmath>

#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "StochasticNode.h"

namespace RevBayesCore { class DagNode; }

using namespace RevBayesCore;

/**
 * Constructor
 *
 * Here we simply allocate and initialize the Proposal object.
 */
ShrinkExpandScaleProposal::ShrinkExpandScaleProposal( std::vector< StochasticNode<double> *> n, StochasticNode<double> *s, double l) : Proposal(),
    variables( n ),
    sd( s ),
    lambda( l ),
    length( variables.size() ),
    stored_scaling_factor( 1.0 )
{
    // tell the base class to add the node
    for (std::vector< StochasticNode<double> *>::const_iterator it = variables.begin(); it != variables.end(); it++)
    {
        addNode( *it );
    }
    addNode( sd );
    
}


/**
 * The cleanProposal function may be called to clean up memory allocations after AbstractMove
 * decides whether to accept, reject, etc. the proposed value.
 *
 */
void ShrinkExpandScaleProposal::cleanProposal( void )
{
    
}

/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the proposal.
 */
ShrinkExpandScaleProposal* ShrinkExpandScaleProposal::clone( void ) const
{
    
    return new ShrinkExpandScaleProposal( *this );
}


/**
 * Get Proposals' name of object
 *
 * \return The Proposals' name.
 */
const std::string& ShrinkExpandScaleProposal::getProposalName( void ) const
{
    static std::string name = "ShrinkExpand";
    
    return name;
}


double ShrinkExpandScaleProposal::getProposalTuningParameter( void ) const
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
double ShrinkExpandScaleProposal::doProposal( void )
{
    
    // Get random number generator
    RandomNumberGenerator* rng     = GLOBAL_RNG;
    
    // Generate new value (no reflection, so we simply abort later if we propose value here outside of support)
    double u = rng->uniform01();
    double scaling_factor = std::exp( lambda * ( u - 0.5 ) );
    
    
    // compute the mean of the vector
    double mean = 0.0;
    for (size_t index=0; index<length; ++index)
    {
        mean += variables[index]->getValue();
    }
    mean /= length;
    
    // store the mean so that we do not need to recompute it
    stored_mean = mean;
    
    // copy value
    stored_scaling_factor = scaling_factor;
    
    for (size_t index=0; index<length; ++index)
    {
        double value_2_mean_ratio = variables[index]->getValue() - mean;
        variables[index]->getValue() = exp(scaling_factor * log(value_2_mean_ratio)) + mean;
    }
    
    // compute the Hastings ratio
    double lnHastingsratio = (length-1) * log( scaling_factor );
    
    // also scale the standard deviation
    if ( sd != NULL )
    {
        sd->getValue() *= scaling_factor;
        lnHastingsratio += log( scaling_factor );
    }
    
    return lnHastingsratio;
}


/**
 * Prepare the proposal, e.g., pick the element that we want to change.
 * Here we do not need to do any preparation.
 */
void ShrinkExpandScaleProposal::prepareProposal( void )
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
void ShrinkExpandScaleProposal::printParameterSummary(std::ostream &o, bool name_only) const
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
void ShrinkExpandScaleProposal::undoProposal( void )
{
    // compute the mean of the vector
    double mean = stored_mean;
    
    for (size_t index=0; index<length; ++index)
    {
        double value_2_mean_ratio = variables[index]->getValue() / mean;
        variables[index]->getValue() = exp(log(value_2_mean_ratio) / stored_scaling_factor) + mean;
    }
    
    // also scale the standard deviation
    if ( sd != NULL )
    {
        sd->getValue() /= stored_scaling_factor;
    }
    
}


/**
 * Swap the current variable for a new one.
 *
 * \param[in]     oldN     The old variable that needs to be replaced.
 * \param[in]     newN     The new RevVariable.
 */
void ShrinkExpandScaleProposal::swapNodeInternal(DagNode *oldN, DagNode *newN)
{
    
    for (size_t i = 0; i < variables.size(); ++i)
    {
        if ( variables[i] == oldN )
        {
            variables[i] = static_cast<StochasticNode<double> *>(newN);
        }
    }
    
    // also swap the sd parameter
    if ( sd != NULL && sd == oldN )
    {
        sd = static_cast<StochasticNode<double> *>(newN);
    }
    
}


void ShrinkExpandScaleProposal::setProposalTuningParameter(double tp)
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
void ShrinkExpandScaleProposal::tune( double rate )
{
    
    if ( rate > 0.23 )
    {
        lambda *= (1.0 + ((rate-0.23)/0.77) );
    }
    else
    {
        lambda /= (2.0 - rate/0.23 );
    }
    
}

