cmake_minimum_required(VERSION 3.7.2)
project(Helix VERSION 0.0.1)
include(FetchContent)
FetchContent_Declare(
    JUCE 
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git 
    GIT_TAG 7.0.5
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(JUCE)
#find_package(JUCE CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
juce_add_plugin(
    Helix 
    PLUGIN_MANUFACTURER_CODE SMPI
    PLUGIN_CODE SM02
    FORMATS AU VST3 Standalone 
    PRODUCT_NAME "Helix"
)
juce_add_module(${CMAKE_CURRENT_SOURCE_DIR}/Modules/SDSP)

add_subdirectory(source)
juce_generate_juce_header(Helix)

target_sources(Helix PRIVATE 
    ${PROJECT_SOURCE}
)

target_include_directories(Helix PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/Asio
${CMAKE_CURRENT_SOURCE_DIR}/source)


target_compile_definitions(Helix PUBLIC 
JUCE_WEB_BROWSER=0
JUCE_USE_CURL=0
JUCE_VST3_CAN_REPLACE_VST2=0
JUCE_ASIO=1
)

#juce_add_binary_data(SprinklerData SOURCES
# Resources/NotoSansJP-Regular.otf
# Resources/SprinklerSliderDark.png
#)
target_link_libraries(Helix PRIVATE 
#SprinklerData
juce::juce_audio_utils 
juce::juce_dsp
SDSP
PUBLIC 
juce::juce_recommended_config_flags
juce::juce_recommended_lto_flags
juce::juce_recommended_warning_flags)