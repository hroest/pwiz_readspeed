cmake_minimum_required(VERSION 2.6)
project(PWIZ_external_build)

###########################################################################
### System specific configuration
###########################################################################

# add executables here... (each should correspond to a .cpp file)
SET(my_executables
  tic_pwiz
  tic_pwiz_parallel
)

set(PWIZ_BUILDNAME "build-linux-x86_64")
# set(PWIZ_MAKEDIR "/gcc-4.6/release/link-static/runtime-link-static/threading-multi/")
set(EXTRA_QUAL "/without-mz5/")
set(PWIZ_MAKEDIR "/gcc-4.4.7/release/link-static/runtime-link-static/threading-multi/")
set(BOOST_VERSION "1_56_0")
set(GCC_VERSION "44")

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# No Edits beyond this place
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

###########################################################################
### 1. Configure proteowizard
###########################################################################

if (NOT PWIZ_MAINDIR)
  message(FATAL_ERROR "Aborting ..., please specify the pwiz build directory (-DPWIZ_MAINDIR)")
endif()

# Set the proteowizard library and base directory
set(PWIZ_LIBBASEDIR "${PWIZ_MAINDIR}/${PWIZ_BUILDNAME}/libraries/")
set(PWIZ_BASEDIR "${PWIZ_MAINDIR}/${PWIZ_BUILDNAME}/pwiz/")

#set all pwiz-specific libs that are needed (.a files in build folders)
set(pwiz_link_libraries
  pwiz_data_msdata
  pwiz_data_msdata_version
  pwiz_utility_misc
  pwiz_utility_minimxml
  pwiz_data_common
  # pwiz_data_msdata_mz5
)

# all boost libs
SET(boost_libs
  # date_time # may be omitted
  # prg_exec_monitor # may be omitted
  # serialization # may be omitted
  # program_options # may be omitted
  filesystem
  iostreams
  thread
  regex # is missing in newest pwiz package ?
  system
  chrono
)

#set all external libs that are needed
set(ext_link_libraries
  SHA1
  zlib
  pthread
  rt # necessary for chrono (real time shared lib)
  # hdf5pp
  # hdf5
)

include_directories(${PWIZ_MAINDIR})

# add the locations of the libraries of pwiz
link_directories(${PWIZ_BASEDIR}/utility/misc/${EXTRA_QUAL}/${PWIZ_MAKEDIR})
link_directories(${PWIZ_BASEDIR}/utility/minimxml/${EXTRA_QUAL}/${PWIZ_MAKEDIR})
link_directories(${PWIZ_BASEDIR}/data/msdata/${EXTRA_QUAL}/${PWIZ_MAKEDIR})
link_directories(${PWIZ_BASEDIR}/data/common/${EXTRA_QUAL}/${PWIZ_MAKEDIR})
link_directories(${PWIZ_BASEDIR}/data/msdata/mz5/${EXTRA_QUAL}/${PWIZ_MAKEDIR})

# add the locations of the external libraries used by proteowizard (hdf5, zlib)
link_directories(${PWIZ_LIBBASEDIR}/hdf5-1.8.7/${PWIZ_MAKEDIR})
link_directories(${PWIZ_MAINDIR}/${PWIZ_BUILDNAME}/standalone/zlib/${PWIZ_MAKEDIR})

# add the location of boost libraries and add the lib itself to the linker
foreach(i ${boost_libs})
  link_directories(${PWIZ_MAINDIR}/${PWIZ_BUILDNAME}/ext/boost/${i}-${BOOST_VERSION}/${EXTRA_QUAL}/boost/${PWIZ_MAKEDIR})
  LIST(APPEND ext_link_libraries boost_${i}-gcc${GCC_VERSION}-mt-s)
endforeach(i)

# should we link against OpenMP?
set(CMAKE_CXX_FLAGS "-fopenmp")

# Add the executables (should .cpp)
foreach(i ${my_executables})
  add_executable(${i} ${i}.cpp)
endforeach(i)

# Link the pwiz and the external libraries in
foreach(i ${pwiz_link_libraries})
  foreach(j ${my_executables})
    target_link_libraries(${j} ${i})
  endforeach(j)
endforeach(i)
foreach(i ${ext_link_libraries})
  foreach(j ${my_executables})
    target_link_libraries(${j} ${i})
  endforeach(j)
endforeach(i)




