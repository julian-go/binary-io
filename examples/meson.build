executable(
    'zip_example', 
    ['zip_example/main.cpp',], 
    dependencies: [binaryio_dep],
    cpp_args : ['-DSOURCE_DIR="' + meson.current_source_dir() / 'zip_example' + '"'],
    override_options : ['cpp_std=c++17'],
)
