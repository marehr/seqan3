const core = require('@actions/core');
const github = require('@actions/github');
const tool_cache = require('@actions/tool-cache');

async function download_node(version) {
    const cached_tool_path = tc.find('doxygen', version);

    // there is a cached version of doxygen, use it
    if (cached_tool_path) {
        console.log(`Found cached doxygen ${doxygen_version}.`)
        return cached_tool_path;
    }

    // acquire doxygen
    console.log(`Attempt to download doxygen ${doxygen_version}.`);

    const doxygen_archive = await tool_cache.downloadTool(
        `https://sourceforge.net/projects/doxygen/files/rel-${doxygen_version}/doxygen-${doxygen_version}.linux.bin.tar.gz`);

    const doxygen_extracted_folder = await tool_cache.extractTar(
        doxygen_archive, `/tmp`);

    // i.e. will be /opt/hostedtoolcache/doxygen/1.8.17/x64
    return await tool_cache.cacheDir(
        `${doxygen_extracted_folder}/doxygen-${doxygen_version}`,
        'doxygen', doxygen_version);
}

async function run() {
    try {
        let doxygen_version = core.getInput('version');
        if (!doxygen_version) {
          doxygen_version = core.getInput('doxygen-version');
        }
        console.log(`Requested doxygen ${doxygen_version}.`);
        const tool_path = await download_node(doxygen_version);

        // add this path to $PATH for subsequent setos
        core.addPath(tool_path + '/bin');

    } catch (error) {
        core.setFailed(error.message);
    }
}

run();
