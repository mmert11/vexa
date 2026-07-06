set_project("VEXA")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

add_requires("llvm", "z3", "lief")

target("VEXA")
    set_kind("static")

    add_files("src/*.cpp", "src/**/*.cpp")
    
    add_includedirs("include", {public = true})
    
    add_packages("llvm", "z3", "lief")
    
    set_languages("c++20")
    
    on_install(function (target)
        os.cp("include/*", path.join(target:installdir(), "include"))
        os.cp(target:targetfile(), path.join(target:installdir(), "lib"))
    end)
