# Copyright (c) AIRBUS and its affiliates.
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

ADD_SUBDIRECTORY(mcts)
ADD_SUBDIRECTORY(riw)

IF (BUILD_PYTHON_BINDING OR ONLY_PYTHON)
        CMAKE_POLICY(SET CMP0079 NEW)
        # TARGET_LINK_LIBRARIES(__skdecide_hub_cpp PRIVATE
        #                         $<TARGET_OBJECTS:py_mcts>
        #                         $<TARGET_OBJECTS:py_riw>)
        # TARGET_SOURCES(__skdecide_hub_cpp PRIVATE
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_astar.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_aostar.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_mcts.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_iw.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_riw.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_bfws.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_lrtdp.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_ilaostar.cc
        #                 ${CMAKE_CURRENT_SOURCE_DIR}/py_martdp.cc)
ENDIF ()
