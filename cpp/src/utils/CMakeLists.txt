IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
    generate_template_instantiation_files(
        PYTHON_DOMAIN_PROXY_BASE_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/python_domain_proxy_base.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par")
    TARGET_SOURCES(__skdecide_hub_cpp PRIVATE ${PYTHON_DOMAIN_PROXY_BASE_TEMPLATE_FILES})
ENDIF ()
