#include <napi.h>

class ProcessorAddon : public Napi::Addon<ProcessorAddon> {
public:
    ProcessorAddon(Napi::Env env, Napi::Object exports) {
        DefineAddon(exports, {
            InstanceMethod("test", &ProcessorAddon::test),
            InstanceMethod("test_promise", &ProcessorAddon::test_promise)
        });
    }

private:
    void test(const Napi::CallbackInfo& info);
    Napi::Value test_promise(const Napi::CallbackInfo& info);
};