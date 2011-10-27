/**
 * @file
 * This file contains the implementation of SyntaxAssignExpr, which is
 * used to hold binary expressions in the syntax tree.
 *
 * @brief Implementation of SyntaxAssignExpr
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 *
 * $Id$
 */

#include "ConstantNode.h"
#include "DAGNode.h"
#include "DagNodeContainer.h"
#include "DeterministicNode.h"
#include "Distribution.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RbOptions.h"
#include "SyntaxFunctionCall.h"
#include "StochasticNode.h"
#include "VectorNatural.h"
#include "VectorString.h"
#include "SyntaxAssignExpr.h"
#include "Workspace.h"
#include "ConstructorFunction.h"

#include <iostream>
#include <list>
#include <sstream>


// Definition of the static type spec member
const TypeSpec SyntaxAssignExpr::typeSpec(SyntaxAssignExpr_name);

/** Static vector of strings giving names of operator types */
std::string SyntaxAssignExpr::opCode[] = { "ARROW_ASSIGN", "TILDE_ASSIGN", "TILDIID_ASSIGN", "EQUATION_ASSIGN" };


/** Construct from operator type, variable and expression */
SyntaxAssignExpr::SyntaxAssignExpr(SyntaxAssignExpr::operatorT op, RbPtr<SyntaxVariable> var, RbPtr<SyntaxElement> expr) 
: SyntaxElement(), variable(var), functionCall(NULL), expression(expr), opType(op) {
}


/** Construct from operator type, function call and expression */
SyntaxAssignExpr::SyntaxAssignExpr(SyntaxAssignExpr::operatorT op, RbPtr<SyntaxFunctionCall> fxnCall, RbPtr<SyntaxElement> expr) 
: SyntaxElement(), variable(NULL), functionCall(fxnCall), expression(expr), opType(op) {
}

/** Construct from operator type, variable and expression */
SyntaxAssignExpr::SyntaxAssignExpr(SyntaxAssignExpr::operatorT op, RbPtr<SyntaxVariable> var, RbPtr<SyntaxFunctionCall> expr) 
: SyntaxElement(), variable(var), functionCall(NULL), expression(RbPtr<SyntaxElement>( expr.get()) ), opType(op) {
}


/** Construct from operator type, function call and expression */
SyntaxAssignExpr::SyntaxAssignExpr(SyntaxAssignExpr::operatorT op, RbPtr<SyntaxFunctionCall> fxnCall, RbPtr<SyntaxFunctionCall> expr) 
: SyntaxElement(), variable(NULL), functionCall(fxnCall), expression(RbPtr<SyntaxElement>( expr.get()) ), opType(op) {
}


/** Deep copy constructor */
SyntaxAssignExpr::SyntaxAssignExpr(const SyntaxAssignExpr& x)
: SyntaxElement(x) {
    
    if ( x.variable != NULL )
        variable   = RbPtr<SyntaxVariable>( x.variable->clone() );

    if ( x.functionCall != NULL )
        functionCall = RbPtr<SyntaxFunctionCall>( x.functionCall->clone() );

    expression = RbPtr<SyntaxElement>( x.expression->clone() );
    opType     = x.opType;
}


/** Destructor deletes operands */
SyntaxAssignExpr::~SyntaxAssignExpr() {
    
}


/** Assignment operator */
SyntaxAssignExpr& SyntaxAssignExpr::operator=(const SyntaxAssignExpr& x) {
    
    if ( this != &x ) {
        
        functionCall = RbPtr<SyntaxFunctionCall>(NULL);
        variable = RbPtr<SyntaxVariable>(NULL);
        
        if ( x.variable.get() != NULL )
            variable   = x.variable;
        
        if ( x.functionCall.get() != NULL )
            functionCall = x.functionCall;
        
        expression = x.expression;
        opType     = x.opType;
    }
    
    return (*this);
}


/** Return brief info about object */
std::string SyntaxAssignExpr::briefInfo () const {
    
    std::ostringstream   o;
    o << "SyntaxAssignExpr: operation = " << opCode[opType];
    
    return o.str();
}


/** Clone syntax element */
SyntaxAssignExpr* SyntaxAssignExpr::clone () const {
    
    return new SyntaxAssignExpr(*this);
}


/** Get class vector describing type of object */
const VectorString& SyntaxAssignExpr::getClass(void) const { 
    
    static VectorString rbClass = VectorString(SyntaxAssignExpr_name) + SyntaxElement::getClass();
	return rbClass; 
}


