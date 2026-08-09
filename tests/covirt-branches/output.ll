define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %0 = getelementptr inbounds nuw i8, ptr %state, i64 2080
  %1 = load i32, ptr %0, align 4
  %RAX1249 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  %2 = load i64, ptr %RAX1249, align 8
  %RBX1250 = getelementptr inbounds nuw i8, ptr %state, i64 2232
  %3 = load i64, ptr %RBX1250, align 8
  %RCX1251 = getelementptr inbounds nuw i8, ptr %state, i64 2248
  %4 = load i64, ptr %RCX1251, align 8
  %RDX1252 = getelementptr inbounds nuw i8, ptr %state, i64 2264
  %5 = load i64, ptr %RDX1252, align 8
  %RSI1253 = getelementptr inbounds nuw i8, ptr %state, i64 2280
  %6 = load i64, ptr %RSI1253, align 8
  %RDI1254 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %7 = load i64, ptr %RDI1254, align 8
  %RBP1256 = getelementptr inbounds nuw i8, ptr %state, i64 2328
  %8 = load i64, ptr %RBP1256, align 8
  %R81258 = getelementptr inbounds nuw i8, ptr %state, i64 2344
  %9 = load i64, ptr %R81258, align 8
  %R121262 = getelementptr inbounds nuw i8, ptr %state, i64 2408
  %10 = load i64, ptr %R121262, align 8
  %R131263 = getelementptr inbounds nuw i8, ptr %state, i64 2424
  %11 = load i64, ptr %R131263, align 8
  %R141264 = getelementptr inbounds nuw i8, ptr %state, i64 2440
  %12 = load i64, ptr %R141264, align 8
  %R151265 = getelementptr inbounds nuw i8, ptr %state, i64 2456
  %13 = load i64, ptr %R151265, align 8
  %DF1308 = getelementptr inbounds nuw i8, ptr %state, i64 2075
  %14 = load i8, ptr %DF1308, align 1
  %stack8422 = alloca [8192 x i8], align 1
  %stack_ptr = getelementptr inbounds nuw i8, ptr %stack8422, i64 4096
  %15 = getelementptr inbounds nuw i8, ptr %stack8422, i64 4088
  store i64 0, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %stack8422, i64 4080
  store i64 %8, ptr %16, align 8
  %17 = getelementptr inbounds nuw i8, ptr %stack8422, i64 4072
  store i64 %7, ptr %17, align 8
  %18 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3560
  %19 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3552
  %20 = getelementptr inbounds nuw i8, ptr %memory, i64 61464
  store i64 8553, ptr %20, align 8
  %21 = getelementptr inbounds nuw i8, ptr %memory, i64 59392
  store i64 -528, ptr %21, align 8
  %22 = getelementptr inbounds nuw i8, ptr %memory, i64 61472
  store i64 53248, ptr %22, align 8
  %23 = getelementptr inbounds nuw i8, ptr %memory, i64 61480
  store i64 53737, ptr %23, align 8
  %24 = getelementptr inbounds nuw i8, ptr %memory, i64 61488
  store i64 53791, ptr %24, align 8
  %25 = getelementptr inbounds nuw i8, ptr %memory, i64 61496
  store i64 53996, ptr %25, align 8
  %26 = getelementptr inbounds nuw i8, ptr %memory, i64 61504
  store i64 54201, ptr %26, align 8
  %27 = getelementptr inbounds nuw i8, ptr %memory, i64 61512
  store i64 54416, ptr %27, align 8
  %28 = getelementptr inbounds nuw i8, ptr %memory, i64 61520
  store i64 54617, ptr %28, align 8
  %29 = getelementptr inbounds nuw i8, ptr %memory, i64 61528
  store i64 54837, ptr %29, align 8
  %30 = getelementptr inbounds nuw i8, ptr %memory, i64 61536
  store i64 55046, ptr %30, align 8
  %31 = getelementptr inbounds nuw i8, ptr %memory, i64 61544
  store i64 55255, ptr %31, align 8
  %32 = getelementptr inbounds nuw i8, ptr %memory, i64 61552
  store i64 55464, ptr %32, align 8
  %33 = getelementptr inbounds nuw i8, ptr %memory, i64 61560
  store i64 55673, ptr %33, align 8
  %34 = getelementptr inbounds nuw i8, ptr %memory, i64 61568
  store i64 55882, ptr %34, align 8
  %35 = getelementptr inbounds nuw i8, ptr %memory, i64 61576
  store i64 56133, ptr %35, align 8
  %36 = getelementptr inbounds nuw i8, ptr %memory, i64 61584
  store i64 56169, ptr %36, align 8
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 61592
  store i64 56228, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 61600
  store i64 56287, ptr %38, align 8
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 61608
  store i64 56346, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 61616
  store i64 56405, ptr %40, align 8
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 61624
  store i64 56477, ptr %41, align 8
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 61632
  store i64 56551, ptr %42, align 8
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 61640
  store i64 56619, ptr %43, align 8
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 61648
  store i64 56700, ptr %44, align 8
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 61656
  store i64 56768, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 61664
  store i64 56851, ptr %46, align 8
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 61672
  store i64 57035, ptr %47, align 8
  %48 = getelementptr inbounds nuw i8, ptr %memory, i64 61680
  store i64 57077, ptr %48, align 8
  store i64 %13, ptr %18, align 8
  store i64 %12, ptr %19, align 8
  %49 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3544
  store i64 %11, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3536
  store i64 %10, ptr %50, align 8
  %51 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3528
  store i64 0, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3520
  store i64 57077, ptr %52, align 8
  %53 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3512
  store i64 61688, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3504
  store i64 %9, ptr %54, align 8
  %55 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3496
  store i64 %7, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3488
  store i64 %6, ptr %56, align 8
  %57 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3480
  store i64 -16, ptr %57, align 8
  %58 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3472
  store i64 -528, ptr %58, align 8
  %59 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3464
  store i64 %3, ptr %59, align 8
  %60 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3456
  store i64 %5, ptr %60, align 8
  %61 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3448
  store i64 %4, ptr %61, align 8
  %62 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3440
  store i64 %2, ptr %62, align 8
  %63 = and i32 %1, -3286
  %64 = and i8 %14, 1
  %65 = zext nneg i8 %64 to i32
  %66 = shl nuw nsw i32 %65, 10
  %67 = or disjoint i32 %66, %63
  %.0.load.ext = zext i32 %67 to i64
  %68 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3432
  store i64 %.0.load.ext, ptr %68, align 8
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 54117
  store i64 54149, ptr %69, align 8
  %70 = getelementptr inbounds nuw i8, ptr %memory, i64 54125
  store i64 54157, ptr %70, align 8
  %71 = getelementptr inbounds nuw i8, ptr %memory, i64 54133
  store i64 54166, ptr %71, align 8
  %72 = getelementptr inbounds nuw i8, ptr %memory, i64 54141
  store i64 54174, ptr %72, align 8
  %73 = getelementptr inbounds nuw i8, ptr %memory, i64 61456
  %74 = getelementptr inbounds nuw i8, ptr %memory, i64 53886
  store i64 53918, ptr %74, align 8
  %75 = getelementptr inbounds nuw i8, ptr %memory, i64 53894
  store i64 53932, ptr %75, align 8
  %76 = getelementptr inbounds nuw i8, ptr %memory, i64 53902
  store i64 53948, ptr %76, align 8
  %77 = getelementptr inbounds nuw i8, ptr %memory, i64 53910
  store i64 53962, ptr %77, align 8
  %78 = getelementptr inbounds nuw i8, ptr %memory, i64 61448
  %79 = getelementptr inbounds nuw i8, ptr %memory, i64 54932
  store i64 54964, ptr %79, align 8
  %80 = getelementptr inbounds nuw i8, ptr %memory, i64 54940
  store i64 54978, ptr %80, align 8
  %81 = getelementptr inbounds nuw i8, ptr %memory, i64 54948
  store i64 54996, ptr %81, align 8
  %82 = getelementptr inbounds nuw i8, ptr %memory, i64 54956
  store i64 55010, ptr %82, align 8
  %83 = getelementptr inbounds nuw i8, ptr %memory, i64 54511
  store i64 54543, ptr %83, align 8
  %84 = getelementptr inbounds nuw i8, ptr %memory, i64 54519
  store i64 54556, ptr %84, align 8
  %85 = getelementptr inbounds nuw i8, ptr %memory, i64 54527
  store i64 54571, ptr %85, align 8
  %86 = getelementptr inbounds nuw i8, ptr %memory, i64 54535
  store i64 54584, ptr %86, align 8
  store i64 %7, ptr %73, align 8
  store i64 1293, ptr %78, align 8
  %87 = getelementptr inbounds nuw i8, ptr %memory, i64 55977
  store i64 56009, ptr %87, align 8
  %88 = getelementptr inbounds nuw i8, ptr %memory, i64 55985
  store i64 56034, ptr %88, align 8
  %89 = getelementptr inbounds nuw i8, ptr %memory, i64 55993
  store i64 56062, ptr %89, align 8
  %90 = getelementptr inbounds nuw i8, ptr %memory, i64 56001
  store i64 56087, ptr %90, align 8
  %91 = add i64 %7, -1293
  %92 = icmp ult i64 %7, 1293
  %93 = trunc i64 %91 to i8
  %94 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %93)
  %95 = xor i64 %91, %7
  %96 = trunc i64 %95 to i32
  %97 = and i32 %96, 16
  %98 = icmp eq i64 %7, 1293
  %99 = lshr i64 %7, 63
  %100 = lshr i64 %95, 63
  %101 = add nuw nsw i64 %100, %99
  %102 = icmp eq i64 %101, 2
  %103 = shl nuw nsw i8 %94, 2
  %104 = and i8 %103, 4
  %105 = zext i1 %92 to i8
  %106 = or disjoint i8 %104, %105
  %107 = xor i8 %106, 4
  %108 = zext nneg i8 %107 to i32
  %sh.diff = lshr i64 %91, 56
  %tr.sh.diff = trunc nuw nsw i64 %sh.diff to i32
  %109 = and i32 %tr.sh.diff, 128
  %110 = select i1 %98, i32 64, i32 0
  %111 = select i1 %102, i32 2048, i32 0
  %112 = or disjoint i32 %110, %109
  %113 = or disjoint i32 %112, %97
  %114 = or disjoint i32 %113, %111
  %115 = or disjoint i32 %114, %108
  %116 = or disjoint i32 %115, %63
  %117 = or disjoint i32 %66, %116
  %.0.load.ext8408 = zext i32 %117 to i64
  %118 = getelementptr inbounds nuw i8, ptr %stack8422, i64 3424
  store i64 %.0.load.ext8408, ptr %118, align 8
  %RCX1235.sroa.0.sroa.0.0.extract.trunc5650 = trunc i32 %116 to i16
  %119 = trunc i32 %117 to i16
  %RCX1235.sroa.0.sroa.442.0.insert.shift5666 = and i16 %119, -256
  %RCX1235.sroa.0.sroa.0.0.insert.ext5229 = and i16 %RCX1235.sroa.0.sroa.0.0.extract.trunc5650, 255
  %RCX1235.sroa.0.sroa.0.0.insert.insert5231 = or disjoint i16 %RCX1235.sroa.0.sroa.442.0.insert.shift5666, %RCX1235.sroa.0.sroa.0.0.insert.ext5229
  %120 = getelementptr inbounds nuw i8, ptr %memory, i64 61462
  store i16 %RCX1235.sroa.0.sroa.0.0.insert.insert5231, ptr %120, align 2
  br i1 %98, label %ADD_GPRv_IMMb_64_1058, label %MOVZX_GPR64_MEMw_268

