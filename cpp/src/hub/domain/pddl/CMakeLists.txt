generate_template_instantiation_files(
        AGGREGATION_EFFECT_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/aggregation_effect.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "ConjunctionEffect!Con;DisjunctionEffect!Dis")

ADD_LIBRARY(skdecide_pddl STATIC
        pddl.cc
        domain.cc
        problem.cc
        requirements.cc
        type.cc
        ${AGGREGATION_EFFECT_TEMPLATE_FILES}
        impl/aggregation_effect.cc
        parser/parser.cc)
TARGET_INCLUDE_DIRECTORIES(skdecide_pddl PRIVATE
        ${INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR})

IF(NOT ONLY_PYTHON)
        CMAKE_POLICY(SET CMP0079 NEW)
        TARGET_LINK_LIBRARIES(skdecide PRIVATE skdecide_pddl)
ENDIF()

IF(BUILD_PYTHON_BINDING OR ONLY_PYTHON)
        ADD_LIBRARY(py_pddl OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/py_pddl.cc)
        TARGET_INCLUDE_DIRECTORIES(py_pddl PRIVATE
                ${INCLUDE_DIRS}
                ${CMAKE_CURRENT_SOURCE_DIR})
        CMAKE_POLICY(SET CMP0079 NEW)
        TARGET_LINK_LIBRARIES(__skdecide_hub_cpp PRIVATE
                skdecide_pddl
                $<TARGET_OBJECTS:py_pddl>)
ENDIF()
