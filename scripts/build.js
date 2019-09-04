const fs = require("fs");
const path = require("path");
const mkdirp = require("mkdirp");
const prettier = require("prettier");
const execSync = require("child_process").execSync;
const os = require("os");

const execute = cmd => execSync(cmd, {stdio: "inherit"});

/**
 * WASM Output Options
 */
const WEB_WASM_OPTIONS = {
    inputFile: "emtest.async.js",
    inputWasmFile: "emtest.async.wasm",
    format: false,
    packageName: "emtest",
    build: true // flag as to whether to build
};

/**
 * Filter for the runtimes we should build
 */
const AVAILABLE_RUNTIMES = [WEB_WASM_OPTIONS];

// Select the runtimes - if no builds are specified then build everything
const RUNTIMES = AVAILABLE_RUNTIMES.filter(runtime => runtime.build).length ? AVAILABLE_RUNTIMES.filter(runtime => runtime.build) : AVAILABLE_RUNTIMES;

function compileRuntime({inputFile, inputWasmFile, format}) {
    console.log("-- Building %s", inputFile);

    const OUTPUT_DIRECTORY = path.join(__dirname, "..", "dist");
    const BUILD_DIRECTORY = path.join(__dirname, "..", "build");

    if (inputWasmFile) {
        console.log("-- Copying WASM file %s", inputWasmFile);
        fs.copyFileSync(path.join(BUILD_DIRECTORY, inputWasmFile), path.join(OUTPUT_DIRECTORY, inputWasmFile));
    }

    console.debug("-- Creating wrapped js runtime");
    const runtimeText = String(
        fs.readFileSync(path.join(BUILD_DIRECTORY, inputFile), {
            encoding: "utf8"
        })
    );

    let source = runtimeText;
    if (format) {
        console.debug("Formatting code");
        source = prettier.format(source, {
            printWidth: 200,
            tabWidth: 4,
            parser: "babylon"
        });
    }

    fs.writeFileSync(path.join(OUTPUT_DIRECTORY, inputFile), source);
}

function compileCPP() {
    const BASE_DIRECTORY = path.join(__dirname, "..", "cmake");
    let cmd = `emcmake cmake ./ && emmake make -j${os.cpus().length}`;
    cmd = `cd ${BASE_DIRECTORY} && ${cmd}`;
    execute(cmd);
}

try {
    compileCPP();
    RUNTIMES.map(compileRuntime);
} catch (e) {
    console.log(e.message);
    process.exit(1);
}
