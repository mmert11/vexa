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
  %stack9564 = alloca [8192 x i8], align 1
  %stack_ptr = getelementptr inbounds nuw i8, ptr %stack9564, i64 4096
  %14 = getelementptr inbounds nuw i8, ptr %stack9564, i64 4088
  store i64 0, ptr %14, align 8
  %15 = getelementptr inbounds nuw i8, ptr %stack9564, i64 4080
  store i64 %8, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %stack9564, i64 4072
  store i64 %7, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3560
  %18 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3552
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
  %48 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3544
  store i64 %11, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3536
  store i64 %10, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3528
  store i64 0, ptr %50, align 8
  %51 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3520
  store i64 57077, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3512
  store i64 61688, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3504
  store i64 %9, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3496
  store i64 %7, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3488
  store i64 %6, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3480
  store i64 -16, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3472
  store i64 -528, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3464
  store i64 %3, ptr %58, align 8
  %59 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3456
  store i64 %5, ptr %59, align 8
  %60 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3448
  store i64 %4, ptr %60, align 8
  %61 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3440
  store i64 %2, ptr %61, align 8
  %62 = and i32 %1, -3286
  %63 = and i8 %13, 1
  %64 = zext nneg i8 %63 to i32
  %65 = shl nuw nsw i32 %64, 10
  %66 = or disjoint i32 %65, %62
  %67 = zext i32 %66 to i64
  %68 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3432
  store i64 %67, ptr %68, align 8
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %69, align 8
  %70 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %70, align 8
  %71 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %71, align 8
  %72 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %72, align 8
  %73 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  %74 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %74, align 8
  %75 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %75, align 8
  %76 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %76, align 8
  %77 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %77, align 8
  store i64 0, ptr %18, align 8
  %78 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %78, align 8
  %79 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %79, align 8
  %80 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %80, align 8
  %81 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %81, align 8
  %82 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %83 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %83, align 8
  %84 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %84, align 8
  %85 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %85, align 8
  %86 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %86, align 8
  %87 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %87, align 8
  %88 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %88, align 8
  %89 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %89, align 8
  %90 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %90, align 8
  %91 = getelementptr inbounds nuw i8, ptr %stack9564, i64 4064
  store i64 0, ptr %91, align 8
  store i64 -20, ptr %82, align 8
  store i64 -36, ptr %73, align 8
  %92 = getelementptr inbounds nuw i8, ptr %stack9564, i64 4060
  store i32 0, ptr %92, align 4
  %93 = getelementptr inbounds nuw i8, ptr %memory, i64 57181
  %94 = getelementptr inbounds nuw i8, ptr %memory, i64 57182
  %95 = getelementptr inbounds nuw i8, ptr %memory, i64 57183
  %96 = getelementptr inbounds nuw i8, ptr %memory, i64 57184
  %97 = getelementptr inbounds nuw i8, ptr %memory, i64 59400
  %98 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  %99 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  %100 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  %101 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  %102 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  %103 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  %104 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  %105 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  %106 = getelementptr inbounds nuw i8, ptr %stack9564, i64 3424
  %107 = and i32 %1, -2100438
  br label %ADD_GPRv_IMMb_64_378

