# Copyright (c) AIRBUS and its affiliates.
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
    generate_template_instantiation_files(
        PYTHON_DOMAIN_PROXY_BASE_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/python_domain_proxy_base.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par")
    
    generate_template_instantiation_files(
        PYTHON_DOMAIN_PROXY_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/python_domain_proxy.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par"
        "Tagent" "skdecide::SingleAgent!Sin;skdecide::MultiAgent!Mul"
        "Tobservability" "skdecide::PartiallyObservable!Pobs;skdecide::FullyObservable!FObs"
        "Tcontrollability" "skdecide::PartiallyControllable!Pcon;skdecide::FullyControllable!Fcon"
        "Tmemory" "skdecide::Markovian!Mar;skdecide::History!His")
    
    generate_template_instantiation_files(
        PYTHON_CONTAINER_PROXY_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/python_container_proxy.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par")

    generate_template_instantiation_files(
        PYTHON_HASH_EQ_TEMPLATE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/impl/python_hash_eq.cc.in"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "Texecution" "skdecide::SequentialExecution!Seq;skdecide::ParallelExecution!Par")
    
    TARGET_SOURCES(__skdecide_hub_cpp PRIVATE
                    ${PYTHON_DOMAIN_PROXY_BASE_TEMPLATE_FILES}
                    ${PYTHON_DOMAIN_PROXY_TEMPLATE_FILES}
                    ${PYTHON_CONTAINER_PROXY_TEMPLATE_FILES}
                    ${PYTHON_HASH_EQ_TEMPLATE_FILES})
ENDIF ()
