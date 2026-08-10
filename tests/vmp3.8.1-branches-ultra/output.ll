define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX43886 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX43886, align 8
  %1 = sub i64 1292, %0
  %2 = sub i64 -9223372036854774515, %0
  %3 = and i64 %2, %1
  %4 = add i64 %0, -30
  %5 = shl i64 %0, 1
  %6 = add i64 %5, 15
  %7 = icmp slt i64 %3, 0
  %8 = select i1 %7, i64 %6, i64 %4
  %RAX43884 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %8, ptr %RAX43884, align 8
  ret ptr %state
}
