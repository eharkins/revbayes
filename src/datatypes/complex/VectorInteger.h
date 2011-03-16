/**
 * @file
 * This file contains the declaration of VectorInteger, a complex type
 * used to hold int vectors.
 *
 * @brief Declaration of VectorInteger
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-12-04, version 1.0
 * @extends RbComplex
 *
 * $Id$
 */

#ifndef VectorInteger_H
#define VectorInteger_H

#include "Vector.h"

#include <iostream>
#include <vector>

class VectorInteger : public Vector {

    public:
        // Constructors and destructor
                                    VectorInteger(void);                                          //!< Default constructor (empty vector)
                                    VectorInteger(int x);                                         //!< Construct vector with one int x
                                    VectorInteger(size_t n, int x);                               //!< Construct vector with n ints x
                                    VectorInteger(const std::vector<int>& x);                     //!< Constructor from int vector
                                    VectorInteger(const std::vector<unsigned int>& x);            //!< Constructor from unsigned int vector
                                    VectorInteger(const ContainerIterator& x);                    //!< Constructor from container iterator

        // Operators
        int&                        operator[](size_t i) { return value[i]; }                     //!< Index op allowing change
        const int&                  operator[](size_t i) const { return value[i]; }               //!< Const index op
        bool                        operator==(const VectorInteger& x) const;                     //!< Equals comparison
        bool                        operator!=(const VectorInteger& x) const;                     //!< Not equals comparison
        
        // Basic utility functions, pure virtual in RbComplex and implemented here
        virtual VectorInteger*      clone(void) const;                                            //!< Clone object
        virtual bool                equals(const RbObject* obj) const;                            //!< Equals comparison
        virtual const VectorString& getClass(void) const;                                         //!< Get class
        virtual void                printValue(std::ostream& o) const;                            //!< Print value (for user)
        virtual std::string         toString(void) const;                                         //!< Complete info about object

        // Element access functions for parser
        virtual const std::string&  getElementType(void) const;                                   //!< Get element type
        const RbObject*             getElement(const VectorInteger& index) const;                 //!< Get element (read-only)
        const VectorInteger&        getLength(void) const;                                        //!< Get length in each dim
        void                        resize(const VectorInteger& len);                             //!< Resize
        void                        setElement(const VectorInteger& index, RbObject* val);        //!< Set element
        void                        setLength(const VectorInteger& len);                          //!< Set length in each dim

        // Regular functions, including STL-like functions
        void                        clear(void) { value.clear(); }                                //!< Clear
        const std::vector<int>&     getValue(void) const { return value; }                        //!< Get value
        void                        pop_back(void) { value.pop_back(); }                          //!< Drop element
        void                        push_back(int x) { value.push_back(x); }                      //!< Append element to end
        void                        push_front(int x) { value.insert(value.begin(), x); }         //!< Add element in front
        void                        resize(size_t n) { value.resize(n); }                         //!< Resize
        virtual void                setValue(const VectorInteger& x);                             //!< Set the value
        virtual void                setValue(const std::vector<int>& x) { value = x; }            //!< Set the value
        size_t                      size(void) const { return value.size(); }                     //!< Get size

    private:
        std::vector<int>            value;                                                        //!< Vector of values
};

#endif


