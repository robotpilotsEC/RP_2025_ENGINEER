cmake_minimum_required(VERSION 3.14)

#####################################
# BMI08x Sensor API Library
#####################################

add_library(BMI08x_SensorAPI STATIC
            bmi08a.c
            bmi08g.c
            bmi08xa.c
            bmi088_anymotiona.c
            bmi088_mma.c)

target_include_directories(BMI08x_SensorAPI PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR})