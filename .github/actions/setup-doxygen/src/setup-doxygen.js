const core = require('@actions/core');
const github = require('@actions/github');
const tool_cache = require('@actions/tool-cache');

async function run() {
    try {
        // `who-to-greet` input defined in action metadata file
        const nameToGreet = core.getInput('who-to-greet');
        let version = core.getInput('version');
        if (!version) {
          version = core.getInput('doxygen-version');
        }
        console.log(`Hello ${nameToGreet}!`);
        console.log(`Hello ${version}!`);
        // const time = (new Date()).toTimeString();
        core.setOutput("time", time);
        const doxygen_version = "1.8.17";

        const doxygen_archive = await tool_cache.downloadTool(
            `https://sourceforge.net/projects/doxygen/files/rel-${doxygen_version}/doxygen-${doxygen_version}.linux.bin.tar.gz`);

        const doxygen_bin_folder = await tool_cache.extractTar(
            doxygen_archive, `/tmp/doxygen-${doxygen_version}`);

        const doxygen_cache_path = await tool_cache.cacheDir(doxygen_bin_folder, 'doxygen', doxygen_version);

        // add this path to $PATH for subsequent setos
        core.addPath(doxygen_cache_path + '/bin');

    } catch (error) {
        core.setFailed(error.message);
    }
}

run();