ADD_GPRv_IMMb_64_378:                             ; preds = %ADD_GPRv_IMMb_64_638, %entry
  %108 = phi i64 [ -16, %entry ], [ %374, %ADD_GPRv_IMMb_64_638 ]
  %109 = phi i64 [ %5, %entry ], [ %373, %ADD_GPRv_IMMb_64_638 ]
  %110 = phi i64 [ %4, %entry ], [ %372, %ADD_GPRv_IMMb_64_638 ]
  %111 = phi i64 [ %67, %entry ], [ %371, %ADD_GPRv_IMMb_64_638 ]
  %112 = phi i64 [ 61464, %entry ], [ %322, %ADD_GPRv_IMMb_64_638 ]
  %113 = phi i64 [ 57390, %entry ], [ %370, %ADD_GPRv_IMMb_64_638 ]
  %114 = phi i32 [ %107, %entry ], [ %226, %ADD_GPRv_IMMb_64_638 ]
  %115 = getelementptr i8, ptr %memory, i64 %113
  %116 = getelementptr i8, ptr %115, i64 2
  %117 = load i8, ptr %116, align 1
  store i8 %117, ptr %93, align 1
  %118 = getelementptr i8, ptr %115, i64 3
  %119 = load i8, ptr %118, align 1
  store i8 %119, ptr %94, align 1
  %120 = getelementptr i8, ptr %115, i64 4
  %121 = load i8, ptr %120, align 1
  store i8 %121, ptr %95, align 1
  %122 = getelementptr i8, ptr %115, i64 5
  %123 = load i8, ptr %122, align 1
  store i8 %123, ptr %96, align 1
  %124 = add nuw nsw i64 %113, 6
  %125 = getelementptr i8, ptr %memory, i64 %112
  %126 = getelementptr i8, ptr %125, i64 -8
  store i64 %124, ptr %126, align 8
  %127 = add i64 %112, -59424
  store i64 %127, ptr %97, align 8
  %128 = trunc i64 %111 to i32
  %129 = and i32 %128, 1024
  %130 = and i32 %128, 2097152
  %131 = or disjoint i32 %114, %130
  %132 = getelementptr i8, ptr %stack_ptr, i64 %108
  %133 = getelementptr i8, ptr %132, i64 -20
  %134 = load i32, ptr %133, align 4
  %135 = sext i32 %134 to i64
  store i64 0, ptr %50, align 8
  %136 = load i64, ptr %20, align 8
  store i64 %136, ptr %57, align 8
  store i64 %109, ptr %59, align 8
  store i64 %110, ptr %60, align 8
  store i64 %135, ptr %61, align 8
  %137 = or disjoint i32 %129, %131
  %138 = or disjoint i32 %137, 132
  %139 = zext i32 %138 to i64
  store i64 %139, ptr %68, align 8
  %140 = load i64, ptr %126, align 8
  %141 = getelementptr i8, ptr %memory, i64 %140
  %142 = getelementptr i8, ptr %141, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %93, i8 -112, i64 16, i1 false)
  %143 = load i8, ptr %142, align 1
  %144 = zext i8 %143 to i64
  %145 = shl nuw nsw i64 %144, 3
  %146 = getelementptr i8, ptr %stack_ptr, i64 %136
  %147 = getelementptr i8, ptr %146, i64 -128
  %148 = getelementptr i8, ptr %147, i64 %145
  %149 = load i64, ptr %148, align 8
  store i64 %149, ptr %126, align 8
  %150 = load i64, ptr %20, align 8
  %151 = getelementptr i8, ptr %141, i64 3
  %152 = load i8, ptr %151, align 1
  %153 = zext i8 %152 to i64
  %154 = shl nuw nsw i64 %153, 3
  %155 = getelementptr i8, ptr %stack_ptr, i64 %150
  %156 = getelementptr i8, ptr %155, i64 -128
  %157 = getelementptr i8, ptr %156, i64 %154
  %158 = load i64, ptr %157, align 8
  %159 = getelementptr i8, ptr %125, i64 -16
  store i64 %158, ptr %159, align 8
  %160 = getelementptr i8, ptr %141, i64 5
  %161 = load i64, ptr %160, align 8
  %162 = getelementptr i8, ptr %125, i64 -24
  store i64 %161, ptr %162, align 8
  %163 = add i64 %161, %158
  store i64 %163, ptr %159, align 8
  store i64 54543, ptr %98, align 8
  store i64 54556, ptr %99, align 8
  store i64 54571, ptr %100, align 8
  store i64 54584, ptr %101, align 8
  %164 = load i64, ptr %159, align 8
  %165 = getelementptr inbounds i8, ptr %stack_ptr, i64 %164
  %166 = load i64, ptr %165, align 8
  store i64 %166, ptr %159, align 8
  store i64 56009, ptr %102, align 8
  store i64 56034, ptr %103, align 8
  store i64 56062, ptr %104, align 8
  store i64 56087, ptr %105, align 8
  %167 = load i64, ptr %159, align 8
  %168 = load i64, ptr %126, align 8
  %169 = sub i64 %168, %167
  %170 = icmp ult i64 %168, %167
  %171 = trunc i64 %169 to i8
  %172 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %171)
  %173 = xor i64 %169, %168
  %174 = xor i64 %173, %167
  %175 = trunc i64 %174 to i32
  %176 = and i32 %175, 16
  %177 = icmp eq i64 %168, %167
  %178 = xor i64 %168, %167
  %179 = lshr i64 %178, 63
  %180 = lshr i64 %173, 63
  %181 = add nuw nsw i64 %180, %179
  %182 = icmp eq i64 %181, 2
  %183 = shl nuw nsw i8 %172, 2
  %184 = and i8 %183, 4
  %185 = zext i1 %170 to i8
  %186 = or disjoint i8 %184, %185
  %187 = xor i8 %186, 4
  %188 = zext nneg i8 %187 to i32
  %189 = lshr i64 %169, 56
  %190 = trunc nuw nsw i64 %189 to i32
  %191 = and i32 %190, 128
  %192 = select i1 %177, i32 64, i32 0
  %193 = select i1 %182, i32 2048, i32 0
  %194 = or disjoint i32 %192, %191
  %195 = or disjoint i32 %194, %176
  %196 = or disjoint i32 %195, %193
  %197 = or disjoint i32 %196, %188
  %198 = or disjoint i32 %197, %131
  %199 = or disjoint i32 %198, %129
  %200 = zext i32 %199 to i64
  store i64 %200, ptr %106, align 8
  %201 = trunc i32 %198 to i16
  %202 = trunc i32 %199 to i16
  %203 = and i16 %202, -256
  %204 = and i16 %201, 255
  %205 = or disjoint i16 %203, %204
  %206 = getelementptr i8, ptr %125, i64 -2
  store i16 %205, ptr %206, align 2
  br i1 %170, label %ADD_GPRv_IMMb_64_638, label %MOVZX_GPR64_MEMw_608

