cmake_minimum_required(VERSION 3.0.0)
project(extensions VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./logic/")
add_subdirectory("./base/")
add_subdirectory("./commons/")
add_subdirectory("./setter/")
add_subdirectory("./exchanger/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    extensions_base
    extensions_commons
    logic_ext
    setter_ext
    exchanger_ext
)

set(EXTENSIONS_INCLUDE_DIR 
    ${EXTENSIONS_BASE_INCLUDE_DIR}
    ${EXTENSIONS_COMMONS_INCLUDE_DIR}
    ${LOGIC_EXT_INCLUDE_DIR}
    ${SETTER_EXT_INCLUDE_DIR}
    ${EXCHANGER_EXT_INCLUDE_DIR}
)
# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)