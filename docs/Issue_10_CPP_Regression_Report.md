# Issue #10 — C++ Regression Test Report

## 1. Environment
- Operating system: Windows
- Compiler: MinGW GCC 6.3.0
- CMake version: 4.4.5
- Build generator: MinGW Makefiles
- GUI: Disabled
- Testing: Enabled

## 2. Build
- Configure command:
  `cmake -S . -B build -G "MinGW Makefiles" -DBUILD_GUI=OFF -DBUILD_TESTING=ON`
- Build command:
  `cmake --build build --parallel 2`
- Build result: PASS
- Build completed successfully with all targets built.

## 3. Test execution
- Test command:
  `ctest --test-dir build --output-on-failure`
- Total tests: 8
- Passed: 8
- Failed: 0
- Not run: 0
- Overall result: 100% tests passed

## 4. Individual test results

| Test | Result |
|---|---|
| vehicle_manager_test | PASS |
| queue_manager_test | PASS |
| queue_empty_safety_test | PASS |
| queue_length_test | PASS |
| traffic_signal_test | PASS |
| traffic_manager_test | PASS |
| gui_model_test | PASS |
| input_validator_test | PASS |

## 5. Defects and observations
- No failures were observed in the eight registered C++ tests.
- This run used MinGW GCC 6.3.0 on Windows.
- GUI was disabled during this test run.
- These results do not establish REST API or browser acceptance.

## 6. Evidence
- Test output: `cpp-test-results.txt`

## 7. Conclusion
- Stage 1 C++ regression status: PASS
- Remaining work: Backend/API regression, browser acceptance, and final integration testing after the required dependencies are completed.
