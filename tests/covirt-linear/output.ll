define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RAX3511 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %RDI3516 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %0 = load i64, ptr %RDI3516, align 8
  %1 = getelementptr inbounds nuw i8, ptr %memory, i64 61464
  store i64 8797, ptr %1, align 8
  %2 = getelementptr inbounds nuw i8, ptr %memory, i64 59392
  store i64 -528, ptr %2, align 8
  %3 = getelementptr inbounds nuw i8, ptr %memory, i64 61472
  store i64 53248, ptr %3, align 8
  %4 = getelementptr inbounds nuw i8, ptr %memory, i64 61480
  store i64 53737, ptr %4, align 8
  %5 = getelementptr inbounds nuw i8, ptr %memory, i64 61488
  store i64 53791, ptr %5, align 8
  %6 = getelementptr inbounds nuw i8, ptr %memory, i64 61496
  store i64 53996, ptr %6, align 8
  %7 = getelementptr inbounds nuw i8, ptr %memory, i64 61504
  store i64 54201, ptr %7, align 8
  %8 = getelementptr inbounds nuw i8, ptr %memory, i64 61512
  store i64 54416, ptr %8, align 8
  %9 = getelementptr inbounds nuw i8, ptr %memory, i64 61520
  store i64 54617, ptr %9, align 8
  %10 = getelementptr inbounds nuw i8, ptr %memory, i64 61528
  store i64 54837, ptr %10, align 8
  %11 = getelementptr inbounds nuw i8, ptr %memory, i64 61536
  store i64 55046, ptr %11, align 8
  %12 = getelementptr inbounds nuw i8, ptr %memory, i64 61544
  store i64 55255, ptr %12, align 8
  %13 = getelementptr inbounds nuw i8, ptr %memory, i64 61552
  store i64 55464, ptr %13, align 8
  %14 = getelementptr inbounds nuw i8, ptr %memory, i64 61560
  store i64 55673, ptr %14, align 8
  %15 = getelementptr inbounds nuw i8, ptr %memory, i64 61568
  store i64 55882, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %memory, i64 61576
  store i64 56133, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %memory, i64 61584
  store i64 56169, ptr %17, align 8
  %18 = getelementptr inbounds nuw i8, ptr %memory, i64 61592
  store i64 56228, ptr %18, align 8
  %19 = getelementptr inbounds nuw i8, ptr %memory, i64 61600
  store i64 56287, ptr %19, align 8
  %20 = getelementptr inbounds nuw i8, ptr %memory, i64 61608
  store i64 56346, ptr %20, align 8
  %21 = getelementptr inbounds nuw i8, ptr %memory, i64 61616
  store i64 56405, ptr %21, align 8
  %22 = getelementptr inbounds nuw i8, ptr %memory, i64 61624
  store i64 56477, ptr %22, align 8
  %23 = getelementptr inbounds nuw i8, ptr %memory, i64 61632
  store i64 56551, ptr %23, align 8
  %24 = getelementptr inbounds nuw i8, ptr %memory, i64 61640
  store i64 56619, ptr %24, align 8
  %25 = getelementptr inbounds nuw i8, ptr %memory, i64 61648
  store i64 56700, ptr %25, align 8
  %26 = getelementptr inbounds nuw i8, ptr %memory, i64 61656
  store i64 56768, ptr %26, align 8
  %27 = getelementptr inbounds nuw i8, ptr %memory, i64 61664
  store i64 56851, ptr %27, align 8
  %28 = getelementptr inbounds nuw i8, ptr %memory, i64 61672
  store i64 57035, ptr %28, align 8
  %29 = getelementptr inbounds nuw i8, ptr %memory, i64 61680
  store i64 57077, ptr %29, align 8
  %30 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %30, align 8
  %31 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %31, align 8
  %32 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %32, align 8
  %33 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %33, align 8
  %34 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  %35 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %35, align 8
  %36 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %36, align 8
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %38, align 8
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %40, align 8
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %41, align 8
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %42, align 8
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 61440
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %46, align 8
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %47, align 8
  %48 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  store i64 54543, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  store i64 54556, ptr %50, align 8
  %51 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  store i64 54571, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  store i64 54584, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %memory, i64 55350
  store i64 55382, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %memory, i64 55358
  store i64 55396, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %memory, i64 55366
  store i64 55414, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %memory, i64 55374
  store i64 55428, ptr %56, align 8
  %57 = xor i64 %0, -6489620444588847481
  store i64 %57, ptr %34, align 8
  %58 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %58, align 8
  %59 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %59, align 8
  %60 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %60, align 8
  %61 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %61, align 8
  %62 = getelementptr inbounds nuw i8, ptr %memory, i64 57181
  %63 = getelementptr inbounds nuw i8, ptr %memory, i64 59400
  %64 = shl i64 %57, 17
  %65 = lshr i64 %57, 47
  %RCX3497.sroa.0.sroa.0.0.insert.ext5364 = and i64 %65, 255
  %66 = getelementptr inbounds nuw i8, ptr %memory, i64 55768
  store i64 55800, ptr %66, align 8
  %67 = getelementptr inbounds nuw i8, ptr %memory, i64 55776
  store i64 55814, ptr %67, align 8
  %68 = getelementptr inbounds nuw i8, ptr %memory, i64 55784
  store i64 55832, ptr %68, align 8
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 55792
  store i64 55846, ptr %69, align 8
  %RDX3498.sroa.0.0.insert.insert4267 = or disjoint i64 %65, %64
  %RCX3497.sroa.0.sroa.786.0.extract.shift5616 = and i64 %RDX3498.sroa.0.0.insert.insert4267, 4294967040
  %RCX3497.sroa.786.0.extract.shift5133 = and i64 %64, -4294967296
  %RCX3497.sroa.0.sroa.0.0.insert.insert5358 = or disjoint i64 %RCX3497.sroa.786.0.extract.shift5133, %RCX3497.sroa.0.sroa.0.0.insert.ext5364
  %RCX3497.sroa.0.0.insert.insert4833 = or disjoint i64 %RCX3497.sroa.0.sroa.0.0.insert.insert5358, %RCX3497.sroa.0.sroa.786.0.extract.shift5616
  store i64 %RCX3497.sroa.0.0.insert.insert4833, ptr %43, align 8
  %70 = getelementptr inbounds nuw i8, ptr %memory, i64 61432
  %71 = mul i64 %RCX3497.sroa.0.0.insert.insert4833, -7046029288634856825
  store i64 %71, ptr %44, align 8
  %72 = getelementptr inbounds nuw i8, ptr %memory, i64 61424
  %73 = lshr i64 %71, 29
  %74 = getelementptr inbounds nuw i8, ptr %memory, i64 61416
  %75 = xor i64 %73, %71
  store i64 %75, ptr %70, align 8
  %76 = getelementptr inbounds nuw i8, ptr %memory, i64 61408
  %77 = add i64 %75, -3335678366873096957
  store i64 %77, ptr %72, align 8
  %RDX3498.sroa.0.0.insert.insert4172 = tail call i64 @llvm.fshl.i64(i64 %77, i64 %77, i64 41)
  store i64 %RDX3498.sroa.0.0.insert.insert4172, ptr %74, align 8
  %78 = getelementptr inbounds nuw i8, ptr %memory, i64 61400
  %79 = getelementptr inbounds nuw i8, ptr %memory, i64 61392
  %80 = xor i64 %RDX3498.sroa.0.0.insert.insert4172, 7222893703706247677
  store i64 %80, ptr %76, align 8
  %81 = getelementptr inbounds nuw i8, ptr %memory, i64 61384
  %82 = getelementptr inbounds nuw i8, ptr %memory, i64 55141
  store i64 55173, ptr %82, align 8
  %83 = getelementptr inbounds nuw i8, ptr %memory, i64 55149
  store i64 55187, ptr %83, align 8
  %84 = getelementptr inbounds nuw i8, ptr %memory, i64 55157
  store i64 55205, ptr %84, align 8
  %85 = getelementptr inbounds nuw i8, ptr %memory, i64 55165
  store i64 55219, ptr %85, align 8
  %86 = and i64 %80, 4294967295
  %87 = sub nsw i64 0, %86
  %88 = or i64 %80, %87
  store i64 %88, ptr %78, align 8
  store i64 1976, ptr %63, align 8
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %62, i8 -112, i64 16, i1 false)
  %isnotneg = icmp sgt i64 %88, -1
  %RDX3498.sroa.0.0.extract.trunc4080 = zext i1 %isnotneg to i64
  store i64 %RDX3498.sroa.0.0.extract.trunc4080, ptr %79, align 8
  %89 = getelementptr inbounds nuw i8, ptr %memory, i64 61376
  store i64 -16, ptr %89, align 8
  store i64 -32, ptr %81, align 8
  store i64 %RDX3498.sroa.0.0.extract.trunc4080, ptr %RAX3511, align 8
  ret ptr %state
}
