/**
 * @file
 * This file contains the implementation of RbBoolean, which is
 * a RevBayes wrapper around a regular bool.
 *
 * @brief Implementation of RbBoolean
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-11-20, version 1.0
 * @extends RbObject
 *
 * $Id$
 */


#include "RbBoolean.h"
#include "Real.h"
#include "Integer.h"
#include "RbNames.h"
#include "TypeSpec.h"
#include "VectorString.h"

#include <sstream>


/** Default constructor */
RbBoolean::RbBoolean(void) : RbLanguageObject(), value(false) {

}

/** Construct from bool */
RbBoolean::RbBoolean(const bool v) : RbLanguageObject(), value(v) {

}


/** Get brief info about object */
std::string RbBoolean::briefInfo(void) const {
    
	std::ostringstream o;
    o << "RbBoolean: value = ";
    printValue(o);
    return o.str();
}

/** Clone object */
RbBoolean* RbBoolean::clone(void) const {

	return  new RbBoolean(*this);
}


/** Convert to type and dim. The caller manages the returned object. */
RbLanguageObject* RbBoolean::convertTo(const std::string& type) const {

    if (type == Integer_name) {
        if (value)
            return new Integer(1);
        else 
            return new Integer(0);
    }
    else if (type == Real_name) {
        if (value)
            return new Real(1.0);
        else 
            return new Real(0.0);
    }

    return RbLanguageObject::convertTo(type);
}


/** Get class vector describing type of object */
const VectorString& RbBoolean::getClass() const {

    static VectorString rbClass = VectorString(RbBoolean_name) + RbObject::getClass();
    return rbClass;
}


/** Is convertible to type and dim? */
bool RbBoolean::isConvertibleTo(const std::string& type, bool once) const {

    if (type == Integer_name)
        return true;
    else if (type == Real_name)
        return true;

    return RbObject::isConvertibleTo(type, once);
}


/** Print value for user */
void RbBoolean::printValue(std::ostream &o) const {

    o << (value ? "true" : "false");
}

/** Get complete info about object */
std::string RbBoolean::richInfo(void) const {

	std::ostringstream o;
    o << "RbBoolean: value = ";
    printValue(o);
    return o.str();
}

