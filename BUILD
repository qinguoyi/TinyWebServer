cc_binary(
    name = "server",
    srcs = [
        ":main.cpp",
        ":webserver.cpp",
        ":webserver.h",
        ":config.cpp",
        ":config.h",
    ],
    deps = [
        "//http:http-lib",
        "//threadpool:threadpool-lib",
        "//timer:timer-lib",
    ],
    linkopts = [
        "-lpthread",
        "-lmysqlclient",
    ],
)
