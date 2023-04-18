# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

#find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(BOARD nrf52dk_nrf52832)

find_package(Zephyr)
project(hog)

include_directories(app PRIVATE inc)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE
  ${app_sources}
  )

# zephyr_compile_options(-save-temps)
