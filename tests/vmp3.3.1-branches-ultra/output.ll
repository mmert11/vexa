define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX53499 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX53499, align 8
  %1 = icmp eq i64 %0, 1293
  %2 = add i64 %0, -30
  %3 = select i1 %1, i64 2601, i64 %2
  %RAX53497 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %3, ptr %RAX53497, align 8
  ret ptr %state
}
