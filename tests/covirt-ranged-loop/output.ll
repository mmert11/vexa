define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RAX1120 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %2 = load i64, ptr %RAX1120, align 8
  %RBX1121 = getelementptr inbounds nuw i8, ptr %state, i64 2232
  %3 = load i64, ptr %RBX1121, align 8
  %RCX1122 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %4 = load i64, ptr %RCX1122, align 8
  %RDX1123 = getelementptr inbounds nuw i8, ptr %state, i64 2264
  %5 = load i64, ptr %RDX1123, align 8
  %RSI1124 = getelementptr inbounds nuw i8, ptr %state, i64 2280
  %6 = load i64, ptr %RSI1124, align 8
  %RDI1125 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %7 = load i64, ptr %RDI1125, align 8
  %RBP1127 = getelementptr inbounds nuw i8, ptr %state, i64 2328
  %8 = load i64, ptr %RBP1127, align 8
  %R81129 = getelementptr inbounds nuw i8, ptr %state, i64 2344
  %9 = load i64, ptr %R81129, align 8
  %R121133 = getelementptr inbounds nuw i8, ptr %state, i64 2408
  %10 = load i64, ptr %R121133, align 8
  %R131134 = getelementptr inbounds nuw i8, ptr %state, i64 2424
  %11 = load i64, ptr %R131134, align 8
  %R151136 = getelementptr inbounds nuw i8, ptr %state, i64 2456
  %12 = load i64, ptr %R151136, align 8
  %DF1179 = getelementptr inbounds nuw i8, ptr %state, i64 2075
  %13 = load i8, ptr %DF1179, align 1
  %stack7095 = alloca [8192 x i8], align 1
  %stack_ptr = getelementptr inbounds nuw i8, ptr %stack7095, i64 4096
  %14 = getelementptr inbounds nuw i8, ptr %stack7095, i64 4088
  store i64 0, ptr %14, align 8
  %15 = getelementptr inbounds nuw i8, ptr %stack7095, i64 4080
  store i64 %8, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %stack7095, i64 4072
  store i64 %7, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3560
  %18 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3552
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
  %48 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3544
  store i64 %11, ptr %48, align 8
  %49 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3536
  store i64 %10, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3528
  store i64 0, ptr %50, align 8
  %51 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3520
  store i64 57077, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3512
  store i64 61688, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3504
  store i64 %9, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3496
  store i64 %7, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3488
  store i64 %6, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3480
  store i64 -16, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3472
  store i64 -528, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3464
  store i64 %3, ptr %58, align 8
  %59 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3456
  store i64 %5, ptr %59, align 8
  %60 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3448
  store i64 %4, ptr %60, align 8
  %61 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3440
  store i64 %2, ptr %61, align 8
  %62 = and i32 %1, -3286
  %63 = and i8 %13, 1
  %64 = zext nneg i8 %63 to i32
  %65 = shl nuw nsw i32 %64, 10
  %66 = or disjoint i32 %65, %62
  %67 = zext i32 %66 to i64
  %68 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3432
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
  %91 = getelementptr inbounds nuw i8, ptr %stack7095, i64 4064
  store i64 0, ptr %91, align 8
  store i64 -20, ptr %82, align 8
  store i64 -36, ptr %73, align 8
  %92 = getelementptr inbounds nuw i8, ptr %stack7095, i64 4060
  store i32 0, ptr %92, align 4
  %93 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  %94 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  %95 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  %96 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  %97 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  %98 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  %99 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  %100 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  %101 = getelementptr inbounds nuw i8, ptr %stack7095, i64 3424
  br label %MOVZX_GPRv_MEMb_64_378

