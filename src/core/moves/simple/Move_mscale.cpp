/**
 * @file
 * This file contains the implementation of Move_mscale, which scales a
 * a real value with a factor drawn from an appropriate distribution.
 *
 * @brief Implementation of Move_mscale
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-08, version 1.0
 *
 * $Id$
 */

#include "ConstantNode.h"
#include "Move_mscale.h"
#include "RandomNumberGenerator.h"
#include "RbUtil.h"
#include "RealPos.h"
#include "StochasticNode.h"
#include "ValueRule.h"
#include "VectorString.h"
#include "Workspace.h"

#include <cmath>


// Definition of the static type spec member
const TypeSpec Move_mscale::typeSpec(Move_mscale_name);

/** Constructor for parser */
Move_mscale::Move_mscale( void )
    : MoveSimple( getMemberRules() ) {
}


/** Clone object */
Move_mscale* Move_mscale::clone( void ) const {

    return new Move_mscale( *this );
}


/** Get class vector describing type of object */
const VectorString& Move_mscale::getClass() const {

    static VectorString rbClass = VectorString( Move_mscale_name ) + MoveSimple::getClass();
    return rbClass;
}


/** Return member rules */
const RbPtr<MemberRules> Move_mscale::getMemberRules( void ) const {

    static RbPtr<MemberRules> memberRules( new MemberRules );
    static bool        rulesSet = false;

    if ( !rulesSet ) {
        TypeSpec varType( RealPos_name );
        memberRules->push_back( RbPtr<ArgumentRule>( new ValueRule( "variable", varType ) ) );

        /* Inherit weight from MoveSimple, put it after variable */
        const RbPtr<MemberRules> inheritedRules = MoveSimple::getMemberRules();
        memberRules->insert( memberRules->end(), inheritedRules->begin(), inheritedRules->end() ); 

        memberRules->push_back( RbPtr<ArgumentRule>( new ValueRule( "lambda", RealPos_name ) ) );

        rulesSet = true;
    }

    return memberRules;
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& Move_mscale::getTypeSpec(void) const {
    return typeSpec;
}


/** Return the random variable type appropriate for the move */
const TypeSpec Move_mscale::getVariableType( void ) const {

    return TypeSpec( RealPos_name );
}


/** Perform the move */
double Move_mscale::perform( std::set<RbPtr<StochasticNode> >& affectedNodes ) {

    // Get random number generator    
    RbPtr<RandomNumberGenerator> rng     = GLOBAL_RNG;

    // Get relevant values
    RbPtr<StochasticNode>  nodePtr( static_cast<StochasticNode*>( (*members)["variable"]->getDagNodePtr().get() ) );
    const RealPos          lambda  = *( static_cast<const RealPos*>( getMemberValue("lambda").get()  ) );

    const RealPos          curVal  = *( static_cast<const RealPos*>( nodePtr->getValue().get() ) );

    // Generate new value (no reflection, so we simply abort later if we propose value here outside of support)
    RealPos u      = rng->uniform01();
    RealPos newVal = curVal * std::exp( lambda * ( u - 0.5 ) );

    // Propose new value
    nodePtr->setValue( RbPtr<RbLanguageObject>( newVal.clone() ), affectedNodes );

    // Return Hastings ratio
    return log( newVal / curVal );
}