MOVZX_GPR64_MEMw_268:                             ; preds = %entry
  %121 = getelementptr inbounds nuw i8, ptr %memory, i64 57368
  %122 = load i16, ptr %121, align 2
  %RCX1235.sroa.0.0.insert.ext4356 = zext i16 %122 to i64
  %123 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext4356, 57344
  %RAX1233.sroa.312.0.extract.shift8200 = and i64 %123, 130816
  %124 = and i16 %122, 255
  %RAX1233.sroa.0.0.insert.ext7115 = zext nneg i16 %124 to i64
  %125 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift8200
  %126 = getelementptr inbounds nuw i8, ptr %125, i64 %RAX1233.sroa.0.0.insert.ext7115
  %127 = getelementptr inbounds nuw i8, ptr %126, i64 1
  %128 = load i8, ptr %127, align 1
  %R101243.sroa.0.0.insert.ext2761 = zext i8 %128 to i64
  %129 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2761, 3
  %130 = getelementptr i8, ptr %stack_ptr, i64 %129
  %131 = getelementptr i8, ptr %130, i64 -656
  %132 = load i64, ptr %131, align 8
  store i64 %132, ptr %73, align 8
  %133 = getelementptr inbounds nuw i8, ptr %126, i64 3
  %134 = load i64, ptr %133, align 8
  store i64 %134, ptr %78, align 8
  %135 = getelementptr i8, ptr %stack_ptr, i64 %134
  %136 = getelementptr i8, ptr %135, i64 %132
  %137 = load i64, ptr %136, align 8
  store i64 %137, ptr %73, align 8
  %138 = getelementptr inbounds nuw i8, ptr %126, i64 14
  %139 = load i64, ptr %138, align 8
  store i64 %139, ptr %78, align 8
  %140 = sub i64 %137, %139
  %141 = icmp ult i64 %137, %139
  %142 = trunc i64 %140 to i8
  %143 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %142)
  %144 = xor i64 %140, %137
  %145 = xor i64 %144, %139
  %146 = trunc i64 %145 to i32
  %147 = and i32 %146, 16
  %.not = icmp eq i64 %137, %139
  %148 = xor i64 %139, %137
  %149 = lshr i64 %148, 63
  %150 = lshr i64 %144, 63
  %151 = add nuw nsw i64 %150, %149
  %152 = icmp eq i64 %151, 2
  %153 = and i32 %116, -3286
  %154 = shl nuw nsw i8 %143, 2
  %155 = and i8 %154, 4
  %156 = zext i1 %141 to i8
  %157 = or disjoint i8 %155, %156
  %158 = xor i8 %157, 4
  %159 = zext nneg i8 %158 to i32
  %sh.diff8433 = lshr i64 %140, 56
  %tr.sh.diff8434 = trunc nuw nsw i64 %sh.diff8433 to i32
  %160 = and i32 %tr.sh.diff8434, 128
  %161 = select i1 %.not, i32 64, i32 0
  %162 = select i1 %152, i32 2048, i32 0
  %163 = or disjoint i32 %161, %160
  %164 = or disjoint i32 %163, %147
  %165 = or disjoint i32 %164, %159
  %166 = or disjoint i32 %165, %153
  %167 = or disjoint i32 %162, %66
  %168 = or disjoint i32 %167, %166
  %.0.load.ext8411 = zext i32 %168 to i64
  store i64 %.0.load.ext8411, ptr %118, align 8
  %RCX1235.sroa.0.sroa.0.0.extract.trunc5573 = trunc i32 %166 to i16
  %169 = trunc i32 %168 to i16
  %RCX1235.sroa.0.sroa.442.0.insert.shift5662 = and i16 %169, -256
  %RCX1235.sroa.0.sroa.0.0.insert.ext5226 = and i16 %RCX1235.sroa.0.sroa.0.0.extract.trunc5573, 255
  %RCX1235.sroa.0.sroa.0.0.insert.insert5228 = or disjoint i16 %RCX1235.sroa.0.sroa.442.0.insert.shift5662, %RCX1235.sroa.0.sroa.0.0.insert.ext5226
  store i16 %RCX1235.sroa.0.sroa.0.0.insert.insert5228, ptr %120, align 2
  br i1 %.not, label %ADD_GPRv_IMMb_64_875, label %MOVZX_GPR64_MEMw_388

