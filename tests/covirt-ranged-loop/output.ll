define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RDI1125 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %2 = load i64, ptr %RDI1125, align 8
  %DF1179 = getelementptr inbounds nuw i8, ptr %state, i64 2075
  %3 = load i8, ptr %DF1179, align 1
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
  %36 = or i32 %35, %1
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %38, align 8
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %40, align 8
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %42, align 8
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %43, align 8
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %44, align 8
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %46, align 8
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %47, align 8
  %48 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %51 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %58, align 8
  %59 = getelementptr inbounds nuw i8, ptr %memory, i64 61460
  %60 = getelementptr inbounds nuw i8, ptr %memory, i64 57393
  %61 = load i64, ptr %60, align 8
  %62 = getelementptr inbounds nuw i8, ptr %memory, i64 57404
  %63 = load i32, ptr %62, align 4
  %64 = getelementptr inbounds nuw i8, ptr %memory, i64 61462
  %65 = getelementptr inbounds nuw i8, ptr %memory, i64 61452
  %66 = getelementptr inbounds nuw i8, ptr %memory, i64 57479
  %67 = getelementptr inbounds nuw i8, ptr %memory, i64 61444
  %68 = getelementptr inbounds nuw i8, ptr %memory, i64 57491
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 61436
  br label %MOVZX_GPRv_MEMb_64_378

MOVZX_GPRv_MEMb_64_378:                           ; preds = %ADD_GPRv_IMMb_64_536, %entry
  %70 = phi i64 [ 0, %entry ], [ %120, %ADD_GPRv_IMMb_64_536 ]
  %71 = phi i32 [ 0, %entry ], [ %122, %ADD_GPRv_IMMb_64_536 ]
  %72 = phi i32 [ %36, %entry ], [ %102, %ADD_GPRv_IMMb_64_536 ]
  store i64 %61, ptr %50, align 8
  store i32 %71, ptr %59, align 4
  store i32 %63, ptr %41, align 4
  %73 = sub i32 %71, %63
  %74 = icmp ult i32 %71, %63
  %75 = trunc i32 %73 to i8
  %76 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %75)
  %77 = xor i32 %73, %71
  %78 = xor i32 %77, %63
  %79 = and i32 %78, 16
  %80 = icmp eq i32 %71, %63
  %81 = xor i32 %71, %63
  %82 = lshr i32 %81, 31
  %83 = lshr i32 %77, 31
  %84 = add nuw nsw i32 %83, %82
  %85 = icmp eq i32 %84, 2
  %86 = zext i1 %74 to i32
  %87 = and i32 %72, -3286
  %88 = shl nuw nsw i8 %76, 2
  %89 = and i8 %88, 4
  %90 = xor i8 %89, 4
  %91 = zext nneg i8 %90 to i32
  %92 = lshr i32 %73, 24
  %93 = and i32 %92, 128
  %94 = select i1 %80, i32 64, i32 0
  %95 = select i1 %85, i32 2048, i32 0
  %96 = or disjoint i32 %94, %87
  %97 = or disjoint i32 %96, %86
  %98 = or disjoint i32 %97, %93
  %99 = or disjoint i32 %98, %79
  %100 = or disjoint i32 %99, %91
  %101 = or disjoint i32 %95, %35
  %102 = or i32 %101, %100
  %103 = trunc i32 %100 to i16
  %104 = trunc i32 %102 to i16
  %105 = and i16 %104, -256
  %106 = and i16 %103, 255
  %107 = or disjoint i16 %105, %106
  store i16 %107, ptr %64, align 2
  br i1 %74, label %ADD_GPRv_IMMb_64_536, label %MOVZX_GPR64_MEMw_508

MOVZX_GPR64_MEMw_508:                             ; preds = %MOVZX_GPRv_MEMb_64_378
  %RAX1120 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %108 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  %109 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  %110 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  %111 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  %112 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  %113 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  %114 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  %115 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  store i64 54543, ptr %115, align 8
  store i64 54556, ptr %114, align 8
  store i64 54571, ptr %113, align 8
  store i64 54584, ptr %112, align 8
  store i64 56009, ptr %111, align 8
  store i64 56034, ptr %110, align 8
  store i64 56062, ptr %109, align 8
  store i64 56087, ptr %108, align 8
  %116 = getelementptr inbounds nuw i8, ptr %memory, i64 57506
  %117 = load i16, ptr %116, align 2
  %118 = load i16, ptr %4, align 2
  %119 = add i16 %118, %117
  store i16 %119, ptr %4, align 2
  store i64 %70, ptr %RAX1120, align 8
  ret ptr %state

ADD_GPRv_IMMb_64_536:                             ; preds = %MOVZX_GPRv_MEMb_64_378
  %120 = add i64 %70, %2
  store i64 %120, ptr %41, align 8
  %121 = load i32, ptr %66, align 4
  %122 = add i32 %121, %71
  store i32 %122, ptr %65, align 4
  %123 = load i64, ptr %68, align 8
  store i64 %123, ptr %69, align 8
  %124 = add i64 %123, -16
  store i64 %124, ptr %67, align 8
  br label %MOVZX_GPRv_MEMb_64_378
}
