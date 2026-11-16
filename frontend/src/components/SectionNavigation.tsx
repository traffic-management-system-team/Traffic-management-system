interface SectionNavigationProps {
  currentPage: "overview" | "intersection";
}

export function SectionNavigation({ currentPage }: SectionNavigationProps) {
  return (
    <nav aria-label="Workspace sections" className="flex items-center gap-1">
      <a
        href="#overview"
        aria-current={currentPage === "overview" ? "page" : undefined}
        className={`nav-link ${currentPage === "overview" ? "nav-link-current" : "nav-link-muted"}`}
      >
        Control Centre
      </a>
      <a
        href="#intersection"
        aria-current={currentPage === "intersection" ? "page" : undefined}
        className={`nav-link ${currentPage === "intersection" ? "nav-link-current" : "nav-link-muted"}`}
      >
        Simulation Monitor
      </a>
    </nav>
  );
}
