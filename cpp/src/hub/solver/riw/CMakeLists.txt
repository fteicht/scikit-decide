IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
        TARGET_SOURCES(__skdecide_hub_cpp PRIVATE
                       ${CMAKE_CURRENT_SOURCE_DIR}/py_riw.cc)
ENDIF ()
