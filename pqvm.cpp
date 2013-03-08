#include <bitset>
#include <list>
#include <iostream>

#include <sexp/sexp.h>
#include <sexp/sexp_ops.h>
#include <sexp/sexp_vis.h>

#include "pqvm.h"

namespace options {
    bool verbose (false);
    bool alt_measure (false);
}

class tangle {
    
    typedef unsigned int size_type;
    typedef unsigned int qid_type;
    typedef std::list<qid_type> list;
    
public:
    size_type size;
    list qids;
    quantum_register qureg;
    
    tangle(): size(0) {}
    
    friend std::ostream& operator<< (std::ostream& o, tangle const& fred);
    
};

//output as S expression.
std::ostream& operator<< (std::ostream& out, tangle const& t) {
    typedef tangle::list::const_iterator iterator;
    out << "(";
    for (iterator i(t.qids.cbegin()), n(t.qids.cend()) ; i != n; ++i)
        out << *i;
    out << ")";
}