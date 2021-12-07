package(default_visibility = ["//visibility:public"])

cc_library(
    name = "CGImysql-lib",
    srcs = [
        ":sql_connection_pool.cpp",
    ],
    hdrs = [
        ":sql_connection_pool.h",
    ],
    deps = [
        "//lock:lock-lib",
        "//log:log-lib",
        "@mysql//:mysql-lib",
    ],
)
