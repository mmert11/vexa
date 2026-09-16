define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RDI1244 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %2 = load i64, ptr %RDI1244, align 8
  %DF1298 = getelementptr inbounds nuw i8, ptr %state, i64 2075
  %3 = load i8, ptr %DF1298, align 1
  %4 = getelementptr inbounds nuw i8, ptr %memory, i64 61464
  store i64 8553, ptr %4, align 8
  %5 = getelementptr inbounds nuw i8, ptr %memory, i64 59392
  store i64 -528, ptr %5, align 8
  %6 = getelementptr inbounds nuw i8, ptr %memory, i64 61472
  store i64 53248, ptr %6, align 8
  %7 = getelementptr inbounds nuw i8, ptr %memory, i64 61480
  store i64 53737, ptr %7, align 8
  %8 = getelementptr inbounds nuw i8, ptr %memory, i64 61488
  store i64 53791, ptr %8, align 8
  %9 = getelementptr inbounds nuw i8, ptr %memory, i64 61496
  store i64 53996, ptr %9, align 8
  %10 = getelementptr inbounds nuw i8, ptr %memory, i64 61504
  store i64 54201, ptr %10, align 8
  %11 = getelementptr inbounds nuw i8, ptr %memory, i64 61512
  store i64 54416, ptr %11, align 8
  %12 = getelementptr inbounds nuw i8, ptr %memory, i64 61520
  store i64 54617, ptr %12, align 8
  %13 = getelementptr inbounds nuw i8, ptr %memory, i64 61528
  store i64 54837, ptr %13, align 8
  %14 = getelementptr inbounds nuw i8, ptr %memory, i64 61536
  store i64 55046, ptr %14, align 8
  %15 = getelementptr inbounds nuw i8, ptr %memory, i64 61544
  store i64 55255, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %memory, i64 61552
  store i64 55464, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %memory, i64 61560
  store i64 55673, ptr %17, align 8
  %18 = getelementptr inbounds nuw i8, ptr %memory, i64 61568
  store i64 55882, ptr %18, align 8
  %19 = getelementptr inbounds nuw i8, ptr %memory, i64 61576
  store i64 56133, ptr %19, align 8
  %20 = getelementptr inbounds nuw i8, ptr %memory, i64 61584
  store i64 56169, ptr %20, align 8
  %21 = getelementptr inbounds nuw i8, ptr %memory, i64 61592
  store i64 56228, ptr %21, align 8
  %22 = getelementptr inbounds nuw i8, ptr %memory, i64 61600
  store i64 56287, ptr %22, align 8
  %23 = getelementptr inbounds nuw i8, ptr %memory, i64 61608
  store i64 56346, ptr %23, align 8
  %24 = getelementptr inbounds nuw i8, ptr %memory, i64 61616
  store i64 56405, ptr %24, align 8
  %25 = getelementptr inbounds nuw i8, ptr %memory, i64 61624
  store i64 56477, ptr %25, align 8
  %26 = getelementptr inbounds nuw i8, ptr %memory, i64 61632
  store i64 56551, ptr %26, align 8
  %27 = getelementptr inbounds nuw i8, ptr %memory, i64 61640
  store i64 56619, ptr %27, align 8
  %28 = getelementptr inbounds nuw i8, ptr %memory, i64 61648
  store i64 56700, ptr %28, align 8
  %29 = getelementptr inbounds nuw i8, ptr %memory, i64 61656
  store i64 56768, ptr %29, align 8
  %30 = getelementptr inbounds nuw i8, ptr %memory, i64 61664
  store i64 56851, ptr %30, align 8
  %31 = getelementptr inbounds nuw i8, ptr %memory, i64 61672
  store i64 57035, ptr %31, align 8
  %32 = getelementptr inbounds nuw i8, ptr %memory, i64 61680
  store i64 57077, ptr %32, align 8
  %33 = and i8 %3, 1
  %34 = zext nneg i8 %33 to i32
  %35 = shl nuw nsw i32 %34, 10
  %36 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %36, align 8
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %38, align 8
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %41, align 8
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %42, align 8
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %43, align 8
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %44, align 8
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %46, align 8
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %47, align 8
  %48 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %50 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %50, align 8
  %51 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %memory, i64 57401
  %59 = load i64, ptr %58, align 8
  %60 = getelementptr inbounds nuw i8, ptr %memory, i64 61440
  %61 = getelementptr inbounds nuw i8, ptr %memory, i64 61462
  %62 = getelementptr inbounds nuw i8, ptr %memory, i64 61452
  %63 = getelementptr inbounds nuw i8, ptr %memory, i64 57473
  %64 = getelementptr inbounds nuw i8, ptr %memory, i64 61444
  %65 = getelementptr inbounds nuw i8, ptr %memory, i64 57485
  %66 = getelementptr inbounds nuw i8, ptr %memory, i64 61436
  %67 = and i32 %1, -3286
  br label %ADD_GPRv_IMMb_64_378