/** Get semantic value: insert symbol and return the rhs value of the assignment */
RbPtr<Variable> SyntaxAssignExpr::getContentAsVariable( RbPtr<Environment> env ) const {
    
    PRINTF( "Evaluating assign expression\n" );
    
    // Get variable info from lhs
    RbPtr<VariableSlot>       theSlot = variable->getSlot( env );
    
    // Declare variable storing the return value of the assignment expression
    RbPtr<Variable> theVariable(NULL);
    
    // Deal with arrow assignments
    if ( opType == ArrowAssign ) {
        
        PRINTF("Arrow assignment\n");
        
        // Calculate the value of the rhs expression
        theVariable = expression->getContentAsVariable( env );
        if ( theVariable == NULL )
            throw RbException( "Invalid NULL variable returned by rhs expression in assignment" );
        
        // fill the slot with the new variable
        theSlot->getVariable()->setDagNode( RbPtr<DAGNode>( new ConstantNode( RbPtr<RbLanguageObject>(theVariable->getDagNodePtr()->getValue()->clone() ) ) ) );
    }
    
    // Deal with equation assignments
    else if ( opType == EquationAssign ) {
        
        PRINTF( "Equation assignment\n" );
        
        // Get DAG node representation of expression
        // We allow direct references without lookup nodes
        // We also allow constant expressions
        theVariable = expression->getContentAsVariable( env );
        PRINTF ( "Created %s with function \"%s\" and value %s \n", theVariable->getDagNode()->getType().c_str(), ((DeterministicNode*)theVariable->getDagNode())->getFunction()->getType().c_str(), theVariable->getDagNodePtr()->getValue() == NULL ? "NULL" : theVariable->getDagNodePtr()->getValue()->getTypeSpec().toString().c_str());
        
        // fill the slot with the new variable
        theSlot->getVariable()->setDagNode( theVariable->getDagNodePtr() );
    }
    
    // Deal with tilde assignments
    else if ( opType == TildeAssign ) {
        
        PRINTF( "Tilde assignment\n" );
        
        // get the rhs expression wrapped and executed into a variable
        theVariable = expression->getContentAsVariable(env);
        
        // Get distribution, which should be the return value of the rhs function
        RbPtr<DAGNode> exprValue = theVariable->getDagNodePtr();
        if ( exprValue == NULL ) {
            throw RbException( "Distribution function returns NULL" );
        }
        
        RbPtr<DeterministicNode> detNode( dynamic_cast<DeterministicNode*>( exprValue.get() ) );
        if ( detNode.get() == NULL || detNode->getFunction() == NULL || !detNode->getFunction()->isType( ConstructorFunction_name ) ) {
            
            throw RbException( "Function does not return a distribution" );
        }
        
        // Make an independent copy of the distribution and delete the exprVal
//        Distribution* distribution = (Distribution*) detNode->getFunctionPtr()->execute();
        RbPtr<Distribution> distribution( (Distribution*) detNode->getValue().get() );
        if ( distribution.get() == NULL )
            throw RbException( "Function returns a NULL distribution" );
        
        // Create new stochastic node
        RbPtr<DAGNode> node(new StochasticNode( distribution ) );
        
        // fill the slot with the new variable
        theSlot->getVariable()->setDagNode( node );
        
    }
//    
//    
//    // Deal with tilde iid assignments
//    else if ( opType == TildeIidAssign ) {
//        
//        PRINTF( "Tilde iid assignment\n" );
//        
//        // Get distribution, which should be the return value of the rhs function
//        // The rhs could evaluate to a reference to a distribution (allowing several
//        // stochastic nodes to share the same distribution)
//        DAGNode* exprValue = expression->getValue( frame );
//        if ( exprValue == NULL ) {
//            throw RbException( "Distribution function returns NULL" );
//        }
//        MemberNode* dist = dynamic_cast<MemberNode*>( exprValue );
//        if ( dist == NULL || dist->getValue() == NULL || !dist->getValue()->isType( Distribution_name ) ) {
//            delete exprValue;
//            throw RbException( "Function does not return a distribution" );
//        }
//        
//        // Make an independent copy of the distribution and delete the exprVal
//        Distribution* distribution = static_cast<Distribution*>( dist->getMemberObject()->clone() );
//        delete exprValue;
//        if ( distribution == NULL )
//            throw RbException( "Function returns a NULL distribution" );
//        
//        delete distribution;
//        throw RbException( "Support of ~iid not complete yet" );
//    }
    
    theSlot->getDagNodePtr()->touchAffected();
    theSlot->getDagNodePtr()->keep();

    
#ifdef DEBUG_PARSER
    env->printValue(std::cerr);
#endif    

    return theVariable;
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& SyntaxAssignExpr::getTypeSpec(void) const {
    return typeSpec;
}


/** Print info about the syntax element */
void SyntaxAssignExpr::print(std::ostream& o) const {
    
    o << "SyntaxAssignExpr:" << std::endl;
    o << "variable      = " << variable->briefInfo() << std::endl;
    o << "expression    = " << expression->briefInfo() << std::endl;
    o << "operation     = " << opCode[opType];
}

