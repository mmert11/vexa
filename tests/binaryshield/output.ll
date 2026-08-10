define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RCX51274 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %0 = load i64, ptr %RCX51274, align 8
  %1 = trunc i64 %0 to i32
  switch i32 %1, label %common.ret.fold.split [
    i32 1859, label %common.ret
    i32 2418, label %common.ret
    i32 1638, label %common.ret
    i32 299902, label %common.ret
    i32 29763, label %common.ret
  ]

common.ret.fold.split:                            ; preds = %entry
  br label %common.ret

common.ret:                                       ; preds = %entry, %entry, %entry, %entry, %entry, %common.ret.fold.split
  %2 = phi i64 [ 1, %entry ], [ 0, %common.ret.fold.split ], [ 1, %entry ], [ 1, %entry ], [ 1, %entry ], [ 1, %entry ]
  %RAX51272 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %2, ptr %RAX51272, align 8
  ret ptr %state
}
