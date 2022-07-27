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
        ASSOCIATIVE_CONTAINER_DOMAIN_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/associative_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom"
        "Tsymbol" "Class!Cls;Predicate!Prd;DerivedPredicate!Dpr;Function!Fun;Object!Obj;Preference!Prf;Type!Typ;Action!Act;DurativeAction!Dac;Event!Evt;Process!Prc")

generate_template_instantiation_files(
        ASSOCIATIVE_CONTAINER_PROBLEM_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/associative_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Problem!Pbm"
        "Tsymbol" "Object!Obj;Preference!Prf")

generate_template_instantiation_files(
        ASSOCIATIVE_CONTAINER_TYPE_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/associative_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Type!Tpe;Object!Obj;Variable!Var"
        "Tsymbol" "Type!Tpe")

generate_template_instantiation_files(
        ASSOCIATIVE_CONTAINER_CLASS_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/associative_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Class!Cls"
        "Tsymbol" "Function!Fun")

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

generate_template_instantiation_files(
        COMPARISON_FORMULA_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/comparison_formula.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "GreaterFormula!GFm;GreaterEqFormula!Gef;LessEqFormula!Lef;LessFormula!Lfm;EqFormula!Efm")

generate_template_instantiation_files(
        DERIVED_PREDICATE_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/derived_predicate_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom")

generate_template_instantiation_files(
        FUNCTION_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/function_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom;Class!Cls")

generate_template_instantiation_files(
        NUMBER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/number.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tnumber" "long!long;long long!llong;int!int;unsigned int!uint;long int!lint;long unsigned int!luint;double!dbl;long double!ldbl;float!flt")

generate_template_instantiation_files(
        OBJECT_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/object_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom;Problem!Pbm")

generate_template_instantiation_files(
        OPERATOR_CONTAINER_BASE_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/operator_container_base.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom"
        "Toperator" "Action!Act;DurativeAction!Dac;Event!Evt;Process!Prc")

generate_template_instantiation_files(
        OPERATOR_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/operator_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom")

generate_template_instantiation_files(
        OPERATOR_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/operator.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Action!Act;DurativeAction!Dac;Event!Evt;Process!Prc")

generate_template_instantiation_files(
        PREDICATE_CONTAINER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/predicate_container.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Tderived" "Domain!Dom")

ADD_LIBRARY(skdecide_pddl STATIC
        problem.cc
        requirements.cc
        type.cc
        ${AGGREGATION_EFFECT_TEMPLATE_FILES}
        impl/aggregation_effect.cc
        ${AGGREGATION_FORMULA_TEMPLATE_FILES}
        impl/aggregation_formula.cc
        ${ASSIGNMENT_EFFECT_TEMPLATE_FILES}
        impl/assignment_effect.cc
        ${ASSOCIATIVE_CONTAINER_DOMAIN_TEMPLATE_FILES}
        ${ASSOCIATIVE_CONTAINER_PROBLEM_TEMPLATE_FILES}
        ${ASSOCIATIVE_CONTAINER_TYPE_TEMPLATE_FILES}
        ${ASSOCIATIVE_CONTAINER_CLASS_TEMPLATE_FILES}
        impl/binary_effect.cc
        ${BINARY_EXPRESSION_TEMPLATE_FILES}
        ${BINARY_FORMULA_TEMPLATE_FILES}
        ${CLASS_CONTAINER_TEMPLATE_FILES}
        impl/class.cc
        ${COMPARISON_FORMULA_TEMPLATE_FILES}
        impl/comparison_formula.cc
        impl/conditional_effect.cc
        impl/constraint_formula.cc
        ${DERIVED_PREDICATE_CONTAINER_TEMPLATE_FILES}
        impl/derived_predicate.cc
        impl/domain.cc
        impl/duration_effect.cc
        impl/duration_expression.cc
        impl/duration_formula.cc
        impl/effect.cc
        impl/equality_formula.cc
        impl/expression.cc
        impl/formula.cc
        ${FUNCTION_CONTAINER_TEMPLATE_FILES}
        impl/function_expression.cc
        impl/function.cc
        impl/identifier.cc
        impl/imply_formula.cc
        impl/minus_expression.cc
        impl/negation_effect.cc
        impl/negation_formula.cc
        ${NUMBER_TEMPLATE_FILES}
        impl/number.cc
        impl/numerical_expression.cc
        ${OBJECT_CONTAINER_TEMPLATE_FILES}
        impl/object.cc
        impl/operation_expression.cc
        ${OPERATOR_CONTAINER_BASE_TEMPLATE_FILES}
        ${OPERATOR_CONTAINER_TEMPLATE_FILES}
        ${OPERATOR_TEMPLATE_FILES}
        impl/operator.cc
        impl/optimization_expression.cc
        impl/pddl.cc
        ${PREDICATE_CONTAINER_TEMPLATE_FILES}
        impl/preference.cc
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
