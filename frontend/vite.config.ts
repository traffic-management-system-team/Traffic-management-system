import { defineConfig } from "vitest/config";
import { loadEnv } from "vite";
import react from "@vitejs/plugin-react";
import tailwindcss from "@tailwindcss/vite";

const defaultApiTarget = "http://127.0.0.1:18080";

export default defineConfig(({ mode }) => {
  const env = loadEnv(mode, process.cwd(), "API_");

  return {
    plugins: [react(), tailwindcss()],
    server: {
      proxy: {
        "/api": {
          target: env.API_PROXY_TARGET || defaultApiTarget,
          // Preserve the browser-facing Host and Origin. The backend's local
          // origin policy compares them unless TMS_PUBLIC_ORIGIN is configured.
          changeOrigin: false,
        },
      },
    },
    test: {
      environment: "node",
      clearMocks: true,
      restoreMocks: true,
      include: ["src/**/*.test.ts", "src/**/*.test.tsx"],
    },
  };
});
