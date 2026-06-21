
# Compile a sample app with the static library
add_executable(sample_app_static main.cpp)
target_link_libraries(sample_app_static PRIVATE libpks)

# And also with the dynamic library
add_executable(sample_app_dynamic main.cpp)
target_link_libraries(sample_app_dynamic PRIVATE libpks_dll)
