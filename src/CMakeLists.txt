file(GLOB SOURCE_FILES "*.cpp")
add_executable(kingdoms ${SOURCE_FILES} ${SDL_fontcache_DIR}/SDL_FontCache.c)

target_link_libraries(kingdoms engine)

# Specify the SDL2 and SDL2_ttf library directories
target_link_libraries(kingdoms
    ${SDL2_DIR}/lib/x64/SDL2main.lib
    ${SDL2_DIR}/lib/x64/SDL2.lib 
    ${SDL2_ttf_DIR}/lib/x64/SDL2_ttf.lib
    ${SDL2_image_DIR}/lib/x64/SDL2_image.lib
)

# Copy SDL, SDL_image, and SDL_ttf dlls into build
add_custom_command(TARGET kingdoms POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DIR}/lib/x64/SDL2.dll
        $<TARGET_FILE_DIR:kingdoms>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_image_DIR}/lib/x64/SDL2_image.dll
        $<TARGET_FILE_DIR:kingdoms>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_ttf_DIR}/lib/x64/SDL2_ttf.dll
        $<TARGET_FILE_DIR:kingdoms>
)