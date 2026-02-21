project('binary-io', 'cpp', default_options: ['warning_level=3', 'cpp_std=c++17'],) 

# doctest library
doctest_proj = subproject('doctest')
doctest_dep = doctest_proj.get_variable('doctest_dep')

# binary-io headers only
subdir('include')

# tests
subdir('tests')