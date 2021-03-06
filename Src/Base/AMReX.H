
#ifndef BL_AMREX_H
#define BL_AMREX_H

#include <AMReX_ccse-mpi.H>

#include <functional>
#include <string>
#include <cstdio>

//
// Error-reporting Functions and Version String

/*
 This class consists entirely of static error-reporting functions that
 are used throughout the BoxLib implementation together with a version
 string.  In the best of all possible worlds we'd use a namespace, but
 we aren't there, yet :-)  Note that all the output functions add a
 terminating exclamation mark, so there is no need to add any punctuation
 to sentences that get output explicitly.
*/

namespace amrex
{
    typedef void (*PTR_TO_VOID_FUNC)(void);

    std::string Version ();

    void Initialize (int& argc, char**& argv, bool build_parm_parse=true,
                     MPI_Comm mpi_comm = MPI_COMM_WORLD,
                     const std::function<void()>& func_parm_parse = {});

    void Finalize(bool finalize_parallel = true);
    /**
    * \brief We maintain a stack of functions that need to be called in Finalize().
    * The functions are called in LIFO order.  The idea here is to allow
    * classes to clean up any "global" state that they maintain when we're
    * exiting from BoxLib land.
    */
    void ExecOnFinalize (PTR_TO_VOID_FUNC);
    void ExecOnInitialize (PTR_TO_VOID_FUNC);

    //! Print out message to cerr and exit via amrex::Abort().
    void Error (const char * msg = 0);
    void Error (const std::string& msg);

    //! Print out warning message to cerr.
    void Warning (const char * msg);
    void Warning (const std::string& msg);

    //! Print out message to cerr and exit via abort().
    void Abort (const char * msg = 0);
    void Abort (const std::string& msg);

    /**
    * \brief Prints assertion failed messages to cerr and exits
    * via abort().  Intended for use by the BL_ASSERT() macro
    * in <BLassert.H>.
    */
    void Assert (const char* EX,
                 const char* file,
                 int         line);
    /**
    * \brief Prints out an out-of-memory message and aborts.  It is
    *         called by various BoxLib routines when a call to new fails.
    *
    *        Called as amrex::OutOfMemory(__FILE__, __LINE__);
    */
    void OutOfMemory (const char* file,
                      int         line);

    /**
    * \brief This is used by amrex::Error(), amrex::Abort(), and amrex::Assert()
    * to ensure that when writing the message to stderr, that no additional
    * heap-based memory is allocated.
    */
    void write_to_stderr_without_buffering (const char* str);

    namespace system
    {
#ifdef NDEBUG
	constexpr bool NDebug = true;
#else
	constexpr bool NDebug = false;
#endif

	extern std::string exename;
	
	extern int verbose;

        extern int signal_handling;
    }
}

#endif /*BL_AMREX_H*/
