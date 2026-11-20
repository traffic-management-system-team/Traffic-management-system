# Sprint 3 reconciliation report

Prepared 19 November 2026 for review. Repository: [Traffic Management System](https://github.com/traffic-management-system-team/Traffic-management-system). Inspected workspace: `C:\Users\prana\Downloads\Traffic-management-final-verification`, branch `feature/professional-traffic-ui`, HEAD `5cbdc012591221ccf300a9656b55bb1c5c38a2d2` plus existing local work.

**Recommendation:** classify issues #26–35 as **“Sprint 3 — web migration, retrospective classification”**, subject to team review, with an **observed activity window of 4–14 November 2026 IST**. The equivalent UTC calendar window is **4–13 November**. This range follows dated backlog creation, coordination, development records and final issue closure. It is not a recovered formal sprint timebox. No evidence establishes a meeting that planned or approved a sprint named Sprint 3.

The original Sprint 1 and Sprint 2 allocations remain unchanged. The proposed label groups ten additional migration issues, estimated at 44 points, alongside the original 47 points, giving 91 total recorded backlog points. A historical burndown increase is **NOT VERIFIED**: the available evidence establishes additional scope, not an observed historical remaining-work curve.

Ready-to-insert text is provided separately in [proposed documentation changes](SPRINT_3_PROPOSED_DOCUMENTATION_CHANGES.md). Neither existing formal documents nor the Sprint Change Log were edited.

## 1 Evidence and method

Read-only live GitHub checks retrieved issue creation/closure timestamps, current bodies, comments, project points/iteration fields, and the ten primary merged PRs with complete commit connections. Related integration/acceptance/two-page PRs #41, #51 and #53 were also inspected. Commit connections had no further page; PR #46 contains 49 commits and #49 contains 25, so their full retrieved sets were considered rather than assuming a single commit per issue.

Local records reviewed were [Sprint Change Log](SPRINT_CHANGE_LOG_FINAL.md), [issue alignment audit](ISSUE_DOCUMENT_ALIGNMENT_AUDIT.md), the original [Part 1 Sprint Backlog](Sprint/Part1_Sprint_Backlog.docx) and [Work Distribution](Sprint/Part1_Work_Distribution.docx), and actual FINAL_REVIEW SRS §4 and SAD §7. Search covered local text/file names, reachable Git history filenames/messages, and the complete non-truncated remote main file tree for sprint/burndown/Agile/backlog/spreadsheet artifacts.

The [GitHub project](https://github.com/orgs/traffic-management-system-team/projects/1) is private. Its current iteration configuration contains only completed **Sprint 1 (start 19 October, duration 5 days)** and **Sprint 2 (start 26 October, duration 5 days)**. There is no configured Sprint 3. All ten proposed Sprint 3 items have null iteration and current status Done. Every current issue body #26–35 explicitly says “Backlog; future sprint placement and dates are not assigned.”

No GitHub fields, issue bodies, iterations, original documents, application code or tests were changed. No commit or push was performed. The separate Issue #27 integration worktree was not accessed.

### What the dates mean

- Issue creation proves when the work item was recorded, not when coding started.
- A dated workflow comment proves documented coordination; it does not establish a meeting, sprint ceremony or a Sprint 3 timebox.
- Authored/committed timestamps describe source-control records. They do not measure developer hours, guarantee the first moment of work, or prove independent acceptance.
- PR creation and merge are server-recorded events. Merge and issue closure are listed separately; neither alone proves every acceptance criterion.
- UTC is used for exact audit timestamps below. IST means Asia/Calcutta, UTC+05:30, with calendar conversions stated explicitly.

## 2 Actual issue, development, PR and completion chronology

All ten issues were created on **4 November 2026**, between **15:31:09 and 15:38:17 UTC** (**21:01:09–21:08:17 IST**). The table gives exact UTC timestamps; the last column is the local date of issue closure.

| Issue and actual title | Points | Issue created UTC | Primary PR created UTC | Primary PR merged UTC | Issue closed UTC | Closure date IST |
|---|---:|---|---|---|---|---|
| [#26](https://github.com/traffic-management-system-team/Traffic-management-system/issues/26) Add Crow API host and serialized simulation service | 5 | 2026-11-04 15:31:09 | [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38): 2026-11-07 12:17:02 | 2026-11-07 12:54:04 | 2026-11-07 13:59:05 | 2026-11-07 |
| [#27](https://github.com/traffic-management-system-team/Traffic-management-system/issues/27) Expose read-only simulation state and vehicle snapshots | 5 | 2026-11-04 15:31:59 | [PR #40](https://github.com/traffic-management-system-team/Traffic-management-system/pull/40): 2026-11-08 17:33:44 | 2026-11-10 08:44:34 | 2026-11-10 08:44:36 | 2026-11-10 |
| [#28](https://github.com/traffic-management-system-team/Traffic-management-system/issues/28) Add simulation control and configuration REST endpoints | 5 | 2026-11-04 15:32:19 | [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39): 2026-11-08 09:15:31 | 2026-11-08 11:04:37 | 2026-11-08 11:04:38 | 2026-11-08 |
| [#29](https://github.com/traffic-management-system-team/Traffic-management-system/issues/29) Scaffold React TypeScript Vite frontend and API client | 3 | 2026-11-04 15:32:48 | [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43): 2026-11-10 06:36:14 | 2026-11-10 06:41:51 | 2026-11-10 06:41:52 | 2026-11-10 |
| [#30](https://github.com/traffic-management-system-team/Traffic-management-system/issues/30) Build responsive intersection, signal, and vehicle queue dashboard | 5 | 2026-11-04 15:33:17 | [PR #45](https://github.com/traffic-management-system-team/Traffic-management-system/pull/45): 2026-11-10 11:19:49 | 2026-11-10 15:25:55 | 2026-11-10 15:25:56 | 2026-11-10 |
| [#31](https://github.com/traffic-management-system-team/Traffic-management-system/issues/31) Add web simulation controls and configuration form | 5 | 2026-11-04 15:33:35 | [PR #44](https://github.com/traffic-management-system-team/Traffic-management-system/pull/44): 2026-11-10 09:08:42 | 2026-11-10 09:14:22 | 2026-11-10 09:14:24 | 2026-11-10 |
| [#32](https://github.com/traffic-management-system-team/Traffic-management-system/issues/32) Display live simulation statistics in the dashboard | 3 | 2026-11-04 15:33:53 | [PR #47](https://github.com/traffic-management-system-team/Traffic-management-system/pull/47): 2026-11-12 15:06:44 | 2026-11-12 15:16:44 | 2026-11-12 15:16:46 | 2026-11-12 |
| [#33](https://github.com/traffic-management-system-team/Traffic-management-system/issues/33) Test REST API contracts, validation, and concurrency | 5 | 2026-11-04 15:34:28 | [PR #46](https://github.com/traffic-management-system-team/Traffic-management-system/pull/46): 2026-11-10 17:05:31 | 2026-11-11 14:52:11 | 2026-11-11 16:07:54 | 2026-11-11 |
| [#34](https://github.com/traffic-management-system-team/Traffic-management-system/issues/34) Test browser integration, responsiveness, and accessibility | 5 | 2026-11-04 15:36:41 | [PR #48](https://github.com/traffic-management-system-team/Traffic-management-system/pull/48): 2026-11-12 19:35:29 | 2026-11-13 18:47:30 | 2026-11-13 18:47:31 | 2026-11-14 |
| [#35](https://github.com/traffic-management-system-team/Traffic-management-system/issues/35) Review REST API security and operational error handling | 3 | 2026-11-04 15:38:17 | [PR #49](https://github.com/traffic-management-system-team/Traffic-management-system/pull/49): 2026-11-13 08:45:44 | 2026-11-13 10:22:00 | 2026-11-13 10:22:01 | 2026-11-13 |

### Recorded development commits

The next table gives the first feature-specific commit returned for each primary PR and the final returned commit, with authored and committed timestamps distinguished. The retrieved message sequences correspond to the issue's functionality; these are evidence bounds, not claims that no earlier uncommitted development existed. Full commit hashes are available through the links.

| Issue / primary PR | First recorded feature commit and authored UTC | First committed UTC | Final recorded commit and authored UTC | Final committed UTC |
|---|---|---|---|---|
| [#26](https://github.com/traffic-management-system-team/Traffic-management-system/issues/26) / [PR #38](https://github.com/traffic-management-system-team/Traffic-management-system/pull/38) | [`9d793c80`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/9d793c8016a028aae26cb766ac1dc367c3255d73) Implement SimulationService with synchronized scheduler; 2026-11-07 12:13:06 | 2026-11-07 12:13:06 | [`9d793c80`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/9d793c8016a028aae26cb766ac1dc367c3255d73) Implement SimulationService with synchronized scheduler; 2026-11-07 12:13:06 | 2026-11-07 12:13:06 |
| [#27](https://github.com/traffic-management-system-team/Traffic-management-system/issues/27) / [PR #40](https://github.com/traffic-management-system-team/Traffic-management-system/pull/40) | [`e688973a`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/e688973aac05b01af6134efda12e206777fe3b17) Add read-only simulation state API; 2026-11-08 17:31:43 | 2026-11-09 18:31:45 | [`a8f1ea6e`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/a8f1ea6e0fb270c4913d8b2108c44c581e9ad42a) Harden simulation state API request handling; 2026-11-10 08:27:25 | 2026-11-10 08:27:25 |
| [#28](https://github.com/traffic-management-system-team/Traffic-management-system/issues/28) / [PR #39](https://github.com/traffic-management-system-team/Traffic-management-system/pull/39) | [`2ca22828`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/2ca2282843dd9691993e390d3cfd5f5de61482f1) Implement simulation control API handlers (#28); 2026-11-08 09:12:51 | 2026-11-08 09:12:51 | [`4ffc3935`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/4ffc3935f561e4356bf180202dd6d5b00cca22f1) Fix simulation control validation and state conflicts; 2026-11-08 10:55:13 | 2026-11-08 10:55:13 |
| [#29](https://github.com/traffic-management-system-team/Traffic-management-system/issues/29) / [PR #43](https://github.com/traffic-management-system-team/Traffic-management-system/pull/43) | [`6bcb8472`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/6bcb8472434ec34a59e9aa7f50f63704a36663a9) feat(frontend): scaffold React app and typed API client; 2026-11-10 06:34:22 | 2026-11-10 06:34:22 | [`6bcb8472`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/6bcb8472434ec34a59e9aa7f50f63704a36663a9) feat(frontend): scaffold React app and typed API client; 2026-11-10 06:34:22 | 2026-11-10 06:34:22 |
| [#30](https://github.com/traffic-management-system-team/Traffic-management-system/issues/30) / [PR #45](https://github.com/traffic-management-system-team/Traffic-management-system/pull/45) | [`4588ab11`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/4588ab11ef59e4cef685f64076f8101d118a58a8) feat(frontend): implement Issue 30 intersection dashboard; 2026-11-10 11:16:33 | 2026-11-10 11:16:33 | [`1e896083`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/1e896083aac8843ac29139258efc06489a8af411) Add dashboard activity and adaptive polling tests; 2026-11-10 13:56:10 | 2026-11-10 13:56:10 |
| [#31](https://github.com/traffic-management-system-team/Traffic-management-system/issues/31) / [PR #44](https://github.com/traffic-management-system-team/Traffic-management-system/pull/44) | [`6391481c`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/6391481cee9663b655c15eee9e94a24f87b433d5) feat(frontend): add simulation controls and configuration; 2026-11-10 09:07:28 | 2026-11-10 09:07:28 | [`6391481c`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/6391481cee9663b655c15eee9e94a24f87b433d5) feat(frontend): add simulation controls and configuration; 2026-11-10 09:07:28 | 2026-11-10 09:07:28 |
| [#32](https://github.com/traffic-management-system-team/Traffic-management-system/issues/32) / [PR #47](https://github.com/traffic-management-system-team/Traffic-management-system/pull/47) | [`261b28e0`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/261b28e004110702f8f0deb67481cd52a29940cc) feat(dashboard): display live simulation statistics; 2026-11-12 15:03:03 | 2026-11-12 15:03:03 | [`261b28e0`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/261b28e004110702f8f0deb67481cd52a29940cc) feat(dashboard): display live simulation statistics; 2026-11-12 15:03:03 | 2026-11-12 15:03:03 |
| [#33](https://github.com/traffic-management-system-team/Traffic-management-system/issues/33) / [PR #46](https://github.com/traffic-management-system-team/Traffic-management-system/pull/46) | [`f7c1d381`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/f7c1d381f4fc873a868e075ef02a26a920746d20) test(api): add focused issue 33 contract coverage; 2026-11-10 17:04:05 | 2026-11-10 17:04:05 | [`9fb98eb4`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/9fb98eb4ed52c354c3697b5e617034a084959726) test(api): bound HTTP server contract test runtime; 2026-11-11 04:15:02 | 2026-11-11 04:15:02 |
| [#34](https://github.com/traffic-management-system-team/Traffic-management-system/issues/34) / [PR #48](https://github.com/traffic-management-system-team/Traffic-management-system/pull/48) | [`f3395c1c`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/f3395c1cc5a0dd8c37abdfb8c4de89997b0cfc1b) test: complete Issue 34 browser and accessibility coverage; 2026-11-12 19:30:04 | 2026-11-13 15:38:41 | [`6dbfb822`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/6dbfb822a4fbaa3b56e33bb31f1bf2005bd0712f) test(e2e): fix backend snapshot parsing; 2026-11-13 16:57:45 | 2026-11-13 16:57:45 |
| [#35](https://github.com/traffic-management-system-team/Traffic-management-system/issues/35) / [PR #49](https://github.com/traffic-management-system-team/Traffic-management-system/pull/49) | [`3afa4c7a`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/3afa4c7affeba4d7feac7060c90182d3dc11d578) Harden HTTP adapter error boundary; 2026-11-13 08:44:05 | 2026-11-13 08:44:05 | [`90efa89f`](https://github.com/traffic-management-system-team/Traffic-management-system/commit/90efa89f4d69c03b0c07091fa89f7440a4a8d846) Fix portable response header assertion in exception test; 2026-11-13 09:58:51 | 2026-11-13 09:58:51 |

The first directly associated feature commit is service PR #38's `9d793c80`, authored/committed **7 November 12:13:06 UTC**. The final primary issue PR, #48, merged **13 November 18:47:30 UTC**; #34 closed one second later, **14 November 00:17:31 IST**.

For #27, the initial snapshot commit was authored 8 November but committed 9 November; its PR was opened 8 November. For #34, the initial browser-test commit was authored 12 November but committed 13 November; its PR was opened 12 November. Those differences are consistent with history being rewritten/replayed, but the specific mechanism is not proven here. Do not replace PR creation times with later commit timestamps or infer a contradiction merely from those differences.

### Dated team coordination

Each proposed migration issue has a real 4 November workflow comment by `pranav9360`, identifying its owner, prerequisites, handoff and review expectations. The first is **17:44:38 UTC** and the last **17:45:38 UTC** (**23:14:38–23:15:38 IST**). These are evidence of organized migration work on that date, but none names Sprint 3 or assigns sprint start/end dates.

- [#26](https://github.com/traffic-management-system-team/Traffic-management-system/issues/26): [workflow comment, 2026-11-04 17:44:38 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/26#issuecomment-6637727566).
- [#27](https://github.com/traffic-management-system-team/Traffic-management-system/issues/27): [workflow comment, 2026-11-04 17:44:43 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/27#issuecomment-6637729568).
- [#28](https://github.com/traffic-management-system-team/Traffic-management-system/issues/28): [workflow comment, 2026-11-04 17:44:49 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/28#issuecomment-6637731680).
- [#29](https://github.com/traffic-management-system-team/Traffic-management-system/issues/29): [workflow comment, 2026-11-04 17:44:57 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/29#issuecomment-6637734428).
- [#30](https://github.com/traffic-management-system-team/Traffic-management-system/issues/30): [workflow comment, 2026-11-04 17:45:05 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/30#issuecomment-6637737125).
- [#31](https://github.com/traffic-management-system-team/Traffic-management-system/issues/31): [workflow comment, 2026-11-04 17:45:11 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/31#issuecomment-6637739205).
- [#32](https://github.com/traffic-management-system-team/Traffic-management-system/issues/32): [workflow comment, 2026-11-04 17:45:18 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/32#issuecomment-6637741638).
- [#33](https://github.com/traffic-management-system-team/Traffic-management-system/issues/33): [workflow comment, 2026-11-04 17:45:23 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/33#issuecomment-6637743627).
- [#34](https://github.com/traffic-management-system-team/Traffic-management-system/issues/34): [workflow comment, 2026-11-04 17:45:30 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/34#issuecomment-6637745889).
- [#35](https://github.com/traffic-management-system-team/Traffic-management-system/issues/35): [workflow comment, 2026-11-04 17:45:38 UTC](https://github.com/traffic-management-system-team/Traffic-management-system/issues/35#issuecomment-6637748944).

## 3 Defensible Sprint 3 range and its limits

Three different ranges must remain distinct:

| Range | UTC dates | IST dates | What it establishes |
|---|---|---|---|
| Proposed retrospective migration window | 4–13 Nov 2026 | **4–14 Nov 2026** | Recorded migration issues/coordination through last issue closure |
| Directly observed feature development and primary PR integration | 7–13 Nov 2026 | 7–14 Nov 2026 | First associated feature commit through final primary PR merge |
| Original formally configured Sprint 3 | **NOT VERIFIED** | **NOT VERIFIED** | No Sprint 3 iteration, timebox or named planning approval found |

Use **4–14 November IST** in the proposed documentation and call it an **observed retrospective window**. It is an 11-calendar-date window, not a documented five-day or two-week sprint commitment. There is no basis to choose 2 November, 6 November, 9 November or another date merely to fit a university testing window or conventional cadence.

The initial backlog comments on 4 November are stronger evidence than a date invented from first coding activity, but they still do not establish a formally named sprint. Team approval of this classification now would be a current retrospective decision. Its decision date must remain the actual approval date; it must not be represented as approval on 4 November.

## 4 Scope increase from 47 to 91 points

| Classification | Issues | Current recorded points | Status of classification |
|---|---|---:|---|
| Original Sprint 1 | #1–6 | 23 | Existing historical allocation, unchanged |
| Original Sprint 2 | #7–12 | 24 | Existing historical allocation, unchanged |
| Proposed Sprint 3 migration phase | #26–35 | 44 | Retrospective grouping proposed for review; currently no iteration |
| Entire recorded issue backlog | 22 distinct issues | **91** | 47 original + 44 additional |

The current migration estimates are **5 + 5 + 5 + 3 + 5 + 5 + 3 + 5 + 5 + 3 = 44**. This is **44/47 × 100 = 93.62%** more estimated scope than the 47-point original backlog, bringing the recorded total to approximately **1.94 times** its original size.

The increase is explained by additional delivery tasks: serialized service/scheduler (#26), safe state/FIFO snapshots (#27), REST controls/configuration (#28), React/client/proxy foundation (#29), intersection/queue presentation (#30), web controls/configuration (#31), backend-derived statistics display (#32), API/concurrency/CI validation (#33), browser/responsive/accessibility validation (#34), and REST security/error/shutdown review (#35). These tasks add transport, presentation, integration and validation work while reusing the C++ rules.

This does not mean 44 new SRS requirements, 44 hours, or an independently measured increase in productivity. The FR/NFR/SEC requirement IDs remain stable while implementation work grows. Current story-point fields verify the arithmetic; historical dates when those numeric estimates were assigned are **NOT VERIFIED**.

Several original Sprint 2 issues also completed during this migration window. For example, #9's [PR #41](https://github.com/traffic-management-system-team/Traffic-management-system/pull/41) integrates the actual Crow host on 9 November and remains within its original 5 points. It must not be counted again as a new migration issue or added above 91. Similarly, original #10/#11/#12 retain their Sprint 2 allocations despite later testing/security/documentation completion. Unpointed later PRs and the current redesign are not assigned invented extra points.

“91 points in the backlog” is supported by current fields. “91 points independently accepted by a university evaluator” is not established by Done/closed status.

## 5 Historical burndown verification

**Verdict: a historical increase in remaining work is NOT VERIFIED from available burndown data.**

No numeric burndown export, dated remaining-point series, burn chart file or historical estimate/status series was located in the inspected repository records or reachable filename/message searches. The two original Part 1 Word records have no embedded chart/media parts and contain no remaining-work series. The current project API supplies current points/status/iteration values, not a recovered historical burndown.

The live private GitHub project's Insights chart/time series was not retrieved. Therefore this report does **not** claim that no chart exists privately; its shape, values, date filtering and visibility remain unverified. A chart or export outside the inspected records could change this conclusion.

What is supported:

- Ten additional migration issues were created on 4 November.
- Their **current** point estimates sum to 44, and the current full backlog totals 91.
- This is evidence of expanded backlog scope alongside the preserved 47-point baseline.

What is not supported:

- That a historical chart visibly jumped by exactly 44 on 4 November.
- That remaining work rose from **47 to 91** on that day; those figures describe total baseline/expanded scope. Some original issues had already closed, so they cannot simply be used as remaining-work values.
- The exact historical date/time of point assignment or status changes, daily remaining totals, an ideal burn line, or sprint velocity.
- That a chart filtered to Sprint 1/2 included the new unassigned items. It could have excluded all ten issues.
- A claim that all ten issues consumed 44 points in a formally committed Sprint 3.

A retrospective closure-based curve could be computed only with explicit assumptions about fixed estimates, scope inclusion, acceptance and reopenings. It would be a new reconstruction, not historical burndown evidence. **No such curve or fabricated daily values are produced here.** To verify an actual rise, obtain the original chart/export with its filter/metric/date range and estimate/status history, then compare points before and after the scope addition.

## 6 Contradictions and reconciliation requirements

1. **No original Sprint 3 field or named approval:** existing issue bodies say unassigned backlog; current project has only Sprint 1/2. A proposed Sprint 3 label must be dated now as retrospective, not written as a historical planning fact. The 4 November comments prove coordination, not a formal ceremony.
2. **Timezone changes the end date:** 13 November 18:47:31 UTC is 14 November 00:17:31 IST. A claimed 4–13 November IST window excludes the final issue's closure. Keep UTC evidence tables and the stated IST range.
3. **The existing change log's “#26–35 completed 7–15 Nov” conflates cohorts:** direct primary PR merges and issue closures for #26–35 finish 13 November UTC / 14 November IST. The 15 November [PR #51](https://github.com/traffic-management-system-team/Traffic-management-system/pull/51) concerns original #10 regression/acceptance; it is related follow-through, not a new #26–35 issue. Propose an explanatory addendum preserving the previous record rather than erasing it.
4. **Original Sprint 2 spilled into November:** #7/#8 dependencies and #9 host integration, then #10/#11/#12 work overlap the proposed window. Do not move those original stories or rewrite October completion. Preserve allocation versus actual-delivery distinction and avoid double-counting their points.
5. **#26 closure does not prove host readiness at that instant:** #38 merged 7 November and explicitly deferred HTTP routes to #9 and left MSVC v143 verification outstanding. Host PR #41 merged 9 November; later build evidence supports the integrated system. Do not label all #26 host/dependency criteria as proved on the closure timestamp.
6. **Primary story closure precedes later product evolution:** [PR #53](https://github.com/traffic-management-system-team/Traffic-management-system/pull/53) adds Control Centre/Simulation Monitor and merges 16 November, outside the proposed ten-issue window. Record it as subsequent GUI refinement/integration. The existing uncommitted redesign and 19 November reconciliation are also later work; do not backdate them into the window or invent points for them.
7. **No historical burndown verification:** 47 → 91 is the scope reconciliation, not a validated remaining-work trajectory or velocity.
8. **Acceptance/evidence caveats survive the relabel:** #33's stale historical test counts and broad deterministic-test wording, and #26's closure/dependency qualifications, remain as reported in [issue alignment audit](ISSUE_DOCUMENT_ALIGNMENT_AUDIT.md). Sprint reclassification does not close those gaps.
9. **Existing formal documents correctly deny prior Sprint 3 approval:** SRS §4/SAD §7 and the Test Plan's “no retrospective approval asserted” statements remain true. Append a clearly marked proposal, leaving the denial of formal historical approval intact. If later approved, record the real review date, not a backdated revision.
10. **Course timing and submission approval:** work continued beyond original October iterations and the previously supplied 2–6 November testing window. Retrospective Sprint 3 naming cannot establish instructor permission, on-time delivery or sprint-video submission.

## 7 Proposed documentation handling

The separate [proposal file](SPRINT_3_PROPOSED_DOCUMENTATION_CHANGES.md) contains an append-only Sprint Change Log addendum, a concise paragraph for SRS §4, a concise paragraph for SAD §7, a scope/burndown note and a viva summary. They are marked proposed and are not applied to existing Markdown/DOCX/PDF artifacts.

Preserve original Sprint 1/2 rows, dates, revision history, tests, measurements, evidence limitations and authorship distinctions. No revision claiming approval should be inserted until an actual review decision exists. Future GitHub iteration changes are outside this task.

## 8 Final Agile summary for the viva

> Our original backlog had 47 story points: 23 in Sprint 1 and 24 in Sprint 2. The desktop-to-web migration introduced ten additional issues, #26–35, currently estimated at 44 points, bringing recorded scope to 91. We propose calling that work Sprint 3 retrospectively. The evidence window is 4–14 November in India: issues and coordination were recorded on 4 November, development commits begin on 7 November, and the last issue closed just after midnight on 14 November IST. We have not found evidence of a formally planned Sprint 3 timebox. We retained the C++ simulation and added a synchronized Crow REST layer, React UI and API/browser/security validation. Original Sprint 2 work overlapped this phase, and the two-page GUI followed on 16 November. We can verify the scope increase, but cannot claim a historical burndown rise without the original chart or data.

**Review outcome proposed:** accept the retrospective grouping and evidence window as a factual explanation of migration scope, while keeping formal planning approval, historical burndown and university compliance claims unverified.
