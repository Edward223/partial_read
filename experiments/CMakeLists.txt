
set(PR_EXAM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)

file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
)

file(GLOB UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
)

list(APPEND PR_EXAM_SOURCES ${COMMON_SOURCES} ${UTILS_SOURCES})

add_executable(pr_exam_exe ${PR_EXAM_SOURCES})

target_link_libraries(pr_exam_exe PRIVATE ${ZSTD_LIB_PATH} pthread)

target_include_directories(pr_exam_exe
    PRIVATE
        ${CMAKE_SOURCE_DIR}/libpr
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(pr_exam_exe
    PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)