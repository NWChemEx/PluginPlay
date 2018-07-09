set(token "d28e8df2f89944f49e7144c747ffe3a114547469")
set(gh_repo "https://api.github.com/repos/NWChemEx-Project/LibChemist")

if(BUILD_PYBINDINGS)
    set(WITH_SHARED TRUE)
endif()
hunter_config(
        LibChemist
        VERSION 0.0.0
        URL "${gh_repo}/tarball/master?access_token=${token}"
        SHA1 "96de0f1fa0145d6540562826187e14e552522a15"
        CMAKE_ARGS "BUILD_TESTS=off;BUILD_SHARED_LIBS=${WITH_SHARED}"
)
set(gh_repo "https://api.github.com/repos/NWChemEx-Project/Utilities")
hunter_config(
        Utilities
        VERSION 0.0.0
        URL "${gh_repo}/tarball/master?access_token=${token}"
        SHA1 "c3c7d10c77b7744a70f1423e50e0cd039d2064f5"
        CMAKE_ARGS "BUILD_TESTS=off;BUILD_SHARED_LIBS=${WITH_SHARED}"
)

hunter_config(
        bphash
        VERSION "v1.1"
        URL "https://github.com/bennybp/BPHash/archive/v1.1.tar.gz"
        SHA1 "88e14eda8b1f8224caeae8c43407770a651e027e"
        CMAKE_ARGS "BUILD_SHARED_LIBS=${WITH_SHARED}"
)