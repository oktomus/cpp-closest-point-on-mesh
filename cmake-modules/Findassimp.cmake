if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ASSIMP_ARCHITECTURE "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ASSIMP_ARCHITECTURE "32")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	
set(ASSIMP_ROOT_DIR "${CMAKE_SOURCE_DIR}/distant/assimp")

if(WIN32)	
	# Find path of each library
	message(STATUS "Looking for assimp in ${ASSIMP_ROOT_DIR}")
	find_path(ASSIMP_INCLUDE_DIR
		NAMES
			assimp/anim.h
		HINTS
			${ASSIMP_ROOT_DIR}/include
	)

	if(MSVC12)
		set(ASSIMP_MSVC_VERSION "vc120")
	elseif(MSVC14)	
		set(ASSIMP_MSVC_VERSION "vc140")
	elseif(MSVC17)
		set(ASSIMP_MSVC_VERSION "vc140")
	endif(MSVC12)
	
	if(MSVC12 OR MSVC14 OR MSVC)
	
		find_path(ASSIMP_LIBRARY_DIR
			NAMES
				assimp-${ASSIMP_MSVC_VERSION}-mt.lib
			HINTS
				${ASSIMP_ROOT_DIR}/lib${ASSIMP_ARCHITECTURE}
				${ASSIMP_ROOT_DIR}/lib
				${ASSIMP_ROOT_DIR}/lib/x${ASSIMP_ARCHITECTURE}
		)
		
		find_library(ASSIMP_LIBRARY_RELEASE				assimp-${ASSIMP_MSVC_VERSION}-mt.lib 			PATHS ${ASSIMP_LIBRARY_DIR})
		
		set(ASSIMP_LIBRARY 
			optimized 	${ASSIMP_LIBRARY_RELEASE}
		)
		
		set(ASSIMP_LIBRARIES "${ASSIMP_LIBRARY_RELEASE}")
	
	endif()
	
else(WIN32)

	find_path(
	  ASSIMP_INCLUDE_DIR
	  NAMES assimp/postprocess.h assimp/scene.h assimp/version.h assimp/config.h assimp/cimport.h
	  PATHS /usr/local/include/ ${ASSIMP_ROOT_DIR}/include /usr/include
	)

	find_library(
	  ASSIMP_LIBRARIES
	  NAMES assimp
	  PATHS /usr/local/lib/ /usr/local/ /usr/lib/ ${ASSIMP_ROOT_DIR}/lib
	)
	
	
endif(WIN32)

if (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)
	  SET(assimp_FOUND TRUE)
ENDIF (ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARIES)

if (assimp_FOUND)
		message(STATUS "Found asset importer library: ${ASSIMP_LIBRARIES}")
		message(STATUS "Found asset importer headers: ${ASSIMP_INCLUDE_DIR}")
	else (assimp_FOUND)
	  if (assimp_FIND_REQUIRED)
		message(STATUS "asset importer library: ${ASSIMP_LIBRARIES}")
		message(STATUS "asset importer headers: ${ASSIMP_INCLUDE_DIR}")
		message(FATAL_ERROR "Could not find asset importer library")		
	  endif (assimp_FIND_REQUIRED)
	endif (assimp_FOUND)