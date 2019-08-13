#ifndef Func_experimentalCodonModelRateMatrixVector_H
#define Func_experimentalCodonModelRateMatrixVector_H

#include "ModelVector.h"
#include "RlRateGenerator.h"
#include "RlTypedFunction.h"

#include <string>

namespace RevLanguage {

    /**
     * The RevLanguage wrapper of the HKY rate matrix function.
     *
     * The RevLanguage wrapper of the HKY rate matrix connects
     * the variables/parameters of the function and creates the internal HkyRateMatrixFunction object.
     * Please read the HkyRateMatrixFunction.h for more info.
     *
     *
     * @copyright Copyright 2009-
     * @author The RevBayes Development Core Team (Sebastian Hoehna)
     * @since 2014-08-14, version 1.0
     *
     */
    class Func_experimentalCodonModelRateMatrixVector : public TypedFunction< ModelVector<RateGenerator> > {

    public:
        Func_experimentalCodonModelRateMatrixVector( void );

        // Basic utility functions
        Func_experimentalCodonModelRateMatrixVector*                     clone(void) const;                                          //!< Clone the object
        static const std::string&                                           getClassType(void);                                         //!< Get Rev type
        static const TypeSpec&                                              getClassTypeSpec(void);                                     //!< Get class type spec
        std::string                                                         getFunctionName(void) const;                                //!< Get the primary name of the function in Rev
        const TypeSpec&                                                     getTypeSpec(void) const;                                    //!< Get the type spec of the instance

        // Function functions you have to override
        RevBayesCore::TypedFunction< RevBayesCore::RbVector<RevBayesCore::RateGenerator> >*         createFunction(void) const;                                 //!< Create a function object
        const ArgumentRules&                                                getArgumentRules(void) const;                               //!< Get argument rules

    };

}

#endif
