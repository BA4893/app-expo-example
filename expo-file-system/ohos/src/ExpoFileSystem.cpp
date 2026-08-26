#include "ExpoFileSystem.h"

#include "JSValue.h"
#include "Promise.h"

#include "hilog/log.h"

#include <cerrno>
#include <cstring>
#include <string>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace expo {

namespace {

constexpr unsigned int EXPO_FS_LOG_DOMAIN = 0x0000;
constexpr const char *EXPO_FS_LOG_TAG = "ExpoFileSystem";

napi_value MakeString(napi_env env, const std::string &value)
{
    napi_value result = nullptr;
    napi_create_string_utf8(env, value.c_str(), value.size(), &result);
    return result;
}

napi_value MakeUndefined(napi_env env)
{
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value MakeError(napi_env env, const std::string &message)
{
    napi_value msg = MakeString(env, message);
    napi_value error = nullptr;
    napi_create_error(env, nullptr, msg, &error);
    return error;
}

// Reads the single string argument at index 0. Returns false on type mismatch.
bool ReadStringArg(napi_env env, napi_callback_info info, std::string &out)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        return false;
    }

    napi_valuetype type;
    napi_typeof(env, args[0], &type);
    if (type != napi_string) {
        return false;
    }

    size_t len = 0;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    std::string s(len, '\0');
    napi_get_value_string_utf8(env, args[0], &s[0], len + 1, &len);
    out = std::move(s);
    return true;
}

bool ReadStringStringArg(napi_env env, napi_callback_info info, std::string &a, std::string &b)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr, nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 2) {
        return false;
    }

    auto read = [&](napi_value v, std::string &out) -> bool {
        napi_valuetype type;
        napi_typeof(env, v, &type);
        if (type != napi_string) {
            return false;
        }
        size_t len = 0;
        napi_get_value_string_utf8(env, v, nullptr, 0, &len);
        std::string s(len, '\0');
        napi_get_value_string_utf8(env, v, &s[0], len + 1, &len);
        out = std::move(s);
        return true;
    };

    return read(args[0], a) && read(args[1], b);
}

// ---- POSIX-backed operations ----

napi_value WriteFile(napi_env env, napi_callback_info info)
{
    // Signature: writeFile(path: string, contents: string) -> Promise<void>
    std::string path;
    std::string contents;
    if (!ReadStringStringArg(env, info, path, contents)) {
        return PromiseRejected(env, MakeError(env, "writeFile expects (string, string)"));
    }

    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: writeFile open failed path=%{public}s errno=%{public}d",
                     EXPO_FS_LOG_TAG, path.c_str(), errno);
        return PromiseRejected(env, MakeError(env, "writeFile: open failed: " + std::string(strerror(errno))));
    }

    ssize_t written = write(fd, contents.data(), contents.size());
    close(fd);
    if (written < 0 || static_cast<size_t>(written) != contents.size()) {
        OH_LOG_ERROR(LOG_APP, "%{public}s: writeFile write failed errno=%{public}d",
                     EXPO_FS_LOG_TAG, errno);
        return PromiseRejected(env, MakeError(env, "writeFile: write failed"));
    }

    OH_LOG_INFO(LOG_APP, "%{public}s: writeFile ok path=%{public}s bytes=%{public}zu",
                EXPO_FS_LOG_TAG, path.c_str(), contents.size());
    return PromiseResolved(env, MakeUndefined(env));
}

napi_value ReadFile(napi_env env, napi_callback_info info)
{
    std::string path;
    if (!ReadStringArg(env, info, path)) {
        return PromiseRejected(env, MakeError(env, "readFile expects a string path"));
    }

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return PromiseRejected(env, MakeError(env, "readFile: open failed: " + std::string(strerror(errno))));
    }

    std::string contents;
    char buf[4096];
    ssize_t n = 0;
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        contents.append(buf, static_cast<size_t>(n));
    }
    close(fd);

    if (n < 0) {
        return PromiseRejected(env, MakeError(env, "readFile: read failed"));
    }

    OH_LOG_INFO(LOG_APP, "%{public}s: readFile ok path=%{public}s bytes=%{public}zu",
                EXPO_FS_LOG_TAG, path.c_str(), contents.size());
    return PromiseResolved(env, MakeString(env, contents));
}

napi_value StatFile(napi_env env, napi_callback_info info)
{
    std::string path;
    if (!ReadStringArg(env, info, path)) {
        return PromiseRejected(env, MakeError(env, "stat expects a string path"));
    }

    struct stat st {};
    if (stat(path.c_str(), &st) != 0) {
        return PromiseRejected(env, MakeError(env, "stat: " + std::string(strerror(errno))));
    }

    napi_value result = nullptr;
    napi_create_object(env, &result);

    napi_value size = nullptr;
    napi_create_double(env, static_cast<double>(st.st_size), &size);
    napi_set_named_property(env, result, "size", size);

    napi_value isDir = nullptr;
    napi_get_boolean(env, S_ISDIR(st.st_mode), &isDir);
    napi_set_named_property(env, result, "isDirectory", isDir);

    napi_value isFile = nullptr;
    napi_get_boolean(env, S_ISREG(st.st_mode), &isFile);
    napi_set_named_property(env, result, "isFile", isFile);

    OH_LOG_INFO(LOG_APP, "%{public}s: stat ok path=%{public}s size=%{public}lld",
                EXPO_FS_LOG_TAG, path.c_str(), static_cast<long long>(st.st_size));
    return PromiseResolved(env, result);
}

