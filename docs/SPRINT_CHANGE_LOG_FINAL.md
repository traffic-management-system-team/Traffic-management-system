# Sprint evolution and contribution record

Reconciled 19 November 2026. Sources are original Part1_Sprint_Backlog/Work_Distribution documents, reachable Git history, and live read-only GitHub project/PR responses checked during audit and reconciliation. GitHub merge timestamps below are UTC to avoid moving late-night work to a different day silently. The viva date is 20 November in Asia/Calcutta.

## Planned iterations and actual completion

The original documents allocate Pranav product ownership/RTM/integration, Gowtham signals/simulation, Dinesh vehicles/queues and Nitish GUI/statistics. These are assignments. PR and commit evidence below records actual authorship and integration separately.

| Planned iteration | Dates in project | Assigned issues and points | Observed actual delivery |
|---|---|---|---|
| Sprint 1 | 19–23 Oct 2026 | #1(2), #2(3), #3(5), #4(3), #5(5), #6(5), total 23 | #17 signal merged 23 Oct, #15/#16 vehicle/queue merged 24 Oct, #18 traffic and #19 GUI merged 26 Oct, #20 timing/GUI finalization merged 30 Oct |
| Sprint 2 | 26–30 Oct 2026 | #7(3), #8(3), #9(5), #10(5), #11(3), #12(5), total 24 | Statistics/config and integration/testing completion extended into November. These dates are actual spillover, not original-sprint completion claims |
| Later web work | No iteration assigned | #26–35, total 44 points | Synchronized service, REST state/control API, React UI and browser/contract/security verification completed 7–15 Nov |

Live project https://github.com/orgs/traffic-management-system-team/projects/1 contains 22 items, all Done, with assignees and story points. Later #26–35 have no iteration. No Sprint 3 approval or dates were located. Optional Sprint 3 must not be invented retrospectively. Some issue bodies retain unchecked acceptance boxes despite Done/closed state. This document explains the evidence without rewriting those GitHub records.

## Actual dated evolution

| UTC date | PR/commit evidence | Change |
|---|---|---|
| 6 Oct | Historical v1.3 documents | Desktop GUI design, modular simulation, TC-01–16 and original responsibility/sprint records |
| 23–30 Oct | PRs #15–20 | Vehicle/queue/signal/traffic modules, initial Qt GUI and timing integration |
| 5 Nov | PR #36, author pranav9360 | Backend-neutral statistics contract integrated |
| 6 Nov | PR #37, author pranav9360 | Safe configuration validation integrated |
| 7 Nov | PR #38, author Dinesh07-18 | Shared synchronized SimulationService and scheduler |
| 8 Nov | PR #39, author nitish5052 | Framework-neutral control/configuration API handlers |
| 9 Nov | PR #41, author pranav9360 | Crow host and REST integration |
| 10 Nov | PR #40 Dinesh07-18, #43/#44 pranav9360, #45 gowth139am-lab | Snapshot API, typed React scaffold, controls/configuration and intersection dashboard |
| 11–13 Nov | PR #46 Dinesh07-18, #47 nitish5052, #49/#25 Dinesh07-18, #48 gowth139am-lab | API contracts, statistics presentation, security validation and browser testing |
| 15 Nov | PR #51 gowth139am-lab, #52 pranav9360 | Regression/web acceptance and documentation alignment |
| 16 Nov | PR #53 pranav9360, merge 5cbdc01 | Control Centre and Simulation Monitor with shared controls/state; browser verification and screenshots |
| 16 Nov working tree | Uncommitted feature/professional-traffic-ui changes | Professional visual redesign. This is not a merged GitHub delivery |
| 19 Nov | Recorded audit and this documentation reconciliation | Actual audit execution and final submission reconciliation. No date is backdated to the 2–6 Nov testing window |

The university testing window supplied by the student was 2–6 Nov. Several implementation/test merges occurred afterward. The team should explain actual scope evolution and completion dates rather than imply that every final test was completed during that window. The migration rationale visible in code is reuse of the C++ core through an explicit HTTP/service boundary and a browser presentation. Instructor approval of that scope change is NOT VERIFIED.

