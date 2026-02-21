example_sources = ['doctest.cpp', 'tests.cpp']

test_exe = executable(
    meson.project_name(), 
    example_sources, 
    dependencies: [binaryio_dep, doctest_dep],
)

test('binary_io_test', test_exe)