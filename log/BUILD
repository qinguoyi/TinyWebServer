package(default_visibility = ["//visibility:public"])

cc_library(
    name = "log-lib",
    srcs = [
        ":log.cpp",
    ],
    hdrs = [
        ":block_queue.h",
        ":log.h",
    ],
    deps = [
        "//lock:lock-lib",
    ],
)
