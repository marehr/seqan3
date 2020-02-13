const core = require('@actions/core');
const github = require('@actions/github');
const tool_cache = require('@actions/tool-cache');

async function run() {
    try {
        // `who-to-greet` input defined in action metadata file
        const nameToGreet = core.getInput('who-to-greet');
        console.log(`Hello ${nameToGreet}!`);
        const time = (new Date()).toTimeString();
        core.setOutput("time", time);
        const doxygen_version = "1.8.17";

        const doxygen_archive = await tc.downloadTool(
            'https://sourceforge.net/projects/doxygen/files/'+
            'rel-' + doxygen_version + '/doxygen-' + doxygen_version + '.linux.bin.tar.gz');

        const doxygen_bin_folder = await tc.extractTar(doxygen_archive, '/tmp/doxygen-' + doxygen_version + '/bin');

        const cached_path = await tc.cacheDir(doxygen_bin_folder, 'doxygen', doxygen_version);
        core.addPath(cached_path);

    } catch (error) {
        core.setFailed(error.message);
    }
}

run();
