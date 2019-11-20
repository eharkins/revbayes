#ifndef BetaSimplexProposal_H
#define BetaSimplexProposal_H

#include <iosfwd>

#include "SimpleProposal.h"
#include "Simplex.h"

namespace RevBayesCore {
class DagNode;
template <class variableType> class StochasticNode;
    
    /**
     * The beta-simplex operator.
     *
     * A beta-simplex proposal randomly changes a single value of a simplex, although the other values 
     * change too because of the renormalization.
     * First, a random index is drawn. Then, draws a random uniform number u ~ beta(a,b)
     * and sets the current vale to u.
     * The simplex is then renormalized.
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2009-09-08, version 1.0
     *
     */
    class BetaSimplexProposal : public SimpleProposal<Simplex> {
        
    public:
        BetaSimplexProposal( StochasticNode<Simplex> *n, double a, double p=0.44);                                                                    //!<  constructor
        
        // Basic utility functions
        void                                    cleanProposal(void);                                                                //!< Clean up proposal
        BetaSimplexProposal*                    clone(void) const;                                                                  //!< Clone object
        double                                  propose(Simplex &v);                                                                //!< Perform proposal
        const std::string&                      getProposalName(void) const;                                                        //!< Get the name of the proposal for summary printing
        double                                  getProposalTuningParameter(void) const;
        void                                    prepareProposal(void);                                                              //!< Prepare the proposal
        void                                    printParameterSummary(std::ostream &o, bool name_only) const;                                       //!< Print the parameter summary
        void                                    setProposalTuningParameter(double tp);
        void                                    tune(double r);                                                                     //!< Tune the proposal to achieve a better acceptance/rejection ratio
        void                                    undoProposal(void);                                                                 //!< Reject the proposal
        
    protected:
        
        void                                    swapNodeInternal(DagNode *oldN, DagNode *newN);                                     //!< Swap the DAG nodes on which the Proposal is working on
        
        
    private:
        // parameters
        
        Simplex                                 storedValue;                                                                        //!< The stored value of the Proposal used for rejections.
        double                                  alpha;                                                                             //!< The scaling parameter of the Proposal
//        double                              proposedValue;                                                                      //!< The value we propose.
        
    };
    
}

#endif