napi_value MakeDirectory(napi_env env, napi_callback_info info)
{
    std::string path;
    if (!ReadStringArg(env, info, path)) {
        return PromiseRejected(env, MakeError(env, "mkdir expects a string path"));
    }

    if (mkdir(path.c_str(), 0777) != 0) {
        return PromiseRejected(env, MakeError(env, "mkdir: " + std::string(strerror(errno))));
    }

    OH_LOG_INFO(LOG_APP, "%{public}s: mkdir ok path=%{public}s", EXPO_FS_LOG_TAG, path.c_str());
    return PromiseResolved(env, MakeUndefined(env));
}

napi_value ListDirectory(napi_env env, napi_callback_info info)
{
    std::string path;
    if (!ReadStringArg(env, info, path)) {
        return PromiseRejected(env, MakeError(env, "listDir expects a string path"));
    }

    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        return PromiseRejected(env, MakeError(env, "listDir: " + std::string(strerror(errno))));
    }

    std::vector<std::string> names;
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        names.emplace_back(entry->d_name);
    }
    closedir(dir);

    napi_value result = nullptr;
    napi_create_array_with_length(env, names.size(), &result);
    for (size_t i = 0; i < names.size(); ++i) {
        napi_set_element(env, result, static_cast<uint32_t>(i), MakeString(env, names[i]));
    }

    OH_LOG_INFO(LOG_APP, "%{public}s: listDir ok path=%{public}s count=%{public}zu",
                EXPO_FS_LOG_TAG, path.c_str(), names.size());
    return PromiseResolved(env, result);
}

napi_value DeleteFile(napi_env env, napi_callback_info info)
{
    std::string path;
    if (!ReadStringArg(env, info, path)) {
        return PromiseRejected(env, MakeError(env, "delete expects a string path"));
    }

    if (unlink(path.c_str()) != 0) {
        return PromiseRejected(env, MakeError(env, "delete: " + std::string(strerror(errno))));
    }

    OH_LOG_INFO(LOG_APP, "%{public}s: delete ok path=%{public}s", EXPO_FS_LOG_TAG, path.c_str());
    return PromiseResolved(env, MakeUndefined(env));
}

} // namespace

void RegisterExpoFileSystemModule(ExpoModulesRegistry &registry)
{
    registry.Register(ModuleDefinition{
        "ExpoFileSystem",
        {
            {"writeFile", WriteFile},
            {"readFile", ReadFile},
            {"statFile", StatFile},
            {"makeDirectory", MakeDirectory},
            {"listDirectory", ListDirectory},
            {"deleteFile", DeleteFile},
        },
    });
}

#ifdef USE_JSI

void RegisterJsiFileSystemModule(ExpoJsiBridge &bridge)
{
    bridge.RegisterModule(JsiModuleDefinition{
        "ExpoFileSystem",
        {
            {"writeFile", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
                if (count < 2) {
                    return jsi::Value::undefined();
                }
                std::string path = args[0].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());
                std::string contents = args[1].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());

                int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd < 0) {
                    return jsi::Value::undefined();
                }
                write(fd, contents.data(), contents.size());
                close(fd);
                return jsi::Value::undefined();
            }},
            {"readFile", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
                if (count < 1) {
                    return jsi::String::createFromUtf8(rt, "");
                }
                std::string path = args[0].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());

                int fd = open(path.c_str(), O_RDONLY);
                if (fd < 0) {
                    return jsi::String::createFromUtf8(rt, "");
                }

                std::string contents;
                char buf[4096];
                ssize_t n = 0;
                while ((n = read(fd, buf, sizeof(buf))) > 0) {
                    contents.append(buf, static_cast<size_t>(n));
                }
                close(fd);
                return jsi::String::createFromUtf8(rt, contents.c_str());
            }},
            {"statFile", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
                if (count < 1) {
                    return jsi::Value::undefined();
                }
                std::string path = args[0].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());

                struct stat st {};
                if (stat(path.c_str(), &st) != 0) {
                    return jsi::Value::undefined();
                }

                auto obj = rt.createObject();
                obj->setProperty(rt, "size", jsi::Value(static_cast<double>(st.st_size)));
                obj->setProperty(rt, "isDirectory", jsi::Value(S_ISDIR(st.st_mode)));
                obj->setProperty(rt, "isFile", jsi::Value(S_ISREG(st.st_mode)));
                return jsi::Value(obj);
            }},
            {"makeDirectory", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
                if (count < 1) {
                    return jsi::Value::undefined();
                }
                std::string path = args[0].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());
                mkdir(path.c_str(), 0777);
                return jsi::Value::undefined();
            }},
            {"listDirectory", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
                if (count < 1) {
                    return jsi::Value::undefined();
                }
                std::string path = args[0].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());

                DIR *dir = opendir(path.c_str());
                if (dir == nullptr) {
                    return jsi::Value::undefined();
                }

                std::vector<std::string> names;
                struct dirent *entry = nullptr;
                while ((entry = readdir(dir)) != nullptr) {
                    names.emplace_back(entry->d_name);
                }
                closedir(dir);

                auto arr = rt.createArray(names.size());
                for (size_t i = 0; i < names.size(); ++i) {
                    arr->setValueAtIndex(rt, static_cast<uint32_t>(i), jsi::String::createFromUtf8(rt, names[i].c_str()));
                }
                return jsi::Value(arr);
            }},
            {"deleteFile", [](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
                if (count < 1) {
                    return jsi::Value::undefined();
                }
                std::string path = args[0].getString(jsi::Runtime::getDefaultJsAllocator())->utf8(jsi::Runtime::getDefaultJsAllocator());
                unlink(path.c_str());
                return jsi::Value::undefined();
            }},
        },
    });
}

#endif

} // namespace expo
