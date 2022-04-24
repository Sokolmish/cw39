; ModuleID = 'top'
source_filename = "top"

@.str0 = private unnamed_addr constant [10 x i8] c"[true %d]\00", align 1
@.str1 = private unnamed_addr constant [11 x i8] c"[false %d]\00", align 1
@.str2 = private unnamed_addr constant [23 x i8] c"Wrong arguments count\0A\00", align 1
@.str3 = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str5 = private unnamed_addr constant [6 x i8] c"true\0A\00", align 1
@.str6 = private unnamed_addr constant [7 x i8] c"false\0A\00", align 1

declare i32 @printf(i8*, ...)

declare i8* @memcpy(i8*, i8*, i64)

declare i64 @strlen(i8*)

declare i32 @atoi(i8*)

define i32 @tr(i32 %.arg_0) {
block_0:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str0, i32 0, i32 0), i32 %.arg_0)
  ret i32 1
}

define i32 @fl(i32 %.arg_0) {
block_2:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str1, i32 0, i32 0), i32 %.arg_0)
  ret i32 0
}

define i32 @main(i32 %.arg_0, i8** %.arg_1) {
block_4:
  %vr83 = icmp ne i32 %.arg_0, 4
  br i1 %vr83, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str2, i32 0, i32 0))
  ret i32 1

block_6:                                          ; preds = %block_4
  %vr86 = ptrtoint i8** %.arg_1 to i64
  %vr87 = add i64 %vr86, 8
  %vr88 = inttoptr i64 %vr87 to i8**
  %vr89 = load i8*, i8** %vr88, align 8
  %vr90 = call i32 @atoi(i8* %vr89)
  %vr92 = ptrtoint i8** %.arg_1 to i64
  %vr93 = add i64 %vr92, 16
  %vr94 = inttoptr i64 %vr93 to i8**
  %vr95 = load i8*, i8** %vr94, align 8
  %vr96 = call i32 @atoi(i8* %vr95)
  %vr98 = ptrtoint i8** %.arg_1 to i64
  %vr99 = add i64 %vr98, 24
  %vr100 = inttoptr i64 %vr99 to i8**
  %vr101 = load i8*, i8** %vr100, align 8
  %vr102 = call i32 @atoi(i8* %vr101)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str3, i32 0, i32 0), i32 %vr90, i32 %vr96, i32 %vr102)
  %vr107 = call i32 @tr(i32 1)
  %2 = icmp ne i32 %vr107, 0
  br i1 %2, label %block_8, label %block_9

block_8:                                          ; preds = %block_6
  %vr112 = call i32 @fl(i32 2)
  br label %block_9

block_9:                                          ; preds = %block_8, %block_6
  %vr109 = phi i32 [ %vr107, %block_6 ], [ %vr112, %block_8 ]
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr109)
  %vr114 = call i32 @fl(i32 1)
  %4 = icmp ne i32 %vr114, 0
  br i1 %4, label %block_10, label %block_11

block_10:                                         ; preds = %block_9
  %vr119 = call i32 @fl(i32 2)
  br label %block_11

block_11:                                         ; preds = %block_10, %block_9
  %vr116 = phi i32 [ %vr114, %block_9 ], [ %vr119, %block_10 ]
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr116)
  %vr121 = call i32 @tr(i32 1)
  %6 = icmp ne i32 %vr121, 0
  br i1 %6, label %block_13, label %block_12

block_12:                                         ; preds = %block_11
  %vr126 = call i32 @fl(i32 2)
  br label %block_13

block_13:                                         ; preds = %block_12, %block_11
  %vr123 = phi i32 [ %vr121, %block_11 ], [ %vr126, %block_12 ]
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr123)
  %vr128 = call i32 @fl(i32 1)
  %8 = icmp ne i32 %vr128, 0
  br i1 %8, label %block_15, label %block_14

block_14:                                         ; preds = %block_13
  %vr133 = call i32 @tr(i32 2)
  br label %block_15

block_15:                                         ; preds = %block_14, %block_13
  %vr130 = phi i32 [ %vr128, %block_13 ], [ %vr133, %block_14 ]
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr130)
  %vr135 = call i32 @tr(i32 1)
  %10 = icmp ne i32 %vr135, 0
  br i1 %10, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr140 = call i32 @fl(i32 2)
  br label %block_17

block_17:                                         ; preds = %block_16, %block_15
  %vr137 = phi i32 [ %vr135, %block_15 ], [ %vr140, %block_16 ]
  %11 = icmp ne i32 %vr137, 0
  br i1 %11, label %block_19, label %block_18

block_18:                                         ; preds = %block_17
  %vr145 = call i32 @tr(i32 3)
  br label %block_19

block_19:                                         ; preds = %block_18, %block_17
  %vr142 = phi i32 [ %vr137, %block_17 ], [ %vr145, %block_18 ]
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr142)
  %vr147 = call i32 @fl(i32 1)
  %13 = icmp ne i32 %vr147, 0
  br i1 %13, label %block_20, label %block_21

block_20:                                         ; preds = %block_19
  %vr152 = call i32 @tr(i32 2)
  %14 = icmp ne i32 %vr152, 0
  br i1 %14, label %block_23, label %block_22

block_21:                                         ; preds = %block_23, %block_19
  %vr149 = phi i32 [ %vr147, %block_19 ], [ %vr155, %block_23 ]
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr149)
  %vr161 = call i32 @tr(i32 1)
  %16 = icmp ne i32 %vr161, 0
  br i1 %16, label %block_24, label %block_25

block_22:                                         ; preds = %block_20
  %vr159 = call i32 @tr(i32 3)
  br label %block_23

block_23:                                         ; preds = %block_22, %block_20
  %vr155 = phi i32 [ %vr152, %block_20 ], [ %vr159, %block_22 ]
  br label %block_21

block_24:                                         ; preds = %block_21
  %vr166 = call i32 @fl(i32 2)
  br label %block_25

block_25:                                         ; preds = %block_24, %block_21
  %vr163 = phi i32 [ %vr161, %block_21 ], [ %vr166, %block_24 ]
  %17 = icmp ne i32 %vr163, 0
  br i1 %17, label %block_26, label %block_27

block_26:                                         ; preds = %block_25
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str5, i32 0, i32 0))
  br label %block_28

block_27:                                         ; preds = %block_25
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str6, i32 0, i32 0))
  br label %block_28

block_28:                                         ; preds = %block_27, %block_26
  ret i32 0
}