## Individual assignment authorship and integration

| Member/account | Assigned issues with points | Actual PR evidence | Review/integration evidence and qualification |
|---|---|---|---|
| Pranav / pranav9360 | #1(2),#9(5),#12(5),#29(3),#31(5), total20 | #20,#36,#37,#41–44,#52,#53 | Integration author for #36/#37 differs from assigned Nitish ownership. Approved #25/#51 and merged #53 according to audit API records |
| Dinesh / Dinesh07-18 | #2(3),#3(5),#11(3),#26(5),#27(5),#33(5),#35(3), total29 | #15,#16,#25,#38,#40,#46,#49 | #25 has Pranav approval. #50 closed unmerged. #15 authored vehicle and release-safe test commits |
| Gowtham / gowth139am-lab | #4(3),#5(5),#10(5),#30(5),#34(5), total23 | #17,#18,#45,#48,#51 | #51 has Pranav approval. #17 includes signal and review-feedback commits. Copilot quota message does not establish a completed review |
| Nitish / nitish5052 | #6(5),#7(3),#8(3),#28(5),#32(3), total19 | #19,#39,#47 merged; #21/#23/#24 closed unmerged | #19 carries Nitish-authored initial GUI commit. Explain earlier statistics/config drafts versus final #36/#37 integration using their actual diffs |

No claim is made that every PR had formal human approval. The audit found no formal reviews on #15/#19/#53. Assignment, PR author, commit author and merger represent different evidence. Revisions listed in document histories remain historical team records and are not proof that every member edited every file.

PR URLs use https://github.com/traffic-management-system-team/Traffic-management-system/pull/ followed by the number. #53 CI has eight SUCCESS jobs at head 88c9a00, confirmed through the read-only API. This applies to that committed source and not the uncommitted redesign.

## Sprint outcome video evidence

The repository was searched again on 19 Nov for mp4, mov, webm, avi, mkv, video and submission references. No actual Sprint 1/2 video file or usable submission link was located. docs/Issue_12_Demo_Readiness.md still identifies recording as a handoff item. Generated redesign screenshots and Playwright captures are not sprint videos.

Both required sprint outcome submissions and their actual 1-minute/2-minute durations remain NOT VERIFIED. They may exist outside this repository in an LMS or private Drive, but no such submission is asserted. Provide real accessible links/files, verify playback/duration and map each to the university's exact sprint requirement before claiming completion.

## Approved retrospective Sprint 3 documentation addendum

Approval recorded 19 November 2026 from the team's explicit instruction to apply the reviewed reconciliation. The approved label is **Sprint 3 — Web Migration (Retrospective Classification)**. This current documentation decision is not evidence that a formal Sprint 3 meeting, original iteration assignment or instructor approval occurred in November.

The project began as a C++ desktop GUI proposal. The team subsequently adopted React/TypeScript for browser-based operation while retaining the implemented C++ simulation core. This introduced service/scheduling, REST transport, frontend, integration, testing and local API security work. The classification groups these ten additional issues without rewriting the original Sprint 1/2 assignments.

| Documentation classification | Issues | Points | Dates and interpretation |
|---|---|---:|---|
| Sprint 1, preserved | #1–6 | 23 | Original planned 19–23 October 2026 |
| Sprint 2, preserved | #7–12 | 24 | Original planned 26–30 October 2026; actual November spillover remains recorded |
| Sprint 3 — Web Migration (Retrospective Classification) | #26–35 | 44 | Observed 4–14 November 2026 IST, equivalent to 4–13 November UTC; not a verified original timebox |
| Total | 22 issues | **91** | Original 47 + additional 44; increase 93.62% |

All ten migration issues were created on 4 November, 15:31:09–15:38:17 UTC (21:01:09–21:08:17 IST). Workflow comments that evening document owners, dependencies and handoffs. Feature commits are recorded from 7 November. Primary PR integration ends 13 November 18:47:30 UTC; final issue #34 closes one second later, 14 November 00:17:31 IST.

### Migration owners points and direct completion evidence

Exact dates below are UTC. Owners are the current issue/project assignees, not inferred from the person merging a PR. Closed status and merge timestamps do not independently prove every acceptance criterion.

