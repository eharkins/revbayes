#include <iosfwd>
#include <vector>

#include "ArgumentRule.h"
#include "Func_upperTriangle.h"
#include "ModelVector.h"
#include "RealPos.h"
#include "RlDeterministicNode.h"
#include "TypedDagNode.h"
#include "TypeSpec.h"
#include "UpperTriangle.h"
#include "Argument.h"
#include "ArgumentRules.h"
#include "ConstantNode.h"
#include "DagNode.h"
#include "DeterministicNode.h"
#include "DynamicNode.h"
#include "IndirectReferenceFunction.h"
#include "ModelObject.h"
#include "RbVector.h"
#include "RevPtr.h"
#include "RevVariable.h"
#include "RlConstantNode.h"
#include "RlFunction.h"
#include "RlMatrixReal.h"
#include "RlTypedFunction.h"
#include "TypedFunction.h"
#include "UserFunctionNode.h"

namespace RevBayesCore { class MatrixReal; }


using namespace RevLanguage;

Func_upperTriangle::Func_upperTriangle() : TypedFunction< ModelVector< RealPos > >() {
    
}

/* Clone object */
Func_upperTriangle* Func_upperTriangle::clone( void ) const
{
    return new Func_upperTriangle( *this );
}


RevBayesCore::TypedFunction< RevBayesCore::RbVector<double> >* Func_upperTriangle::createFunction( void ) const
{
    
    RevBayesCore::TypedDagNode<RevBayesCore::MatrixReal >* m = static_cast<const MatrixReal& >( this->args[0].getVariable()->getRevObject() ).getDagNode();
    RevBayesCore::UpperTriangle *func = new RevBayesCore::UpperTriangle( m );
    
    return func;
}


/** Get argument rules */
const ArgumentRules& Func_upperTriangle::getArgumentRules( void ) const
{
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rules_set = false;
    
    if ( !rules_set )
    {
        argumentRules.push_back( new ArgumentRule( "matrix", MatrixReal::getClassTypeSpec(), "A matrix.", ArgumentRule::BY_CONSTANT_REFERENCE, ArgumentRule::ANY ) );
        rules_set = true;
    }
    
    return argumentRules;
}


/** Get class name of object */
const std::string& Func_upperTriangle::getClassName(void)
{
    
    static std::string rbClassName = "Func_upperTriangle";
    
	return rbClassName; 
}


/** Get class type spec describing type of object */
const RevLanguage::TypeSpec& Func_upperTriangle::getClassTypeSpec(void)
{
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return rbClass; 
}


/**
 * Get the primary Rev name for this function.
 */
std::string Func_upperTriangle::getFunctionName( void ) const
{
    // create a name variable that is the same for all instance of this class
    std::string f_name = "fnUpperTriangle";
    
    return f_name;
}


/** Get type spec */
const TypeSpec& Func_upperTriangle::getTypeSpec( void ) const
{
    
    static TypeSpec type_spec = getClassTypeSpec();
    
    return type_spec;
}