MOVZX_GPR64_MEMw_388:                             ; preds = %MOVZX_GPR64_MEMw_268
  %170 = getelementptr inbounds nuw i8, ptr %126, i64 24
  %171 = load i16, ptr %170, align 2
  %RCX1235.sroa.0.0.insert.ext4255 = zext i16 %171 to i64
  %172 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext4255, 57344
  %RAX1233.sroa.312.0.extract.shift8036 = and i64 %172, 130816
  %173 = and i16 %171, 255
  %RAX1233.sroa.0.0.insert.ext6991 = zext nneg i16 %173 to i64
  %174 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift8036
  %175 = getelementptr inbounds nuw i8, ptr %174, i64 %RAX1233.sroa.0.0.insert.ext6991
  %176 = getelementptr inbounds nuw i8, ptr %175, i64 1
  %177 = load i8, ptr %176, align 1
  %R101243.sroa.0.0.insert.ext2756 = zext i8 %177 to i64
  %178 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2756, 3
  %179 = getelementptr i8, ptr %stack_ptr, i64 %178
  %180 = getelementptr i8, ptr %179, i64 -656
  %181 = load i64, ptr %180, align 8
  store i64 %181, ptr %73, align 8
  %182 = getelementptr inbounds nuw i8, ptr %175, i64 3
  %183 = load i64, ptr %182, align 8
  store i64 %183, ptr %78, align 8
  %184 = getelementptr i8, ptr %stack_ptr, i64 %183
  %185 = getelementptr i8, ptr %184, i64 %181
  %186 = load i64, ptr %185, align 8
  store i64 %186, ptr %73, align 8
  %187 = getelementptr inbounds nuw i8, ptr %175, i64 14
  %188 = load i64, ptr %187, align 8
  store i64 %188, ptr %78, align 8
  %189 = sub i64 %186, %188
  %190 = icmp ult i64 %186, %188
  %191 = trunc i64 %189 to i8
  %192 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %191)
  %193 = xor i64 %189, %186
  %194 = xor i64 %193, %188
  %195 = trunc i64 %194 to i32
  %196 = and i32 %195, 16
  %197 = icmp eq i64 %186, %188
  %198 = xor i64 %188, %186
  %199 = lshr i64 %198, 63
  %200 = lshr i64 %193, 63
  %201 = add nuw nsw i64 %200, %199
  %202 = icmp eq i64 %201, 2
  %203 = shl nuw nsw i8 %192, 2
  %204 = and i8 %203, 4
  %205 = zext i1 %190 to i8
  %206 = or disjoint i8 %204, %205
  %207 = xor i8 %206, 4
  %208 = zext nneg i8 %207 to i32
  %sh.diff8447 = lshr i64 %189, 56
  %tr.sh.diff8448 = trunc nuw nsw i64 %sh.diff8447 to i32
  %209 = and i32 %tr.sh.diff8448, 128
  %210 = select i1 %197, i32 64, i32 0
  %211 = select i1 %202, i32 2048, i32 0
  %212 = or disjoint i32 %210, %209
  %213 = or disjoint i32 %212, %196
  %214 = or disjoint i32 %213, %211
  %215 = or disjoint i32 %214, %208
  %216 = or disjoint i32 %215, %153
  %217 = or i32 %216, %66
  %.0.load.ext8414 = zext i32 %217 to i64
  store i64 %.0.load.ext8414, ptr %118, align 8
  %RCX1235.sroa.0.sroa.0.0.extract.trunc5492 = trunc i32 %216 to i16
  %218 = trunc i32 %217 to i16
  %RCX1235.sroa.0.sroa.442.0.insert.shift = and i16 %218, -256
  %RCX1235.sroa.0.sroa.0.0.insert.ext = and i16 %RCX1235.sroa.0.sroa.0.0.extract.trunc5492, 255
  %RCX1235.sroa.0.sroa.0.0.insert.insert = or disjoint i16 %RCX1235.sroa.0.sroa.442.0.insert.shift, %RCX1235.sroa.0.sroa.0.0.insert.ext
  store i16 %RCX1235.sroa.0.sroa.0.0.insert.insert, ptr %120, align 2
  br i1 %197, label %ADD_GPRv_IMMb_64_684, label %MOVZX_GPR64_MEMw_508

