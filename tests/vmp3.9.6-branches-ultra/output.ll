define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX40779 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX40779, align 8
  %1 = add i64 %0, -30
  %2 = icmp eq i64 %0, 1293
  %3 = select i1 %2, i64 2601, i64 %1
  %RAX40777 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %3, ptr %RAX40777, align 8
  ret ptr %state
}
