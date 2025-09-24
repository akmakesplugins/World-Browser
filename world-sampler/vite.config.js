import { defineConfig } from "vite";

export default defineConfig({
    base: "./",
    build: {
        minify: false,
        // target: "es2015",
        // rollupOptions: {
        //     output: {
        //         format: "iife", // Immediately Invoked Function Expression
        //         entryFileNames: "bundle.js", // single JS file name
        //         assetFileNames: "[name][extname]",
        //     },
        // },
    },
});
