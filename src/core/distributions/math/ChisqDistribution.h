#ifndef ChisqDistribution_H
#define ChisqDistribution_H

#include "ContinuousDistribution.h"

namespace RevBayesCore {
class DagNode;
template <class valueType> class TypedDagNode;
    
    /**
     * @brief Chi-square distribution class.
     *
     * The Chi-square distribution represents a family of distributions defined
     * on the positive real number. The Chi-squared distribution has 1 parameters:
     * @param df the degrees of freedom
     * Instances of this class can be associated to stochastic variables.
     */

    class ChisqDistribution : public ContinuousDistribution {
        
    public:
        ChisqDistribution(const TypedDagNode<long> *df);
        virtual                                            ~ChisqDistribution(void);                                             //!< Virtual destructor
        
        // public member functions
        double                                              cdf(void) const;                                                            //!< Cummulative density function
        ChisqDistribution*                                  clone(void) const;                                                          //!< Create an independent clone
        double                                              computeLnProbability(void);
        double                                              getMax(void) const;
        double                                              getMin(void) const;
        double                                              quantile(double p) const;                                                   //!< Qu
        void                                                redrawValue(void);
        
    protected:
        // Parameter management functions
        void                                                swapParameterInternal(const DagNode *oldP, const DagNode *newP);        //!< Swap a parameter
        
    private:
        
        // members
        const TypedDagNode<long>*                            degrees;
    };
    
}

#endif