MOVZX_GPR64_MEMw_508:                             ; preds = %MOVZX_GPR64_MEMw_388
  %219 = getelementptr inbounds nuw i8, ptr %175, i64 24
  %220 = load i16, ptr %219, align 2
  %RCX1235.sroa.0.0.insert.ext4154 = zext i16 %220 to i64
  %221 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext4154, 57344
  %RAX1233.sroa.312.0.extract.shift7872 = and i64 %221, 130816
  %222 = and i16 %220, 255
  %RAX1233.sroa.0.0.insert.ext6867 = zext nneg i16 %222 to i64
  %223 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7872
  %224 = getelementptr inbounds nuw i8, ptr %223, i64 %RAX1233.sroa.0.0.insert.ext6867
  %225 = getelementptr inbounds nuw i8, ptr %224, i64 1
  %226 = load i8, ptr %225, align 1
  %227 = getelementptr inbounds nuw i8, ptr %memory, i64 61463
  store i8 %226, ptr %227, align 1
  %228 = getelementptr inbounds nuw i8, ptr %224, i64 3
  %229 = load i8, ptr %228, align 1
  %R101243.sroa.0.0.insert.ext2751 = zext i8 %229 to i64
  %230 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2751, 3
  %231 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %231, align 8
  %232 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %232, align 8
  %233 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %233, align 8
  %234 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %234, align 8
  %235 = getelementptr i8, ptr %stack_ptr, i64 %230
  %236 = getelementptr i8, ptr %235, i64 -656
  store i8 %226, ptr %236, align 1
  %237 = getelementptr inbounds nuw i8, ptr %224, i64 5
  %238 = load i8, ptr %237, align 1
  %R101243.sroa.0.0.insert.ext2726 = zext i8 %238 to i64
  %239 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2726, 3
  %240 = getelementptr i8, ptr %stack_ptr, i64 %239
  %241 = getelementptr i8, ptr %240, i64 -656
  %242 = load i64, ptr %241, align 8
  store i64 %242, ptr %73, align 8
  %243 = getelementptr inbounds nuw i8, ptr %224, i64 7
  %244 = load i64, ptr %243, align 8
  store i64 %244, ptr %78, align 8
  %245 = add i64 %244, %242
  store i64 %245, ptr %73, align 8
  %246 = getelementptr inbounds nuw i8, ptr %224, i64 17
  %247 = load i8, ptr %246, align 1
  %R101243.sroa.0.0.insert.ext2721 = zext i8 %247 to i64
  %248 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2721, 3
  %249 = getelementptr i8, ptr %stack_ptr, i64 %248
  %250 = getelementptr i8, ptr %249, i64 -656
  %251 = load i64, ptr %250, align 8
  %RDX1236.sroa.0.0.extract.trunc3419 = trunc i64 %251 to i8
  %252 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %252, align 8
  %253 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %253, align 8
  %254 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %254, align 8
  %255 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %255, align 8
  %256 = getelementptr inbounds i8, ptr %stack_ptr, i64 %245
  store i8 %RDX1236.sroa.0.0.extract.trunc3419, ptr %256, align 1
  %257 = getelementptr inbounds nuw i8, ptr %224, i64 19
  %258 = load i16, ptr %257, align 2
  %RCX1235.sroa.0.0.insert.ext4069 = zext i16 %258 to i64
  %259 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext4069, 57344
  %RAX1233.sroa.312.0.extract.shift7684 = and i64 %259, 130816
  %260 = and i16 %258, 255
  %RAX1233.sroa.0.0.insert.ext6725 = zext nneg i16 %260 to i64
  %261 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7684
  %262 = getelementptr inbounds nuw i8, ptr %261, i64 %RAX1233.sroa.0.0.insert.ext6725
  %263 = getelementptr inbounds nuw i8, ptr %262, i64 1
  %264 = load i16, ptr %263, align 2
  %RCX1235.sroa.0.0.insert.ext4059 = zext i16 %264 to i64
  %265 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext4059, 57344
  %RAX1233.sroa.312.0.extract.shift7658 = and i64 %265, 130816
  %266 = and i16 %264, 255
  %RAX1233.sroa.0.0.insert.ext6706 = zext nneg i16 %266 to i64
  %267 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7658
  %268 = getelementptr inbounds nuw i8, ptr %267, i64 %RAX1233.sroa.0.0.insert.ext6706
  %269 = getelementptr inbounds nuw i8, ptr %268, i64 1
  %270 = load i16, ptr %269, align 2
  %271 = load i16, ptr %20, align 2
  %272 = add i16 %271, %270
  store i16 %272, ptr %20, align 2
  %273 = load i64, ptr %62, align 8
  %274 = load i64, ptr %57, align 8
  %275 = getelementptr i8, ptr %stack_ptr, i64 %274
  %276 = getelementptr i8, ptr %275, i64 -9
  %277 = load i8, ptr %276, align 1
  %278 = and i8 %277, 1
  br label %common.ret

