define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RCX94238 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %2 = load i64, ptr %RCX94238, align 8
  %stack174584.sroa.13026.sroa.3 = alloca [16 x i8], align 1
  %3 = and i32 %1, 40
  %4 = zext nneg i32 %3 to i64
  %5 = add i64 %2, -1293
  %6 = sub i64 1292, %2
  %7 = and i64 %6, %2
  %8 = lshr i64 %7, 52
  %9 = and i64 %8, 2048
  %10 = xor i64 %2, -1
  %11 = and i64 %5, %2
  %12 = and i64 %6, %10
  %13 = or i64 %11, %12
  %14 = and i64 %13, 16
  %15 = or disjoint i64 %14, %9
  %16 = trunc i64 %5 to i8
  %17 = lshr i8 %16, 4
  %stack174584.sroa.13026.sroa.3.12..sroa_idx203381 = getelementptr inbounds nuw i8, ptr %stack174584.sroa.13026.sroa.3, i64 12
  store i16 0, ptr %stack174584.sroa.13026.sroa.3.12..sroa_idx203381, align 1
  %stack174584.sroa.13026.sroa.3.8..sroa_idx203380 = getelementptr inbounds nuw i8, ptr %stack174584.sroa.13026.sroa.3, i64 8
  store i32 0, ptr %stack174584.sroa.13026.sroa.3.8..sroa_idx203380, align 1
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
  %stack174584.sroa.13026.sroa.3.6..sroa_idx203379 = getelementptr inbounds nuw i8, ptr %stack174584.sroa.13026.sroa.3, i64 6
  store i16 %27, ptr %stack174584.sroa.13026.sroa.3.6..sroa_idx203379, align 1
  %28 = load i64, ptr %stack174584.sroa.13026.sroa.3.6..sroa_idx203379, align 1
  %29 = shl i64 %28, 2
  %30 = sub i64 -9223372036854774515, %2
  %31 = and i64 %30, %6
  %32 = lshr i64 %31, 57
  %33 = and i64 %32, 64
  %34 = lshr i64 %5, 56
  %35 = and i64 %34, 128
  %36 = or disjoint i64 %33, %35
  %37 = or disjoint i64 %15, %36
  %38 = or disjoint i64 %37, %4
  %39 = xor i64 %38, 16
  %40 = add i64 %39, %29
  %41 = and i64 %40, 64
  %42 = icmp eq i64 %41, 0
  %43 = select i1 %42, i64 308, i64 498
  %44 = mul nuw nsw i64 %43, 254
  %45 = shl i64 %43, 56
  %46 = ashr i64 %45, 63
  %47 = add nsw i64 %46, %44
  %48 = and i64 %47, 255
  %49 = icmp eq i64 %48, 27
  br i1 %49, label %MOV_GPRv_IMMv_32_55763, label %MOV_GPRv_IMMv_32_25087

MOV_GPRv_IMMv_32_25087:                           ; preds = %entry
  %50 = add i64 %2, -30
  br label %common.ret

MOV_GPRv_IMMv_32_55763:                           ; preds = %entry
  %51 = shl i64 %2, 1
  %52 = add i64 %51, 15
  br label %common.ret

common.ret:                                       ; preds = %MOV_GPRv_IMMv_32_55763, %MOV_GPRv_IMMv_32_25087
  %53 = phi i64 [ %52, %MOV_GPRv_IMMv_32_55763 ], [ %50, %MOV_GPRv_IMMv_32_25087 ]
  %RAX94236 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %53, ptr %RAX94236, align 8
  ret ptr %state
}
