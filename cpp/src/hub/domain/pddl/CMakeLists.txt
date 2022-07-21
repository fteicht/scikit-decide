generate_template_instantiation_files(
        AGGREGATION_EFFECT_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/aggregation_effect.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "ConjunctionEffect!Con;DisjunctionEffect!Dis")

generate_template_instantiation_files(
        AGGREGATION_FORMULA_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/aggregation_formula.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "ConjunctionFormula!Con;DisjunctionFormula!Dis")

generate_template_instantiation_files(
        ASSIGNMENT_EFFECT_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/assignment_effect.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "AssignEffect!Asg;ScaleUpEffect!ScU;ScaleDownEffect!ScD;IncreaseEffect!Inc;DecreaseEffect!Dec")

generate_template_instantiation_files(
        ASSOCIATIVE_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/associative_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom;Problem!Pbm"
        "Tsymbol" "Class!Cls;Predicate!Prd;DerivedPredicate!Dpr;Function!Fun;Object!Obj;Preference!Prf;Type!Typ;Action!Act;DurativeAction!Dac;Event!Evt;Process!Prc")

generate_template_instantiation_files(
        BINARY_EXPRESSION_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/binary_expression.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "AddExpression!Ade;SubExpression!Sue;MulExpression!Mue;DivExpression!Die;GreaterFormula!Grf;GreaterEqFormula!Gef;LessFormula!Lsf;LessEqFormula!Lef;EqFormula!Eqf")

generate_template_instantiation_files(
        BINARY_FORMULA_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/binary_formula.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "SometimeAfterFormula!Saf;SometimeBeforeFormula!Sbf;AlwaysWithinFormula!Awf;ImplyFormula!Imf")

generate_template_instantiation_files(
        CLASS_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/class_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom")

ADD_LIBRARY(skdecide_pddl STATIC
        pddl.cc
        domain.cc
        problem.cc
        requirements.cc
        type.cc
        ${AGGREGATION_EFFECT_TEMPLATE_FILES}
        impl/aggregation_effect.cc
        ${AGGREGATION_FORMULA_TEMPLATE_FILES}
        impl/aggregation_formula.cc
        ${ASSIGNMENT_EFFECT_TEMPLATE_FILES}
        impl/assignment_effect.cc
        ${ASSOCIATIVE_CONTAINER_TEMPLATE_FILES}
        impl/binary_effect.cc
        ${BINARY_EXPRESSION_TEMPLATE_FILES}
        ${BINARY_FORMULA_TEMPLATE_FILES}
        ${CLASS_CONTAINER_TEMPLATE_FILES}
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
