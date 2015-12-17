#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "ContinuousStochasticNode.h"
#include "Dist_UniformNatural.h"
#include "Real.h"
#include "RealPos.h"
#include "UniformIntegerDistribution.h"

using namespace RevLanguage;

Dist_UniformNatural::Dist_UniformNatural() : TypedDistribution<Natural>()
{
    
}


Dist_UniformNatural::~Dist_UniformNatural()
{
    
}



Dist_UniformNatural* Dist_UniformNatural::clone( void ) const
{
    return new Dist_UniformNatural(*this);
}


RevBayesCore::UniformIntegerDistribution* Dist_UniformNatural::createDistribution( void ) const
{
    // get the parameters
    RevBayesCore::TypedDagNode<int>* l   = static_cast<const Natural &>( lower->getRevObject() ).getDagNode();
    RevBayesCore::TypedDagNode<int>* u   = static_cast<const Natural &>( upper->getRevObject() ).getDagNode();
    RevBayesCore::UniformIntegerDistribution* d    = new RevBayesCore::UniformIntegerDistribution(l, u);
    
    return d;
}



/* Get Rev type of object */
const std::string& Dist_UniformNatural::getClassType(void)
{
    
    static std::string revType = "Dist_UniformNatural";
    
    return revType;
}

/* Get class type spec describing type of object */
const TypeSpec& Dist_UniformNatural::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( TypedDistribution<Natural>::getClassTypeSpec() ) );
    
    return revTypeSpec;
}


/**
 * Get the alternative Rev names (aliases) for the constructor function.
 *
 * \return Rev aliases of constructor function.
 */
std::vector<std::string> Dist_UniformNatural::getDistributionFunctionAliases( void ) const
{
    // create alternative constructor function names variable that is the same for all instance of this class
    std::vector<std::string> a_names;
    a_names.push_back( "unifNat" );
    
    return a_names;
}


/**
 * Get the Rev name for the distribution.
 * This name is used for the constructor and the distribution functions,
 * such as the density and random value function
 *
 * \return Rev name of constructor function.
 */
std::string Dist_UniformNatural::getDistributionFunctionName( void ) const
{
    // create a distribution name variable that is the same for all instance of this class
    std::string d_name = "UniformNatural";
    
    return d_name;
}


/** Return member rules */
const MemberRules& Dist_UniformNatural::getParameterRules(void) const
{
    
    static MemberRules distUnifMemberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet )
    {
        
        distUnifMemberRules.push_back( new ArgumentRule( "lower", Natural::getClassTypeSpec(), "The lower bound.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        distUnifMemberRules.push_back( new ArgumentRule( "upper", Natural::getClassTypeSpec(), "The upper bound.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        
        rulesSet = true;
    }
    
    return distUnifMemberRules;
}


const TypeSpec& Dist_UniformNatural::getTypeSpec( void ) const
{
    
    static TypeSpec ts = getClassTypeSpec();
    
    return ts;
}


/** Print value for user */
void Dist_UniformNatural::printValue(std::ostream& o) const
{
    
    o << " unif (lower=";
    if ( lower != NULL )
    {
        o << lower->getName();
    }
    else
    {
        o << "?";
    }
    o << ", upper=";
    if ( lower != NULL )
    {
        o << lower->getName();
    }
    else
    {
        o << "?";
    }
    o << ")";
}


/** Set a member variable */
void Dist_UniformNatural::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var) {
    
    if ( name == "lower" )
    {
        lower = var;
    }
    else if ( name == "upper" )
    {
        upper = var;
    }
    else
    {
        TypedDistribution<Natural>::setConstParameter(name, var);
    }
}
