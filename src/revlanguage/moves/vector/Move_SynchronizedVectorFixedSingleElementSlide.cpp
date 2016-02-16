#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "RlBoolean.h"
#include "ContinuousStochasticNode.h"
#include "MetropolisHastingsMove.h"
#include "ModelVector.h"
#include "Move_SynchronizedVectorFixedSingleElementSlide.h"
#include "Natural.h"
#include "RbException.h"
#include "Real.h"
#include "RealPos.h"
#include "RevObject.h"
#include "TypedDagNode.h"
#include "TypeSpec.h"
#include "SynchronizedVectorFixedSingleElementSlideProposal.h"


using namespace RevLanguage;

Move_SynchronizedVectorFixedSingleElementSlide::Move_SynchronizedVectorFixedSingleElementSlide() : Move()
{
    
}

/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the process.
 */
Move_SynchronizedVectorFixedSingleElementSlide* Move_SynchronizedVectorFixedSingleElementSlide::clone(void) const
{
    
    return new Move_SynchronizedVectorFixedSingleElementSlide(*this);
}


void Move_SynchronizedVectorFixedSingleElementSlide::constructInternalObject( void )
{
    
    // we free the memory first
    delete value;
    
    // now allocate a new sliding move
    double l = static_cast<const RealPos &>( lambda->getRevObject() ).getValue();
    double w = static_cast<const RealPos &>( weight->getRevObject() ).getValue();
    
    RevBayesCore::TypedDagNode<RevBayesCore::RbVector<RevBayesCore::RbVector<double> > >* tmp = static_cast<const ModelVector<ModelVector<RealPos> > &>( v->getRevObject() ).getDagNode();
    std::vector<const RevBayesCore::DagNode*> par = tmp->getParents();
    std::vector< RevBayesCore::StochasticNode<RevBayesCore::RbVector<double> > *> n;
    for (std::vector<const RevBayesCore::DagNode*>::const_iterator it = par.begin(); it != par.end(); ++it)
    {
        const RevBayesCore::StochasticNode<RevBayesCore::RbVector<double> > *theNode = dynamic_cast< const RevBayesCore::StochasticNode<RevBayesCore::RbVector<double> >* >( *it );
        if ( theNode != NULL )
        {
            n.push_back( const_cast< RevBayesCore::StochasticNode<RevBayesCore::RbVector<double> >* >( theNode ) );
        }
        else
        {
            throw RbException("Could not create a mvElementScale because the node isn't a vector of stochastic nodes.");
        }
    }
    
    
    bool t = static_cast<const RlBoolean &>( tune->getRevObject() ).getValue();
    size_t e = static_cast<const Natural &>( which_element->getRevObject() ).getValue();
    
    RevBayesCore::Proposal *prop = new RevBayesCore::SynchronizedVectorFixedSingleElementSlideProposal(n, l, e-1);
    value = new RevBayesCore::MetropolisHastingsMove(prop, w, t);
    
}


/** Get Rev type of object */
const std::string& Move_SynchronizedVectorFixedSingleElementSlide::getClassType(void)
{
    
    static std::string revType = "Move_SynchronizedVectorFixedSingleElementSlide";
    return revType;
}

/** Get class type spec describing type of object */
const TypeSpec& Move_SynchronizedVectorFixedSingleElementSlide::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Move::getClassTypeSpec() ) );
    return revTypeSpec;
}


/**
 * Get the Rev name for the constructor function.
 *
 * \return Rev name of constructor function.
 */
std::string Move_SynchronizedVectorFixedSingleElementSlide::getMoveName( void ) const
{
    // create a constructor function name variable that is the same for all instance of this class
    std::string c_name = "SynchronizedVectorFixedSingleElementSlide";
    
    return c_name;
}


/** Return member rules (no members) */
const MemberRules& Move_SynchronizedVectorFixedSingleElementSlide::getParameterRules(void) const
{
    
    static MemberRules moveMemberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet )
    {
        moveMemberRules.push_back( new ArgumentRule( "x"      , ModelVector<ModelVector<Real> >::getClassTypeSpec(), "The variable (a deterministic variable holding the vector of stochastic variable) on which this move operates.", ArgumentRule::BY_REFERENCE, ArgumentRule::DETERMINISTIC ) );
        moveMemberRules.push_back( new ArgumentRule( "lambda" , RealPos::getClassTypeSpec()          , "The scaling factor (strength) of this move.", ArgumentRule::BY_VALUE    , ArgumentRule::ANY, new Real(1.0) ) );
        moveMemberRules.push_back( new ArgumentRule( "tune"   , RlBoolean::getClassTypeSpec()        , "Should we tune the scaling factor during burnin?", ArgumentRule::BY_VALUE    , ArgumentRule::ANY, new RlBoolean( true ) ) );
        moveMemberRules.push_back( new ArgumentRule( "element", Natural::getClassTypeSpec()          , "The index of the element to scale.", ArgumentRule::BY_VALUE    , ArgumentRule::ANY, new Natural( 1 ) ) );
        
        /* Inherit weight from Move, put it after variable */
        const MemberRules& inheritedRules = Move::getParameterRules();
        moveMemberRules.insert( moveMemberRules.end(), inheritedRules.begin(), inheritedRules.end() );
        
        rulesSet = true;
    }
    return moveMemberRules;
}

/** Get type spec */
const TypeSpec& Move_SynchronizedVectorFixedSingleElementSlide::getTypeSpec( void ) const
{
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get type spec */
void Move_SynchronizedVectorFixedSingleElementSlide::printValue(std::ostream &o) const
{
    
    o << "Move_SynchronizedVectorFixedSingleElementSlide(";
    if (v != NULL)
    {
        o << v->getName();
    }
    else
    {
        o << "?";
    }
    o << ")";
    
}


/** 
 * Set a member variable
 */
void Move_SynchronizedVectorFixedSingleElementSlide::setConstParameter(const std::string& name, const RevPtr<const RevVariable> &var)
{
    
    
    if ( name == "x" )
    {
        v = var;
    }
    else if ( name == "lambda" )
    {
        lambda = var;
    }
    else if ( name == "weight" )
    {
        weight = var;
    }
    else if ( name == "tune" )
    {
        tune = var;
    }
    else if ( name == "element" )
    {
        which_element = var;
    }
    else
    {
        Move::setConstParameter(name, var);
    }
    
}
