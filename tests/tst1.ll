; ModuleID = 'top'
source_filename = "top"

%struct12 = type { i32, i32 }

@.str0 = private unnamed_addr constant [9 x i8] c"R??: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str3 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str6 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@gg = internal global i32 142, align 8

define internal void @__dummy_func() {
dummy_block:
  ret void
}

declare i32 @printf(i8*, ...)

declare i8* @memcpy(i8*, i8*, i64)

declare i64 @strlen(i8*)

declare i32 @atoi(i8*)

define void @store555(i32* %.arg_0) {
block_0:
  store i32 554, i32* %.arg_0, align 4
  %vr3 = load i32, i32* %.arg_0, align 4
  %vr4 = add i32 %vr3, 1
  store i32 %vr4, i32* %.arg_0, align 4
  ret void
}

define i32 @add(i32 %.arg_0, i32 %.arg_1) {
block_1:
  %vr10 = icmp eq i32 %.arg_0, %.arg_1
  %vr11 = sext i1 %vr10 to i32
  %0 = icmp ne i32 %vr11, 0
  br i1 %0, label %block_2, label %block_3

block_2:                                          ; preds = %block_1
  br label %block_3

block_3:                                          ; preds = %block_2, %block_1
  %vr153 = phi i32 [ %.arg_1, %block_1 ], [ %.arg_0, %block_2 ]
  %vr15 = add i32 %.arg_0, %vr153
  ret i32 %vr15
}

define i32 @mysqr(i32 %.arg_0) {
block_4:
  %vr19 = mul i32 %.arg_0, %.arg_0
  ret i32 %vr19
}

define i32 @main() {
block_5:
  %vr21 = load i32, i32* @gg, align 4
  %vr22 = alloca i32, align 4
  call void @store555(i32* %vr22)
  %vr25 = add i32 %vr21, 1
  %vr27 = icmp eq i32 %vr25, 7
  %vr28 = sext i1 %vr27 to i32
  %0 = icmp ne i32 %vr28, 0
  br i1 %0, label %block_6, label %block_7

block_6:                                          ; preds = %block_5
  %vr29 = load i32, i32* %vr22, align 4
  %vr30 = add i32 %vr29, 3
  store i32 %vr30, i32* %vr22, align 4
  store i32 25, i32* @gg, align 4
  br label %block_8

block_7:                                          ; preds = %block_5
  %vr32 = call i32 @add(i32 %vr25, i32 5)
  br label %block_8

block_8:                                          ; preds = %block_7, %block_6
  %vr155 = phi i32 [ %vr25, %block_6 ], [ %vr32, %block_7 ]
  %vr34 = load i32, i32* %vr22, align 4
  %vr35 = call i32 @add(i32 %vr155, i32 %vr34)
  store i32 %vr35, i32* %vr22, align 4
  %vr37 = alloca [16 x i8], align 1
  %vr38 = bitcast [16 x i8]* %vr37 to i8*
  %vr41 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr42 = add i64 %vr41, 1
  %1 = call i8* @memcpy(i8* %vr38, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr42)
  %vr45 = bitcast [16 x i8]* %vr37 to i8*
  %vr46 = ptrtoint i8* %vr45 to i64
  %vr47 = add i64 %vr46, 2
  %vr48 = inttoptr i64 %vr47 to i8*
  store i8 115, i8* %vr48, align 1
  %vr50 = bitcast [16 x i8]* %vr37 to i8*
  %vr51 = ptrtoint i8* %vr50 to i64
  %vr52 = add i64 %vr51, 1
  %vr53 = inttoptr i64 %vr52 to i8*
  store i8 101, i8* %vr53, align 1
  %vr54 = bitcast [16 x i8]* %vr37 to i8*
  %vr55 = load i32, i32* %vr22, align 4
  %2 = call i32 (i8*, ...) @printf(i8* %vr54, i32 %vr55)
  %vr57 = alloca %struct12, align 8
  %vr58 = load %struct12, %struct12* %vr57, align 4
  %vr59 = insertvalue %struct12 %vr58, i32 111, 0
  store %struct12 %vr59, %struct12* %vr57, align 4
  %vr60 = load %struct12, %struct12* %vr57, align 4
  %vr61 = insertvalue %struct12 %vr60, i32 222, 1
  store %struct12 %vr61, %struct12* %vr57, align 4
  %vr62 = load %struct12, %struct12* %vr57, align 4
  %vr63 = extractvalue %struct12 %vr62, 0
  %vr64 = load %struct12, %struct12* %vr57, align 4
  %vr65 = extractvalue %struct12 %vr64, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr63, i32 %vr65)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 123, i32 -123)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str3, i32 0, i32 0), i8 97)
  br label %block_9

block_9:                                          ; preds = %block_10, %block_8
  %vr161 = phi i32 [ 10, %block_8 ], [ %vr83, %block_10 ]
  %7 = icmp ne i32 %vr161, 0
  br i1 %7, label %block_10, label %block_11

block_10:                                         ; preds = %block_9
  %vr83 = sub i32 %vr161, 1
  br label %block_9

block_11:                                         ; preds = %block_9
  %vr84 = alloca [4 x i32], align 4
  br label %block_12

