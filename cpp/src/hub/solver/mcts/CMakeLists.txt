# Copyright (c) AIRBUS and its affiliates.
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
    generate_template_instantiation_files(
        PYTHON_MCTS_SOLVER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/py_mcts_solver.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "SequentialExecution!Seq;ParallelExecution!Par"
        "TtransitionMode" "StepTransitionMode!Stp;SampleTransitionMode!Sam;DistributionTransitionMode!Dis"
        "TtreePolicy" "DefaultTreePolicy!Dtp"
        "Texpander" "FullExpand!Fex;PartialExpand!Pex"
        "TactionSelectorOptimization" "UCB1ActionSelector!Oua;BestQValueActionSelector!Oqa"
        "TactionSelectorExecution" "UCB1ActionSelector!Eua;BestQValueActionSelector!Eqa"
        "TrolloutPolicy" "DefaultRolloutPolicy!Drp"
        "TbackPropagator" "GraphBackup!Gbk")
    
    ADD_LIBRARY(py_mcts STATIC
                ${CMAKE_CURRENT_SOURCE_DIR}/py_mcts.cc
                ${PYTHON_MCTS_SOLVER_TEMPLATE_FILES})
    TARGET_INCLUDE_DIRECTORIES(py_mcts PRIVATE ${INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(py_mcts ${LIBS})
    IF (MSVC)
        TARGET_COMPILE_OPTIONS(py_mcts PRIVATE "/bigobj")
    ENDIF()
    
    CMAKE_POLICY(SET CMP0079 NEW)
    TARGET_LINK_LIBRARIES(__skdecide_hub_cpp PRIVATE py_mcts)
ENDIF ()
