function configure_vexa_consumer()
    add_deps("vexa", {inherit = false})
    add_includedirs(path.join(os.projectdir(), "include"))
    add_linkdirs("$(builddir)/$(os)/$(arch)/$(mode)")

    add_packages("llvm", "z3", "lief", "quill")

	-- !!!!!!!!!!!!!!!!!!!!!!!!!!
    local remilldir = get_config("remill") or path.join(os.projectdir(), "..", "remill")
    add_includedirs(path.join(remilldir, "dependencies/install/include"))
    add_linkdirs(path.join(remilldir, "dependencies/install/lib"))

    add_links("vexa", "z3")
    add_links(
        "remill_arch",
        "remill_arch_x86",
        "remill_arch_aarch64",
        "remill_arch_sparc32",
        "remill_arch_sparc64",
        "remill_arch_sleigh",
        "remill_bc",
        "remill_os",
        "remill_version",
        "decomp", "sla", "slaSupport",
        "xed", "xed-ild", "glog", "gflags"
    )

    add_links("LLVMWindowsManifest", "LLVMXRay", "LLVMLibDriver", "LLVMDlltoolDriver", "LLVMTelemetry", "LLVMTextAPIBinaryReader", "LLVMCoverage", "LLVMLineEditor", "LLVMXCoreDisassembler", "LLVMXCoreCodeGen", "LLVMXCoreDesc", "LLVMXCoreInfo", "LLVMX86TargetMCA", "LLVMX86Disassembler", "LLVMX86AsmParser", "LLVMX86CodeGen", "LLVMX86Desc", "LLVMX86Info", "LLVMWebAssemblyDisassembler", "LLVMWebAssemblyAsmParser", "LLVMWebAssemblyCodeGen", "LLVMWebAssemblyUtils", "LLVMWebAssemblyDesc", "LLVMWebAssemblyInfo", "LLVMVEDisassembler", "LLVMVEAsmParser", "LLVMVECodeGen", "LLVMVEDesc", "LLVMVEInfo", "LLVMSystemZDisassembler", "LLVMSystemZCodeGen", "LLVMSystemZDesc", "LLVMSystemZInfo", "LLVMSPIRVCodeGen", "LLVMSPIRVDesc", "LLVMSPIRVInfo", "LLVMSPIRVAnalysis", "LLVMSparcDisassembler", "LLVMSparcAsmParser", "LLVMSparcCodeGen", "LLVMSparcDesc", "LLVMSparcInfo", "LLVMRISCVTargetMCA", "LLVMRISCVDisassembler", "LLVMRISCVAsmParser", "LLVMRISCVCodeGen", "LLVMRISCVDesc", "LLVMRISCVInfo", "LLVMPowerPCDisassembler", "LLVMPowerPCAsmParser", "LLVMPowerPCCodeGen", "LLVMPowerPCDesc", "LLVMPowerPCInfo", "LLVMNVPTXCodeGen", "LLVMNVPTXDesc", "LLVMNVPTXInfo", "LLVMMSP430Disassembler", "LLVMMSP430CodeGen", "LLVMMSP430Desc", "LLVMMSP430Info", "LLVMMipsDisassembler", "LLVMMipsAsmParser", "LLVMMipsCodeGen", "LLVMMipsDesc", "LLVMMipsInfo", "LLVMLoongArchDisassembler", "LLVMLoongArchAsmParser", "LLVMLoongArchCodeGen", "LLVMLoongArchDesc", "LLVMLoongArchInfo", "LLVMLanaiDisassembler", "LLVMLanaiCodeGen", "LLVMLanaiAsmParser", "LLVMLanaiDesc", "LLVMLanaiInfo", "LLVMHexagonDisassembler", "LLVMHexagonCodeGen", "LLVMHexagonAsmParser", "LLVMHexagonDesc", "LLVMHexagonInfo", "LLVMBPFDisassembler", "LLVMBPFAsmParser", "LLVMBPFCodeGen", "LLVMBPFDesc", "LLVMBPFInfo", "LLVMAVRDisassembler", "LLVMAVRAsmParser", "LLVMAVRCodeGen", "LLVMAVRDesc", "LLVMAVRInfo", "LLVMARMDisassembler", "LLVMARMAsmParser", "LLVMARMCodeGen", "LLVMARMDesc", "LLVMARMUtils", "LLVMARMInfo", "LLVMAMDGPUTargetMCA", "LLVMAMDGPUDisassembler", "LLVMAMDGPUAsmParser", "LLVMAMDGPUCodeGen", "LLVMAMDGPUDesc", "LLVMAMDGPUUtils", "LLVMAMDGPUInfo", "LLVMAArch64Disassembler", "LLVMAArch64AsmParser", "LLVMAArch64CodeGen", "LLVMAArch64Desc", "LLVMAArch64Utils", "LLVMAArch64Info", "LLVMOrcDebugging", "LLVMOrcJIT", "LLVMWindowsDriver", "LLVMMCJIT", "LLVMJITLink", "LLVMInterpreter", "LLVMExecutionEngine", "LLVMRuntimeDyld", "LLVMOrcTargetProcess", "LLVMOrcShared", "LLVMDWP", "LLVMDWARFCFIChecker", "LLVMDebugInfoLogicalView", "LLVMOption", "LLVMObjCopy", "LLVMMCA", "LLVMMCDisassembler", "LLVMDTLTO", "LLVMLTO", "LLVMPlugins", "LLVMPasses", "LLVMHipStdPar", "LLVMCFGuard", "LLVMCoroutines", "LLVMipo", "LLVMVectorize", "LLVMSandboxIR", "LLVMLinker", "LLVMFrontendOpenMP", "LLVMFrontendOffloading", "LLVMObjectYAML", "LLVMFrontendOpenACC", "LLVMFrontendDriver", "LLVMInstrumentation", "LLVMFrontendDirective", "LLVMFrontendAtomic", "LLVMExtensions", "LLVMDWARFLinkerParallel", "LLVMDWARFLinkerClassic", "LLVMDWARFLinker", "LLVMGlobalISel", "LLVMMIRParser", "LLVMAsmPrinter", "LLVMSelectionDAG", "LLVMCodeGen", "LLVMTarget", "LLVMObjCARCOpts", "LLVMCodeGenTypes", "LLVMCGData", "LLVMCAS", "LLVMIRPrinter", "LLVMInterfaceStub", "LLVMFileCheck", "LLVMFuzzMutate", "LLVMScalarOpts", "LLVMInstCombine", "LLVMAggressiveInstCombine", "LLVMTransformUtils", "LLVMBitWriter", "LLVMAnalysis", "LLVMProfileData", "LLVMSymbolize", "LLVMDebugInfoBTF", "LLVMDebugInfoPDB", "LLVMDebugInfoMSF", "LLVMDebugInfoCodeView", "LLVMDebugInfoGSYM", "LLVMDebugInfoDWARF", "LLVMObject", "LLVMTextAPI", "LLVMMCParser", "LLVMIRReader", "LLVMAsmParser", "LLVMMC", "LLVMDebugInfoDWARFLowLevel", "LLVMBitReader", "LLVMFrontendHLSL", "LLVMFuzzerCLI", "LLVMABI", "LLVMCore", "LLVMRemarks", "LLVMBitstreamReader", "LLVMBinaryFormat", "LLVMTargetParser", "LLVMTableGen", "LLVMSupportLSP", "LLVMSupport", "LLVMDemangle")
    add_links("LLVMSystemZAsmParser", "LLVMMSP430AsmParser")

    if is_plat("linux") then
        add_syslinks("z", "zstd", "tinfo", "xml2", "pthread", "dl", "m")
    end
end
