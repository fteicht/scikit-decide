ADD_LIBRARY(skdecide_pddl STATIC
                pddl.cc
                domain.cc
                problem.cc
                requirements.cc
                type.cc
                parser/parser.cc)
TARGET_INCLUDE_DIRECTORIES(skdecide_pddl PRIVATE
                                ${INCLUDE_DIRS}
                                ${CMAKE_CURRENT_SOURCE_DIR})

IF (NOT ONLY_PYTHON)
        CMAKE_POLICY(SET CMP0079 NEW)
        TARGET_LINK_LIBRARIES(skdecide PRIVATE skdecide_pddl)
ENDIF ()

IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
        ADD_LIBRARY(py_pddl OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/py_pddl.cc)
        TARGET_INCLUDE_DIRECTORIES(py_pddl PRIVATE
                                        ${INCLUDE_DIRS}
                                        ${CMAKE_CURRENT_SOURCE_DIR})
        CMAKE_POLICY(SET CMP0079 NEW)
        TARGET_LINK_LIBRARIES(__skdecide_hub_cpp PRIVATE
                                skdecide_pddl
                                $<TARGET_OBJECTS:py_pddl>)
ENDIF ()
