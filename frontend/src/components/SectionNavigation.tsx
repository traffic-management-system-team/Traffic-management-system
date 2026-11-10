export function SectionNavigation() {
  return (
    <nav aria-label="Workspace sections" className="flex items-center gap-1">
      <a href="#overview" aria-current="page" className="nav-link nav-link-current">Overview</a>
      <a href="#intersection" className="nav-link nav-link-muted">Intersection</a>
    </nav>
  );
}
