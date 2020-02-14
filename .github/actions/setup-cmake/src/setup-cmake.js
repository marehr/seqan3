const core = require('@actions/core');
// const github = require('@actions/github');
const tool_cache = require('@actions/tool-cache');

async function download_node(version) {
    const cached_tool_path = tool_cache.find('cmake', version);

    // there is a cached version of cmake, use it
    if (cached_tool_path) {
        console.log(`Found cached cmake ${version}.`)
        return cached_tool_path;
    }

    // acquire cmake
    const download_url = `https://github.com/Kitware/CMake/releases/download/v${version}/cmake-${version}-Linux-x86_64.tar.gz`;
    console.log(`Attempt to download cmake ${version} (${download_url}).`);
    const cmake_archive = await tool_cache.downloadTool(download_url);
    console.log(`Download seems to be a success (${cmake_archive}).`);

    console.log(`Attempt to unpack cmake ${version}.`);
    const cmake_extracted_folder = await tool_cache.extractTar(
        cmake_archive, `/tmp/cmake-${version}`);
    console.log(`Unpack seems to be a success (${cmake_extracted_folder}).`);

    console.log(`Cache cmake ${version}.`);
    // i.e. will be /opt/hostedtoolcache/cmake/1.8.17/x64
    return await tool_cache.cacheDir(
        `${cmake_extracted_folder}/cmake-${version}-Linux-x86_64/`,
        'cmake', version);
}

async function run() {
    try {
        let version = core.getInput('version');
        if (!version) {
            version = core.getInput('cmake-version');
        }
        console.log(`Requested cmake ${version}.`);
        const tool_path = await download_node(version);

        // add this path to $PATH for subsequent setos
        core.addPath(tool_path + '/bin');

    } catch (error) {
        core.setFailed(error.message);
    }
}

run();
