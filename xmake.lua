set_project("vexa")
set_version("1.3")

add_rules("mode.debug", "mode.release")
add_requires("llvm", "bitwuzla", "lief", "quill", "cli11")

option("remill")
	-- !!!!!!!!!!!!!!!!!!!!!!!!!!
    set_default(path.join(os.projectdir(), "..", "remill"))
    set_showmenu(true)
    set_description("remill path")

includes("xmake/vexa_consumer.lua")

target("vexa")
    set_kind("static")
    set_languages("c++20")
    add_rules("utils.install.cmake_importfiles")
    add_files("src/*.cpp", "src/**/*.cpp")
    add_includedirs("include", {public = true})
    add_packages("llvm", "bitwuzla", "lief", "quill")

    -- install options
    set_languages("c++20")
    on_install(function (target)
        os.cp("include/*", path.join(target:installdir(), "include"))
        os.cp(target:targetfile(), path.join(target:installdir(), "lib"))
    end)

    set_optimize("fastest")
    set_symbols("debug")
    set_strip("none")

target("vexa-cli")
    set_kind("binary")
    set_languages("c++20")
    add_files("cli/*.cpp")

    configure_vexa_consumer()
    add_packages("cli11")

    set_optimize("fastest")
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
