AC_PREREQ(2.59)

AC_INIT(silix, 1.2.11)
AC_CONFIG_SRCDIR([src/main_buildfam.cc])
AM_INIT_AUTOMAKE


AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

dnl Checks for programs---------------------------------
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

dnl Language
AC_LANG_CPLUSPLUS

dnl Checks for header files----------------------
AC_HEADER_STDC

dnl Checks for CppUnit
AC_MSG_NOTICE(Checking CppUnit)	
AC_PATH_PROG(CPPUNITCONFIG,cppunit-config)
AM_CONDITIONAL([CPPUNIT], [test "x$CPPUNITCONFIG" != "x"])
if test "x$CPPUNITCONFIG" == "x" ; then
   AC_MSG_NOTICE(CppUnit not found... unittesting disabled. Please install CppUnit)
   CXXFLAGS="-O3"
else
   AC_MSG_NOTICE(CppUnit found... unittesting enabled.)
   CXXFLAGS="-O3 -Wall"
fi


dnl Checks for boost::program_options
AC_MSG_NOTICE(Checking Boost::program_options)
AC_CHECK_HEADERS(boost/program_options.hpp, 
		AC_MSG_NOTICE(Boost::program_options header files found on the system... compiling enabled),	
		AC_MSG_ERROR(Boost::program_options header files were not found on the system... unable to compile. Please install Boost::program_options headers))

dnl Optionnal - Checks for boost::unordered_map
AC_ARG_ENABLE(hash, [--enable-hash    Enables the use of Boost::unordered_map hash table], [AC_DEFINE(WITHBOOST,1, boost activation)
		     AC_CHECK_HEADERS(boost/unordered_map.hpp,
		     [AC_MSG_NOTICE(Boost::unordered_map found on the system... compiling with hash table enabled)],
   		     [AC_MSG_ERROR(Boost::unordered_map was not found on the system... unable to compile. Please install Boost::unordered_map)]) ]) 

dnl  Optionnal - Configures for mpi
AC_ARG_ENABLE([mpi],
[ --enable-mpi Enables MPI use],
[case "${enableval}" in
yes) mpi=true ;;
no) mpi=false ;;
*) AC_MSG_ERROR([bad value ${enableval} for --enable-mpi]) ;;
esac],[mpi=false])
if test x$mpi = xtrue ; then
   AC_DEFINE(WITHMPI,1, mpi activation) 
   CXX="mpic++ -DMPICH_IGNORE_CXX_SEEK"
fi
AM_CONDITIONAL([MPI], [test x$mpi = xtrue])


dnl  Optionnal - Configures for verbosity
AC_ARG_ENABLE(verbose, [--enable-verbose    Enables verbosity], [AC_DEFINE(VERBOSE,1, verbosity activation)])

dnl  Optionnal - Configures for disabling the checking tests
AC_ARG_ENABLE(expert, [--enable-expert    Expert mode], [AC_DEFINE(EXPERT,1, expert mode activation)])


dnl Optimization or debug-----------------------
AC_ARG_ENABLE(debug, [ --enable-debug    Debug Mode], [CXXFLAGS="-g"])

dnl Checks for libraries------------------------
AC_CHECK_LIB(m, main)
AC_SEARCH_LIBS(main, boost_program_options, 
			[AC_MSG_NOTICE(Boost::program_options library file found on the system... compiling enabled); LIBS+='-lboost_program_options'; AC_DEFINE(HAVE_LIBBOOST_PROGRAM_OPTIONS, 1, boost activation)],	
			[AC_MSG_ERROR(Boost::program_options library file was not found on the system... unable to compile. Please install Boost::program_options library)])

dnl Checks for typedefs, structures, and compiler characteristics
AC_C_CONST
AC_C_INLINE


AC_CONFIG_FILES([Makefile libforest/Makefile libbuildfam/Makefile src/Makefile unittests/Makefile utils/Makefile doc/Makefile doc/man/Makefile])	
AC_OUTPUT