MOVZX_GPRv_MEMb_64_378:                           ; preds = %ADD_GPRv_IMMb_64_536, %entry
  %102 = phi i64 [ -528, %entry ], [ %319, %ADD_GPRv_IMMb_64_536 ]
  %103 = phi i64 [ 61464, %entry ], [ %285, %ADD_GPRv_IMMb_64_536 ]
  %104 = phi i64 [ 57390, %entry ], [ %333, %ADD_GPRv_IMMb_64_536 ]
  %105 = phi i32 [ %66, %entry ], [ %158, %ADD_GPRv_IMMb_64_536 ]
  %106 = getelementptr i8, ptr %memory, i64 %104
  %107 = getelementptr i8, ptr %106, i64 1
  %108 = load i8, ptr %107, align 1
  %109 = zext i8 %108 to i64
  %110 = shl nuw nsw i64 %109, 3
  %111 = getelementptr i8, ptr %stack_ptr, i64 %102
  %112 = getelementptr i8, ptr %111, i64 -128
  %113 = getelementptr i8, ptr %112, i64 %110
  %114 = load i64, ptr %113, align 8
  %115 = getelementptr i8, ptr %memory, i64 %103
  %116 = getelementptr i8, ptr %115, i64 -8
  store i64 %114, ptr %116, align 8
  %117 = getelementptr i8, ptr %106, i64 3
  %118 = load i64, ptr %117, align 8
  %119 = getelementptr i8, ptr %115, i64 -16
  store i64 %118, ptr %119, align 8
  %120 = add i64 %118, %114
  store i64 %120, ptr %116, align 8
  store i64 54543, ptr %93, align 8
  store i64 54556, ptr %94, align 8
  store i64 54571, ptr %95, align 8
  store i64 54584, ptr %96, align 8
  %121 = load i64, ptr %116, align 8
  %122 = getelementptr inbounds i8, ptr %stack_ptr, i64 %121
  %123 = load i32, ptr %122, align 4
  %124 = getelementptr i8, ptr %115, i64 -4
  store i32 %123, ptr %124, align 4
  %125 = getelementptr i8, ptr %106, i64 14
  %126 = load i32, ptr %125, align 4
  store i32 %126, ptr %116, align 4
  store i64 56009, ptr %97, align 8
  store i64 56034, ptr %98, align 8
  store i64 56062, ptr %99, align 8
  store i64 56087, ptr %100, align 8
  %127 = load i32, ptr %116, align 4
  %128 = load i32, ptr %124, align 4
  %129 = sub i32 %128, %127
  %130 = icmp ult i32 %128, %127
  %131 = trunc i32 %129 to i8
  %132 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %131)
  %133 = xor i32 %129, %128
  %134 = xor i32 %133, %127
  %135 = and i32 %134, 16
  %136 = icmp eq i32 %128, %127
  %137 = xor i32 %128, %127
  %138 = lshr i32 %137, 31
  %139 = lshr i32 %133, 31
  %140 = add nuw nsw i32 %139, %138
  %141 = icmp eq i32 %140, 2
  %142 = zext i1 %130 to i32
  %143 = and i32 %105, -3286
  %144 = shl nuw nsw i8 %132, 2
  %145 = and i8 %144, 4
  %146 = xor i8 %145, 4
  %147 = zext nneg i8 %146 to i32
  %148 = lshr i32 %129, 24
  %149 = and i32 %148, 128
  %150 = select i1 %136, i32 64, i32 0
  %151 = select i1 %141, i32 2048, i32 0
  %152 = or disjoint i32 %150, %143
  %153 = or disjoint i32 %152, %142
  %154 = or disjoint i32 %153, %149
  %155 = or disjoint i32 %154, %135
  %156 = or disjoint i32 %155, %147
  %157 = or disjoint i32 %151, %65
  %158 = or i32 %157, %156
  %159 = zext i32 %158 to i64
  store i64 %159, ptr %101, align 8
  %160 = trunc i32 %156 to i16
  %161 = trunc i32 %158 to i16
  %162 = and i16 %161, -256
  %163 = and i16 %160, 255
  %164 = or disjoint i16 %162, %163
  %165 = getelementptr i8, ptr %115, i64 -2
  store i16 %164, ptr %165, align 2
  br i1 %130, label %ADD_GPRv_IMMb_64_536, label %MOVZX_GPR64_MEMw_508

MOVZX_GPR64_MEMw_508:                             ; preds = %MOVZX_GPRv_MEMb_64_378
  %166 = getelementptr i8, ptr %106, i64 20
  %167 = load i16, ptr %166, align 2
  %168 = zext i16 %167 to i64
  %169 = getelementptr inbounds nuw i8, ptr %memory, i64 %168
  %170 = getelementptr inbounds nuw i8, ptr %169, i64 57345
  %171 = load i16, ptr %170, align 2
  %172 = load i16, ptr %19, align 2
  %173 = add i16 %172, %171
  store i16 %173, ptr %19, align 2
  %174 = load i64, ptr %56, align 8
  %175 = getelementptr i8, ptr %stack_ptr, i64 %174
  %176 = getelementptr i8, ptr %175, i64 -16
  %177 = load i64, ptr %176, align 8
  store i64 %177, ptr %RAX1120, align 8
  ret ptr %state

