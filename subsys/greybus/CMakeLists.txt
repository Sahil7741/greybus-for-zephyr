# SPDX-License-Identifier: BSD-3-Clause

include(../../cmake/greybus.cmake)

zephyr_include_directories(../../../include)

zephyr_library()

zephyr_library_sources(
  greybus-stubs.c
  greybus-core.c
  greybus-manifest.c
  platform/manifest.c
  platform/platform.c
  platform/service.c
  platform/certificate.c
  platform/bundle.c
  platform/bus.c
  platform/control.c
  platform/interface.c
  platform/string.c)

  # Manually generate greybus.mnfs and greybus.mnfb
set(MANIFEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/control.mnfs)
set(MANIFEST_OUT ${ZEPHYR_BINARY_DIR}/zephyr/include/generated/greybus.mnfs)
set(MANIFEST_BIN ${ZEPHYR_BINARY_DIR}/zephyr/include/generated/greybus.mnfb)

add_custom_command(
    OUTPUT ${MANIFEST_OUT}
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/manifesto/manifesto
            -I mnfs
            -O mnfs
            -o ${MANIFEST_OUT}
            ${MANIFEST_SRC}
    DEPENDS ${MANIFEST_SRC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating greybus.mnfs"
)

add_custom_command(
    OUTPUT ${MANIFEST_BIN}
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/manifesto/manifesto
            -I mnfs
            -O mnfb
            -o ${MANIFEST_BIN}
            ${MANIFEST_OUT}
    DEPENDS ${MANIFEST_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating greybus.mnfb"
)

add_custom_target(greybus_manifest DEPENDS ${MANIFEST_OUT} ${MANIFEST_BIN})
add_dependencies(app greybus_manifest)

if(CONFIG_GREYBUS_MANIFEST_BUILTIN)

  devicetree_unfixed_h_to_mnfs(
    ${PROJECT_BINARY_DIR}/include/generated/zephyr/devicetree_generated.h
    ${PROJECT_BINARY_DIR}/include/generated/greybus.mnfs)

  mnfs_to_mnfb(${PROJECT_BINARY_DIR}/include/generated/greybus.mnfs
               ${PROJECT_BINARY_DIR}/include/generated/greybus.mnfb)

  generate_inc_file_for_target(
    app ${PROJECT_BINARY_DIR}/include/generated/greybus.mnfb
    ${PROJECT_BINARY_DIR}/include/generated/greybus_mnfb.inc)
endif()

if(CONFIG_GREYBUS_TLS_BUILTIN)
  set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

  if(CONFIG_GREYBUS_TLS_CLIENT_VERIFY_OPTIONAL
     OR CONFIG_GREYBUS_TLS_CLIENT_VERIFY_REQUIRED)
    generate_inc_file_for_target(app ${CONFIG_GREYBUS_TLS_BUILTIN_CA_CERT}
                                 ${gen_dir}/greybus_tls_builtin_ca_cert.inc)
  endif()

  generate_inc_file_for_target(app ${CONFIG_GREYBUS_TLS_BUILTIN_SERVER_CERT}
                               ${gen_dir}/greybus_tls_builtin_server_cert.inc)

  generate_inc_file_for_target(
    app ${CONFIG_GREYBUS_TLS_BUILTIN_SERVER_PRIVKEY}
    ${gen_dir}/greybus_tls_builtin_server_privkey.inc)
endif()

zephyr_library_sources_ifdef(CONFIG_GREYBUS_XPORT_TCPIP_MULTIPLEX
                             platform/transport-tcpip-multiplex.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_XPORT_TCPIP
                             platform/transport-tcpip.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_XPORT_UART
                             platform/transport-uart.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_CONTROL control-gpb.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_AUDIO audio.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_CAMERA camera.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_GPIO gpio.c platform/gpio.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_HID hid.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_I2C i2c.c platform/i2c.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_LIGHTS lights.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_LOOPBACK loopback.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_POWER_SUPPLY power_supply.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_PWM pwm-protocol.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_SDIO sdio.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_SPI spi.c platform/spi.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_UART uart.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_USB usb.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_VIBRATOR vibrator.c)
zephyr_library_sources_ifdef(CONFIG_GREYBUS_CLICK_MANIFEST_CLICKID
                             platform/mikrobus.c platform/w1-gpio.c)
