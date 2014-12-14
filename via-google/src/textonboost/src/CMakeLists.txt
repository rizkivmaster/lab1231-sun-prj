# Find and add TBB
find_package(TBB)
if (TBB_FOUND)
	set(USE_TBB ON)
else(TBB_FOUND)
	set(USE_TBB OFF)
	set(TBB_LIBRARIES "")
	set(TBB_INCLUDE_DIR "")
endif(TBB_FOUND)


# Find and add Eigen
find_package(Eigen3 REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${TBB_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR})

include_directories(./)

# fPIC for our swig shared library
add_definitions(-fPIC)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# Add the executables
#add_executable( textonize textonize.cpp )
#target_link_libraries( textonize textonutil feature )

#add_executable( textonview textonview.cpp )
#target_link_libraries( textonview textonutil feature )

#add_executable( textonboost textonboost.cpp )
#target_link_libraries( textonboost textonutil feature classifier )

#add_executable( evaluate evaluate.cpp )
#target_link_libraries( evaluate textonutil feature classifier )

add_executable( trainer trainer.cpp )
target_link_libraries( trainer textonutil feature classifier )

add_executable( predictor predictor.cpp )
target_link_libraries( predictor textonutil feature classifier )


# Add the subdirectories
add_subdirectory( algorithm )
add_subdirectory( feature )
add_subdirectory( util )
add_subdirectory( classifier )
