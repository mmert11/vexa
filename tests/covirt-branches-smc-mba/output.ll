define ptr @vexa_lifted(ptr noalias %state, i64 %program_counter, ptr noalias %memory) {
entry:
  %RDI266653 = getelementptr inbounds nuw i8, ptr %state, i64 2296
  %0 = load i64, ptr %RDI266653, align 8
  %1 = getelementptr inbounds nuw i8, ptr %memory, i64 1138712
  store i64 8553, ptr %1, align 8
  %2 = getelementptr inbounds nuw i8, ptr %memory, i64 1136640
  store i64 -528, ptr %2, align 8
  %3 = getelementptr inbounds nuw i8, ptr %memory, i64 1138720
  store i64 53248, ptr %3, align 8
  %4 = getelementptr inbounds nuw i8, ptr %memory, i64 55065
  store i32 146899785, ptr %4, align 4
  %5 = getelementptr inbounds nuw i8, ptr %memory, i64 1138728
  store i64 151760, ptr %5, align 8
  %6 = getelementptr inbounds nuw i8, ptr %memory, i64 56881
  store i32 146899785, ptr %6, align 4
  %7 = getelementptr inbounds nuw i8, ptr %memory, i64 1138736
  store i64 163026, ptr %7, align 8
  %8 = getelementptr inbounds nuw i8, ptr %memory, i64 58733
  store i32 146899785, ptr %8, align 4
  %9 = getelementptr inbounds nuw i8, ptr %memory, i64 1138744
  store i64 205933, ptr %9, align 8
  %10 = getelementptr inbounds nuw i8, ptr %memory, i64 60617
  store i32 146899785, ptr %10, align 4
  %11 = getelementptr inbounds nuw i8, ptr %memory, i64 1138752
  store i64 242994, ptr %11, align 8
  %12 = getelementptr inbounds nuw i8, ptr %memory, i64 62373
  store i32 146899785, ptr %12, align 4
  %13 = getelementptr inbounds nuw i8, ptr %memory, i64 1138760
  store i64 283997, ptr %13, align 8
  %14 = getelementptr inbounds nuw i8, ptr %memory, i64 64223
  store i32 146899785, ptr %14, align 4
  %15 = getelementptr inbounds nuw i8, ptr %memory, i64 1138768
  store i64 319717, ptr %15, align 8
  %16 = getelementptr inbounds nuw i8, ptr %memory, i64 66001
  store i32 146899785, ptr %16, align 4
  %17 = getelementptr inbounds nuw i8, ptr %memory, i64 1138776
  store i64 354459, ptr %17, align 8
  %18 = getelementptr inbounds nuw i8, ptr %memory, i64 67875
  store i32 146899785, ptr %18, align 4
  %19 = getelementptr inbounds nuw i8, ptr %memory, i64 1138784
  store i64 413450, ptr %19, align 8
  %20 = getelementptr inbounds nuw i8, ptr %memory, i64 69717
  store i32 146899785, ptr %20, align 4
  %21 = getelementptr inbounds nuw i8, ptr %memory, i64 1138792
  store i64 457525, ptr %21, align 8
  %22 = getelementptr inbounds nuw i8, ptr %memory, i64 71567
  store i32 146899785, ptr %22, align 4
  %23 = getelementptr inbounds nuw i8, ptr %memory, i64 1138800
  store i64 516712, ptr %23, align 8
  %24 = getelementptr inbounds nuw i8, ptr %memory, i64 73415
  store i32 146899785, ptr %24, align 4
  %25 = getelementptr inbounds nuw i8, ptr %memory, i64 1138808
  store i64 570535, ptr %25, align 8
  %26 = getelementptr inbounds nuw i8, ptr %memory, i64 75269
  store i32 146899785, ptr %26, align 4
  %27 = getelementptr inbounds nuw i8, ptr %memory, i64 1138816
  store i64 619432, ptr %27, align 8
  %28 = getelementptr inbounds nuw i8, ptr %memory, i64 77129
  store i32 146899785, ptr %28, align 4
  %29 = getelementptr inbounds nuw i8, ptr %memory, i64 1138824
  store i64 679262, ptr %29, align 8
  %30 = getelementptr inbounds nuw i8, ptr %memory, i64 78925
  store i32 146899785, ptr %30, align 4
  %31 = getelementptr inbounds nuw i8, ptr %memory, i64 1138832
  store i64 697457, ptr %31, align 8
  %32 = getelementptr inbounds nuw i8, ptr %memory, i64 80765
  store i32 146899785, ptr %32, align 4
  %33 = getelementptr inbounds nuw i8, ptr %memory, i64 1138840
  store i64 723200, ptr %33, align 8
  %34 = getelementptr inbounds nuw i8, ptr %memory, i64 82537
  store i32 146899785, ptr %34, align 4
  %35 = getelementptr inbounds nuw i8, ptr %memory, i64 1138848
  store i64 748611, ptr %35, align 8
  %36 = getelementptr inbounds nuw i8, ptr %memory, i64 84305
  store i32 146899785, ptr %36, align 4
  %37 = getelementptr inbounds nuw i8, ptr %memory, i64 1138856
  store i64 774098, ptr %37, align 8
  %38 = getelementptr inbounds nuw i8, ptr %memory, i64 86123
  store i32 146899785, ptr %38, align 4
  %39 = getelementptr inbounds nuw i8, ptr %memory, i64 1138864
  store i64 799621, ptr %39, align 8
  %40 = getelementptr inbounds nuw i8, ptr %memory, i64 87887
  store i32 146899785, ptr %40, align 4
  %41 = getelementptr inbounds nuw i8, ptr %memory, i64 1138872
  store i64 828869, ptr %41, align 8
  %42 = getelementptr inbounds nuw i8, ptr %memory, i64 89721
  store i32 146899785, ptr %42, align 4
  %43 = getelementptr inbounds nuw i8, ptr %memory, i64 1138880
  store i64 857888, ptr %43, align 8
  %44 = getelementptr inbounds nuw i8, ptr %memory, i64 91639
  store i32 146899785, ptr %44, align 4
  %45 = getelementptr inbounds nuw i8, ptr %memory, i64 1138888
  store i64 887651, ptr %45, align 8
  %46 = getelementptr inbounds nuw i8, ptr %memory, i64 93471
  store i32 146899785, ptr %46, align 4
  %47 = getelementptr inbounds nuw i8, ptr %memory, i64 1138896
  store i64 920931, ptr %47, align 8
  %48 = getelementptr inbounds nuw i8, ptr %memory, i64 95309
  store i32 146899785, ptr %48, align 4
  %49 = getelementptr inbounds nuw i8, ptr %memory, i64 1138904
  store i64 950776, ptr %49, align 8
  %50 = getelementptr inbounds nuw i8, ptr %memory, i64 97135
  store i32 146899785, ptr %50, align 4
  %51 = getelementptr inbounds nuw i8, ptr %memory, i64 1138912
  store i64 984331, ptr %51, align 8
  %52 = getelementptr inbounds nuw i8, ptr %memory, i64 98941
  store i32 146899785, ptr %52, align 4
  %53 = getelementptr inbounds nuw i8, ptr %memory, i64 1138920
  store i64 1044170, ptr %53, align 8
  %54 = getelementptr inbounds nuw i8, ptr %memory, i64 100737
  store i32 146899785, ptr %54, align 4
  %55 = getelementptr inbounds nuw i8, ptr %memory, i64 1138928
  store i64 1065650, ptr %55, align 8
  %56 = getelementptr inbounds nuw i8, ptr %memory, i64 102613
  store i32 146899785, ptr %56, align 4
  %57 = getelementptr inbounds nuw i8, ptr %memory, i64 104617
  store i16 22337, ptr %57, align 2
  %58 = getelementptr inbounds nuw i8, ptr %memory, i64 106585
  store i16 22081, ptr %58, align 2
  %59 = getelementptr inbounds nuw i8, ptr %memory, i64 108616
  store i16 21825, ptr %59, align 2
  %60 = getelementptr inbounds nuw i8, ptr %memory, i64 110605
  store i16 21569, ptr %60, align 2
  %61 = getelementptr inbounds nuw i8, ptr %memory, i64 112578
  store i16 21313, ptr %61, align 2
  %62 = getelementptr inbounds nuw i8, ptr %memory, i64 114457
  store i16 21057, ptr %62, align 2
  %63 = getelementptr inbounds nuw i8, ptr %memory, i64 116336
  store i16 20801, ptr %63, align 2
  %64 = getelementptr inbounds nuw i8, ptr %memory, i64 118307
  store i16 20545, ptr %64, align 2
  %65 = getelementptr inbounds nuw i8, ptr %memory, i64 119933
  store i8 87, ptr %65, align 1
  %66 = getelementptr inbounds nuw i8, ptr %memory, i64 121484
  store i8 86, ptr %66, align 1
  %67 = getelementptr inbounds nuw i8, ptr %memory, i64 122985
  store i8 85, ptr %67, align 1
  %68 = getelementptr inbounds nuw i8, ptr %memory, i64 124494
  store i8 83, ptr %68, align 1
  %69 = getelementptr inbounds nuw i8, ptr %memory, i64 126009
  store i8 82, ptr %69, align 1
  %70 = getelementptr inbounds nuw i8, ptr %memory, i64 127570
  store i8 81, ptr %70, align 1
  %71 = getelementptr inbounds nuw i8, ptr %memory, i64 129037
  store i8 80, ptr %71, align 1
  %72 = getelementptr inbounds nuw i8, ptr %memory, i64 130570
  store i8 -100, ptr %72, align 1
  %73 = getelementptr inbounds nuw i8, ptr %memory, i64 145975
  store i32 146149192, ptr %73, align 4
  %74 = getelementptr inbounds nuw i8, ptr %memory, i64 151756
  store i32 -920322239, ptr %74, align 4
  %75 = getelementptr inbounds nuw i8, ptr %memory, i64 164898
  %76 = getelementptr inbounds nuw i8, ptr %memory, i64 166703
  %77 = getelementptr inbounds nuw i8, ptr %memory, i64 175799
  store i64 175831, ptr %77, align 8
  %78 = getelementptr inbounds nuw i8, ptr %memory, i64 168512
  store i32 146899785, ptr %78, align 4
  %79 = getelementptr inbounds nuw i8, ptr %memory, i64 175807
  store i64 183440, ptr %79, align 8
  %80 = getelementptr inbounds nuw i8, ptr %memory, i64 170356
  store i32 146899785, ptr %80, align 4
  %81 = getelementptr inbounds nuw i8, ptr %memory, i64 175815
  store i64 187221, ptr %81, align 8
  %82 = getelementptr inbounds nuw i8, ptr %memory, i64 172140
  store i32 146899785, ptr %82, align 4
  %83 = getelementptr inbounds nuw i8, ptr %memory, i64 175823
  store i64 194686, ptr %83, align 8
  %84 = getelementptr inbounds nuw i8, ptr %memory, i64 173964
  store i32 146899785, ptr %84, align 4
  %85 = getelementptr inbounds nuw i8, ptr %memory, i64 175795
  %86 = getelementptr inbounds nuw i8, ptr %memory, i64 196488
  %87 = getelementptr inbounds nuw i8, ptr %memory, i64 1138704
  %88 = getelementptr inbounds nuw i8, ptr %memory, i64 198308
  %89 = getelementptr inbounds nuw i8, ptr %memory, i64 200178
  %90 = getelementptr inbounds nuw i8, ptr %memory, i64 205929
  %91 = getelementptr inbounds nuw i8, ptr %memory, i64 244798
  store i32 146149192, ptr %91, align 4
  %92 = getelementptr inbounds nuw i8, ptr %memory, i64 246585
  store i32 29393736, ptr %92, align 4
  %93 = getelementptr inbounds nuw i8, ptr %memory, i64 248621
  store i32 280366924, ptr %93, align 4
  %94 = getelementptr inbounds nuw i8, ptr %memory, i64 250413
  store i32 -787182261, ptr %94, align 4
  %95 = getelementptr inbounds nuw i8, ptr %memory, i64 252317
  store i32 29393736, ptr %95, align 4
  %96 = getelementptr inbounds nuw i8, ptr %memory, i64 261475
  store i64 261507, ptr %96, align 8
  %97 = getelementptr inbounds nuw i8, ptr %memory, i64 254124
  store i32 146899785, ptr %97, align 4
  %98 = getelementptr inbounds nuw i8, ptr %memory, i64 261483
  store i64 267254, ptr %98, align 8
  %99 = getelementptr inbounds nuw i8, ptr %memory, i64 255976
  store i32 146899785, ptr %99, align 4
  %100 = getelementptr inbounds nuw i8, ptr %memory, i64 261491
  store i64 269049, ptr %100, align 8
  %101 = getelementptr inbounds nuw i8, ptr %memory, i64 257852
  store i32 146899785, ptr %101, align 4
  %102 = getelementptr inbounds nuw i8, ptr %memory, i64 261499
  store i64 274756, ptr %102, align 8
  %103 = getelementptr inbounds nuw i8, ptr %memory, i64 259670
  store i32 146899785, ptr %103, align 4
  %104 = getelementptr inbounds nuw i8, ptr %memory, i64 261471
  store i32 -920322239, ptr %104, align 4
  %105 = getelementptr inbounds nuw i8, ptr %memory, i64 276563
  store i32 147227464, ptr %105, align 4
  %106 = getelementptr inbounds nuw i8, ptr %memory, i64 278344
  store i32 146149192, ptr %106, align 4
  %107 = getelementptr inbounds nuw i8, ptr %memory, i64 283993
  store i32 -920322239, ptr %107, align 4
  %108 = getelementptr inbounds nuw i8, ptr %memory, i64 207729
  %109 = getelementptr inbounds nuw i8, ptr %memory, i64 209520
  %110 = getelementptr inbounds nuw i8, ptr %memory, i64 211534
  %111 = getelementptr inbounds nuw i8, ptr %memory, i64 213364
  %112 = getelementptr inbounds nuw i8, ptr %memory, i64 215240
  %113 = getelementptr inbounds nuw i8, ptr %memory, i64 224396
  store i64 224428, ptr %113, align 8
  %114 = getelementptr inbounds nuw i8, ptr %memory, i64 217031
  store i32 146899785, ptr %114, align 4
  %115 = getelementptr inbounds nuw i8, ptr %memory, i64 224404
  store i64 228218, ptr %115, align 8
  %116 = getelementptr inbounds nuw i8, ptr %memory, i64 218827
  store i32 146899785, ptr %116, align 4
  %117 = getelementptr inbounds nuw i8, ptr %memory, i64 224412
  store i64 230040, ptr %117, align 8
  %118 = getelementptr inbounds nuw i8, ptr %memory, i64 220655
  store i32 146899785, ptr %118, align 4
  %119 = getelementptr inbounds nuw i8, ptr %memory, i64 224420
  store i64 233766, ptr %119, align 8
  %120 = getelementptr inbounds nuw i8, ptr %memory, i64 222555
  store i32 146899785, ptr %120, align 4
  %121 = getelementptr inbounds nuw i8, ptr %memory, i64 224392
  %122 = getelementptr inbounds nuw i8, ptr %memory, i64 235574
  %123 = getelementptr inbounds nuw i8, ptr %memory, i64 237335
  %124 = getelementptr inbounds nuw i8, ptr %memory, i64 242990
  %125 = getelementptr inbounds nuw i8, ptr %memory, i64 1138696
  %126 = getelementptr inbounds nuw i8, ptr %memory, i64 356303
  %127 = getelementptr inbounds nuw i8, ptr %memory, i64 358098
  %128 = getelementptr inbounds nuw i8, ptr %memory, i64 367276
  store i64 367308, ptr %128, align 8
  %129 = getelementptr inbounds nuw i8, ptr %memory, i64 359985
  store i32 146899785, ptr %129, align 4
  %130 = getelementptr inbounds nuw i8, ptr %memory, i64 367284
  store i64 377061, ptr %130, align 8
  %131 = getelementptr inbounds nuw i8, ptr %memory, i64 361795
  store i32 146899785, ptr %131, align 4
  %132 = getelementptr inbounds nuw i8, ptr %memory, i64 367292
  store i64 387382, ptr %132, align 8
  %133 = getelementptr inbounds nuw i8, ptr %memory, i64 363637
  store i32 146899785, ptr %133, align 4
  %134 = getelementptr inbounds nuw i8, ptr %memory, i64 367300
  store i64 396949, ptr %134, align 8
  %135 = getelementptr inbounds nuw i8, ptr %memory, i64 365467
  store i32 146899785, ptr %135, align 4
  %136 = getelementptr inbounds nuw i8, ptr %memory, i64 367272
  %137 = getelementptr inbounds nuw i8, ptr %memory, i64 398768
  %138 = getelementptr inbounds nuw i8, ptr %memory, i64 407761
  %139 = getelementptr inbounds nuw i8, ptr %memory, i64 413446
  %140 = getelementptr inbounds nuw i8, ptr %memory, i64 321471
  store i32 146149192, ptr %140, align 4
  %141 = getelementptr inbounds nuw i8, ptr %memory, i64 323306
  store i32 29393736, ptr %141, align 4
  %142 = getelementptr inbounds nuw i8, ptr %memory, i64 325298
  store i32 280366924, ptr %142, align 4
  %143 = getelementptr inbounds nuw i8, ptr %memory, i64 327088
  store i32 -787182773, ptr %143, align 4
  %144 = getelementptr inbounds nuw i8, ptr %memory, i64 328916
  store i32 29393736, ptr %144, align 4
  %145 = getelementptr inbounds nuw i8, ptr %memory, i64 337880
  store i64 337912, ptr %145, align 8
  %146 = getelementptr inbounds nuw i8, ptr %memory, i64 330707
  store i32 146899785, ptr %146, align 4
  %147 = getelementptr inbounds nuw i8, ptr %memory, i64 337888
  store i64 339687, ptr %147, align 8
  %148 = getelementptr inbounds nuw i8, ptr %memory, i64 332463
  store i32 146899785, ptr %148, align 4
  %149 = getelementptr inbounds nuw i8, ptr %memory, i64 337896
  store i64 343311, ptr %149, align 8
  %150 = getelementptr inbounds nuw i8, ptr %memory, i64 334245
  store i32 146899785, ptr %150, align 4
  %151 = getelementptr inbounds nuw i8, ptr %memory, i64 337904
  store i64 345084, ptr %151, align 8
  %152 = getelementptr inbounds nuw i8, ptr %memory, i64 336033
  store i32 146899785, ptr %152, align 4
  %153 = getelementptr inbounds nuw i8, ptr %memory, i64 337876
  store i32 -920322239, ptr %153, align 4
  %154 = getelementptr inbounds nuw i8, ptr %memory, i64 346914
  store i32 147227464, ptr %154, align 4
  %155 = getelementptr inbounds nuw i8, ptr %memory, i64 348720
  store i32 146149192, ptr %155, align 4
  %156 = getelementptr inbounds nuw i8, ptr %memory, i64 354455
  store i32 -920322239, ptr %156, align 4
  store i32 146149192, ptr %108, align 4
  store i32 29393736, ptr %109, align 4
  store i32 280366924, ptr %110, align 4
  store i32 -787182773, ptr %111, align 4
  store i32 29393736, ptr %112, align 4
  store i32 -920322239, ptr %121, align 4
  store i32 149848904, ptr %122, align 4
  store i32 146149192, ptr %123, align 4
  store i32 -920322239, ptr %124, align 4
  store i32 146149192, ptr %126, align 4
  store i32 29393736, ptr %127, align 4
  store i32 -920322239, ptr %136, align 4
  store i32 147227464, ptr %137, align 4
  store i32 146149192, ptr %138, align 4
  store i32 -920322239, ptr %139, align 4
  %157 = getelementptr inbounds nuw i8, ptr %memory, i64 285807
  store i32 146149192, ptr %157, align 4
  %158 = getelementptr inbounds nuw i8, ptr %memory, i64 287712
  store i32 29393736, ptr %158, align 4
  %159 = getelementptr inbounds nuw i8, ptr %memory, i64 297022
  store i64 297054, ptr %159, align 8
  %160 = getelementptr inbounds nuw i8, ptr %memory, i64 289559
  store i32 146899785, ptr %160, align 4
  %161 = getelementptr inbounds nuw i8, ptr %memory, i64 297030
  store i64 302891, ptr %161, align 8
  %162 = getelementptr inbounds nuw i8, ptr %memory, i64 291445
  store i32 146899785, ptr %162, align 4
  %163 = getelementptr inbounds nuw i8, ptr %memory, i64 297038
  store i64 304659, ptr %163, align 8
  %164 = getelementptr inbounds nuw i8, ptr %memory, i64 293311
  store i32 146899785, ptr %164, align 4
  %165 = getelementptr inbounds nuw i8, ptr %memory, i64 297046
  store i64 310441, ptr %165, align 8
  %166 = getelementptr inbounds nuw i8, ptr %memory, i64 295163
  store i32 146899785, ptr %166, align 4
  %167 = getelementptr inbounds nuw i8, ptr %memory, i64 297018
  store i32 -920322239, ptr %167, align 4
  %168 = getelementptr inbounds nuw i8, ptr %memory, i64 312246
  store i32 13009736, ptr %168, align 4
  store i64 %0, ptr %87, align 8
  %169 = getelementptr inbounds nuw i8, ptr %memory, i64 314006
  store i32 146149192, ptr %169, align 4
  %170 = getelementptr inbounds nuw i8, ptr %memory, i64 319713
  store i32 -920322239, ptr %170, align 4
  store i32 146149192, ptr %75, align 4
  store i32 29393736, ptr %76, align 4
  store i32 -920322239, ptr %85, align 4
  store i32 149848904, ptr %86, align 4
  store i64 1293, ptr %125, align 8
  store i32 146834248, ptr %88, align 4
  store i32 146149192, ptr %89, align 4
  store i32 -920322239, ptr %90, align 4
  %171 = getelementptr inbounds nuw i8, ptr %memory, i64 621244
  store i32 146149192, ptr %171, align 4
  %172 = getelementptr inbounds nuw i8, ptr %memory, i64 622993
  store i32 29393736, ptr %172, align 4
  %173 = getelementptr inbounds nuw i8, ptr %memory, i64 632197
  store i64 632229, ptr %173, align 8
  %174 = getelementptr inbounds nuw i8, ptr %memory, i64 624900
  store i32 146899785, ptr %174, align 4
  %175 = getelementptr inbounds nuw i8, ptr %memory, i64 632205
  store i64 645039, ptr %175, align 8
  %176 = getelementptr inbounds nuw i8, ptr %memory, i64 626740
  store i32 146899785, ptr %176, align 4
  %177 = getelementptr inbounds nuw i8, ptr %memory, i64 632213
  store i64 651986, ptr %177, align 8
  %178 = getelementptr inbounds nuw i8, ptr %memory, i64 628578
  store i32 146899785, ptr %178, align 4
  %179 = getelementptr inbounds nuw i8, ptr %memory, i64 632221
  store i64 664791, ptr %179, align 8
  %180 = getelementptr inbounds nuw i8, ptr %memory, i64 630436
  store i32 146899785, ptr %180, align 4
  %181 = getelementptr inbounds nuw i8, ptr %memory, i64 632193
  store i32 -920322239, ptr %181, align 4
  %182 = getelementptr inbounds nuw i8, ptr %memory, i64 666558
  store i32 147227464, ptr %182, align 4
  %183 = icmp eq i64 %0, 1293
  %184 = getelementptr inbounds nuw i8, ptr %memory, i64 668110
  store i8 -100, ptr %184, align 1
  %185 = getelementptr inbounds nuw i8, ptr %memory, i64 669910
  store i32 147227464, ptr %185, align 4
  %186 = getelementptr inbounds nuw i8, ptr %memory, i64 671754
  store i32 49185608, ptr %186, align 4
  %187 = getelementptr inbounds nuw i8, ptr %memory, i64 673543
  store i32 146149192, ptr %187, align 4
  %188 = getelementptr inbounds nuw i8, ptr %memory, i64 679258
  store i32 -920322239, ptr %188, align 4
  %189 = getelementptr inbounds nuw i8, ptr %memory, i64 724968
  store i32 29393736, ptr %189, align 4
  %190 = getelementptr inbounds nuw i8, ptr %memory, i64 726788
  store i32 381095752, ptr %190, align 4
  %191 = getelementptr inbounds nuw i8, ptr %memory, i64 728604
  store i32 1088586568, ptr %191, align 4
  %192 = getelementptr inbounds nuw i8, ptr %memory, i64 741158
  %193 = getelementptr inbounds nuw i8, ptr %memory, i64 742886
  %194 = getelementptr inbounds nuw i8, ptr %memory, i64 748607
  br i1 %183, label %PUSHFQ_169629, label %PUSHFQ_94237

PUSHFQ_94237:                                     ; preds = %entry
  %195 = getelementptr inbounds nuw i8, ptr %memory, i64 732170
  store i32 146214728, ptr %195, align 4
  store i32 46564168, ptr %192, align 4
  store i32 146149192, ptr %193, align 4
  store i32 -920322239, ptr %194, align 4
  store i32 146149192, ptr %157, align 4
  store i32 29393736, ptr %158, align 4
  store i32 -920322239, ptr %167, align 4
  store i32 13009736, ptr %168, align 4
  store i32 146149192, ptr %169, align 4
  store i32 -920322239, ptr %170, align 4
  %196 = getelementptr inbounds nuw i8, ptr %memory, i64 415228
  store i32 146149192, ptr %196, align 4
  %197 = getelementptr inbounds nuw i8, ptr %memory, i64 417007
  store i32 29393736, ptr %197, align 4
  %198 = getelementptr inbounds nuw i8, ptr %memory, i64 426183
  store i64 426215, ptr %198, align 8
  %199 = getelementptr inbounds nuw i8, ptr %memory, i64 418800
  store i32 146899785, ptr %199, align 4
  %200 = getelementptr inbounds nuw i8, ptr %memory, i64 426191
  store i64 436046, ptr %200, align 8
  %201 = getelementptr inbounds nuw i8, ptr %memory, i64 420616
  store i32 146899785, ptr %201, align 4
  %202 = getelementptr inbounds nuw i8, ptr %memory, i64 426199
  store i64 438043, ptr %202, align 8
  %203 = getelementptr inbounds nuw i8, ptr %memory, i64 422434
  store i32 146899785, ptr %203, align 4
  %204 = getelementptr inbounds nuw i8, ptr %memory, i64 426207
  store i64 447886, ptr %204, align 8
  %205 = getelementptr inbounds nuw i8, ptr %memory, i64 424310
  store i32 146899785, ptr %205, align 4
  %206 = getelementptr inbounds nuw i8, ptr %memory, i64 426179
  store i32 -920322239, ptr %206, align 4
  %207 = getelementptr inbounds nuw i8, ptr %memory, i64 449701
  store i32 147227464, ptr %207, align 4
  %208 = tail call i64 @llvm.fshl.i64(i64 %0, i64 %0, i64 60)
  %209 = xor i64 %208, 3458764513820540926
  %210 = tail call i64 @llvm.fshl.i64(i64 %0, i64 %0, i64 39)
  %211 = and i64 %210, -15942918602753
  %212 = tail call i64 @llvm.fshl.i64(i64 %209, i64 %209, i64 4)
  %213 = tail call i64 @llvm.fshl.i64(i64 %211, i64 %211, i64 25)
  %214 = add i64 %213, %212
  %215 = tail call i64 @llvm.fshl.i64(i64 %0, i64 %0, i64 28)
  %216 = and i64 %215, -7784628225
  %217 = tail call i64 @llvm.fshl.i64(i64 %216, i64 %216, i64 36)
  %218 = add i64 %214, %217
  %219 = getelementptr inbounds nuw i8, ptr %memory, i64 451764
  store i32 146149192, ptr %219, align 4
  %220 = getelementptr inbounds nuw i8, ptr %memory, i64 457521
  store i32 -920322239, ptr %220, align 4
  store i32 146149192, ptr %91, align 4
  store i32 29393736, ptr %92, align 4
  store i32 280366924, ptr %93, align 4
  store i32 -787182261, ptr %94, align 4
  store i32 29393736, ptr %95, align 4
  store i32 -920322239, ptr %104, align 4
  store i32 147227464, ptr %105, align 4
  store i32 146149192, ptr %106, align 4
  store i32 -920322239, ptr %107, align 4
  store i64 %218, ptr %87, align 8
  store i32 146149192, ptr %108, align 4
  store i32 29393736, ptr %109, align 4
  store i32 280366924, ptr %110, align 4
  store i32 -787182773, ptr %111, align 4
  store i32 29393736, ptr %112, align 4
  store i32 -920322239, ptr %121, align 4
  store i32 149848904, ptr %122, align 4
  store i32 146149192, ptr %123, align 4
  store i32 -920322239, ptr %124, align 4
  store i32 146149192, ptr %75, align 4
  store i32 29393736, ptr %76, align 4
  store i32 -920322239, ptr %85, align 4
  store i32 149848904, ptr %86, align 4
  %221 = getelementptr inbounds nuw i8, ptr %memory, i64 1138688
  store i64 -16, ptr %221, align 8
  store i32 146834248, ptr %88, align 4
  store i32 146149192, ptr %89, align 4
  store i32 -920322239, ptr %90, align 4
  store i32 146149192, ptr %126, align 4
  store i32 29393736, ptr %127, align 4
  store i32 -920322239, ptr %136, align 4
  store i32 147227464, ptr %137, align 4
  store i64 -32, ptr %125, align 8
  store i32 146149192, ptr %138, align 4
  store i32 -920322239, ptr %139, align 4
  store i32 146149192, ptr %140, align 4
  store i32 29393736, ptr %141, align 4
  store i32 280366924, ptr %142, align 4
  store i32 -787182773, ptr %143, align 4
  store i32 29393736, ptr %144, align 4
  store i32 -920322239, ptr %153, align 4
  store i32 147227464, ptr %154, align 4
  store i32 146149192, ptr %155, align 4
  store i32 -920322239, ptr %156, align 4
  br label %common.ret

PUSHFQ_169629:                                    ; preds = %entry
  %222 = getelementptr inbounds nuw i8, ptr %memory, i64 730389
  store i32 46170952, ptr %222, align 4
  store i32 46564168, ptr %192, align 4
  store i32 146149192, ptr %193, align 4
  store i32 -920322239, ptr %194, align 4
  store i32 146149192, ptr %157, align 4
  store i32 29393736, ptr %158, align 4
  store i32 -920322239, ptr %167, align 4
  store i32 13009736, ptr %168, align 4
  store i32 146149192, ptr %169, align 4
  store i32 -920322239, ptr %170, align 4
  %223 = getelementptr inbounds nuw i8, ptr %memory, i64 1067468
  store i32 29393736, ptr %223, align 4
  %224 = getelementptr inbounds nuw i8, ptr %memory, i64 1069200
  store i32 146149192, ptr %224, align 4
  %225 = getelementptr inbounds nuw i8, ptr %memory, i64 1071019
  store i32 29393736, ptr %225, align 4
  %226 = getelementptr inbounds nuw i8, ptr %memory, i64 1079999
  %227 = getelementptr inbounds nuw i8, ptr %memory, i64 1072780
  %228 = getelementptr inbounds nuw i8, ptr %memory, i64 1074580
  %229 = getelementptr inbounds nuw i8, ptr %memory, i64 1076454
  store i32 29524808, ptr %227, align 4
  store i32 29393736, ptr %228, align 4
  store i32 32080712, ptr %229, align 4
  %230 = getelementptr inbounds nuw i8, ptr %memory, i64 1078253
  store i32 149848904, ptr %230, align 4
  %231 = getelementptr inbounds nuw i8, ptr %memory, i64 1136648
  store i64 2040, ptr %231, align 8
  %232 = getelementptr inbounds nuw i8, ptr %memory, i64 1079967
  store i8 -99, ptr %232, align 1
  %233 = getelementptr inbounds nuw i8, ptr %memory, i64 1082025
  store i16 22337, ptr %233, align 2
  %234 = getelementptr inbounds nuw i8, ptr %memory, i64 1083940
  store i16 22081, ptr %234, align 2
  %235 = getelementptr inbounds nuw i8, ptr %memory, i64 1085884
  store i16 21825, ptr %235, align 2
  %236 = getelementptr inbounds nuw i8, ptr %memory, i64 1087875
  store i16 21569, ptr %236, align 2
  %237 = getelementptr inbounds nuw i8, ptr %memory, i64 1089776
  store i16 21313, ptr %237, align 2
  %238 = getelementptr inbounds nuw i8, ptr %memory, i64 1091727
  store i16 21057, ptr %238, align 2
  %239 = getelementptr inbounds nuw i8, ptr %memory, i64 1093696
  store i16 20801, ptr %239, align 2
  %240 = getelementptr inbounds nuw i8, ptr %memory, i64 1095634
  store i16 20545, ptr %240, align 2
  %241 = getelementptr inbounds nuw i8, ptr %memory, i64 1097146
  store i8 87, ptr %241, align 1
  %242 = getelementptr inbounds nuw i8, ptr %memory, i64 1098647
  store i8 86, ptr %242, align 1
  %243 = getelementptr inbounds nuw i8, ptr %memory, i64 1100208
  store i8 85, ptr %243, align 1
  %244 = getelementptr inbounds nuw i8, ptr %memory, i64 1101727
  store i8 83, ptr %244, align 1
  %245 = getelementptr inbounds nuw i8, ptr %memory, i64 1103178
  store i8 82, ptr %245, align 1
  %246 = getelementptr inbounds nuw i8, ptr %memory, i64 1104741
  store i8 81, ptr %246, align 1
  %247 = getelementptr inbounds nuw i8, ptr %memory, i64 1106210
  store i8 80, ptr %247, align 1
  %248 = getelementptr inbounds nuw i8, ptr %memory, i64 1107707
  store i8 -100, ptr %248, align 1
  %249 = getelementptr inbounds nuw i8, ptr %memory, i64 1116009
  store i32 147227464, ptr %249, align 4
  %250 = getelementptr inbounds nuw i8, ptr %memory, i64 1124969
  tail call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(16) %226, i8 -112, i64 16, i1 false)
  store i32 146149192, ptr %250, align 4
  %251 = getelementptr inbounds nuw i8, ptr %memory, i64 1130660
  store i32 -920322239, ptr %251, align 4
  store i32 146149192, ptr %91, align 4
  store i32 29393736, ptr %92, align 4
  store i32 280366924, ptr %93, align 4
  store i32 -787182261, ptr %94, align 4
  store i32 29393736, ptr %95, align 4
  store i32 -920322239, ptr %104, align 4
  store i32 147227464, ptr %105, align 4
  store i32 146149192, ptr %106, align 4
  store i32 -920322239, ptr %107, align 4
  store i64 2601, ptr %87, align 8
  store i32 146149192, ptr %108, align 4
  store i32 29393736, ptr %109, align 4
  store i32 280366924, ptr %110, align 4
  store i32 -787182773, ptr %111, align 4
  store i32 29393736, ptr %112, align 4
  store i32 -920322239, ptr %121, align 4
  store i32 149848904, ptr %122, align 4
  store i32 146149192, ptr %123, align 4
  store i32 -920322239, ptr %124, align 4
  store i32 146149192, ptr %75, align 4
  store i32 29393736, ptr %76, align 4
  store i32 -920322239, ptr %85, align 4
  store i32 149848904, ptr %86, align 4
  %252 = getelementptr inbounds nuw i8, ptr %memory, i64 1138688
  store i64 -16, ptr %252, align 8
  store i32 146834248, ptr %88, align 4
  store i32 146149192, ptr %89, align 4
  store i32 -920322239, ptr %90, align 4
  store i32 146149192, ptr %126, align 4
  store i32 29393736, ptr %127, align 4
  store i32 -920322239, ptr %136, align 4
  store i32 147227464, ptr %137, align 4
  store i64 -32, ptr %125, align 8
  store i32 146149192, ptr %138, align 4
  store i32 -920322239, ptr %139, align 4
  store i32 146149192, ptr %140, align 4
  store i32 29393736, ptr %141, align 4
  store i32 280366924, ptr %142, align 4
  store i32 -787182773, ptr %143, align 4
  store i32 29393736, ptr %144, align 4
  store i32 -920322239, ptr %153, align 4
  store i32 147227464, ptr %154, align 4
  store i32 146149192, ptr %155, align 4
  store i32 -920322239, ptr %156, align 4
  %253 = getelementptr inbounds nuw i8, ptr %memory, i64 681074
  store i32 29393736, ptr %253, align 4
  %254 = getelementptr inbounds nuw i8, ptr %memory, i64 682820
  store i32 146214728, ptr %254, align 4
  %255 = getelementptr inbounds nuw i8, ptr %memory, i64 691730
  store i32 146149192, ptr %255, align 4
  %256 = getelementptr inbounds nuw i8, ptr %memory, i64 697453
  store i32 -920322239, ptr %256, align 4
  br label %common.ret

common.ret:                                       ; preds = %PUSHFQ_169629, %PUSHFQ_94237
  %257 = phi i64 [ 2601, %PUSHFQ_169629 ], [ %218, %PUSHFQ_94237 ]
  %258 = getelementptr inbounds nuw i8, ptr %memory, i64 153510
  store i32 29393736, ptr %258, align 4
  %259 = getelementptr inbounds nuw i8, ptr %memory, i64 155328
  store i32 277562470, ptr %259, align 4
  store i16 8615, ptr %1, align 2
  %260 = getelementptr inbounds nuw i8, ptr %memory, i64 162988
  store i8 -99, ptr %260, align 1
  %RAX266648 = getelementptr inbounds nuw i8, ptr %state, i64 2216
  store i64 %257, ptr %RAX266648, align 8
  ret ptr %state
}
