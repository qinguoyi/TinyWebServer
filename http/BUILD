package(default_visibility = ["//visibility:public"])

cc_library(
    name = "http-lib",
    srcs = [
        ":http_conn.cpp",
    ],
    deps = [
        ":http-hdr",
        "//CGImysql:CGImysql-lib",
    ],
)

cc_library(
    name = "http-hdr",
    hdrs = [
        ":http_conn.h",
    ],
    deps = [
        "//lock:lock-lib",
        "//log:log-lib",
        "//CGImysql:CGImysql-lib",
        "//timer:timer-hdr",
    ],
)
