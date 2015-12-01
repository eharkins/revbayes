#include "WattersonThetaFunction.h"
#include "RbException.h"
#include "RbMathCombinatorialFunctions.h"

using namespace RevBayesCore;

WattersonThetaFunction::WattersonThetaFunction(const TypedDagNode<AbstractHomologousDiscreteCharacterData> *a, bool ps) : TypedFunction<double>( new double(0.0) ),
    alignment( a ),
    perSite( ps )
{
    // add the lambda parameter as a parent
    addParameter( alignment );
    
    update();
}


WattersonThetaFunction::~WattersonThetaFunction( void )
{
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
}



WattersonThetaFunction* WattersonThetaFunction::clone( void ) const
{
    
    return new WattersonThetaFunction( *this );
}


void WattersonThetaFunction::update( void )
{
    int nss = int( alignment->getValue().getNumberOfSegregatingSites() );
    size_t nTaxa = alignment->getValue().getNumberOfTaxa();
    *value = nss / RbMath::harmonicNumber(nTaxa-1);
    
    if ( perSite == true )
    {
        int nSites = int( alignment->getValue().getNumberOfCharacters() );
        *value /= nSites;
    }
    
}



void WattersonThetaFunction::swapParameterInternal(const DagNode *oldP, const DagNode *newP)
{
    
    if (oldP == alignment)
    {
        alignment = static_cast<const TypedDagNode< AbstractHomologousDiscreteCharacterData >* >( newP );
    }
    
}


