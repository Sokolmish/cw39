; ModuleID = 'top'
source_filename = "top"

%struct2 = type { i32, i32 }

@.str0 = private unnamed_addr constant [9 x i8] c"R??: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str3 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1
@.str5 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str6 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str7 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str8 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@gg = internal global i32 142, align 8

define internal void @__dummy_func() {
dummy_block:
  ret void
}

declare i32 @printf(i8*, ...)

declare i8* @memcpy(i8*, i8*, i64)

declare i64 @strlen(i8*)

define void @store555(i32* %.arg_0) {
block_0:
  store i32 555, i32* %.arg_0, align 4
  ret void
}

define i32 @add(i32 %.arg_0, i32 %.arg_1) {
block_1:
  %vr6 = icmp eq i32 %.arg_0, %.arg_1
  br i1 %vr6, label %block_2, label %block_3

block_2:                                          ; preds = %block_1
  br label %block_3

block_3:                                          ; preds = %block_2, %block_1
  %vr116 = phi i32 [ %.arg_0, %block_2 ], [ %.arg_1, %block_1 ]
  %vr10 = add i32 %.arg_0, %vr116
  ret i32 %vr10
}

define i32 @main() {
block_4:
  %vr12 = load i32, i32* @gg, align 4
  %vr13 = alloca i32, align 4
  call void @store555(i32* %vr13)
  %vr16 = add i32 %vr12, 1
  %vr18 = icmp eq i32 %vr16, 7
  br i1 %vr18, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %vr19 = load i32, i32* %vr13, align 4
  %vr20 = add i32 %vr19, 3
  store i32 %vr20, i32* %vr13, align 4
  store i32 25, i32* @gg, align 4
  br label %block_7

block_6:                                          ; preds = %block_4
  %vr22 = call i32 @add(i32 %vr16, i32 5)
  br label %block_7

block_7:                                          ; preds = %block_6, %block_5
  %vr117 = phi i32 [ %vr16, %block_5 ], [ %vr22, %block_6 ]
  %vr24 = load i32, i32* %vr13, align 4
  %vr25 = call i32 @add(i32 %vr117, i32 %vr24)
  store i32 %vr25, i32* %vr13, align 4
  %vr27 = alloca [16 x i8], align 1
  %vr28 = bitcast [16 x i8]* %vr27 to i8*
  %vr31 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr32 = add i64 %vr31, 1
  %0 = call i8* @memcpy(i8* %vr28, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr32)
  %vr35 = bitcast [16 x i8]* %vr27 to i8*
  %vr36 = ptrtoint i8* %vr35 to i64
  %vr37 = add i64 %vr36, 2
  %vr38 = inttoptr i64 %vr37 to i8*
  store i8 115, i8* %vr38, align 1
  %vr40 = bitcast [16 x i8]* %vr27 to i8*
  %vr41 = ptrtoint i8* %vr40 to i64
  %vr42 = add i64 %vr41, 1
  %vr43 = inttoptr i64 %vr42 to i8*
  store i8 101, i8* %vr43, align 1
  %vr44 = bitcast [16 x i8]* %vr27 to i8*
  %vr45 = load i32, i32* %vr13, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr44, i32 %vr45)
  %vr47 = alloca %struct2, align 8
  %vr48 = load %struct2, %struct2* %vr47, align 4
  %vr49 = insertvalue %struct2 %vr48, i32 111, 0
  store %struct2 %vr49, %struct2* %vr47, align 4
  %vr50 = load %struct2, %struct2* %vr47, align 4
  %vr51 = insertvalue %struct2 %vr50, i32 222, 1
  store %struct2 %vr51, %struct2* %vr47, align 4
  %vr52 = load %struct2, %struct2* %vr47, align 4
  %vr53 = extractvalue %struct2 %vr52, 0
  %vr54 = load %struct2, %struct2* %vr47, align 4
  %vr55 = extractvalue %struct2 %vr54, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr53, i32 %vr55)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str3, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i8 97)
  br label %block_8

block_8:                                          ; preds = %block_9, %block_7
  %vr123 = phi i32 [ 10, %block_7 ], [ %vr75, %block_9 ]
  %6 = icmp ne i32 %vr123, 0
  br i1 %6, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr75 = sub i32 %vr123, 1
  br label %block_8

block_10:                                         ; preds = %block_8
  %vr76 = alloca [4 x i32], align 4
  br label %block_11

block_11:                                         ; preds = %block_12, %block_10
  %vr124.1 = phi i32 [ 0, %block_10 ], [ %vr91, %block_12 ]
  %vr79 = icmp slt i32 %vr124.1, 4
  br i1 %vr79, label %block_12, label %block_13

block_12:                                         ; preds = %block_11
  %vr82 = mul i32 %vr124.1, %vr124.1
  %vr83 = bitcast [4 x i32]* %vr76 to i32*
  %vr85 = zext i32 %vr124.1 to i64
  %vr86 = mul i64 %vr85, 4
  %vr87 = ptrtoint i32* %vr83 to i64
  %vr88 = add i64 %vr87, %vr86
  %vr89 = inttoptr i64 %vr88 to i32*
  store i32 %vr82, i32* %vr89, align 4
  %vr91 = add i32 %vr124.1, 1
  br label %block_11

block_13:                                         ; preds = %block_11
  br label %block_14

block_14:                                         ; preds = %block_15, %block_13
  %vr124.4 = phi i32 [ 0, %block_13 ], [ %vr96, %block_15 ]
  %vr93 = icmp slt i32 %vr124.4, 4
  br i1 %vr93, label %block_15, label %block_16

block_15:                                         ; preds = %block_14
  %vr94 = bitcast [4 x i32]* %vr76 to i32*
  %vr96 = add i32 %vr124.4, 1
  %vr97 = zext i32 %vr124.4 to i64
  %vr98 = mul i64 %vr97, 4
  %vr99 = ptrtoint i32* %vr94 to i64
  %vr100 = add i64 %vr99, %vr98
  %vr101 = inttoptr i64 %vr100 to i32*
  %vr102 = load i32, i32* %vr101, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str5, i32 0, i32 0), i32 %vr102)
  br label %block_14

block_16:                                         ; preds = %block_14
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str6, i32 0, i32 0))
  br label %block_17

block_17:                                         ; preds = %block_21, %block_16
  %vr124.7 = phi i32 [ 3, %block_16 ], [ %vr106, %block_21 ]
  br i1 true, label %block_18, label %block_19

block_18:                                         ; preds = %block_17
  %vr106 = sub i32 %vr124.7, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str7, i32 0, i32 0), i32 %vr106)
  %vr110 = icmp eq i32 %vr106, 0
  br i1 %vr110, label %block_20, label %block_21

block_19:                                         ; preds = %block_20, %block_17
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str8, i32 0, i32 0))
  ret i32 %vr117

block_20:                                         ; preds = %block_18
  br label %block_19

block_21:                                         ; preds = %block_18
  br label %block_17
}

