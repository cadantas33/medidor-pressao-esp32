# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/caioc/esp/v5.3.1/esp-idf/components/bootloader/subproject"
  "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader"
  "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix"
  "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix/tmp"
  "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix/src"
  "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Games/PI-II/ESP32-Gauge/esp32-gauge/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
