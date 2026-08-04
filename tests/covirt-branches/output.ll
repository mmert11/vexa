define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RAX1249 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %0 = load i64, ptr %RAX1249, align 8
  %RDI1254 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %1 = load i64, ptr %RDI1254, align 8
  %2 = getelementptr inbounds nuw i8, ptr %memory, i64 61464
  store i64 8553, ptr %2, align 8
  %3 = getelementptr inbounds nuw i8, ptr %memory, i64 59392
  store i64 -528, ptr %3, align 8
  %4 = getelementptr inbounds nuw i8, ptr %memory, i64 61472
  store i64 53248, ptr %4, align 8
  %5 = getelementptr inbounds nuw i8, ptr %memory, i64 61480
  store i64 53737, ptr %5, align 8
  %6 = getelementptr inbounds nuw i8, ptr %memory, i64 61488
  store i64 53791, ptr %6, align 8
  %7 = getelementptr inbounds nuw i8, ptr %memory, i64 61496
  store i64 53996, ptr %7, align 8
  %8 = getelementptr inbounds nuw i8, ptr %memory, i64 61504
  store i64 54201, ptr %8, align 8
  %9 = getelementptr inbounds nuw i8, ptr %memory, i64 61512
  store i64 54416, ptr %9, align 8
  %10 = getelementptr inbounds nuw i8, ptr %memory, i64 61520
  store i64 54617, ptr %10, align 8
  %11 = getelementptr inbounds nuw i8, ptr %memory, i64 61528
  store i64 54837, ptr %11, align 8
  %12 = getelementptr inbounds nuw i8, ptr %memory, i64 61536
  store i64 55046, ptr %12, align 8
  %13 = getelementptr inbounds nuw i8, ptr %memory, i64 61544
  store i64 55255, ptr %13, align 8
  %14 = getelementptr inbounds nuw i8, ptr %memory, i64 61552
  store i64 55464, ptr %14, align 8
  %15 = getelementptr inbounds nuw i8, ptr %memory, i64 61560
  store i64 55673, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %memory, i64 61568
  store i64 55882, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %memory, i64 61576
  store i64 56133, ptr %17, align 8
  %18 = getelementptr inbounds nuw i8, ptr %memory, i64 61584
  store i64 56169, ptr %18, align 8
  %19 = getelementptr inbounds nuw i8, ptr %memory, i64 61592
  store i64 56228, ptr %19, align 8
  %20 = getelementptr inbounds nuw i8, ptr %memory, i64 61600
  store i64 56287, ptr %20, align 8
  %21 = getelementptr inbounds nuw i8, ptr %memory, i64 61608
  store i64 56346, ptr %21, align 8
  %22 = getelementptr inbounds nuw i8, ptr %memory, i64 61616
  store i64 56405, ptr %22, align 8
  %23 = getelementptr inbounds nuw i8, ptr %memory, i64 61624
  store i64 56477, ptr %23, align 8
  %24 = getelementptr inbounds nuw i8, ptr %memory, i64 61632
  store i64 56551, ptr %24, align 8
  %25 = getelementptr inbounds nuw i8, ptr %memory, i64 61640
  store i64 56619, ptr %25, align 8
  %26 = getelementptr inbounds nuw i8, ptr %memory, i64 61648
  store i64 56700, ptr %26, align 8
  %27 = getelementptr inbounds nuw i8, ptr %memory, i64 61656
  store i64 56768, ptr %27, align 8
  %28 = getelementptr inbounds nuw i8, ptr %memory, i64 61664
  store i64 56851, ptr %28, align 8
  %29 = getelementptr inbounds nuw i8, ptr %memory, i64 61672
  store i64 57035, ptr %29, align 8
  %30 = getelementptr inbounds nuw i8, ptr %memory, i64 61680
  store i64 57077, ptr %30, align 8
  %31 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %31, align 8
  %32 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %32, align 8
  %33 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %33, align 8
  %34 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %34, align 8
  %35 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %35, align 8
  %36 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %36, align 8
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %38, align 8
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %40, align 8
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %41, align 8
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %42, align 8
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  store i64 54543, ptr %43, align 8
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  store i64 54556, ptr %44, align 8
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  store i64 54571, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  store i64 54584, ptr %46, align 8
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  store i64 56009, ptr %47, align 8
  %48 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  store i64 56034, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  store i64 56062, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  store i64 56087, ptr %50, align 8
  switch i64 %1, label %MOVZX_GPR64_MEMw_388 [
    i64 1293, label %ADD_GPRv_IMMb_64_1058
    i64 911, label %ADD_GPRv_IMMb_64_875
  ]

ADD_GPRv_IMMb_64_875:                             ; preds = %entry
  %51 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %54, align 8
  br label %common.ret

ADD_GPRv_IMMb_64_1058:                            ; preds = %entry
  %55 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %58, align 8
  br label %common.ret

MOVZX_GPR64_MEMw_388:                             ; preds = %entry
  %59 = icmp eq i64 %1, 1453
  %60 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %60, align 8
  %61 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %61, align 8
  %62 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %62, align 8
  %63 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %63, align 8
  %spec.select = zext i1 %59 to i64
  br label %common.ret

common.ret:                                       ; preds = %MOVZX_GPR64_MEMw_388, %ADD_GPRv_IMMb_64_1058, %ADD_GPRv_IMMb_64_875
  %RAX1233.sroa.0.0 = phi i64 [ 1, %ADD_GPRv_IMMb_64_1058 ], [ 1, %ADD_GPRv_IMMb_64_875 ], [ %spec.select, %MOVZX_GPR64_MEMw_388 ]
  %64 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %65 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  store i64 -9, ptr %64, align 8
  store i64 -25, ptr %65, align 8
  %66 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %66, align 8
  %67 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %67, align 8
  %68 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %68, align 8
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %69, align 8
  store i16 8624, ptr %2, align 2
  %RAX1233.sroa.128.sroa.0.0.in = and i64 %0, -256
  %RAX1233.sroa.0.0.insert.insert1670 = or disjoint i64 %RAX1233.sroa.0.0, %RAX1233.sroa.128.sroa.0.0.in
  store i64 %RAX1233.sroa.0.0.insert.insert1670, ptr %RAX1249, align 8
  ret ptr %state
}
