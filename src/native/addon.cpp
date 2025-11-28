#include "addon.hpp"
#include "napi.h"
#include <functional>
#include <iostream>
#include <sndfile.h>
#include <thread>

#define NOOP_FUNC(env) Napi::Function::New(env, [](const Napi::CallbackInfo& info){})

void ProcessorAddon::test(const Napi::CallbackInfo& info) {
    if (info[0].IsNumber()) {
        std::cout << sf_version_string() << "\n";
        std::cout << info[0].As<Napi::Number>().Int32Value() << "\n";
    }
}

Napi::Value ProcessorAddon::test_promise(const Napi::CallbackInfo& info) {
    auto deffered = Napi::Promise::Deferred::New(info.Env());
    auto tsfn = Napi::ThreadSafeFunction::New(info.Env(), NOOP_FUNC(info.Env()), "tsfn", 0, 1);

    std::thread([tsfn, deffered]() {
        tsfn.NonBlockingCall([deffered](Napi::Env env, Napi::Function) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            deffered.Resolve(Napi::Boolean::New(env, true));
        });

        tsfn.Release();
    }).detach();

    return deffered.Promise();
}

NODE_API_ADDON(ProcessorAddon)