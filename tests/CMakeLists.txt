find_package(PythonInterp)

if (PYTHONINTERP_FOUND)
    add_subdirectory(regression_tests)
else ()
    message(WARNING "Python interperter not found, required for regression tests.")
endif ()
