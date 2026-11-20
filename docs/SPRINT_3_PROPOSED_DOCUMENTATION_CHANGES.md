# Proposed Sprint 3 documentation changes

Prepared 19 November 2026. **PROPOSED FOR REVIEW — NOT APPLIED.** Evidence and exact timestamps: [Sprint 3 reconciliation report](SPRINT_3_RECONCILIATION_REPORT.md).

This file contains ready-to-insert addenda. The existing Sprint Change Log, FINAL_REVIEW SRS/SAD/Test Plan, original Sprint 1/2 artifacts and GitHub project remain unchanged. The phrase “Sprint 3” below is a proposed retrospective classification, not proof of a formally scheduled iteration.

## 1 Sprint Change Log append-only addendum

Target: [docs/SPRINT_CHANGE_LOG_FINAL.md](SPRINT_CHANGE_LOG_FINAL.md). Append the following section. Keep the original Sprint 1/2 rows and actual delivery records. Preserve the earlier “Later web work” entry as the pre-reconciliation record and qualify it with this addendum.

### Proposed retrospective Sprint 3 classification

On 19 November 2026, a retrospective classification was proposed for the web-migration backlog. Issues #26–35 were all created on 4 November and have current estimates totalling 44 story points. Dated workflow comments that day identify owners, dependencies and handoffs. Their primary development PRs merged from 7 to 13 November UTC. The last issue closed at 13 November 18:47:31 UTC, which is 14 November 00:17:31 IST.

The proposed label is **Sprint 3 — web migration, retrospective classification**, with an **observed activity window of 4–14 November 2026 IST (4–13 November UTC)**. This describes recorded activity, not a recovered formal sprint timebox. At review time the GitHub project has only Sprint 1 and Sprint 2, and #26–35 still have no assigned iteration. No meeting, original Sprint 3 approval or historical commitment date is asserted.

| Preserved or proposed allocation | Issues | Points | Date interpretation |
|---|---|---:|---|
| Original Sprint 1, preserved | #1–6 | 23 | Existing planned dates 19–23 Oct; actual delivery/spillover remains recorded separately |
| Original Sprint 2, preserved | #7–12 | 24 | Existing planned dates 26–30 Oct; November completions remain spillover under this original allocation |
| Proposed retrospective Sprint 3 | #26–35 | 44 | Observed migration window 4–14 Nov IST; no formal historical timebox verified |
| Total recorded backlog | 22 distinct issues | **91** | Original 47 + additional 44; no duplicate points for shared integration PRs |

Scope covers the synchronized C++ SimulationService, snapshot/control APIs, React client/dashboard/controls/statistics, API/CI and real-backend browser tests, and local REST security/reliability review. The existing C++ simulation rules are reused. Requirements keep their original IDs.

The previous broad “7–15 Nov” migration/completion summary included related regression follow-through. Exact direct completion of #26–35 ends on 13 November UTC / 14 November IST; PR #51 on 15 November belongs to original #10. PR #41 host integration remains original #9, and the two-page GUI PR #53 merged 16 November as later refinement. Neither those original story points nor the later unpointed refinements are added again to the 44-point cohort.

This is a current proposal for explaining scope evolution. Team/instructor approval of the original migration or a formally planned Sprint 3 is not established. Original dates, allocation, authorship and evidence remain unchanged.

## 2 SRS Section 4 architectural evolution paragraph

Target: [FINAL_REVIEW SRS](history/sprint-3-approved-2026-11-19/docs/SRS/SRS_Traffic_Management_System_FINAL_REVIEW_2026-11-19.docx), Section 4, and its corresponding editable Markdown edition. Append the following paragraph, marked proposed during review; retain the existing account of PR #53, the current uncommitted redesign, source baseline and lack of formal Sprint 3 approval.

> The desktop design evolved into a React/TypeScript browser interface communicating over localhost REST with the retained C++17 simulation. Migration issues #26–35 added synchronized service ownership, APIs, browser presentation and validation work, estimated at 44 points beyond the original 47. We propose classifying this as Sprint 3 retrospectively, using the observed 4–14 November 2026 IST activity window. This does not establish a formally planned sprint or instructor approval. Original Sprint 1/2 allocations and requirement IDs remain unchanged; the two-page GUI followed in PR #53 on 16 November.

## 3 SAD Section 7 architectural evolution paragraph

Target: [FINAL_REVIEW SAD](history/sprint-3-approved-2026-11-19/docs/SAD/SAD_Traffic_Management_System_FINAL_REVIEW_2026-11-19.docx), Section 7, and its corresponding editable Markdown edition. Append the following paragraph; retain all existing residual-risk and approval limitations.

> The migration retained the C++ model, FIFO queues, signal rules, validation and statistics while adding SimulationService ownership, a single synchronized scheduler, Crow REST transport, copied-state serialization and a React client with shared polling and controls. Issues #26–35 group this additional architecture, presentation and verification work. Their proposed retrospective Sprint 3 window is 4–14 November 2026 IST, derived from recorded activity rather than a verified formal timebox. Original #9 supplied host integration; later PR #53 supplied the two-page GUI. No historical sprint-planning approval is implied.

## 4 Scope and burndown wording

Use the following wherever scope growth is explained:

> Recorded backlog scope increased from 47 to 91 story points through ten additional migration issues estimated at 44 points, a 93.62% increase over the original estimate. This reconciles current issue estimates and preserves the original Sprint 1/2 allocations. It does not show that remaining work rose from 47 to 91 on a historical burndown. No original remaining-work series or chart export was verified; historical estimate timing and chart filters are also unverified.

Do not introduce daily remaining values, ideal lines, completed velocity, a 44-point jump on a specific day, or a newly drawn chart labelled historical. Current issue closure dates and estimates alone do not prove the original burndown values.

## 5 Optional consistency note for the Test Plan

The Test Plan already states that work continued after the planned window and does not assert Sprint 3 approval. If the proposed classification is adopted in a later reviewed revision, append only:

> “Sprint 3” is a retrospective grouping proposed for migration issues #26–35, with an observed 4–14 November 2026 IST activity window. This classification does not change the actual testing dates, revision-bound results, original Sprint 1/2 allocations or unresolved evidence limitations.

No test results or coverage values need changing because of this proposed classification.

## 6 Viva summary

> We began with 47 points across the original two sprints. Moving the desktop interface to a browser added ten migration issues worth 44 points, bringing recorded scope to 91. We propose calling that work Sprint 3 retrospectively, using the observed 4–14 November IST window. The records show coordination and delivery, but no formal Sprint 3 timebox. We reused the C++ core and added the synchronized REST service, React UI and supporting validation. Original Sprint 2 work spilled over and the two-page GUI followed on 16 November. Scope growth is verified; a historical burndown rise is not.

If asked “Was Sprint 3 planned?”, say:

> We have dated migration assignments and dependency comments from 4 November, but no evidence that this was formally named or scheduled as Sprint 3. The name is a proposed retrospective classification.

If asked “Did the burndown increase?”, say:

> The backlog expanded by 44 currently estimated points. We have not verified the original burndown data, so we cannot claim a particular rise in remaining work.

## 7 Review disposition

These changes are proposals only. Any later acceptance must carry the actual review date and retain the retrospective qualification. Do not backdate a meeting, approval, iteration, document revision or testing run. No GitHub iteration or issue update is included in this task.
