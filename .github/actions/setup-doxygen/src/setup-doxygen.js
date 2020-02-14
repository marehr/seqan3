const core = require('@actions/core');
const github = require('@actions/github');
const tool_cache = require('@actions/tool-cache');

async function run() {
    try {
        // `who-to-greet` input defined in action metadata file
        const nameToGreet = core.getInput('who-to-greet');
        let doxygen_version = core.getInput('version');
        if (!doxygen_version) {
          doxygen_version = core.getInput('doxygen-version');
        }
        console.log(`Hello ${nameToGreet}!`);
        console.log(`Hello ${doxygen_version}!`);
        // const time = (new Date()).toTimeString();
        // core.setOutput("time", time);
        // const doxygen_version = "1.8.17";

        const doxygen_archive = await tool_cache.downloadTool(
            `https://sourceforge.net/projects/doxygen/files/rel-${doxygen_version}/doxygen-${doxygen_version}.linux.bin.tar.gz`);
        console.log(`doxygen_archive: ${doxygen_archive}`);

        const doxygen_extracted_folder = await tool_cache.extractTar(
            doxygen_archive, `/tmp`);
        console.log(`doxygen_extracted_folder: ${doxygen_extracted_folder}`);

        const doxygen_folder = `${doxygen_extracted_folder}/doxygen-${doxygen_version}`;
        console.log(`doxygen_extracted_folder: ${doxygen_extracted_folder}`);

        const doxygen_cache_path = await tool_cache.cacheDir(doxygen_folder, 'doxygen', doxygen_version);
        console.log(`doxygen_cache_path: ${doxygen_cache_path}`);

        // add this path to $PATH for subsequent setos
        core.addPath(doxygen_cache_path + '/bin');

    } catch (error) {
        core.setFailed(error.message);
    }
}

run();
