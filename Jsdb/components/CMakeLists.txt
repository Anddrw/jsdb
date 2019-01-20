FILE(GLOB JsdbComponentsSrc *.cpp *.hpp)

# Create components library
add_library(JsdbComponents ${JsdbComponentsSrc})