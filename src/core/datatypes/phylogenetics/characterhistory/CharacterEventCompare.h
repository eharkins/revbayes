#ifndef CharacterEventCompare_H
#define CharacterEventCompare_H

#include "CharacterEvent.h"

namespace RevBayesCore {

    class CharacterEventCompare
    {
    public:
        // make sure that we order the events by the most recent event first
        bool operator()(CharacterEvent* lhs, CharacterEvent* rhs) const { return (*lhs < *rhs); }
    protected:
        // ...
    private:
        // ...

    };
}

#endif /* defined(__rb_mlandis__CharacterEventCompare__) */