ADD_GPRv_IMMb_64_378:                             ; preds = %ADD_GPRv_IMMb_64_638, %entry
  %68 = phi i64 [ 0, %entry ], [ %126, %ADD_GPRv_IMMb_64_638 ]
  %69 = phi i32 [ 0, %entry ], [ %129, %ADD_GPRv_IMMb_64_638 ]
  %70 = phi i64 [ 61464, %entry ], [ %127, %ADD_GPRv_IMMb_64_638 ]
  %71 = sext i32 %69 to i64
  store i64 %71, ptr %40, align 8
  store i64 %59, ptr %60, align 8
  store i64 %2, ptr %49, align 8
  %72 = sub i64 %71, %2
  %73 = icmp ugt i64 %2, %71
  %74 = trunc i64 %72 to i8
  %75 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %74)
  %76 = xor i64 %72, %71
  %77 = xor i64 %76, %2
  %78 = trunc i64 %77 to i32
  %79 = and i32 %78, 16
  %80 = icmp eq i64 %2, %71
  %81 = xor i64 %2, %71
  %82 = lshr i64 %81, 63
  %83 = lshr i64 %76, 63
  %84 = add nuw nsw i64 %83, %82
  %85 = icmp eq i64 %84, 2
  %86 = shl nuw nsw i8 %75, 2
  %87 = and i8 %86, 4
  %88 = zext i1 %73 to i8
  %89 = or disjoint i8 %87, %88
  %90 = xor i8 %89, 4
  %91 = zext nneg i8 %90 to i32
  %92 = lshr i64 %72, 56
  %93 = trunc nuw nsw i64 %92 to i32
  %94 = and i32 %93, 128
  %95 = select i1 %80, i32 64, i32 0
  %96 = select i1 %85, i32 2048, i32 0
  %97 = or disjoint i32 %95, %94
  %98 = or disjoint i32 %97, %67
  %99 = or disjoint i32 %79, %91
  %100 = or disjoint i32 %99, %98
  %101 = or disjoint i32 %35, %96
  %102 = or disjoint i32 %101, %67
  %103 = trunc i32 %100 to i16
  %104 = trunc i32 %102 to i16
  %105 = and i16 %104, -256
  %106 = and i16 %103, 255
  %107 = or disjoint i16 %106, %105
  store i16 %107, ptr %61, align 2
  br i1 %73, label %ADD_GPRv_IMMb_64_638, label %MOVZX_GPR64_MEMw_608

MOVZX_GPR64_MEMw_608:                             ; preds = %ADD_GPRv_IMMb_64_378
  %108 = getelementptr inbounds nuw i8, ptr %memory, i64 57181
  store i32 -1869574000, ptr %108, align 4
  %RAX1239 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %109 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  %110 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  %111 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  %112 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  %113 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  %114 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  %115 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  %116 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  %117 = getelementptr inbounds nuw i8, ptr %memory, i64 57193
  %118 = getelementptr inbounds nuw i8, ptr %memory, i64 57189
  %119 = getelementptr inbounds nuw i8, ptr %memory, i64 57185
  %120 = getelementptr inbounds nuw i8, ptr %memory, i64 59400
  %121 = add i64 %70, -59424
  store i64 %121, ptr %120, align 8
  store i32 -1869574000, ptr %119, align 4
  store i32 -1869574000, ptr %118, align 4
  store i32 -1869574000, ptr %117, align 4
  store i64 54543, ptr %116, align 8
  store i64 54556, ptr %115, align 8
  store i64 54571, ptr %114, align 8
  store i64 54584, ptr %113, align 8
  store i64 56009, ptr %112, align 8
  store i64 56034, ptr %111, align 8
  store i64 56062, ptr %110, align 8
  store i64 56087, ptr %109, align 8
  %122 = getelementptr inbounds nuw i8, ptr %memory, i64 57500
  %123 = load i16, ptr %122, align 2
  %124 = load i16, ptr %4, align 2
  %125 = add i16 %124, %123
  store i16 %125, ptr %4, align 2
  store i64 %68, ptr %RAX1239, align 8
  ret ptr %state

ADD_GPRv_IMMb_64_638:                             ; preds = %ADD_GPRv_IMMb_64_378
  %126 = add i64 %68, %71
  store i64 %126, ptr %40, align 8
  %127 = add i64 %70, -12
  %128 = load i32, ptr %63, align 4
  %129 = add i32 %128, %69
  store i32 %129, ptr %62, align 4
  %130 = load i64, ptr %65, align 8
  store i64 %130, ptr %66, align 8
  %131 = add i64 %130, -16
  store i64 %131, ptr %64, align 8
  br label %ADD_GPRv_IMMb_64_378
}
