# Copyright (c) AIRBUS and its affiliates.
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
    generate_template_instantiation_files(
        PYTHON_MCTS_SOLVER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/py_mcts_solver.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par"
        "TtransitionMode" "skdecide::StepTransitionMode!Stp;skdecide::SampleTransitionMode!Sam;skdecide::DistributionTransitionMode!Dis"
        "TtreePolicy" "skdecide::DefaultTreePolicy!Dtp"
        "Texpander" "skdecide::FullExpand!Fex;skdecide::PartialExpand!Pex"
        "TactionSelectorOptimization" "skdecide::UCB1ActionSelector!Oua;skdecide::BestQValueActionSelector!Oqa"
        "TactionSelectorExecution" "skdecide::UCB1ActionSelector!Eua;skdecide::BestQValueActionSelector!Eqa"
        "TrolloutPolicy" "skdecide::DefaultRolloutPolicy!Drp"
        "TbackPropagator" "skdecide::GraphBackup!Gbk")

    TARGET_SOURCES(__skdecide_hub_cpp PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/py_mcts.cc
                    ${PYTHON_MCTS_SOLVER_TEMPLATE_FILES})
ENDIF ()