ADD_GPRv_IMMb_64_684:                             ; preds = %MOVZX_GPR64_MEMw_388
  %279 = getelementptr inbounds nuw i8, ptr %175, i64 27
  %280 = load i8, ptr %279, align 1
  %281 = getelementptr inbounds nuw i8, ptr %memory, i64 61463
  store i8 %280, ptr %281, align 1
  %282 = getelementptr inbounds nuw i8, ptr %175, i64 29
  %283 = load i8, ptr %282, align 1
  %R101243.sroa.0.0.insert.ext2690 = zext i8 %283 to i64
  %284 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2690, 3
  %285 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %285, align 8
  %286 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %286, align 8
  %287 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %287, align 8
  %288 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %288, align 8
  %289 = getelementptr i8, ptr %stack_ptr, i64 %284
  %290 = getelementptr i8, ptr %289, i64 -656
  store i8 %280, ptr %290, align 1
  %291 = getelementptr inbounds nuw i8, ptr %175, i64 31
  %292 = load i8, ptr %291, align 1
  %R101243.sroa.0.0.insert.ext2665 = zext i8 %292 to i64
  %293 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2665, 3
  %294 = getelementptr i8, ptr %stack_ptr, i64 %293
  %295 = getelementptr i8, ptr %294, i64 -656
  %296 = load i64, ptr %295, align 8
  store i64 %296, ptr %73, align 8
  %297 = getelementptr inbounds nuw i8, ptr %175, i64 33
  %298 = load i64, ptr %297, align 8
  store i64 %298, ptr %78, align 8
  %299 = add i64 %298, %296
  store i64 %299, ptr %73, align 8
  %300 = getelementptr inbounds nuw i8, ptr %175, i64 43
  %301 = load i8, ptr %300, align 1
  %R101243.sroa.0.0.insert.ext2660 = zext i8 %301 to i64
  %302 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2660, 3
  %303 = getelementptr i8, ptr %stack_ptr, i64 %302
  %304 = getelementptr i8, ptr %303, i64 -656
  %305 = load i64, ptr %304, align 8
  %RDX1236.sroa.0.0.extract.trunc3402 = trunc i64 %305 to i8
  %306 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %306, align 8
  %307 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %307, align 8
  %308 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %308, align 8
  %309 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %309, align 8
  %310 = getelementptr inbounds i8, ptr %stack_ptr, i64 %299
  store i8 %RDX1236.sroa.0.0.extract.trunc3402, ptr %310, align 1
  %311 = getelementptr inbounds nuw i8, ptr %175, i64 45
  %312 = load i16, ptr %311, align 2
  %RCX1235.sroa.0.0.insert.ext3968 = zext i16 %312 to i64
  %313 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext3968, 57344
  %RAX1233.sroa.312.0.extract.shift7444 = and i64 %313, 130816
  %314 = and i16 %312, 255
  %RAX1233.sroa.0.0.insert.ext6545 = zext nneg i16 %314 to i64
  %315 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7444
  %316 = getelementptr inbounds nuw i8, ptr %315, i64 %RAX1233.sroa.0.0.insert.ext6545
  %317 = getelementptr inbounds nuw i8, ptr %316, i64 1
  %318 = load i16, ptr %317, align 2
  %RCX1235.sroa.0.0.insert.ext3958 = zext i16 %318 to i64
  %319 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext3958, 57344
  %RAX1233.sroa.312.0.extract.shift7418 = and i64 %319, 130816
  %320 = and i16 %318, 255
  %RAX1233.sroa.0.0.insert.ext6526 = zext nneg i16 %320 to i64
  %321 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7418
  %322 = getelementptr inbounds nuw i8, ptr %321, i64 %RAX1233.sroa.0.0.insert.ext6526
  %323 = getelementptr inbounds nuw i8, ptr %322, i64 1
  %324 = load i16, ptr %323, align 2
  %RCX1235.sroa.0.0.insert.ext3948 = zext i16 %324 to i64
  %325 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext3948, 57344
  %RAX1233.sroa.312.0.extract.shift7392 = and i64 %325, 130816
  %326 = and i16 %324, 255
  %RAX1233.sroa.0.0.insert.ext6507 = zext nneg i16 %326 to i64
  %327 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7392
  %328 = getelementptr inbounds nuw i8, ptr %327, i64 %RAX1233.sroa.0.0.insert.ext6507
  %329 = getelementptr inbounds nuw i8, ptr %328, i64 1
  %330 = load i16, ptr %329, align 2
  %331 = load i16, ptr %20, align 2
  %332 = add i16 %331, %330
  store i16 %332, ptr %20, align 2
  %333 = load i64, ptr %62, align 8
  %334 = load i64, ptr %57, align 8
  %335 = getelementptr i8, ptr %stack_ptr, i64 %334
  %336 = getelementptr i8, ptr %335, i64 -9
  %337 = load i8, ptr %336, align 1
  %338 = and i8 %337, 1
  br label %common.ret

