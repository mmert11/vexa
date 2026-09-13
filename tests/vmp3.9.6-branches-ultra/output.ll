define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RCX40206 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %2 = load i64, ptr %RCX40206, align 8
  %stack115437.sroa.6943.sroa.19 = alloca [16 x i8], align 1
  %3 = and i32 %1, 40
  %4 = add i64 %2, -1293
  %5 = xor i64 %2, -1
  %6 = sub i64 12, %2
  %7 = and i64 %4, %2
  %8 = and i64 %6, %5
  %9 = or i64 %7, %8
  %10 = and i64 %9, 16
  %11 = add i64 %2, 9223372036854774514
  %12 = or i64 %11, %4
  %13 = lshr i64 %12, 57
  %14 = and i64 %13, 64
  %15 = or disjoint i64 %10, %14
  %16 = trunc i64 %4 to i8
  %17 = lshr i8 %16, 4
  %stack115437.sroa.6943.sroa.19.12..sroa_idx131890 = getelementptr inbounds nuw i8, ptr %stack115437.sroa.6943.sroa.19, i64 12
  store i16 0, ptr %stack115437.sroa.6943.sroa.19.12..sroa_idx131890, align 1
  %stack115437.sroa.6943.sroa.19.8..sroa_idx131888 = getelementptr inbounds nuw i8, ptr %stack115437.sroa.6943.sroa.19, i64 8
  store i32 0, ptr %stack115437.sroa.6943.sroa.19.8..sroa_idx131888, align 1
  %18 = xor i8 %17, %16
  %19 = xor i8 %18, -1
  %20 = zext i8 %19 to i16
  %21 = or i16 %20, -16
  %22 = xor i16 %21, -1
  %23 = icmp eq i16 %21, -1
  %24 = tail call i16 @llvm.usub.sat.i16(i16 %22, i16 1)
  %25 = lshr i16 19252, %24
  %26 = and i16 %25, 1
  %27 = select i1 %23, i16 1, i16 %26
  %stack115437.sroa.6943.sroa.19.6..sroa_idx131879 = getelementptr inbounds nuw i8, ptr %stack115437.sroa.6943.sroa.19, i64 6
  store i16 %27, ptr %stack115437.sroa.6943.sroa.19.6..sroa_idx131879, align 1
  %28 = load i64, ptr %stack115437.sroa.6943.sroa.19.6..sroa_idx131879, align 1
  %29 = shl i64 %28, 2
  %30 = zext nneg i32 %3 to i64
  %31 = or disjoint i64 %15, %30
  %32 = xor i64 %31, 80
  %33 = add i64 %32, %29
  %34 = and i64 %33, 64
  %35 = icmp eq i64 %34, 0
  %36 = select i1 %35, i64 5368967658, i64 5368751204
  %37 = trunc i64 %36 to i32
  %38 = add nuw nsw i64 %36, 8589934588
  %39 = and i64 %38, 5368968702
  %40 = icmp eq i64 %39, 5368751200
  %41 = select i1 %40, i32 -1296742556, i32 1405358826
  %42 = xor i32 %41, %37
  %43 = icmp eq i32 %42, 331866880
  %44 = add i64 %2, -30
  %45 = shl i64 %2, 1
  %46 = add i64 %45, 15
  %47 = select i1 %43, i64 %44, i64 %46
  %RAX40204 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %47, ptr %RAX40204, align 8
  ret ptr %state
}
