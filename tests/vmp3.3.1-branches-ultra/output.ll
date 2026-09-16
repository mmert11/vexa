define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX53499 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX53499, align 8
  %1 = icmp eq i64 %0, 1293
  %2 = select i1 %1, i64 0, i64 64
  %3 = lshr exact i64 %2, 6
  %4 = add nuw nsw i64 %3, 4294967295
  %5 = and i64 %4, 4294967295
  %6 = mul nsw i64 %2, -67108864
  %7 = or disjoint i64 %5, %6
  %8 = xor i64 %7, 4294967295
  %9 = and i64 %8, 1074004144
  %10 = and i64 %6, 4294967296
  %11 = or disjoint i64 %9, %10
  %12 = and i64 %8, 5368971670
  %13 = xor i64 %12, 5368971670
  %14 = xor i64 %7, 4294967296
  %15 = and i64 %13, %14
  %16 = add nuw nsw i64 %15, %11
  %17 = trunc i64 %16 to i32
  %18 = icmp eq i64 %16, 5368971670
  %19 = select i1 %18, i32 -1051591146, i32 -743112912
  %20 = xor i32 %19, %17
  %21 = sub nsw i32 0, %20
  %22 = tail call i32 @llvm.bswap.i32(i32 %21)
  %23 = add nsw i32 %22, -1
  %24 = tail call i32 @llvm.fshl.i32(i32 %23, i32 %23, i32 3)
  %25 = shl i32 %23, 3
  %26 = and i32 %24, 255
  %27 = and i32 %25, -256
  %28 = or disjoint i32 %27, %26
  %29 = icmp eq i32 %28, 162652
  %30 = shl i64 %0, 1
  %31 = add i64 %30, 15
  %32 = add i64 %0, -30
  %33 = select i1 %29, i64 %32, i64 %31
  %RAX53497 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %33, ptr %RAX53497, align 8
  ret ptr %state
}