ADD_GPRv_IMMb_64_875:                             ; preds = %MOVZX_GPR64_MEMw_268
  %339 = getelementptr inbounds nuw i8, ptr %126, i64 27
  %340 = load i8, ptr %339, align 1
  %341 = getelementptr inbounds nuw i8, ptr %memory, i64 61463
  store i8 %340, ptr %341, align 1
  %342 = getelementptr inbounds nuw i8, ptr %126, i64 29
  %343 = load i8, ptr %342, align 1
  %R101243.sroa.0.0.insert.ext2629 = zext i8 %343 to i64
  %344 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2629, 3
  %345 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %345, align 8
  %346 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %346, align 8
  %347 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %347, align 8
  %348 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %348, align 8
  %349 = getelementptr i8, ptr %stack_ptr, i64 %344
  %350 = getelementptr i8, ptr %349, i64 -656
  store i8 %340, ptr %350, align 1
  %351 = getelementptr inbounds nuw i8, ptr %126, i64 31
  %352 = load i8, ptr %351, align 1
  %R101243.sroa.0.0.insert.ext2604 = zext i8 %352 to i64
  %353 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2604, 3
  %354 = getelementptr i8, ptr %stack_ptr, i64 %353
  %355 = getelementptr i8, ptr %354, i64 -656
  %356 = load i64, ptr %355, align 8
  store i64 %356, ptr %73, align 8
  %357 = getelementptr inbounds nuw i8, ptr %126, i64 33
  %358 = load i64, ptr %357, align 8
  store i64 %358, ptr %78, align 8
  %359 = add i64 %358, %356
  store i64 %359, ptr %73, align 8
  %360 = getelementptr inbounds nuw i8, ptr %126, i64 43
  %361 = load i8, ptr %360, align 1
  %R101243.sroa.0.0.insert.ext2599 = zext i8 %361 to i64
  %362 = shl nuw nsw i64 %R101243.sroa.0.0.insert.ext2599, 3
  %363 = getelementptr i8, ptr %stack_ptr, i64 %362
  %364 = getelementptr i8, ptr %363, i64 -656
  %365 = load i64, ptr %364, align 8
  %RDX1236.sroa.0.0.extract.trunc3388 = trunc i64 %365 to i8
  %366 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %366, align 8
  %367 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %367, align 8
  %368 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %368, align 8
  %369 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %369, align 8
  %370 = getelementptr inbounds i8, ptr %stack_ptr, i64 %359
  store i8 %RDX1236.sroa.0.0.extract.trunc3388, ptr %370, align 1
  %371 = getelementptr inbounds nuw i8, ptr %126, i64 45
  %372 = load i16, ptr %371, align 2
  %RCX1235.sroa.0.0.insert.ext3857 = zext i16 %372 to i64
  %373 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext3857, 57344
  %RAX1233.sroa.312.0.extract.shift7178 = and i64 %373, 130816
  %374 = and i16 %372, 255
  %RAX1233.sroa.0.0.insert.ext6346 = zext nneg i16 %374 to i64
  %375 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7178
  %376 = getelementptr inbounds nuw i8, ptr %375, i64 %RAX1233.sroa.0.0.insert.ext6346
  %377 = getelementptr inbounds nuw i8, ptr %376, i64 1
  %378 = load i16, ptr %377, align 2
  %RCX1235.sroa.0.0.insert.ext3847 = zext i16 %378 to i64
  %379 = add nuw nsw i64 %RCX1235.sroa.0.0.insert.ext3847, 57344
  %RAX1233.sroa.312.0.extract.shift7152 = and i64 %379, 130816
  %380 = and i16 %378, 255
  %RAX1233.sroa.0.0.insert.ext6327 = zext nneg i16 %380 to i64
  %381 = getelementptr inbounds nuw i8, ptr %memory, i64 %RAX1233.sroa.312.0.extract.shift7152
  %382 = getelementptr inbounds nuw i8, ptr %381, i64 %RAX1233.sroa.0.0.insert.ext6327
  %383 = getelementptr inbounds nuw i8, ptr %382, i64 1
  %384 = load i16, ptr %383, align 2
  %385 = load i16, ptr %20, align 2
  %386 = add i16 %385, %384
  store i16 %386, ptr %20, align 2
  %387 = load i64, ptr %62, align 8
  %388 = load i64, ptr %57, align 8
  %389 = getelementptr i8, ptr %stack_ptr, i64 %388
  %390 = getelementptr i8, ptr %389, i64 -9
  %391 = load i8, ptr %390, align 1
  %392 = and i8 %391, 1
  br label %common.ret

