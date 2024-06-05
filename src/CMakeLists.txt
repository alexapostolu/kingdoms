add_executable(kingdoms main.cpp)

# Specify the SDL2 library directories
target_link_libraries(kingdoms 
    ${PROJECT_SOURCE_DIR}/external/SDL2/lib/x64/SDL2.lib 
    ${PROJECT_SOURCE_DIR}/external/SDL2/lib/x64/SDL2main.lib
)