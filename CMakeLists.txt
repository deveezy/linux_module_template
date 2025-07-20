cmake_minimum_required(VERSION 3.20)

project(driver LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Module info
add_definitions(-D__KERNEL__ -DMODULE)

# Путь к headers
set(KERNELHEADERS_DIR /usr/src/linux/build/include)
# message(FATAL_ERROR ${KERNELHEADERS_DIR})

# Add header files
include_directories(${KERNELHEADERS_DIR})

# Имя модуля
set(DRIVER_NAME mydriver)

# Папка с исходниками
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB DRIVER_SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.c)

# Папки сборки
set(MODWORK_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(MODDEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/modbuild)

file(MAKE_DIRECTORY ${MODWORK_DIR})
file(MAKE_DIRECTORY ${MODDEST_DIR})

# Указание на каталог сборки ядра
set(KERNEL_BUILD_DIR /usr/src/linux/build)

# Генерируем Kbuild-файл в каталоге сборки
set(KBUILD_PATH ${MODWORK_DIR}/Kbuild)

# Список .c файлов -> список .o файлов
set(OBJECTS "")
foreach(src_file ${DRIVER_SOURCES})
  get_filename_component(name ${src_file} NAME_WE)
  list(APPEND OBJECTS "${name}.o")
endforeach()

# Генерируем строку mydriver-y := ...
string(REPLACE ";" " " OBJECTS_LINE "${OBJECTS}")
file(WRITE ${KBUILD_PATH}
     "obj-m := ${DRIVER_NAME}.o\n${DRIVER_NAME}-y := ${OBJECTS_LINE}\n")

foreach(src_file ${DRIVER_SOURCES})
  get_filename_component(name ${src_file} NAME)
  configure_file(${src_file} ${MODWORK_DIR}/${name} COPYONLY)
endforeach()

# Команда сборки: make + копирование .ko в modbuild
add_custom_command(
  OUTPUT ${MODDEST_DIR}/${DRIVER_NAME}.ko
  COMMAND make -C ${KERNEL_BUILD_DIR} M=${MODWORK_DIR} modules
  COMMAND ${CMAKE_COMMAND} -E copy ${MODWORK_DIR}/${DRIVER_NAME}.ko
          ${MODDEST_DIR}/${DRIVER_NAME}.ko
  WORKING_DIRECTORY ${MODWORK_DIR}
  DEPENDS ${DRIVER_SOURCES}
  VERBATIM)

add_custom_target(driver ALL DEPENDS ${MODDEST_DIR}/${DRIVER_NAME}.ko)

add_custom_target(
  driver_clean
  COMMAND make -C ${KERNEL_BUILD_DIR} M=${MODWORK_DIR} clean
  COMMAND ${CMAKE_COMMAND} -E rm -f ${MODDEST_DIR}/${DRIVER_NAME}.ko
  WORKING_DIRECTORY ${MODWORK_DIR}
  COMMENT "Cleaning kernel module build")

# Parse c files
add_library(dummy_target ${DRIVER_SOURCES})
set_target_properties(dummy_target PROPERTIES EXCLUDE_FROM_ALL ON)
