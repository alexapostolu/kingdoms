# Collect all source files in the engine/src directory
file(GLOB ENGINE_SOURCE_FILES "*.cpp")
set(CMAKE_CXX_STANDARD 20)

# Create a static library for the engine
add_library(engine STATIC ${ENGINE_SOURCE_FILES})

# Include the directories for the engine
target_include_directories(engine PUBLIC
    ${PROJECT_SOURCE_DIR}/engine/include
)

# Specify the SDL2, SDL2_image, and SDL2_ttf libraries for linking if necessary
target_link_libraries(engine
    ${SDL2_DIR}/lib/x64/SDL2.lib
    ${SDL2_image_DIR}/lib/x64/SDL2_image.lib
    ${SDL2_ttf_DIR}/lib/x64/SDL2_ttf.lib
)

# Copy SDL, SDL_image, and SDL_ttf DLLs into the build directory for the engine (if needed)
add_custom_command(TARGET engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DIR}/lib/x64/SDL2.dll
        $<TARGET_FILE_DIR:engine>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_image_DIR}/lib/x64/SDL2_image.dll
        $<TARGET_FILE_DIR:engine>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_ttf_DIR}/lib/x64/SDL2_ttf.dll
        $<TARGET_FILE_DIR:engine>
)
