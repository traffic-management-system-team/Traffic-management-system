# Final submission documentation reconciliation

Completed documentation reconciliation on 19 November 2026 for the 20 November viva. This register supersedes the initial audit's issue statuses only where supporting changes are identified below. The initial audit and every modified preexisting document are safely backed up with SHA-256 entries under history/final-reconciliation-2026-11-19/manifest.json.

## Final formal document editions

- [Final SRS revision 1.4](SRS/SRS_Traffic_Management_System_Final_Submission_2026-11-19.md)
- [Final SAD revision 1.4](SAD/SAD_Traffic_Management_System_Final_Submission_2026-11-19.md)
- [Final Test Plan revision 1.4](Test_Plan/Test_Plan_Traffic_Management_System_Final_Submission_2026-11-19.md)

Matching *_PROPOSED_Final_Submission_2026-11-19.docx files are proposed Word editions. Original DOCX/PDF files remain historical v1.3. The renderer failed with FileNotFoundError for LibreOffice soffice.exe on PATH. XML/package integrity and content are checked, but pagination, table layout and diagram rendering are NOT VERIFIED. Final Markdown content and Mermaid source diagrams are usable for review. Before submitting a required Word/PDF format, open the proposed files in Word, inspect tables/page breaks and export the approved final version. Instructor/team sign-off is not fabricated.

## Findings verified and disposition

| Initial gap | Verified repository finding | Reconciliation | Final status |
|---|---|---|---|
| P0 SRS desktop/browser mismatch | Original v1.3 desktop wording and obsolete MainWindow/Statistics references | Complete final SRS Markdown and proposed Word revision preserve IDs and history, describe React/C++/localhost and faithful queues | Content resolved; formal Word/PDF visual finalization open |
| P0 SAD missing service/HTTP/browser | Source has shared service, adapter/control API, serializer and Crow | Complete final SAD and four new component/sequence Mermaid sources map actual runtime | Content resolved; proposed Word visual check and rendered diagrams open |
| P0/P1 Test Plan stale environment/schedule | Original draft desktop plan uses October preparation dates | Final plan records CTest/Vitest/Playwright, actual Nov audit dates and spillover | Content resolved; proposed Word visual check open |
| P1 four reports old revision/counts | Reports belong to 42a57c4 and 38 tests | New dated reconciliation sections retain original reports verbatim and distinguish 33 current tests | Resolved for available evidence |
| P0 C++ branch measurement | Historical XML uses branch=False with no outcome counts | Branch report explicitly preserves NOT MEASURED and separates frontend 80.67% | OPEN evidence gap |
| P1 RTM NFR-01 absent frontend | App, both page components, controls and browser tests exist | Incorrect audit/RTM row corrected; all current RTM evidence refreshed and old CSV backed up | Resolved |
| P1 CSV structural inconsistency | Historical SEC-04 row has extra unquoted comma fields | Rewritten valid CSV with exact schema, preserved original bytes and explicit limitations | Resolved |
| P1 TC expectations for single page | Current UI has two pages with shared state/controls | Updated TC-08/12/14 mappings and current table preserve all TC IDs | Resolved for tested scope |
| P1 later work no iteration/schedule | GitHub project has 22 pointed assigned Done items, later 26â€“35 have no sprint | Factual sprint change log with actual dates, assignments, integration and scope changes | Documentation resolved; any approval/board changes require real evidence |
| P1 individual ownership/integration differences | Nitish assigned #7/#8, Pranav authored final #36/#37; all four have individual PRs | Explained without conflating assignment, author and merger | Documentation resolved; oral contribution explanation remains team responsibility |
| P0 sprint videos | Repeated repository search locates no actual files or submission links | NOT VERIFIED retained with required evidence | OPEN |
| P1 fresh C++ rebuild | Audit build stalled after MSBuild banner, interrupted | Every current report/plan/ledger states existing Release binary scope | Evidence limitation retained |
| P2 Architecture wording | Chromium suite exists despite future-testing language | Corrected current wording and linked final SAD | Resolved |
| P2 code comments in sim_config.h | Arrival/per-axis comments differ from model's rotating approach behavior | Formal documents describe actual behavior | Code comment unchanged by this documentation task |

