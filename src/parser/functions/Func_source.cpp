/**
 * @file
 * This file contains the implementation of Func_source, which is
 * the function used to read commands (source) from a file.
 *
 * @brief Implementation of Func_source
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @interface RbFunction
 * @package functions
 * @since Version 1.0, 2009-09-03
 *
 * $Id$
 */

#include "DAGNode.h"
#include "Func_source.h"
#include "Model.h"
#include "Parser.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RbString.h"
#include "TypeSpec.h"
#include "UserInterface.h"
#include "ValueRule.h"
#include "VectorString.h"
#include "Workspace.h"

#include <fstream>


// Definition of the static type spec member
const TypeSpec Func_source::typeSpec(Func_source_name);
const TypeSpec Func_source::returnTypeSpec(RbVoid_name);

/** Clone object */
Func_source* Func_source::clone( void ) const {

    return new Func_source( *this );
}


/** Execute function */
RbPtr<RbLanguageObject> Func_source::execute( void ) {

    /* Open file */
    std::string filename = static_cast<const RbString*>( args[0].getValue().get() )->getValue();
    std::ifstream inFile( filename.c_str() );
    
    
    bool echo = static_cast<const RbBoolean*>( args[1].getValue().get() )->getValue();
    
    if ( !inFile )
        throw RbException( "Could not open file \"" + filename + "\"" );

    /* Initialize */
    std::string commandLine;
    RBOUT("Processing file \"" + filename + "\"");
    Parser::getParser().reset();

    /* Command-processing loop */
    while ( inFile.good() ) {

        // Read a line
        std::string line;
        getline( inFile, line );
        
        if (echo) RBOUT(line);

        // Process the line
        if ( Parser::getParser().processCommand(line) == 2 )
            throw RbException( "Problem processing file \"" + filename + "\"" );
    }

    /* Return control */
    RBOUT("Processing of file \"" + filename + "\" completed");

    return RbPtr<RbLanguageObject>::getNullPtr();
}


/** Get argument rules */
const ArgumentRules& Func_source::getArgumentRules( void ) const {

    static ArgumentRules argumentRules;
    static bool          rulesSet = false;

    if ( !rulesSet ) {

        argumentRules.push_back( new ValueRule( "file", RbString_name ) );
        argumentRules.push_back( new ValueRule( "echo.on", RbBoolean_name, RbPtr<RbLanguageObject>( new RbBoolean(false) ) ) );
        rulesSet = true;
    }

    return argumentRules;
}


/** Get class vector describing type of object */
const VectorString& Func_source::getClass( void ) const {

    static VectorString rbClass = VectorString( Func_source_name ) + RbFunction::getClass();
    return rbClass;
}


/** Get return type */
const TypeSpec& Func_source::getReturnType( void ) const {

    return returnTypeSpec;
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& Func_source::getTypeSpec(void) const {
    return typeSpec;
}

