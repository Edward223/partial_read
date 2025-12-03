
add_executable(pr_exam_exe main.cc)

target_link_libraries(pr_exam_exe PRIVATE ${ZSTD_LIB_PATH} pthread)

target_include_directories(pr_exam_exe
    PRIVATE
        ${CMAKE_SOURCE_DIR}/libpr    # 若头文件都在 libpr 根目录或其子目录
)