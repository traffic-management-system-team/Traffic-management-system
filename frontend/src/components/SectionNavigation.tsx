const sections = [
  { label: "Overview", href: "#overview", current: true },
  { label: "Intersection", href: "#planned", current: false },
  { label: "Activity", href: "#planned", current: false },
];

export function SectionNavigation() {
  return (
    <nav aria-label="Workspace sections" className="flex items-center gap-1">
      {sections.map((section) =>
        section.current ? (
          <a
            key={section.label}
            href={section.href}
            aria-current="page"
            className="nav-link nav-link-current"
          >
            {section.label}
          </a>
        ) : (
          <span
            key={section.label}
            aria-disabled="true"
            className="nav-link nav-link-muted"
            title="Planned for a later frontend issue"
          >
            {section.label}
            <span className="ml-2 rounded-full border border-white/10 px-2 py-0.5 text-[10px] uppercase tracking-[0.12em] text-slate-500">
              Soon
            </span>
          </span>
        ),
      )}
    </nav>
  );
}