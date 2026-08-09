define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RAX1239 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %2 = load i64, ptr %RAX1239, align 8
  %RBX1240 = getelementptr inbounds nuw i8, ptr %state, i64 2232
  %3 = load i64, ptr %RBX1240, align 8
  %RCX1241 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %4 = load i64, ptr %RCX1241, align 8
  %RDX1242 = getelementptr inbounds nuw i8, ptr %state, i64 2264
  %5 = load i64, ptr %RDX1242, align 8
  %RSI1243 = getelementptr inbounds nuw i8, ptr %state, i64 2280
  %6 = load i64, ptr %RSI1243, align 8
  %RDI1244 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %7 = load i64, ptr %RDI1244, align 8
  %RBP1246 = getelementptr inbounds nuw i8, ptr %state, i64 2328
  %8 = load i64, ptr %RBP1246, align 8
  %R81248 = getelementptr inbounds nuw i8, ptr %state, i64 2344
  %9 = load i64, ptr %R81248, align 8
  %R121252 = getelementptr inbounds nuw i8, ptr %state, i64 2408
  %10 = load i64, ptr %R121252, align 8
  %R131253 = getelementptr inbounds nuw i8, ptr %state, i64 2424
  %11 = load i64, ptr %R131253, align 8
  %R151255 = getelementptr inbounds nuw i8, ptr %state, i64 2456
  %12 = load i64, ptr %R151255, align 8
  %DF1298 = getelementptr inbounds nuw i8, ptr %state, i64 2075
  %13 = load i8, ptr %DF1298, align 1
  %stack9616 = alloca [8192 x i8], align 1
  %stack_ptr = getelementptr inbounds nuw i8, ptr %stack9616, i64 4096
  %14 = getelementptr inbounds nuw i8, ptr %stack9616, i64 4088
  store i64 0, ptr %14, align 8
  %15 = getelementptr inbounds nuw i8, ptr %stack9616, i64 4080
  store i64 %8, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %stack9616, i64 4072
  store i64 %7, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3560
  %18 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3552
  %19 = getelementptr inbounds nuw i8, ptr %memory, i64 61464
  store i64 8553, ptr %19, align 8
  %20 = getelementptr inbounds nuw i8, ptr %memory, i64 59392
  store i64 -528, ptr %20, align 8
  %21 = getelementptr inbounds nuw i8, ptr %memory, i64 61472
  store i64 53248, ptr %21, align 8
  %22 = getelementptr inbounds nuw i8, ptr %memory, i64 61480
  store i64 53737, ptr %22, align 8
  %23 = getelementptr inbounds nuw i8, ptr %memory, i64 61488
  store i64 53791, ptr %23, align 8
  %24 = getelementptr inbounds nuw i8, ptr %memory, i64 61496
  store i64 53996, ptr %24, align 8
  %25 = getelementptr inbounds nuw i8, ptr %memory, i64 61504
  store i64 54201, ptr %25, align 8
  %26 = getelementptr inbounds nuw i8, ptr %memory, i64 61512
  store i64 54416, ptr %26, align 8
  %27 = getelementptr inbounds nuw i8, ptr %memory, i64 61520
  store i64 54617, ptr %27, align 8
  %28 = getelementptr inbounds nuw i8, ptr %memory, i64 61528
  store i64 54837, ptr %28, align 8
  %29 = getelementptr inbounds nuw i8, ptr %memory, i64 61536
  store i64 55046, ptr %29, align 8
  %30 = getelementptr inbounds nuw i8, ptr %memory, i64 61544
  store i64 55255, ptr %30, align 8
  %31 = getelementptr inbounds nuw i8, ptr %memory, i64 61552
  store i64 55464, ptr %31, align 8
  %32 = getelementptr inbounds nuw i8, ptr %memory, i64 61560
  store i64 55673, ptr %32, align 8
  %33 = getelementptr inbounds nuw i8, ptr %memory, i64 61568
  store i64 55882, ptr %33, align 8
  %34 = getelementptr inbounds nuw i8, ptr %memory, i64 61576
  store i64 56133, ptr %34, align 8
  %35 = getelementptr inbounds nuw i8, ptr %memory, i64 61584
  store i64 56169, ptr %35, align 8
  %36 = getelementptr inbounds nuw i8, ptr %memory, i64 61592
  store i64 56228, ptr %36, align 8
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 61600
  store i64 56287, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 61608
  store i64 56346, ptr %38, align 8
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 61616
  store i64 56405, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 61624
  store i64 56477, ptr %40, align 8
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 61632
  store i64 56551, ptr %41, align 8
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 61640
  store i64 56619, ptr %42, align 8
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 61648
  store i64 56700, ptr %43, align 8
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 61656
  store i64 56768, ptr %44, align 8
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 61664
  store i64 56851, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 61672
  store i64 57035, ptr %46, align 8
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 61680
  store i64 57077, ptr %47, align 8
  store i64 %12, ptr %17, align 8
  %48 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3544
  store i64 %11, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3536
  store i64 %10, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3528
  store i64 0, ptr %50, align 8
  %51 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3520
  store i64 57077, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3512
  store i64 61688, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3504
  store i64 %9, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3496
  store i64 %7, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3488
  store i64 %6, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3480
  store i64 -16, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3472
  store i64 -528, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3464
  store i64 %3, ptr %58, align 8
  %59 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3456
  store i64 %5, ptr %59, align 8
  %60 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3448
  store i64 %4, ptr %60, align 8
  %61 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3440
  store i64 %2, ptr %61, align 8
  %62 = and i32 %1, -3286
  %63 = and i8 %13, 1
  %64 = zext nneg i8 %63 to i32
  %65 = shl nuw nsw i32 %64, 10
  %66 = or disjoint i32 %65, %62
  %.0.load.ext = zext i32 %66 to i64
  %67 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3432
  store i64 %.0.load.ext, ptr %67, align 8
  %68 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %68, align 8
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %69, align 8
  %70 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %70, align 8
  %71 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %71, align 8
  %72 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  %73 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %73, align 8
  %74 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %74, align 8
  %75 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %75, align 8
  %76 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %76, align 8
  store i64 0, ptr %18, align 8
  %77 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %77, align 8
  %78 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %78, align 8
  %79 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %79, align 8
  %80 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %80, align 8
  %81 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %82 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %82, align 8
  %83 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %83, align 8
  %84 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %84, align 8
  %85 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %85, align 8
  %86 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %86, align 8
  %87 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %87, align 8
  %88 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %88, align 8
  %89 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %89, align 8
  %90 = getelementptr inbounds nuw i8, ptr %stack9616, i64 4064
  store i64 0, ptr %90, align 8
  store i64 -20, ptr %81, align 8
  store i64 -36, ptr %72, align 8
  %91 = getelementptr inbounds nuw i8, ptr %stack9616, i64 4060
  store i32 0, ptr %91, align 4
  %92 = getelementptr inbounds nuw i8, ptr %memory, i64 57181
  %93 = getelementptr inbounds nuw i8, ptr %memory, i64 57182
  %94 = getelementptr inbounds nuw i8, ptr %memory, i64 57183
  %95 = getelementptr inbounds nuw i8, ptr %memory, i64 57184
  %96 = getelementptr inbounds nuw i8, ptr %memory, i64 59400
  %97 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  %98 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  %99 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  %100 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  %101 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  %102 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  %103 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  %104 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  %105 = getelementptr inbounds nuw i8, ptr %stack9616, i64 3424
  %106 = and i32 %1, -2100438
  br label %ADD_GPRv_IMMb_64_378