block_12:                                         ; preds = %block_13, %block_11
  %vr162.1 = phi i32 [ 0, %block_11 ], [ %vr100, %block_13 ]
  %vr87 = icmp slt i32 %vr162.1, 4
  %vr88 = sext i1 %vr87 to i32
  %8 = icmp ne i32 %vr88, 0
  br i1 %8, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr91 = mul i32 %vr162.1, %vr162.1
  %vr92 = bitcast [4 x i32]* %vr84 to i32*
  %vr94 = zext i32 %vr162.1 to i64
  %vr95 = shl i64 %vr94, 2
  %vr96 = ptrtoint i32* %vr92 to i64
  %vr97 = add i64 %vr96, %vr95
  %vr98 = inttoptr i64 %vr97 to i32*
  store i32 %vr91, i32* %vr98, align 4
  %vr100 = add i32 %vr162.1, 1
  br label %block_12

block_14:                                         ; preds = %block_12
  br label %block_15

block_15:                                         ; preds = %block_18, %block_14
  %vr163 = phi i32 [ 0, %block_14 ], [ %vr106, %block_18 ]
  %vr103 = icmp slt i32 %vr163, 4
  %vr104 = sext i1 %vr103 to i32
  %9 = icmp ne i32 %vr104, 0
  br i1 %9, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr107 = bitcast [4 x i32]* %vr84 to i32*
  %vr109 = zext i32 %vr163 to i64
  %vr110 = shl i64 %vr109, 2
  %vr111 = ptrtoint i32* %vr107 to i64
  %vr112 = add i64 %vr111, %vr110
  %vr113 = inttoptr i64 %vr112 to i32*
  %vr114 = load i32, i32* %vr113, align 4
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr114)
  br label %block_18

block_17:                                         ; preds = %block_15
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_19

block_18:                                         ; preds = %block_16
  %vr106 = add i32 %vr163, 1
  br label %block_15

block_19:                                         ; preds = %block_23, %block_17
  %vr162.4 = phi i32 [ 3, %block_17 ], [ %vr118, %block_23 ]
  br i1 true, label %block_20, label %block_21

block_20:                                         ; preds = %block_19
  %vr118 = sub i32 %vr162.4, 1
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr118)
  %vr122 = icmp eq i32 %vr118, 0
  %vr123 = sext i1 %vr122 to i32
  %13 = icmp ne i32 %vr123, 0
  br i1 %13, label %block_22, label %block_23

block_21:                                         ; preds = %block_22, %block_19
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_25

block_22:                                         ; preds = %block_20
  br label %block_21

block_23:                                         ; preds = %block_20
  br label %block_19

block_25:                                         ; preds = %block_24, %block_21
  %vr133.0 = phi i32 [ undef, %block_21 ], [ %vr133.2, %block_24 ]
  %vr134.0 = phi i32 [ undef, %block_21 ], [ %vr134.2, %block_24 ]
  %vr162.8 = phi i32 [ 0, %block_21 ], [ %vr162.11, %block_24 ]
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr162.8)
  %vr131 = icmp eq i32 %vr162.8, 0
  %vr132 = sext i1 %vr131 to i32
  %16 = icmp ne i32 %vr132, 0
  br i1 %16, label %block_27, label %block_28

block_27:                                         ; preds = %block_25
  br label %block_29

block_28:                                         ; preds = %block_25
  %vr134.1 = sub i32 %vr162.8, 1
  br label %block_29

block_29:                                         ; preds = %block_28, %block_27
  %vr133.2 = phi i32 [ %vr133.0, %block_27 ], [ %vr162.8, %block_28 ]
  %vr134.2 = phi i32 [ %vr134.0, %block_27 ], [ %vr134.1, %block_28 ]
  %vr162.11 = phi i32 [ 2, %block_27 ], [ %vr134.1, %block_28 ]
  br label %block_24

block_24:                                         ; preds = %block_29
  %vr126 = icmp ne i32 %vr162.11, 0
  %vr127 = sext i1 %vr126 to i32
  %17 = icmp ne i32 %vr127, 0
  br i1 %17, label %block_25, label %block_26

block_26:                                         ; preds = %block_24
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_30

block_30:                                         ; preds = %block_33, %block_26
  %vr164 = phi i32 [ 0, %block_26 ], [ %vr138, %block_33 ]
  br i1 true, label %block_31, label %block_32

block_31:                                         ; preds = %block_30
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr164)
  %vr142 = icmp eq i32 %vr164, 4
  %vr143 = sext i1 %vr142 to i32
  %20 = icmp ne i32 %vr143, 0
  br i1 %20, label %block_34, label %block_35

block_32:                                         ; preds = %block_34, %block_30
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  %vr147 = call i32 @mysqr(i32 5)
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str6, i32 0, i32 0), i32 %vr147)
  br label %block_36

block_34:                                         ; preds = %block_31
  br label %block_32

block_35:                                         ; preds = %block_31
  br label %block_33

block_36:                                         ; preds = %block_32
  br i1 true, label %block_37, label %block_38

block_33:                                         ; preds = %block_35
  %vr138 = add i32 %vr164, 1
  br label %block_30

block_37:                                         ; preds = %block_36
  ret i32 %vr155

block_38:                                         ; preds = %block_36
  ret i32 199
}

