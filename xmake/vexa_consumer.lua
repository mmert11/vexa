function configure_vexa_consumer()
    add_deps("vexa", {inherit = false})
    add_includedirs(path.join(os.projectdir(), "include"))
    add_linkdirs("$(builddir)/$(os)/$(arch)/$(mode)")

    add_packages("llvm", "bitwuzla", "lief", "quill", "remill")

    add_linkgroups(
        "vexa",
        "remill_arch",
        "remill_arch_x86",
        "remill_arch_aarch64",
        "remill_arch_sparc32",
        "remill_arch_sparc64",
        "remill_arch_sleigh",
        "remill_bc",
        "remill_os",
        "remill_version",

        "decomp",
        "sla",
        "slaSupport",

        "xed",
        "xed-ild",
        "glog",
        "gflags",

        {group = true}
    )

    local llvm_links = {
        "LLVMX86TargetMCA",
        "LLVMMCA",
        "LLVMX86Disassembler",
        "LLVMX86AsmParser",
        "LLVMX86CodeGen",
        "LLVMX86Desc",
        "LLVMX86Info",
        "LLVMMCDisassembler",
        "LLVMAsmPrinter",
        "LLVMPasses",
        "LLVMIRPrinter",
        "LLVMHipStdPar",
        "LLVMCoroutines",
        "LLVMipo",
        "LLVMInstrumentation",
        "LLVMVectorize",
        "LLVMSandboxIR",
        "LLVMLinker",
        "LLVMFrontendOpenMP",
        "LLVMFrontendDirective",
        "LLVMFrontendAtomic",
        "LLVMFrontendOffloading",
        "LLVMObjectYAML",
        "LLVMGlobalISel",
        "LLVMSelectionDAG",
        "LLVMCodeGen",
        "LLVMTarget",
        "LLVMScalarOpts",
        "LLVMInstCombine",
        "LLVMObjCARCOpts",
        "LLVMCodeGenTypes",
        "LLVMCGData",
        "LLVMCFGuard",
        "LLVMAggressiveInstCombine",
        "LLVMTransformUtils",
        "LLVMBitWriter",
        "LLVMAnalysis",
        "LLVMProfileData",
        "LLVMSymbolize",
        "LLVMDebugInfoBTF",
        "LLVMDebugInfoPDB",
        "LLVMDebugInfoMSF",
        "LLVMDebugInfoCodeView",
        "LLVMDebugInfoGSYM",
        "LLVMDebugInfoDWARF",
        "LLVMObject",
        "LLVMTextAPI",
        "LLVMMCParser",
        "LLVMMC",
        "LLVMDebugInfoDWARFLowLevel",
        "LLVMFrontendHLSL",
        "LLVMIRReader",
        "LLVMBitReader",
        "LLVMAsmParser",
        "LLVMCore",
        "LLVMRemarks",
        "LLVMBitstreamReader",
        "LLVMBinaryFormat",
        "LLVMTargetParser",
        "LLVMSupport",
        "LLVMDemangle"
    }

    add_links(table.unpack(llvm_links))

    if is_plat("linux") then
        add_syslinks("z", "zstd", "tinfo", "xml2", "pthread", "dl", "m")
    end
end
