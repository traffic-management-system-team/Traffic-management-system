# Final verification evidence

Reconciled 19 November 2026 for the 20 November 2026 viva. Source baseline 5cbdc012591221ccf300a9656b55bb1c5c38a2d2, branch feature/professional-traffic-ui, plus the seven existing uncommitted frontend redesign files. This documentation change does not commit or approve the redesign. Execution results below belong to the 19 November audit working tree. Historical results retain their original dates and revisions. No new execution is implied by this reconciliation.

## Verified recorded execution on 19 November

Evidence is the completed audit tool output and docs/SE_FINAL_AUDIT.md sections D/E, checked against actual source/tests. These values are reproduced, not described as new reconciliation executions. The audit's preserved pre-reconciliation copy is under docs/history/final-reconciliation-2026-11-19/docs/SE_FINAL_AUDIT.md.

| Command | Scope | Actual recorded result |
|---|---|---|
| npm run type-check | Frontend TypeScript | Exit 0 |
| npm test | Four Vitest files | 33 passed, 0 failed, 0 skipped |
| npm run build | Frontend production | Exit 0, 28 modules |
| ctest --test-dir build/release -C Release --output-on-failure | Existing Release binaries | 15 passed, 0 failed after loopback permission |
| npm run test:e2e | Real API and Chromium | 3 passed, 0 failed, 0 skipped after API startup, 28.7 seconds |
| Vitest V8 coverage | Frontend src TS/TSX | 33 tests passed with 20-second test timeout |
| cmake --build build/release --config Release --parallel 2 | Attempted fresh native rebuild | Stalled after MSBuild banner, interrupted, NOT VERIFIED |

Initial sandbox CTest failed two HTTP targets with 0xc0000409. The socket-enabled rerun passed all 15. Initial Playwright had 2 failed/1 passed because API 18080 was absent and proxy returned 502/ECONNREFUSED. Starting the documented API resolved this and all three passed. These failures remain recorded rather than deleted.

## Coverage identity

| Metric | Covered / total | Result | Revision/scope |
|---|---|---|---|
| Frontend lines | 271/297 | 91.24% | Audit working tree, V8 |
| Frontend statements | 282/311 | 90.67% | Same |
| Frontend functions | 85/91 | 93.40% | Same |
| Frontend branches | 263/326 | 80.67% | Same |
| C++ line/block | 1090/1219 | 89.42% | Historical 42a57c4, Microsoft collector XML recalculated during audit |
| C++ branches | unavailable | NOT MEASURED | XML line branch=False supplies no branch outcomes |

Audit coverage command from frontend was npx --no-install vitest run --coverage --coverage.provider=v8 --coverage.reportsDirectory=$env:TEMP/tms-se-audit-coverage --coverage.include=src/**/*.ts --coverage.include=src/**/*.tsx --testTimeout=20000. The locally installed provider is not a package.json dependency. Existing report describes version-matched temporary installation. Durable availability of fresh raw frontend coverage output is NOT VERIFIED because later TEMP lookup did not locate the package. Do not claim that raw data was archived.

Historical native raw XML and collector logs remain in coverage/msvc-debug-42a57c45311551058c41ccf54fec8c2ca88215d2/. The XML was independently parsed and unique src/include executable lines unioned to obtain 1090/1219. Test/dependency lines are excluded. api_server_main.cpp was not measured. A native branch-rate root field is not a native branch measurement.

## Limits

Fresh native build completion, fresh sanitizers/static analysis, deployed environment, broad browser matrix, quantified target-device performance and full manual assistive-technology review remain unexecuted/unverified. GitHub PR #53's eight successful CI jobs apply to head 88c9a00 and were verified during the audit. They do not prove the current uncommitted redesign's CI status. New documentation structural validation is recorded in FINAL_SUBMISSION_RECONCILIATION.md.
