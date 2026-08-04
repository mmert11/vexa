local testsdir = os.scriptdir()

for _, sourcefile in ipairs(os.files(path.join(testsdir, "**", "*.cpp"))) do
    local testdir = path.directory(sourcefile)
    local caseid = path.relative(testdir, testsdir):gsub("[/\\]+", "-")
    local targetname = "test-" .. caseid

    target(targetname)
        set_kind("binary")
        set_default(false)
        set_group("tests")
        set_languages("c++20")
        add_files(sourcefile)
        configure_vexa_consumer()

        add_tests("run", {
            rundir = testdir
        })
end
