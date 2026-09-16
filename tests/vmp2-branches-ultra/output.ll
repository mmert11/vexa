define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
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
  ret ptr %state
}
