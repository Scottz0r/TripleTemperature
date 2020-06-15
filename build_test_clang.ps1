$target_dir = "$PsScriptRoot\debug"
$test_root = "$PsScriptRoot/tests"
$tt = "$PsScriptRoot/triple_temperature_uno"
$target = "TripleTemperaturetests.exe"

if(-not(Test-Path $target_dir))
{
    mkdir $target_dir
}

Push-Location $target_dir

clang++ --coverage `
    -I $tt -I $test_root/mocks -I $env:BOOST_PATH `
    $test_root/*.cpp `
    $test_root/mocks/*.cpp `
    $tt/*.cpp `
    -o $target

if($lastExitCode -eq 0)
{
    Write-Output "Running test executable..."
    & ".\$target"

    Write-Output "Generating coverage with grcov..."
    ..\grcov.exe . -s ..\triple_temperature_uno\ -t html --llvm --branch --ignore-not-existing -o ./coverage/
}
else
{
    Write-Warning "Build failed."
}

Pop-Location
