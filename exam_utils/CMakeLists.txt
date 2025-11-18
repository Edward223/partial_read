
add_executable(pr_exam_exe main.cc)

target_link_libraries(pr_exam_exe PRIVATE ${ZSTD_LIB_PATH})