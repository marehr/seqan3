const core = require('@actions/core');
const github = require('@actions/github');
const tool_cache = require('@actions/tool-cache');

async function download_node(version) {
    const cached_tool_path = tool_cache.find('doxygen', version);

    // there is a cached version of doxygen, use it
    if (cached_tool_path) {
        console.log(`Found cached doxygen ${version}.`)
        return cached_tool_path;
    }

    // acquire doxygen
    console.log(`Attempt to download doxygen ${version}.`);

    const doxygen_archive = await tool_cache.downloadTool(
        `https://sourceforge.net/projects/doxygen/files/rel-${version}/doxygen-${version}.linux.bin.tar.gz`);

    const doxygen_extracted_folder = await tool_cache.extractTar(
        doxygen_archive, `/tmp`);

    // i.e. will be /opt/hostedtoolcache/doxygen/1.8.17/x64
    return await tool_cache.cacheDir(
        `${doxygen_extracted_folder}/doxygen-${version}`,
        'doxygen', version);
}

async function run() {
    try {
        let version = core.getInput('version');
        if (!version) {
            version = core.getInput('doxygen-version');
        }
        console.log(`Requested doxygen ${version}.`);
        const tool_path = await download_node(version);

        // add this path to $PATH for subsequent setos
        core.addPath(tool_path + '/bin');

    } catch (error) {
        core.setFailed(error.message);
    }
}

run();