| Issue | Scope | Owner and account | Points | Primary merged PR | PR merged UTC | Issue closed UTC |
|---|---|---|---:|---|---|---|
| #26 | Serialized service and scheduler; host/dependency foundation | Dinesh / Dinesh07-18 | 5 | #38 | 7 Nov 12:54:04 | 7 Nov 13:59:05 |
| #27 | Consistent read-only state and FIFO snapshots | Dinesh / Dinesh07-18 | 5 | #40 | 10 Nov 08:44:34 | 10 Nov 08:44:36 |
| #28 | REST lifecycle and configuration commands | C NITISH / nitish5052 | 5 | #39 | 8 Nov 11:04:37 | 8 Nov 11:04:38 |
| #29 | React/TypeScript/Vite frontend and typed API client | Daruvuri Pranav Sai / pranav9360 | 3 | #43 | 10 Nov 06:41:51 | 10 Nov 06:41:52 |
| #30 | Responsive signal/intersection/FIFO dashboard | GOWTHAM S / gowth139am-lab | 5 | #45 | 10 Nov 15:25:55 | 10 Nov 15:25:56 |
| #31 | Web controls and configuration form | Daruvuri Pranav Sai / pranav9360 | 5 | #44 | 10 Nov 09:14:22 | 10 Nov 09:14:24 |
| #32 | Backend-derived statistics display | C NITISH / nitish5052 | 3 | #47 | 12 Nov 15:16:44 | 12 Nov 15:16:46 |
| #33 | API/service contract, concurrency tests and CI | Dinesh / Dinesh07-18 | 5 | #46 | 11 Nov 14:52:11 | 11 Nov 16:07:54 |
| #34 | Real-backend browser, responsive and accessibility checks | GOWTHAM S / gowth139am-lab | 5 | #48 | 13 Nov 18:47:30 | 13 Nov 18:47:31 |
| #35 | REST security and operational error/shutdown review | Dinesh / Dinesh07-18 | 3 | #49 | 13 Nov 10:22:00 | 13 Nov 10:22:01 |

PR and issue URLs use https://github.com/traffic-management-system-team/Traffic-management-system/pull/ and /issues/ followed by the number. Full creation/commit/PR timestamps and linked evidence remain in [Sprint 3 reconciliation](SPRINT_3_RECONCILIATION_REPORT.md). Current acceptance qualifications remain in [issue alignment audit](ISSUE_DOCUMENT_ALIGNMENT_AUDIT.md).

### Preserved spillover and later integration

Original Sprint 2 issues #7/#8 provided core statistics/configuration dependencies, and #9/PR #41 supplied the Crow host on 9 November. Original #11 security review, #12 traceability and #10 regression/acceptance also completed in November. Their original points are retained, not counted again in the additional 44.

The earlier “#26–35 completed 7–15 Nov” entry is a broad pre-reconciliation summary. Direct primary merges/closures for that cohort finish 13 November UTC / 14 November IST. PR #51 on 15 November belongs to original #10. PR #53 added Control Centre and Simulation Monitor on 16 November as subsequent refinement, after the ten-issue window. The current uncommitted redesign and 19 November audit/documentation work are later activities and have no invented additional estimates.

### Scope and historical burndown

The verified current scope is 47 original + 44 additional = 91 points across 22 distinct issues, a 93.62% increase over the original estimate. This is total estimated scope, not a measured daily remaining-work series or accepted velocity.

Backlog scope increased by 44 story points; a historical rise in the burndown chart has not been independently verified.

No original chart/export with date filters and historical estimate/status values was recovered. A private project Insights chart may exist, but its values and scope were not retrieved. No daily values or historical spike is fabricated.

### Documentation approval and historical limits

The team has now approved the retrospective documentation classification. Historical passages above describing the absence of a Sprint 3 assignment/approval reflect the evidence at their original reconciliation. The live GitHub project still assigns no iteration to #26–35 and has not been modified. Formal original planning and instructor approval remain NOT VERIFIED; this approval does not verify videos, quantitative performance, C++ branch coverage or other outstanding evidence.