ADD_GPRv_IMMb_64_536:                             ; preds = %MOVZX_GPRv_MEMb_64_378
  %178 = load i64, ptr %20, align 8
  %179 = getelementptr i8, ptr %106, i64 23
  %180 = load i8, ptr %179, align 1
  %181 = zext i8 %180 to i64
  %182 = shl nuw nsw i64 %181, 3
  %183 = getelementptr i8, ptr %stack_ptr, i64 %178
  %184 = getelementptr i8, ptr %183, i64 -128
  %185 = getelementptr i8, ptr %184, i64 %182
  %186 = load i64, ptr %185, align 8
  store i64 %186, ptr %116, align 8
  %187 = getelementptr i8, ptr %106, i64 25
  %188 = load i64, ptr %187, align 8
  store i64 %188, ptr %119, align 8
  %189 = getelementptr i8, ptr %stack_ptr, i64 %188
  %190 = getelementptr i8, ptr %189, i64 %186
  %191 = load i64, ptr %190, align 8
  store i64 %191, ptr %116, align 8
  %192 = load i64, ptr %20, align 8
  %193 = getelementptr i8, ptr %106, i64 36
  %194 = load i8, ptr %193, align 1
  %195 = zext i8 %194 to i64
  %196 = shl nuw nsw i64 %195, 3
  %197 = getelementptr i8, ptr %stack_ptr, i64 %192
  %198 = getelementptr i8, ptr %197, i64 -128
  %199 = getelementptr i8, ptr %198, i64 %196
  store i64 %191, ptr %199, align 8
  %200 = getelementptr i8, ptr %106, i64 38
  %201 = load i8, ptr %200, align 1
  %202 = zext i8 %201 to i64
  %203 = shl nuw nsw i64 %202, 3
  %204 = getelementptr i8, ptr %198, i64 %203
  %205 = load i64, ptr %204, align 8
  store i64 %205, ptr %116, align 8
  %206 = load i64, ptr %20, align 8
  %207 = getelementptr i8, ptr %106, i64 40
  %208 = load i8, ptr %207, align 1
  %209 = zext i8 %208 to i64
  %210 = shl nuw nsw i64 %209, 3
  %211 = getelementptr i8, ptr %stack_ptr, i64 %206
  %212 = getelementptr i8, ptr %211, i64 -128
  %213 = getelementptr i8, ptr %212, i64 %210
  %214 = load i64, ptr %213, align 8
  store i64 %214, ptr %119, align 8
  %215 = getelementptr i8, ptr %106, i64 42
  %216 = load i64, ptr %215, align 8
  %217 = getelementptr i8, ptr %115, i64 -24
  store i64 %216, ptr %217, align 8
  %218 = getelementptr i8, ptr %stack_ptr, i64 %216
  %219 = getelementptr i8, ptr %218, i64 %214
  %220 = load i64, ptr %219, align 8
  store i64 %220, ptr %119, align 8
  %221 = add i64 %220, %205
  store i64 %221, ptr %116, align 8
  %222 = load i64, ptr %20, align 8
  %223 = getelementptr i8, ptr %106, i64 54
  %224 = load i8, ptr %223, align 1
  %225 = zext i8 %224 to i64
  %226 = shl nuw nsw i64 %225, 3
  %227 = getelementptr i8, ptr %stack_ptr, i64 %222
  %228 = getelementptr i8, ptr %227, i64 -128
  %229 = getelementptr i8, ptr %228, i64 %226
  store i64 %221, ptr %229, align 8
  %230 = getelementptr i8, ptr %106, i64 56
  %231 = load i8, ptr %230, align 1
  %232 = zext i8 %231 to i64
  %233 = shl nuw nsw i64 %232, 3
  %234 = getelementptr i8, ptr %228, i64 %233
  %235 = load i64, ptr %234, align 8
  store i64 %235, ptr %116, align 8
  %236 = load i64, ptr %20, align 8
  %237 = getelementptr i8, ptr %106, i64 58
  %238 = load i8, ptr %237, align 1
  %239 = zext i8 %238 to i64
  %240 = shl nuw nsw i64 %239, 3
  %241 = getelementptr i8, ptr %stack_ptr, i64 %236
  %242 = getelementptr i8, ptr %241, i64 -128
  %243 = getelementptr i8, ptr %242, i64 %240
  %244 = load i64, ptr %243, align 8
  store i64 %244, ptr %119, align 8
  %245 = getelementptr i8, ptr %106, i64 60
  %246 = load i64, ptr %245, align 8
  store i64 %246, ptr %217, align 8
  %247 = add i64 %246, %244
  store i64 %247, ptr %119, align 8
  %248 = load i64, ptr %20, align 8
  %249 = getelementptr i8, ptr %106, i64 70
  %250 = load i8, ptr %249, align 1
  %251 = zext i8 %250 to i64
  %252 = shl nuw nsw i64 %251, 3
  %253 = getelementptr i8, ptr %stack_ptr, i64 %248
  %254 = getelementptr i8, ptr %253, i64 -128
  %255 = getelementptr i8, ptr %254, i64 %252
  %256 = load i64, ptr %255, align 8
  %257 = getelementptr inbounds i8, ptr %stack_ptr, i64 %247
  store i64 %256, ptr %257, align 8
  %258 = getelementptr i8, ptr %106, i64 72
  %259 = load i8, ptr %258, align 1
  %260 = zext i8 %259 to i64
  %261 = shl nuw nsw i64 %260, 3
  %262 = getelementptr i8, ptr %254, i64 %261
  %263 = load i64, ptr %262, align 8
  store i64 %263, ptr %119, align 8
  %264 = getelementptr i8, ptr %106, i64 74
  %265 = load i64, ptr %264, align 8
  store i64 %265, ptr %217, align 8
  %266 = add i64 %265, %263
  store i64 %266, ptr %119, align 8
  %267 = getelementptr inbounds i8, ptr %stack_ptr, i64 %266
  %268 = load i32, ptr %267, align 4
  %269 = getelementptr i8, ptr %115, i64 -12
  store i32 %268, ptr %269, align 4
  %270 = load i64, ptr %20, align 8
  %271 = getelementptr i8, ptr %106, i64 85
  %272 = load i8, ptr %271, align 1
  %273 = zext i8 %272 to i64
  %274 = shl nuw nsw i64 %273, 3
  %275 = getelementptr i8, ptr %stack_ptr, i64 %270
  %276 = getelementptr i8, ptr %275, i64 -128
  %277 = getelementptr i8, ptr %276, i64 %274
  store i32 %268, ptr %277, align 4
  %278 = getelementptr i8, ptr %106, i64 87
  %279 = load i8, ptr %278, align 1
  %280 = zext i8 %279 to i64
  %281 = shl nuw nsw i64 %280, 3
  %282 = getelementptr i8, ptr %276, i64 %281
  %283 = load i64, ptr %282, align 8
  %284 = trunc i64 %283 to i32
  %285 = add i64 %103, -12
  %286 = getelementptr inbounds i8, ptr %memory, i64 %285
  store i32 %284, ptr %286, align 4
  %287 = getelementptr i8, ptr %106, i64 89
  %288 = load i32, ptr %287, align 4
  store i32 %288, ptr %119, align 4
  %289 = add i32 %288, %284
  store i32 %289, ptr %286, align 4
  %290 = load i64, ptr %20, align 8
  %291 = getelementptr i8, ptr %106, i64 95
  %292 = load i8, ptr %291, align 1
  %293 = zext i8 %292 to i64
  %294 = shl nuw nsw i64 %293, 3
  %295 = getelementptr i8, ptr %stack_ptr, i64 %290
  %296 = getelementptr i8, ptr %295, i64 -128
  %297 = getelementptr i8, ptr %296, i64 %294
  store i32 %289, ptr %297, align 4
  %298 = getelementptr i8, ptr %106, i64 97
  %299 = load i8, ptr %298, align 1
  %300 = zext i8 %299 to i64
  %301 = shl nuw nsw i64 %300, 3
  %302 = getelementptr i8, ptr %296, i64 %301
  %303 = load i64, ptr %302, align 8
  %304 = trunc i64 %303 to i32
  store i32 %304, ptr %286, align 4
  %305 = load i64, ptr %20, align 8
  %306 = getelementptr i8, ptr %106, i64 99
  %307 = load i8, ptr %306, align 1
  %308 = zext i8 %307 to i64
  %309 = shl nuw nsw i64 %308, 3
  %310 = getelementptr i8, ptr %stack_ptr, i64 %305
  %311 = getelementptr i8, ptr %310, i64 -128
  %312 = getelementptr i8, ptr %311, i64 %309
  %313 = load i64, ptr %312, align 8
  %314 = getelementptr i8, ptr %115, i64 -20
  store i64 %313, ptr %314, align 8
  %315 = getelementptr i8, ptr %106, i64 101
  %316 = load i64, ptr %315, align 8
  %317 = getelementptr i8, ptr %115, i64 -28
  store i64 %316, ptr %317, align 8
  %318 = add i64 %316, %313
  store i64 %318, ptr %314, align 8
  %319 = load i64, ptr %20, align 8
  %320 = getelementptr i8, ptr %106, i64 111
  %321 = load i8, ptr %320, align 1
  %322 = zext i8 %321 to i64
  %323 = shl nuw nsw i64 %322, 3
  %324 = getelementptr i8, ptr %stack_ptr, i64 %319
  %325 = getelementptr i8, ptr %324, i64 -128
  %326 = getelementptr i8, ptr %325, i64 %323
  %327 = load i64, ptr %326, align 8
  %328 = trunc i64 %327 to i32
  %329 = getelementptr inbounds i8, ptr %stack_ptr, i64 %318
  store i32 %328, ptr %329, align 4
  %330 = getelementptr i8, ptr %106, i64 113
  %331 = load i16, ptr %330, align 2
  %332 = zext i16 %331 to i64
  %333 = add nuw nsw i64 %332, 57344
  br label %MOVZX_GPRv_MEMb_64_378
}
