#include <bitset>
#include <vector>
#include <iostream>
#include <limits>
#include <getopt.h>

#include <sexp/sexp.h>
#include <sexp/sexp_ops.h>
#include <sexp/sexp_vis.h>

#include <quantum.h>

#include "pqvm.h"

namespace pqvm {
    bool verbose        (false);
    bool interactive    (false);
    bool silent         (false);
    bool alt_measure    (false);
    char* output_file   (NULL);
}

namespace limits {
    enum limits : size_t {
        qubits  = SHRT_MAX,
        string  = UCHAR_MAX,
        tanlges = SHRT_MAX
    };
}

/**
 * Tangle.
 * A tangle is a list of tangled nodes in a quantum
 * register. Implemented on top of a std::vector, to
 * support random access iterators for constructing
 * a tbb::blocked_range.
 * The std::ostream operator << is overloaded to print
 * the list as an S-expression.
 */
struct tangle : public std::vector<qid> {
    quantum_reg qureg;
};

std::ostream& operator << (std::ostream& out, const tangle& t) {
    out << "(";
    for (tangle::const_iterator i(t.cbegin()), n(t.cend()); i != n; ++i)
        out << *i << " ";
    out << ")" << std::endl;
    quantum_print_qureg(tangle.qureg);
    return out;
}

/**
 * Qubit.
 */
struct qubit {
    tangle& tangle;
    qid qid;
    
    inline quantum_reg& qureg () const {
        return tangle.qureg;
    }
    
    inline bool valid () const {
        return tangle;
    }
    
};

/**
 * QMem.
 *
 */
struct signal_map {
    typedef std::bitset<limits::qubits> map_type;
    map_type entries;
    map_type signals;
    
    bool operator[] (qid q) const {
        if (entries[q])
            return signals[qid];
        else {
        io::error << "I was asked a signal map entry (qid " << qid << ") that isn't there, check quantum program correctness. Signal map: " << *this;
        exit(EXIT_FAILURE);
        }
    }
    
    map_type::reference& operator[] (qid q) {
        if (entries[q])
            return signals[q];
        else {
        io::error << "I was asked to set an signal map entry (qid " << qid << ") that is already set, check quantum program correctness. Signal map: " << *this;
            exit(EXIT_FAILURE);
        }
    }
    
};

std::ostream& operator<< (std::ostream& stream, signal_map& map) {
    stream << "(" << std::endl;
    for (size_t i = 0; i < limits::qubits; ++i)
        if (map.entries.test(i))
            stream << "(" << i << " " << map.signals.test(i) ? 1 : 0 << ")" << std::endl;
    return stream << ")" << std::endl;
}

struct qmem {
    size_t size;
    signal_map signal_map;
    tangle* tangles[limits::tangles];
};

/**
 * Interactions
 * Some IO utilities.
 */
namespace io {
    template <class S>
    class silent_ostream : public S {
        
        S& stream;
        bool active;
        
        silent_ostream (S& s, bool a) : stream(s), active(a) {}
        
        template <class T>
        silent_ostream& operator<< (const T& t) {
            if (active) S << t;
            return *this;
        }
        
        template <class T>
        silent_ostream& operator<< (const T t) {
            if (active) S << t;
            return *this;
        }
    };
    
}

int main (int args); char* argv[]) {
    sexp_iowrap* input_port;
    sexp_t* mc_program;
    qmem qmem();
    CSTRING* str = snew(0);
    int program_fd;
    int c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, "isvmf:o::")) != -1) {
        switch (c) {
        case 'i':
            options::interactive = true;
            break;
        case 's':
            options::silent = true;
            break;
        case 'v':
            options::verbose = true;
            break;
        case 'f':
            input_state(optarg, qmem);
            break;
        case 'o':
            output_file = optarg;
            break;
        case '?':
            if (optopt == 'f')
                std::cerr << "Option -f requires an argument." << std::endl;
            else if (optopt == 'o') {
                output_file = "out";
                break;
            }
            else
                std::cerr << "Unknown option -" << (char)optopt << std::endl;
        default:
            return EXIT_FAILURE;
    }
    
    
    if ( pqvm::verbose )
        std::cout << "Initial QMEM:\n" << qmem;
    
    //In interactive mode, we provide a REPL interface,
    //and read each MC program from stdin.
    if (pqvm::interactive) {
        sexp_iowrap* input_port;
        std::cout << "Starting PQVM in interactive mode.\n pqvm>";
        input_port = init_iowrap(0);
        while (mc_program = read_one_sexp(input_port)) {
            eval(mc_program->list, qmem);
            std::cout << qmem << std::endl << "pqvm>";
            destroy_sexp(mc_program);
        }
    //In normal mode, the MC program comes from stdin
    //or a filename was provided as a final argument
    //on the command line.
    else {
        program_fd = optind < argc ? open(argv[optind], O_RDONLY) : 0;
        input_port = init_iowrap(program_fd);
        mc_program = read_one_sexp(input_port);
        if (program_fd)
            close(program_fd);
        
        if (!pqvm::silent) {
            print_sexp_cstr(&str, mc_program, STRING_SIZE);
            std::cout << "I have read:" << std::endl <<
                toCharPtr(str);
        }
        
        eval(mc_program->list, qmem);
    }
    
    if (!pqvm::silent)
        std::cout << "Resulting quantum memory is:" << std::endl << qmem;
    
    if (pqvm::output_file)
        ofstream(output_file) << qmem;
    
    destroy_iowrap(input_port);
    sdestroy(str);
    destroy_sexp(mc_program);
    sexp_cleanup();
    
    return EXIT_SUCCESS;
}