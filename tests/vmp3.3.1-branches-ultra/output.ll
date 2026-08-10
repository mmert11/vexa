define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX53521 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX53521, align 8
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
  %16 = add nuw nsw i64 %11, %15
  %17 = icmp eq i64 %16, 5368971670
  %18 = shl i64 %0, 1
  %19 = add i64 %18, 15
  %20 = add i64 %0, -30
  %21 = select i1 %17, i64 %19, i64 %20
  %RAX53519 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %21, ptr %RAX53519, align 8
  ret ptr %state
}
