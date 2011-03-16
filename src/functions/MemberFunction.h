/**
 * @file
 * This file contains the declaration of MemberFunction, which is used
 * for member functions (methods) of complex objects with methods.
 *
 * @brief Declaration of MemberFunction
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef MemberFunction_H
#define MemberFunction_H

#include "RbFunction.h"

#include <map>
#include <set>
#include <string>
#include <vector>

class ArgumentRule;
class DAGNode;
class VectorInteger;
class VectorString;

class MemberFunction :  public RbFunction {

    public:
                                    MemberFunction(const std::string& retType, const ArgumentRules& argRules);                 //!< Constructor
                                    MemberFunction(const std::string& retType, const int d, const ArgumentRules& argRules);  //!< Constructor

        // Basic utility functions
        std::string                 briefInfo(void) const;                  //!< Brief info about object
        MemberFunction*             clone(void) const;                      //!< Clone the object
    	bool                        equals(const RbObject* obj) const;      //!< Equals comparison
    	const VectorString&         getClass(void) const;                   //!< Get class vector
        std::string                 toString(void) const;                   //!< Complete info about object

        // Regular functions
        const ArgumentRules&        getArgumentRules(void) const;           //!< Get argument rules
        const std::string&          getReturnType(void) const;              //!< Get type of return value
        int                         getReturnDim(void) const { return dim; }                                              //!< Get dim of return value

	protected:
        DAGNode*                    executeOperation(const std::vector<DAGNode*>& args);    //!< Execute operation

        std::string                 returnType;                             //!< Return type
        const ArgumentRules&        argumentRules;                          //!< Argument rules
        
        int                         dim;
};

#endif