## Reports and evidence

Updated Unit_Test_Report_Part3.md, System_Validation_Report_Part3.md, Code_Coverage_Report_Part3.md and Branch_Coverage_Report_Part3.md retain historical bodies and prepend current evidence. Test_Plan/Test_Case_Document_Part3.md and sprint-1/RTM.csv plus RTM_TC_Mapping.csv now reflect current code. See [Evidence ledger](FINAL_VERIFICATION_EVIDENCE_2026-11-19.md) and [Sprint change log](SPRINT_CHANGE_LOG_FINAL.md).

Recorded audit results, verified against available outputs, are Vitest33/33, existing Release CTest15/15, real-backend Chromium3/3, successful TypeScript/production frontend build, frontend lines91.24% and branches80.67%. Historical native line/block89.42% remains at42a57c4. Native branches are NOT MEASURED. These are existing recorded executions, not tests newly run during reconciliation. Native branch coverage cannot be inferred from a Cobertura root field. No overall branch percentage is claimed.

## Remaining university compliance gaps and priorities

P0 locate actual Sprint1/Sprint2 videos and verify playback, access and the mandated durations. P0 retain honest native branch limitation or obtain a real branch-capable measurement. P1 open/visually finalize the proposed formal Word editions and export final PDFs if required by submission format. P1 confirm team/instructor acceptance of evolved scope, without claiming an approval that is unavailable. Preserve native raw coverage evidence through an approved submission mechanism because it currently exists as local untracked data. Rehearse the startup and individual contribution explanations using real PRs.

**Final readiness verdict INSUFFICIENT EVIDENCE for complete university submission compliance.** The documentation content gaps are substantially reconciled, and recorded application tests pass. Video evidence, native branch measurement and visually verified final Word/PDF artifacts remain open. This verdict does not turn the interrupted native build into a successful rebuild.

## Validation performed

Structural validation results are appended after execution. Application/test changed paths are compared with the seven preexisting redesign files, and documentation-only editing operations are reviewed. The initial source-hash file was not retained by ephemeral TEMP, so byte-level baseline comparison is unavailable. Backup hashes, all24 requirement IDs, TC-01â€“16 presence, CSV field counts, DOCX XML/package integrity, relative documentation links and whitespace diffs are checked. The native coverage XML is independently recounted. No source/test edits, dependency installation, commit or push are part of this reconciliation.

### Completed structural checks

- All 16 backup SHA-256 values match. Original DOCX/PDF bytes match their backups.
- The changed application/test paths match the seven preexisting redesign files. Editing operations in this task target documentation only. The initial source-hash manifest was written to ephemeral TEMP and was not retained, so a start-to-finish byte comparison is unavailable.
- All 24 SRS IDs and revision-history entries are retained in the final SRS/proposed DOCX.
- Both CSVs parse with exactly 24 records and consistent schemas. TC-01â€“16 are present.
- All three proposed DOCX ZIP/XML packages parse successfully. This does not verify pagination or visual layout.
- All four historical report bodies remain in their refreshed reports.
- Native XML recount remains 1090/1219 lines. Native branches remain unmeasured.
- Relative links in the final register/formal Markdown editions resolve. Four final Mermaid source diagrams have expected document headers. Diagram rendering is not claimed.
- Original video search and live GitHub recheck results are recorded in the sprint change log. No video submission was found.

DOCX render attempted through the documents skill renderer. It failed before PDF generation because LibreOffice soffice.exe was unavailable on PATH. All Word revisions remain explicitly PROPOSED. Review their visual layout before final Word/PDF submission.

## File inventory and mandatory artifact check

