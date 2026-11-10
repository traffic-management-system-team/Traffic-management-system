export function BrandMark() {
  return (
    <span
      aria-hidden="true"
      className="brand-mark grid h-10 w-10 place-items-center rounded-2xl"
    >
      <svg viewBox="0 0 40 40" className="h-7 w-7" fill="none">
        <rect
          x="12"
          y="5"
          width="16"
          height="30"
          rx="8"
          fill="currentColor"
          fillOpacity=".14"
        />
        <circle cx="20" cy="12" r="3" fill="#F87171" />
        <circle cx="20" cy="20" r="3" fill="#FBBF24" />
        <circle cx="20" cy="28" r="3" fill="#4ADE80" />
      </svg>
    </span>
  );
}