ADD_GPRv_IMMb_64_378:                             ; preds = %ADD_GPRv_IMMb_64_638, %entry
  %107 = phi i64 [ -16, %entry ], [ %.pre9692, %ADD_GPRv_IMMb_64_638 ]
  %108 = phi i64 [ %5, %entry ], [ %.pre9690, %ADD_GPRv_IMMb_64_638 ]
  %109 = phi i64 [ %4, %entry ], [ %.pre9689, %ADD_GPRv_IMMb_64_638 ]
  %110 = phi i64 [ %.0.load.ext, %entry ], [ %.pre, %ADD_GPRv_IMMb_64_638 ]
  %RSI1227.0 = phi i64 [ 61464, %entry ], [ %296, %ADD_GPRv_IMMb_64_638 ]
  %RAX1223.0 = phi i64 [ 57390, %entry ], [ %337, %ADD_GPRv_IMMb_64_638 ]
  %.0 = phi i32 [ %106, %entry ], [ %211, %ADD_GPRv_IMMb_64_638 ]
  %111 = getelementptr i8, ptr %memory, i64 %RAX1223.0
  %112 = getelementptr i8, ptr %111, i64 2
  %113 = load i8, ptr %112, align 1
  store i8 %113, ptr %92, align 1
  %114 = getelementptr i8, ptr %111, i64 3
  %115 = load i8, ptr %114, align 1
  store i8 %115, ptr %93, align 1
  %116 = getelementptr i8, ptr %111, i64 4
  %117 = load i8, ptr %116, align 1
  store i8 %117, ptr %94, align 1
  %118 = getelementptr i8, ptr %111, i64 5
  %119 = load i8, ptr %118, align 1
  store i8 %119, ptr %95, align 1
  %120 = add nuw nsw i64 %RAX1223.0, 6
  %121 = getelementptr i8, ptr %memory, i64 %RSI1227.0
  %122 = getelementptr i8, ptr %121, i64 -8
  store i64 %120, ptr %122, align 8
  %123 = add i64 %RSI1227.0, -59424
  store i64 %123, ptr %96, align 8
  %124 = trunc i64 %110 to i32
  %125 = and i32 %124, 1024
  %126 = and i32 %124, 2097152
  %127 = or disjoint i32 %.0, %126
  %128 = getelementptr i8, ptr %stack_ptr, i64 %107
  %129 = getelementptr i8, ptr %128, i64 -20
  %130 = load i32, ptr %129, align 4
  %131 = sext i32 %130 to i64
  store i64 0, ptr %50, align 8
  %132 = load i64, ptr %20, align 8
  store i64 %132, ptr %57, align 8
  store i64 %108, ptr %59, align 8
  store i64 %109, ptr %60, align 8
  store i64 %131, ptr %61, align 8
  %133 = or disjoint i32 %125, %127
  %134 = or disjoint i32 %133, 132
  %.0.load.ext9603 = zext i32 %134 to i64
  store i64 %.0.load.ext9603, ptr %67, align 8
  %135 = load i64, ptr %122, align 8
  %136 = getelementptr i8, ptr %memory, i64 %135
  %137 = getelementptr i8, ptr %136, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %92, i8 -112, i64 16, i1 false)
  %138 = load i8, ptr %137, align 1
  %R101233.sroa.0.0.insert.ext2823 = zext i8 %138 to i64
  %139 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2823, 3
  %140 = getelementptr i8, ptr %stack_ptr, i64 %132
  %141 = getelementptr i8, ptr %140, i64 -128
  %142 = getelementptr i8, ptr %141, i64 %139
  %143 = load i64, ptr %142, align 8
  store i64 %143, ptr %122, align 8
  %144 = load i64, ptr %20, align 8
  %145 = getelementptr i8, ptr %136, i64 3
  %146 = load i8, ptr %145, align 1
  %R101233.sroa.0.0.insert.ext2818 = zext i8 %146 to i64
  %147 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2818, 3
  %148 = getelementptr i8, ptr %stack_ptr, i64 %144
  %149 = getelementptr i8, ptr %148, i64 -128
  %150 = getelementptr i8, ptr %149, i64 %147
  %151 = load i64, ptr %150, align 8
  %152 = getelementptr i8, ptr %121, i64 -16
  store i64 %151, ptr %152, align 8
  %153 = getelementptr i8, ptr %136, i64 5
  %154 = load i64, ptr %153, align 8
  %155 = getelementptr i8, ptr %121, i64 -24
  store i64 %154, ptr %155, align 8
  %156 = add i64 %154, %151
  store i64 %156, ptr %152, align 8
  store i64 54543, ptr %97, align 8
  store i64 54556, ptr %98, align 8
  store i64 54571, ptr %99, align 8
  store i64 54584, ptr %100, align 8
  %157 = load i64, ptr %152, align 8
  %158 = getelementptr inbounds i8, ptr %stack_ptr, i64 %157
  %159 = load i64, ptr %158, align 8
  store i64 %159, ptr %152, align 8
  store i64 56009, ptr %101, align 8
  store i64 56034, ptr %102, align 8
  store i64 56062, ptr %103, align 8
  store i64 56087, ptr %104, align 8
  %160 = load i64, ptr %152, align 8
  %161 = load i64, ptr %122, align 8
  %162 = sub i64 %161, %160
  %163 = icmp ult i64 %161, %160
  %164 = trunc i64 %162 to i8
  %165 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %164)
  %166 = xor i64 %162, %161
  %167 = xor i64 %166, %160
  %168 = trunc i64 %167 to i32
  %169 = and i32 %168, 16
  %170 = icmp eq i64 %161, %160
  %171 = xor i64 %161, %160
  %172 = lshr i64 %171, 63
  %173 = lshr i64 %166, 63
  %174 = add nuw nsw i64 %173, %172
  %175 = icmp eq i64 %174, 2
  %176 = shl nuw nsw i8 %165, 2
  %177 = and i8 %176, 4
  %178 = zext i1 %163 to i8
  %179 = or disjoint i8 %177, %178
  %180 = xor i8 %179, 4
  %181 = zext nneg i8 %180 to i32
  %sh.diff = lshr i64 %162, 56
  %tr.sh.diff = trunc nuw nsw i64 %sh.diff to i32
  %182 = and i32 %tr.sh.diff, 128
  %183 = select i1 %170, i32 64, i32 0
  %184 = select i1 %175, i32 2048, i32 0
  %185 = or disjoint i32 %183, %182
  %186 = or disjoint i32 %185, %169
  %187 = or disjoint i32 %186, %184
  %188 = or disjoint i32 %187, %181
  %189 = or disjoint i32 %188, %127
  %190 = or disjoint i32 %189, %125
  %.0.load.ext9606 = zext i32 %190 to i64
  store i64 %.0.load.ext9606, ptr %105, align 8
  %RCX1225.sroa.0.sroa.0.sroa.0.0.extract.trunc8548 = trunc i32 %189 to i16
  %191 = trunc i32 %190 to i16
  %RCX1225.sroa.0.sroa.0.sroa.442.0.insert.shift = and i16 %191, -256
  %RCX1225.sroa.0.sroa.0.sroa.0.0.insert.ext = and i16 %RCX1225.sroa.0.sroa.0.sroa.0.0.extract.trunc8548, 255
  %RCX1225.sroa.0.sroa.0.sroa.0.0.insert.insert = or disjoint i16 %RCX1225.sroa.0.sroa.0.sroa.442.0.insert.shift, %RCX1225.sroa.0.sroa.0.sroa.0.0.insert.ext
  %192 = getelementptr i8, ptr %121, i64 -2
  store i16 %RCX1225.sroa.0.sroa.0.sroa.0.0.insert.insert, ptr %192, align 2
  br i1 %163, label %ADD_GPRv_IMMb_64_638, label %MOVZX_GPR64_MEMw_608