| Artifact group | Changed or created files | Integrity and current status |
|---|---|---|
| Formal SRS | docs/SRS/SRS_Traffic_Management_System_Final_Submission_2026-11-19.md and matching PROPOSED DOCX | All24 requirement IDs, revision1.0–1.4 and final source references present. Word visual finalization open |
| Formal SAD | docs/SAD/SAD_Traffic_Management_System_Final_Submission_2026-11-19.md and matching PROPOSED DOCX | Actual components/endpoints/scheduler and concrete sequence sources included. Word visual finalization open |
| Formal Test Plan | docs/Test_Plan/Test_Plan_Traffic_Management_System_Final_Submission_2026-11-19.md and matching PROPOSED DOCX | Actual frameworks/dates/limits and TC references present. Word visual finalization open |
| Four reports | docs/Unit_Test_Report_Part3.md, System_Validation_Report_Part3.md, Code_Coverage_Report_Part3.md, Branch_Coverage_Report_Part3.md | Current sections added and original bodies preserved. Native branch gap open |
| Traceability | docs/sprint-1/RTM.csv, RTM_TC_Mapping.csv, docs/Test_Plan/Test_Case_Document_Part3.md |24 requirement mappings and all16 TC IDs verified |
| Audit correction | docs/SE_FINAL_AUDIT.md | NFR-01 factual correction and dated register pointer |
| Agile record | docs/SPRINT_CHANGE_LOG_FINAL.md | Actual iterations/PR dates and individual evidence. Videos NOT VERIFIED |
| Evidence/index | docs/FINAL_VERIFICATION_EVIDENCE_2026-11-19.md, docs/FINAL_SUBMISSION_RECONCILIATION.md | Commands, scopes, limitations and final disposition |
| Design entry points | docs/Architecture.md, Design_README.md | Links final revisions and corrects stale browser-testing wording |
| Diagram sources | diagrams/final-submission/component.mmd, arrival-processing.mmd, lifecycle.mmd, configuration.mmd | Actual modules and six endpoints. Source reviewed, rendered images not generated |
| Historical backups | docs/history/final-reconciliation-2026-11-19/ |16 byte-preserved inputs and SHA-256 manifest |

Final git diff --check passed. Branch remains feature/professional-traffic-ui at5cbdc01. Documentation changes are unstaged. The seven frontend working-tree modifications predate this reconciliation. FINAL_PROJECT_VERIFICATION_REPORT.md, coverage/ and redesign-screenshots/ remain existing untracked items.

## Approved Sprint 3 and final reviewed editions

Update recorded 19 November 2026: the team explicitly approved Sprint 3 — Web Migration (Retrospective Classification) for #26–35, 44 points, observed 4–14 November 2026 IST. Original Sprint 1/2 allocations remain unchanged; total estimated scope is 22 issues and 91 points. This is a current retrospective documentation approval, not verified historical planning, original GitHub assignment or instructor approval.

Revision 1.5 editable Markdown editions and new FINAL_REVIEW_SPRINT3 Word/PDF copies supersede the earlier proposed/1.4 final editions for submission. Earlier originals, proposed documents and reviewed 1.4 Word/PDF copies remain preserved; modified input backups are under docs/history/sprint-3-approved-2026-11-19/. The earlier visual-finalization-open statements above are historical and superseded only by the new render/inspection results in docs/FINAL_SUBMISSION_STATUS_2026-11-20.md.

- [SRS revision 1.5 Word](SRS/SRS_Traffic_Management_System_FINAL_REVIEW_SPRINT3_2026-11-19.docx) and [PDF](SRS/SRS_Traffic_Management_System_FINAL_REVIEW_SPRINT3_2026-11-19.pdf).
- [SAD revision 1.5 Word](SAD/SAD_Traffic_Management_System_FINAL_REVIEW_SPRINT3_2026-11-19.docx) and [PDF](SAD/SAD_Traffic_Management_System_FINAL_REVIEW_SPRINT3_2026-11-19.pdf).
- [Test Plan revision 1.5 Word](Test_Plan/Test_Plan_Traffic_Management_System_FINAL_REVIEW_SPRINT3_2026-11-19.docx) and [PDF](Test_Plan/Test_Plan_Traffic_Management_System_FINAL_REVIEW_SPRINT3_2026-11-19.pdf).

Backlog scope increased by 44 story points; a historical rise in the burndown chart has not been independently verified.

The original testing results/dates and historical-versus-current limits are unchanged. Videos, native branch measurement, fresh native rebuild completion and instructor acceptance of evolved scope remain unverified. Team approval of this documentation classification does not establish complete university compliance. The earlier reconciliation proposal and issue audit are retained as dated evidence; the current approved addendum is in docs/SPRINT_CHANGE_LOG_FINAL.md.
