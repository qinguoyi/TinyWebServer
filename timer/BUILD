package(default_visibility = ["//visibility:public"])

cc_library(
    name = "timer-lib",
    srcs = [
        ":lst_timer.cpp",
    ],
    deps = [
        ":timer-hdr",
        "//http:http-hdr",
    ],
)

cc_library(
    name = "timer-hdr",
    hdrs = [
        ":lst_timer.h",
    ],
    deps = [
        "//log:log-lib",
    ],
)
