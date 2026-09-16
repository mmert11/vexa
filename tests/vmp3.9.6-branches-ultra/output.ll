define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX40206 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX40206, align 8
  %1 = add i64 %0, -1293
  %2 = add i64 %0, 9223372036854774514
  %3 = or i64 %2, %1
  %4 = icmp sgt i64 %3, -1
  %5 = select i1 %4, i64 5368751204, i64 5368967658
  %6 = trunc i64 %5 to i32
  %7 = add nuw nsw i64 %5, 8589934588
  %8 = and i64 %7, 5368968702
  %9 = icmp eq i64 %8, 5368751200
  %10 = select i1 %9, i32 -1296742556, i32 1405358826
  %11 = xor i32 %10, %6
  %12 = icmp eq i32 %11, 331866880
  %13 = add i64 %0, -30
  %14 = shl i64 %0, 1
  %15 = add i64 %14, 15
  %16 = select i1 %12, i64 %13, i64 %15
  %RAX40204 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %16, ptr %RAX40204, align 8
  ret ptr %state
}