ADD_GPRv_IMMb_64_1058:                            ; preds = %entry
  %393 = getelementptr inbounds nuw i8, ptr %memory, i64 54322
  store i64 54354, ptr %393, align 8
  %394 = getelementptr inbounds nuw i8, ptr %memory, i64 54330
  store i64 54364, ptr %394, align 8
  %395 = getelementptr inbounds nuw i8, ptr %memory, i64 54338
  store i64 54376, ptr %395, align 8
  %396 = getelementptr inbounds nuw i8, ptr %memory, i64 54346
  store i64 54386, ptr %396, align 8
  store i64 -9, ptr %78, align 8
  store i64 -25, ptr %73, align 8
  %397 = getelementptr inbounds nuw i8, ptr %memory, i64 54738
  store i64 54770, ptr %397, align 8
  %398 = getelementptr inbounds nuw i8, ptr %memory, i64 54746
  store i64 54782, ptr %398, align 8
  %399 = getelementptr inbounds nuw i8, ptr %memory, i64 54754
  store i64 54795, ptr %399, align 8
  %400 = getelementptr inbounds nuw i8, ptr %memory, i64 54762
  store i64 54807, ptr %400, align 8
  store i16 8624, ptr %20, align 2
  br label %common.ret

common.ret:                                       ; preds = %ADD_GPRv_IMMb_64_1058, %ADD_GPRv_IMMb_64_875, %ADD_GPRv_IMMb_64_684, %MOVZX_GPR64_MEMw_508
  %RAX1233.sroa.0.0 = phi i8 [ %278, %MOVZX_GPR64_MEMw_508 ], [ %338, %ADD_GPRv_IMMb_64_684 ], [ %392, %ADD_GPRv_IMMb_64_875 ], [ 1, %ADD_GPRv_IMMb_64_1058 ]
  %RAX1233.sroa.312.sroa.0.0.in.in = phi i64 [ %273, %MOVZX_GPR64_MEMw_508 ], [ %333, %ADD_GPRv_IMMb_64_684 ], [ %387, %ADD_GPRv_IMMb_64_875 ], [ %2, %ADD_GPRv_IMMb_64_1058 ]
  %RAX1233.sroa.312.sroa.0.0.in = and i64 %RAX1233.sroa.312.sroa.0.0.in.in, -256
  %RAX1233.sroa.0.0.insert.ext7133 = zext nneg i8 %RAX1233.sroa.0.0 to i64
  %RAX1233.sroa.0.0.insert.insert7135 = or disjoint i64 %RAX1233.sroa.312.sroa.0.0.in, %RAX1233.sroa.0.0.insert.ext7133
  store i64 %RAX1233.sroa.0.0.insert.insert7135, ptr %RAX1249, align 8
  ret ptr %state
}
