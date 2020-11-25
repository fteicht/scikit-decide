IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
    generate_template_instantiation_files(
        PYTHON_RIW_SOLVER_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/py_riw_inst.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par"
        "Thashing_policy" "skdecide::StateFeatureHash!Sfa;skdecide::DomainStateHash!Dsh"
        "Trollout_policy" "skdecide::SimulationRollout!Sim;skdecide::EnvironmentRollout!Env")

    TARGET_SOURCES(__skdecide_hub_cpp PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}/py_riw.cc
                    ${PYTHON_RIW_SOLVER_TEMPLATE_FILES})
ENDIF ()
