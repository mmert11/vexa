define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
<<<<<<< Updated upstream
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RCX95892 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %2 = load i64, ptr %RCX95892, align 8
  %stack176787.sroa.13291.sroa.3 = alloca [10 x i8], align 1
  %3 = and i32 %1, 40
  %4 = zext nneg i32 %3 to i64
  %5 = xor i64 %2, -1
  %6 = add i64 %2, -1293
  %7 = sub i64 1292, %2
  %8 = and i64 %7, %2
  %9 = lshr i64 %8, 52
  %10 = and i64 %9, 2048
  %11 = and i64 %6, %2
  %12 = and i64 %7, %5
  %13 = or i64 %11, %12
  %14 = and i64 %13, 16
  %15 = or disjoint i64 %14, %10
  %16 = trunc i64 %6 to i8
  %17 = lshr i8 %16, 4
  %stack176787.sroa.13291.sroa.3.6..sroa_idx206163 = getelementptr inbounds nuw i8, ptr %stack176787.sroa.13291.sroa.3, i64 6
  store i16 0, ptr %stack176787.sroa.13291.sroa.3.6..sroa_idx206163, align 1
  %stack176787.sroa.13291.sroa.3.2..sroa_idx206162 = getelementptr inbounds nuw i8, ptr %stack176787.sroa.13291.sroa.3, i64 2
  store i32 0, ptr %stack176787.sroa.13291.sroa.3.2..sroa_idx206162, align 1
  %18 = xor i8 %17, %16
  %19 = xor i8 %18, -1
  %20 = or i8 %19, -16
  %21 = xor i8 %20, -1
  %22 = zext nneg i8 %21 to i16
  %23 = icmp eq i8 %20, -1
  %24 = tail call i16 @llvm.usub.sat.i16(i16 %22, i16 1)
  %25 = lshr i16 19252, %24
  %26 = and i16 %25, 1
  %27 = select i1 %23, i16 1, i16 %26
  store i16 %27, ptr %stack176787.sroa.13291.sroa.3, align 1
  %28 = load i64, ptr %stack176787.sroa.13291.sroa.3, align 1
  %29 = shl i64 %28, 2
  %30 = sub i64 -9223372036854774515, %2
  %31 = and i64 %30, %7
  %32 = lshr i64 %31, 57
  %33 = and i64 %32, 64
  %34 = lshr i64 %6, 56
  %35 = and i64 %34, 128
  %36 = or i64 %7, %2
  %37 = icmp sgt i64 %36, -1
  %38 = zext i1 %37 to i64
  %39 = or disjoint i64 %33, %35
  %40 = or disjoint i64 %39, %38
  %41 = or disjoint i64 %15, %40
  %42 = or disjoint i64 %41, %4
  %43 = xor i64 %42, 16
  %44 = add i64 %43, %29
  %45 = and i64 %44, 64
  %46 = icmp eq i64 %45, 0
  br i1 %46, label %read_ffffffffffffffd8, label %read_ffffffffffffffd0

read_ffffffffffffffd0:                            ; preds = %entry
  %47 = shl i64 %2, 1
  %48 = add i64 %47, 15
  br label %common.ret

read_ffffffffffffffd8:                            ; preds = %entry
  %49 = add i64 %2, -30
  br label %common.ret

common.ret:                                       ; preds = %read_ffffffffffffffd0, %read_ffffffffffffffd8
  %50 = phi i64 [ %48, %read_ffffffffffffffd0 ], [ %49, %read_ffffffffffffffd8 ]
  %RAX95890 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %50, ptr %RAX95890, align 8
=======
  %RCX94238 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX94238, align 8
  %1 = sub i64 1292, %0
  %2 = sub i64 -9223372036854774515, %0
  %3 = and i64 %2, %1
  %4 = icmp sgt i64 %3, -1
  %5 = select i1 %4, i64 308, i64 498
  %6 = mul nuw nsw i64 %5, 254
  %7 = shl i64 %5, 56
  %8 = ashr i64 %7, 63
  %9 = add nsw i64 %8, %6
  %10 = and i64 %9, 255
  %11 = icmp eq i64 %10, 27
  %12 = shl i64 %0, 1
  %13 = add i64 %12, 15
  %14 = add i64 %0, -30
  %15 = select i1 %11, i64 %13, i64 %14
  %RAX94236 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %15, ptr %RAX94236, align 8
>>>>>>> Stashed changes
  ret ptr %state
}