MOVZX_GPR64_MEMw_608:                             ; preds = %ADD_GPRv_IMMb_64_378
  %193 = getelementptr i8, ptr %136, i64 17
  %194 = load i16, ptr %193, align 2
  %RCX1225.sroa.0.0.insert.ext5920 = zext i16 %194 to i64
  %195 = getelementptr inbounds nuw i8, ptr %memory, i64 %RCX1225.sroa.0.0.insert.ext5920
  %196 = getelementptr inbounds nuw i8, ptr %195, i64 57345
  %197 = load i16, ptr %196, align 2
  %198 = load i16, ptr %19, align 2
  %199 = add i16 %198, %197
  store i16 %199, ptr %19, align 2
  %200 = getelementptr i8, ptr %128, i64 -16
  %201 = load i64, ptr %200, align 8
  store i64 %201, ptr %RAX1239, align 8
  ret ptr %state

ADD_GPRv_IMMb_64_638:                             ; preds = %ADD_GPRv_IMMb_64_378
  %202 = getelementptr i8, ptr %136, i64 21
  %203 = load i8, ptr %202, align 1
  store i8 %203, ptr %92, align 1
  %204 = getelementptr i8, ptr %136, i64 22
  %205 = load i8, ptr %204, align 1
  store i8 %205, ptr %93, align 1
  %206 = getelementptr i8, ptr %136, i64 23
  %207 = load i8, ptr %206, align 1
  store i8 %207, ptr %94, align 1
  %208 = getelementptr i8, ptr %136, i64 24
  %209 = load i8, ptr %208, align 1
  store i8 %209, ptr %95, align 1
  %210 = add i64 %135, 25
  store i64 %210, ptr %122, align 8
  store i64 %123, ptr %96, align 8
  %211 = and i32 %189, -2100438
  %212 = load i32, ptr %129, align 4
  %213 = sext i32 %212 to i64
  store i64 0, ptr %50, align 8
  %214 = load i64, ptr %20, align 8
  store i64 %214, ptr %57, align 8
  store i64 %108, ptr %59, align 8
  store i64 %109, ptr %60, align 8
  store i64 %213, ptr %61, align 8
  %215 = and i32 %124, 2098176
  %216 = or disjoint i32 %215, %211
  %217 = or disjoint i32 %216, 132
  %.0.load.ext9611 = zext i32 %217 to i64
  store i64 %.0.load.ext9611, ptr %67, align 8
  %218 = load i64, ptr %122, align 8
  %219 = getelementptr i8, ptr %memory, i64 %218
  %220 = getelementptr i8, ptr %219, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %92, i8 -112, i64 16, i1 false)
  %221 = load i8, ptr %220, align 1
  %R101233.sroa.0.0.insert.ext2767 = zext i8 %221 to i64
  %222 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2767, 3
  %223 = getelementptr i8, ptr %stack_ptr, i64 %214
  %224 = getelementptr i8, ptr %223, i64 -128
  %225 = getelementptr i8, ptr %224, i64 %222
  %226 = load i64, ptr %225, align 8
  store i64 %226, ptr %122, align 8
  %227 = load i64, ptr %20, align 8
  %228 = getelementptr i8, ptr %219, i64 3
  %229 = load i8, ptr %228, align 1
  %R101233.sroa.0.0.insert.ext2762 = zext i8 %229 to i64
  %230 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2762, 3
  %231 = getelementptr i8, ptr %stack_ptr, i64 %227
  %232 = getelementptr i8, ptr %231, i64 -128
  %233 = getelementptr i8, ptr %232, i64 %230
  %234 = load i64, ptr %233, align 8
  store i64 %234, ptr %152, align 8
  %235 = getelementptr i8, ptr %219, i64 5
  %236 = load i64, ptr %235, align 8
  store i64 %236, ptr %155, align 8
  %237 = getelementptr i8, ptr %stack_ptr, i64 %236
  %238 = getelementptr i8, ptr %237, i64 %234
  %239 = load i64, ptr %238, align 8
  store i64 %239, ptr %152, align 8
  %240 = add i64 %239, %226
  store i64 %240, ptr %122, align 8
  %241 = load i64, ptr %20, align 8
  %242 = getelementptr i8, ptr %219, i64 17
  %243 = load i8, ptr %242, align 1
  %R101233.sroa.0.0.insert.ext2757 = zext i8 %243 to i64
  %244 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2757, 3
  %245 = getelementptr i8, ptr %stack_ptr, i64 %241
  %246 = getelementptr i8, ptr %245, i64 -128
  %247 = getelementptr i8, ptr %246, i64 %244
  store i64 %240, ptr %247, align 8
  %248 = getelementptr i8, ptr %219, i64 19
  %249 = load i8, ptr %248, align 1
  %R101233.sroa.0.0.insert.ext2752 = zext i8 %249 to i64
  %250 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2752, 3
  %251 = getelementptr i8, ptr %246, i64 %250
  %252 = load i64, ptr %251, align 8
  store i64 %252, ptr %122, align 8
  %253 = load i64, ptr %20, align 8
  %254 = getelementptr i8, ptr %219, i64 21
  %255 = load i8, ptr %254, align 1
  %R101233.sroa.0.0.insert.ext2747 = zext i8 %255 to i64
  %256 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2747, 3
  %257 = getelementptr i8, ptr %stack_ptr, i64 %253
  %258 = getelementptr i8, ptr %257, i64 -128
  %259 = getelementptr i8, ptr %258, i64 %256
  %260 = load i64, ptr %259, align 8
  store i64 %260, ptr %152, align 8
  %261 = getelementptr i8, ptr %219, i64 23
  %262 = load i64, ptr %261, align 8
  store i64 %262, ptr %155, align 8
  %263 = add i64 %262, %260
  store i64 %263, ptr %152, align 8
  %264 = load i64, ptr %20, align 8
  %265 = getelementptr i8, ptr %219, i64 33
  %266 = load i8, ptr %265, align 1
  %R101233.sroa.0.0.insert.ext2742 = zext i8 %266 to i64
  %267 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2742, 3
  %268 = getelementptr i8, ptr %stack_ptr, i64 %264
  %269 = getelementptr i8, ptr %268, i64 -128
  %270 = getelementptr i8, ptr %269, i64 %267
  %271 = load i64, ptr %270, align 8
  %272 = getelementptr inbounds i8, ptr %stack_ptr, i64 %263
  store i64 %271, ptr %272, align 8
  %273 = getelementptr i8, ptr %219, i64 35
  %274 = load i8, ptr %273, align 1
  %R101233.sroa.0.0.insert.ext2732 = zext i8 %274 to i64
  %275 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2732, 3
  %276 = getelementptr i8, ptr %269, i64 %275
  %277 = load i64, ptr %276, align 8
  store i64 %277, ptr %152, align 8
  %278 = getelementptr i8, ptr %219, i64 37
  %279 = load i64, ptr %278, align 8
  store i64 %279, ptr %155, align 8
  %280 = add i64 %279, %277
  store i64 %280, ptr %152, align 8
  %281 = getelementptr inbounds i8, ptr %stack_ptr, i64 %280
  %282 = load i32, ptr %281, align 4
  %283 = getelementptr i8, ptr %121, i64 -12
  store i32 %282, ptr %283, align 4
  %284 = load i64, ptr %20, align 8
  %285 = getelementptr i8, ptr %219, i64 48
  %286 = load i8, ptr %285, align 1
  %R101233.sroa.0.0.insert.ext2727 = zext i8 %286 to i64
  %287 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2727, 3
  %288 = getelementptr i8, ptr %stack_ptr, i64 %284
  %289 = getelementptr i8, ptr %288, i64 -128
  %290 = getelementptr i8, ptr %289, i64 %287
  store i32 %282, ptr %290, align 4
  %291 = getelementptr i8, ptr %219, i64 50
  %292 = load i8, ptr %291, align 1
  %R101233.sroa.0.0.insert.ext2722 = zext i8 %292 to i64
  %293 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2722, 3
  %294 = getelementptr i8, ptr %289, i64 %293
  %295 = load i64, ptr %294, align 8
  %RDX1226.sroa.0.0.extract.trunc4823 = trunc i64 %295 to i32
  %296 = add i64 %RSI1227.0, -12
  %297 = getelementptr inbounds i8, ptr %memory, i64 %296
  store i32 %RDX1226.sroa.0.0.extract.trunc4823, ptr %297, align 4
  %298 = getelementptr i8, ptr %219, i64 52
  %299 = load i32, ptr %298, align 4
  store i32 %299, ptr %152, align 4
  %300 = add i32 %299, %RDX1226.sroa.0.0.extract.trunc4823
  store i32 %300, ptr %297, align 4
  %301 = load i64, ptr %20, align 8
  %302 = getelementptr i8, ptr %219, i64 58
  %303 = load i8, ptr %302, align 1
  %R101233.sroa.0.0.insert.ext2717 = zext i8 %303 to i64
  %304 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2717, 3
  %305 = getelementptr i8, ptr %stack_ptr, i64 %301
  %306 = getelementptr i8, ptr %305, i64 -128
  %307 = getelementptr i8, ptr %306, i64 %304
  store i32 %300, ptr %307, align 4
  %308 = getelementptr i8, ptr %219, i64 60
  %309 = load i8, ptr %308, align 1
  %R101233.sroa.0.0.insert.ext2712 = zext i8 %309 to i64
  %310 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2712, 3
  %311 = getelementptr i8, ptr %306, i64 %310
  %312 = load i64, ptr %311, align 8
  %RDX1226.sroa.0.0.extract.trunc4816 = trunc i64 %312 to i32
  store i32 %RDX1226.sroa.0.0.extract.trunc4816, ptr %297, align 4
  %313 = load i64, ptr %20, align 8
  %314 = getelementptr i8, ptr %219, i64 62
  %315 = load i8, ptr %314, align 1
  %R101233.sroa.0.0.insert.ext2707 = zext i8 %315 to i64
  %316 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2707, 3
  %317 = getelementptr i8, ptr %stack_ptr, i64 %313
  %318 = getelementptr i8, ptr %317, i64 -128
  %319 = getelementptr i8, ptr %318, i64 %316
  %320 = load i64, ptr %319, align 8
  %321 = getelementptr i8, ptr %121, i64 -20
  store i64 %320, ptr %321, align 8
  %322 = getelementptr i8, ptr %219, i64 64
  %323 = load i64, ptr %322, align 8
  %324 = getelementptr i8, ptr %121, i64 -28
  store i64 %323, ptr %324, align 8
  %325 = add i64 %323, %320
  store i64 %325, ptr %321, align 8
  %326 = load i64, ptr %20, align 8
  %327 = getelementptr i8, ptr %219, i64 74
  %328 = load i8, ptr %327, align 1
  %R101233.sroa.0.0.insert.ext2702 = zext i8 %328 to i64
  %329 = shl nuw nsw i64 %R101233.sroa.0.0.insert.ext2702, 3
  %330 = getelementptr i8, ptr %stack_ptr, i64 %326
  %331 = getelementptr i8, ptr %330, i64 -128
  %332 = getelementptr i8, ptr %331, i64 %329
  %333 = load i64, ptr %332, align 8
  %RDX1226.sroa.0.0.extract.trunc4808 = trunc i64 %333 to i32
  %334 = getelementptr inbounds i8, ptr %stack_ptr, i64 %325
  store i32 %RDX1226.sroa.0.0.extract.trunc4808, ptr %334, align 4
  %335 = getelementptr i8, ptr %219, i64 76
  %336 = load i16, ptr %335, align 2
  %RCX1225.sroa.0.0.insert.ext5483 = zext i16 %336 to i64
  %337 = add nuw nsw i64 %RCX1225.sroa.0.0.insert.ext5483, 57344
  %.pre = load i64, ptr %67, align 8
  %.pre9689 = load i64, ptr %60, align 8
  %.pre9690 = load i64, ptr %59, align 8
  %.pre9692 = load i64, ptr %56, align 8
  br label %ADD_GPRv_IMMb_64_378
}
