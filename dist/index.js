"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const osu_beatmap_processor_node_1 = __importDefault(require("../build/Release/osu-beatmap-processor.node"));
(() => {
    osu_beatmap_processor_node_1.default.test_promise().then(() => {
        console.log("finished doing ts");
    });
    osu_beatmap_processor_node_1.default.test(5);
    console.log();
})();
