add_executable(Main "main.cpp")
set(IMGUI_SOURCES
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui.cpp"
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui_draw.cpp"
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui_stdlib.cpp"
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui_widgets.cpp"
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui_tables.cpp"
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui_impl_dx9.cpp"
    "${BASE_INCLUDE_PATH}/IMGUI-DX9/imgui_impl_win32.cpp"
)
target_sources(Main PRIVATE ${IMGUI_SOURCES})
