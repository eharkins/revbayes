/**
 * @file
 * This file contains the declaration of SyntaxUnaryExpr, which is
 * used to hold unary expressions in the syntax tree.
 *
 * @brief Declaration of SyntaxUnaryExpr
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 *
 * $Id$
 */

#ifndef SyntaxUnaryExpr_H
#define SyntaxUnaryExpr_H

#include "SyntaxElement.h"

#include <iostream>
#include <vector>


/**
 * This is the class used to hold binary expressions in the syntax tree.
 *
 * We store the operands and a flag signalling the type of operation to
 * be performed when getValue is called or to be represented when
 * getDAGNodeExpr is called.
 *
 */
const std::string SyntaxUnaryExpr_name = "Unary expression";

class SyntaxUnaryExpr : public SyntaxElement {

    public:
        // Unary operator types
        enum                    operatorT { UMinus, UPlus, UNot, UAnd };                                //!< Operator types
        static std::string      opCode[];                                                               //!< Operator codes for printing

        // Constructors and destructor
                                SyntaxUnaryExpr(SyntaxUnaryExpr::operatorT op, RbPtr<SyntaxElement> expr); //!< Standard constructor 
                                SyntaxUnaryExpr(const SyntaxUnaryExpr& x);                              //!< Copy constructor
	    virtual                ~SyntaxUnaryExpr();                                                      //!< Destroy operands

        // Assignment operator
        SyntaxUnaryExpr&        operator=(const SyntaxUnaryExpr& x);                                    //!< Assignment operator

        // Basic utility functions
        std::string             briefInfo() const;                                                      //!< Brief info about object
        SyntaxUnaryExpr*        clone() const;                                                          //!< Clone object
        const VectorString&     getClass(void) const;                                                   //!< Get class vector
        const TypeSpec&         getTypeSpec(void) const;                                                //!< Get language type of the object 
        void                    print(std::ostream& o) const;                                           //!< Print info about object

        // Regular functions
        RbPtr<Variable>         getContentAsVariable(RbPtr<Environment> env) const;                           //!< Get semantic value

    protected:
        RbPtr<SyntaxElement>    expression;                                                             //!< The expression
        enum operatorT          operation;                                                              //!< The type of operation
    
    private:
        static const TypeSpec   typeSpec;
};

#endif

