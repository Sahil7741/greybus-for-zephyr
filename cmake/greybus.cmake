#
# Copyright (c) 2020 Friedt Professional Engineering Services, Inc
#
# SPDX-License-Identifier: BSD-3-Clause

set(G4Z_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

function(devicetree_unfixed_h_to_mnfs
    source_file    # The source file to be converted
    generated_file # The generated file
    )
  # Determine which optional descriptors to include based on Kconfig
  # MikroBUS/DEVICE/PROPERTY are non-standard on Linux; include only when enabled.
  set(_GBUTIL_INCLUDE_MIKROBUS "0")
  if(CONFIG_GREYBUS_MIKROBUS)
    set(_GBUTIL_INCLUDE_MIKROBUS "1")
  endif()
  set(_GBUTIL_INCLUDE_DEVICE "0")
  if(CONFIG_GREYBUS_INCLUDE_DEVICE_DESC)
    set(_GBUTIL_INCLUDE_DEVICE "1")
  endif()
  set(_GBUTIL_INCLUDE_PROPERTY "0")
  if(CONFIG_GREYBUS_INCLUDE_PROPERTY_DESC)
    set(_GBUTIL_INCLUDE_PROPERTY "1")
  endif()

  add_custom_command(
    OUTPUT ${generated_file}
    COMMAND
    ${CMAKE_COMMAND} -E env
    GBUTIL_INCLUDE_MIKROBUS_DESC=${_GBUTIL_INCLUDE_MIKROBUS}
    GBUTIL_INCLUDE_DEVICE_DESC=${_GBUTIL_INCLUDE_DEVICE}
    GBUTIL_INCLUDE_PROPERTY_DESC=${_GBUTIL_INCLUDE_PROPERTY}
    ${PYTHON_EXECUTABLE}
    ${G4Z_DIR}/scripts/gbutil.py
    ${ARGN} # Extra arguments are passed to gbutil.py
    ${source_file}
    ${generated_file}
    DEPENDS ${source_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

function(mnfs_to_mnfb
    source_file    # The source file to be converted
    generated_file # The generated file
    )
  add_custom_command(
    OUTPUT ${generated_file}
    COMMAND
    ${PYTHON_EXECUTABLE}
    ${G4Z_DIR}/scripts/manifesto/manifesto
    -I mnfs
    -O mnfb
    -o ${generated_file}
    ${source_file}
    DEPENDS ${source_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

function(mnfb_to_mnfs
    source_file    # The source file to be converted to hex
    generated_file # The generated file
    )
  add_custom_command(
    OUTPUT ${generated_file}
    COMMAND
    ${PYTHON_EXECUTABLE}
    ${G4Z_DIR}/scripts/manifesto/manifesto
    -I mnfb
    -O mnfs
    -o ${generated_file}
    ${source_file}
    DEPENDS ${source_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()