MOVZX_GPR64_MEMw_608:                             ; preds = %ADD_GPRv_IMMb_64_378
  %207 = getelementptr i8, ptr %141, i64 17
  %208 = load i16, ptr %207, align 2
  %209 = zext i16 %208 to i64
  %210 = getelementptr inbounds nuw i8, ptr %memory, i64 %209
  %211 = getelementptr inbounds nuw i8, ptr %210, i64 57345
  %212 = load i16, ptr %211, align 2
  %213 = load i16, ptr %19, align 2
  %214 = add i16 %213, %212
  store i16 %214, ptr %19, align 2
  %215 = getelementptr i8, ptr %132, i64 -16
  %216 = load i64, ptr %215, align 8
  store i64 %216, ptr %RAX1239, align 8
  ret ptr %state

ADD_GPRv_IMMb_64_638:                             ; preds = %ADD_GPRv_IMMb_64_378
  %217 = getelementptr i8, ptr %141, i64 21
  %218 = load i8, ptr %217, align 1
  store i8 %218, ptr %93, align 1
  %219 = getelementptr i8, ptr %141, i64 22
  %220 = load i8, ptr %219, align 1
  store i8 %220, ptr %94, align 1
  %221 = getelementptr i8, ptr %141, i64 23
  %222 = load i8, ptr %221, align 1
  store i8 %222, ptr %95, align 1
  %223 = getelementptr i8, ptr %141, i64 24
  %224 = load i8, ptr %223, align 1
  store i8 %224, ptr %96, align 1
  %225 = add i64 %140, 25
  store i64 %225, ptr %126, align 8
  store i64 %127, ptr %97, align 8
  %226 = and i32 %198, -2100438
  %227 = load i32, ptr %133, align 4
  %228 = sext i32 %227 to i64
  store i64 0, ptr %50, align 8
  %229 = load i64, ptr %20, align 8
  store i64 %229, ptr %57, align 8
  store i64 %109, ptr %59, align 8
  store i64 %110, ptr %60, align 8
  store i64 %228, ptr %61, align 8
  %230 = and i32 %128, 2098176
  %231 = or disjoint i32 %230, %226
  %232 = or disjoint i32 %231, 132
  %233 = zext i32 %232 to i64
  store i64 %233, ptr %68, align 8
  %234 = load i64, ptr %126, align 8
  %235 = getelementptr i8, ptr %memory, i64 %234
  %236 = getelementptr i8, ptr %235, i64 1
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %93, i8 -112, i64 16, i1 false)
  %237 = load i8, ptr %236, align 1
  %238 = zext i8 %237 to i64
  %239 = shl nuw nsw i64 %238, 3
  %240 = getelementptr i8, ptr %stack_ptr, i64 %229
  %241 = getelementptr i8, ptr %240, i64 -128
  %242 = getelementptr i8, ptr %241, i64 %239
  %243 = load i64, ptr %242, align 8
  store i64 %243, ptr %126, align 8
  %244 = load i64, ptr %20, align 8
  %245 = getelementptr i8, ptr %235, i64 3
  %246 = load i8, ptr %245, align 1
  %247 = zext i8 %246 to i64
  %248 = shl nuw nsw i64 %247, 3
  %249 = getelementptr i8, ptr %stack_ptr, i64 %244
  %250 = getelementptr i8, ptr %249, i64 -128
  %251 = getelementptr i8, ptr %250, i64 %248
  %252 = load i64, ptr %251, align 8
  store i64 %252, ptr %159, align 8
  %253 = getelementptr i8, ptr %235, i64 5
  %254 = load i64, ptr %253, align 8
  store i64 %254, ptr %162, align 8
  %255 = getelementptr i8, ptr %stack_ptr, i64 %254
  %256 = getelementptr i8, ptr %255, i64 %252
  %257 = load i64, ptr %256, align 8
  store i64 %257, ptr %159, align 8
  %258 = add i64 %257, %243
  store i64 %258, ptr %126, align 8
  %259 = load i64, ptr %20, align 8
  %260 = getelementptr i8, ptr %235, i64 17
  %261 = load i8, ptr %260, align 1
  %262 = zext i8 %261 to i64
  %263 = shl nuw nsw i64 %262, 3
  %264 = getelementptr i8, ptr %stack_ptr, i64 %259
  %265 = getelementptr i8, ptr %264, i64 -128
  %266 = getelementptr i8, ptr %265, i64 %263
  store i64 %258, ptr %266, align 8
  %267 = getelementptr i8, ptr %235, i64 19
  %268 = load i8, ptr %267, align 1
  %269 = zext i8 %268 to i64
  %270 = shl nuw nsw i64 %269, 3
  %271 = getelementptr i8, ptr %265, i64 %270
  %272 = load i64, ptr %271, align 8
  store i64 %272, ptr %126, align 8
  %273 = load i64, ptr %20, align 8
  %274 = getelementptr i8, ptr %235, i64 21
  %275 = load i8, ptr %274, align 1
  %276 = zext i8 %275 to i64
  %277 = shl nuw nsw i64 %276, 3
  %278 = getelementptr i8, ptr %stack_ptr, i64 %273
  %279 = getelementptr i8, ptr %278, i64 -128
  %280 = getelementptr i8, ptr %279, i64 %277
  %281 = load i64, ptr %280, align 8
  store i64 %281, ptr %159, align 8
  %282 = getelementptr i8, ptr %235, i64 23
  %283 = load i64, ptr %282, align 8
  store i64 %283, ptr %162, align 8
  %284 = add i64 %283, %281
  store i64 %284, ptr %159, align 8
  %285 = load i64, ptr %20, align 8
  %286 = getelementptr i8, ptr %235, i64 33
  %287 = load i8, ptr %286, align 1
  %288 = zext i8 %287 to i64
  %289 = shl nuw nsw i64 %288, 3
  %290 = getelementptr i8, ptr %stack_ptr, i64 %285
  %291 = getelementptr i8, ptr %290, i64 -128
  %292 = getelementptr i8, ptr %291, i64 %289
  %293 = load i64, ptr %292, align 8
  %294 = getelementptr inbounds i8, ptr %stack_ptr, i64 %284
  store i64 %293, ptr %294, align 8
  %295 = getelementptr i8, ptr %235, i64 35
  %296 = load i8, ptr %295, align 1
  %297 = zext i8 %296 to i64
  %298 = shl nuw nsw i64 %297, 3
  %299 = getelementptr i8, ptr %291, i64 %298
  %300 = load i64, ptr %299, align 8
  store i64 %300, ptr %159, align 8
  %301 = getelementptr i8, ptr %235, i64 37
  %302 = load i64, ptr %301, align 8
  store i64 %302, ptr %162, align 8
  %303 = add i64 %302, %300
  store i64 %303, ptr %159, align 8
  %304 = getelementptr inbounds i8, ptr %stack_ptr, i64 %303
  %305 = load i32, ptr %304, align 4
  %306 = getelementptr i8, ptr %125, i64 -12
  store i32 %305, ptr %306, align 4
  %307 = load i64, ptr %20, align 8
  %308 = getelementptr i8, ptr %235, i64 48
  %309 = load i8, ptr %308, align 1
  %310 = zext i8 %309 to i64
  %311 = shl nuw nsw i64 %310, 3
  %312 = getelementptr i8, ptr %stack_ptr, i64 %307
  %313 = getelementptr i8, ptr %312, i64 -128
  %314 = getelementptr i8, ptr %313, i64 %311
  store i32 %305, ptr %314, align 4
  %315 = getelementptr i8, ptr %235, i64 50
  %316 = load i8, ptr %315, align 1
  %317 = zext i8 %316 to i64
  %318 = shl nuw nsw i64 %317, 3
  %319 = getelementptr i8, ptr %313, i64 %318
  %320 = load i64, ptr %319, align 8
  %321 = trunc i64 %320 to i32
  %322 = add i64 %112, -12
  %323 = getelementptr inbounds i8, ptr %memory, i64 %322
  store i32 %321, ptr %323, align 4
  %324 = getelementptr i8, ptr %235, i64 52
  %325 = load i32, ptr %324, align 4
  store i32 %325, ptr %159, align 4
  %326 = add i32 %325, %321
  store i32 %326, ptr %323, align 4
  %327 = load i64, ptr %20, align 8
  %328 = getelementptr i8, ptr %235, i64 58
  %329 = load i8, ptr %328, align 1
  %330 = zext i8 %329 to i64
  %331 = shl nuw nsw i64 %330, 3
  %332 = getelementptr i8, ptr %stack_ptr, i64 %327
  %333 = getelementptr i8, ptr %332, i64 -128
  %334 = getelementptr i8, ptr %333, i64 %331
  store i32 %326, ptr %334, align 4
  %335 = getelementptr i8, ptr %235, i64 60
  %336 = load i8, ptr %335, align 1
  %337 = zext i8 %336 to i64
  %338 = shl nuw nsw i64 %337, 3
  %339 = getelementptr i8, ptr %333, i64 %338
  %340 = load i64, ptr %339, align 8
  %341 = trunc i64 %340 to i32
  store i32 %341, ptr %323, align 4
  %342 = load i64, ptr %20, align 8
  %343 = getelementptr i8, ptr %235, i64 62
  %344 = load i8, ptr %343, align 1
  %345 = zext i8 %344 to i64
  %346 = shl nuw nsw i64 %345, 3
  %347 = getelementptr i8, ptr %stack_ptr, i64 %342
  %348 = getelementptr i8, ptr %347, i64 -128
  %349 = getelementptr i8, ptr %348, i64 %346
  %350 = load i64, ptr %349, align 8
  %351 = getelementptr i8, ptr %125, i64 -20
  store i64 %350, ptr %351, align 8
  %352 = getelementptr i8, ptr %235, i64 64
  %353 = load i64, ptr %352, align 8
  %354 = getelementptr i8, ptr %125, i64 -28
  store i64 %353, ptr %354, align 8
  %355 = add i64 %353, %350
  store i64 %355, ptr %351, align 8
  %356 = load i64, ptr %20, align 8
  %357 = getelementptr i8, ptr %235, i64 74
  %358 = load i8, ptr %357, align 1
  %359 = zext i8 %358 to i64
  %360 = shl nuw nsw i64 %359, 3
  %361 = getelementptr i8, ptr %stack_ptr, i64 %356
  %362 = getelementptr i8, ptr %361, i64 -128
  %363 = getelementptr i8, ptr %362, i64 %360
  %364 = load i64, ptr %363, align 8
  %365 = trunc i64 %364 to i32
  %366 = getelementptr inbounds i8, ptr %stack_ptr, i64 %355
  store i32 %365, ptr %366, align 4
  %367 = getelementptr i8, ptr %235, i64 76
  %368 = load i16, ptr %367, align 2
  %369 = zext i16 %368 to i64
  %370 = add nuw nsw i64 %369, 57344
  %371 = load i64, ptr %68, align 8
  %372 = load i64, ptr %60, align 8
  %373 = load i64, ptr %59, align 8
  %374 = load i64, ptr %56, align 8
  br label %ADD_GPRv_IMMb_64_378
}
