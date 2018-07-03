set(token "d28e8df2f89944f49e7144c747ffe3a114547469")
set(gh_repo "https://api.github.com/repos/NWChemEx-Project/LibChemist")

if(BUILD_PYBINDINGS)
    set(WITH_SHARED TRUE)
endif()
hunter_config(
        LibChemist
        VERSION 0.0.0
        URL "${gh_repo}/tarball/master?access_token=${token}"
        SHA1 "0a02b5457db7d87dd00e1a991c530bb027fdff51"
        CMAKE_ARGS "BUILD_TESTS=off;BUILD_SHARED_LIBS=${WITH_SHARED}"
)
set(gh_repo "https://api.github.com/repos/NWChemEx-Project/Utilities")
hunter_config(
        Utilities
        VERSION 0.0.0
        URL "${gh_repo}/tarball/master?access_token=${token}"
        SHA1 "42446742165e643a10778f4a3402a5164728d0bb"
        CMAKE_ARGS "BUILD_TESTS=off;BUILD_SHARED_LIBS=${WITH_SHARED}"
)

hunter_config(
        bphash
        VERSION "v1.1"
        URL "https://github.com/bennybp/BPHash/archive/v1.1.tar.gz"
        SHA1 "88e14eda8b1f8224caeae8c43407770a651e027e"
        CMAKE_ARGS "BUILD_SHARED_LIBS=${WITH_SHARED}"
)
