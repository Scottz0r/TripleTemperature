Push-Location $PSScriptRoot

# Assumes clang-format.exe is on system PATH.
clang-format.exe -i --verbose triple_temperature_uno/*.cpp triple_temperature_uno/*.h
clang-format.exe -i --verbose serial_tester_windows/*.cpp serial_tester_windows/*.h
clang-format.exe -i --verbose tests/*.cpp tests/*.h tests/mocks/*.cpp tests/mocks/*.h

Pop-Location
