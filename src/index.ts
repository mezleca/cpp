import processor from "../build/Release/osu-beatmap-processor.node"

(() => {
    processor.test_promise().then(() => {
        console.log("finished doing ts");
    })
    processor.test(5);
    console.log()
})();