set_project("vexa")
set_version("1.4")

add_rules("mode.debug", "mode.release")

package("remill")
    set_sourcedir(path.join(os.scriptdir(), "external/remill"))

    on_install(function (package)
    import("package.tools.cmake")

    local srcdir = package:sourcedir()
    local depsdir = path.join(srcdir, "dependencies")
    local depsbuild = path.join(depsdir, "build")
    local depsinstall = path.join(depsdir, "install")

    local deps_args = {
        "-G", "Ninja",
        "-S", depsdir,
        "-B", depsbuild
    }

    local remill_args = {
        "-G Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_PREFIX_PATH=" .. depsinstall
    }

    --if package:is_plat("windows") then
    --    table.insert(deps_args, "-DCMAKE_C_COMPILER=clang-cl")
    --    table.insert(deps_args, "-DCMAKE_CXX_COMPILER=clang-cl")

    --    table.insert(remill_args, "-DCMAKE_C_COMPILER=clang-cl")
    --    table.insert(remill_args, "-DCMAKE_CXX_COMPILER=clang-cl")
    --else
    --    table.insert(deps_args, "-DUSE_EXTERNAL_LLVM=ON")
    --end

    table.insert(deps_args, "-DUSE_EXTERNAL_LLVM=ON")
    
    os.vrunv("cmake", deps_args)
    os.vrunv("cmake", {"--build", depsbuild})

    cmake.install(package, remill_args)

    os.cp(path.join(depsinstall, "include", "*"),
          path.join(package:installdir(), "include"))

    os.cp(path.join(depsinstall, "lib", "*"),
          path.join(package:installdir(), "lib"))
    end)
package_end()

package("bitwuzla")
    set_sourcedir(path.join(os.scriptdir(), "external/bitwuzla"))

    add_includedirs("include")

    add_links(
        "bitwuzla",
        "bitwuzlabb",
        "bitwuzlabv",
        "bitwuzlals"
    )

    add_syslinks(
        "mpfr",
        "gmp"
    )

    on_install(function (package)
        local srcdir = package:sourcedir()
        local builddir = path.join(srcdir, "build-xmake")

        os.vrunv("python3", {
            path.join(srcdir, "configure.py"),
            "release",
            "--static",
            "--no-testing",
            "--no-unit-testing",
            "--no-python",
            "--prefix=" .. package:installdir(),
            "-b", builddir
        }, {curdir = srcdir})

        os.vrunv("ninja", {
            "-C", builddir,
            "install"
        })

        if package:is_plat("linux") then
            local libdir = path.join(
                package:installdir(),
                "lib",
                "x86_64-linux-gnu"
            )

            os.cp(
                path.join(libdir, "*.a"),
                path.join(package:installdir(), "lib")
            )
        end
    end)
package_end()

add_requires("llvm", "lief", "quill", "cli11")
add_requires("bitwuzla", {system = false})
add_requires("remill", {system = false})
includes("xmake/vexa_consumer.lua")

target("vexa")
    set_kind("static")
    set_languages("c++20")
    add_rules("utils.install.cmake_importfiles")
    add_rules("utils.install.pkgconfig_importfiles")
    add_files("src/*.cpp", "src/**/*.cpp")
    add_includedirs("include", {public = true})
    add_packages("llvm", "bitwuzla", "lief", "quill", "remill")

    -- install options
    set_languages("c++20")
    add_headerfiles(
        "include/(vexa/**.h)",
        "include/(vexa/**.hpp)"
    )

    set_symbols("debug")
    set_strip("none")

target("vexa-cli")
    set_kind("binary")
    set_languages("c++20")
    add_files("cli/*.cpp")

    configure_vexa_consumer()
    add_packages("cli11")

    set_symbols("debug")
    set_strip("none")

after_build(function (target)
    import("core.project.task")
    -- generate CMakeLists.txt
    --task.run("project", {kind = "cmake"})
    -- generate compile_commands.json
    task.run("project", {kind = "compile_commands"})
end)

target_end()
includes("tests/xmake.lua")
