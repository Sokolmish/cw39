; ModuleID = 'top'
source_filename = "top"

@.str0 = private unnamed_addr constant [10 x i8] c"[true %d]\00", align 1
@.str1 = private unnamed_addr constant [11 x i8] c"[false %d]\00", align 1
@.str2 = private unnamed_addr constant [22 x i8] c"Wrong arguments count\00", align 1
@.str3 = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str5 = private unnamed_addr constant [6 x i8] c"true\0A\00", align 1
@.str6 = private unnamed_addr constant [7 x i8] c"false\0A\00", align 1

define internal void @__dummy_func() {
dummy_block:
  ret void
}

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
block_1:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str1, i32 0, i32 0), i32 %.arg_0)
  ret i32 0
}

define i32 @main(i32 %.arg_0, i8** %.arg_1) {
block_2:
  %vr9 = icmp ne i32 %.arg_0, 4
  %vr10 = sext i1 %vr9 to i32
  %0 = icmp ne i32 %vr10, 0
  br i1 %0, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str2, i32 0, i32 0))
  ret i32 1

block_4:                                          ; preds = %block_2
  %vr16 = ptrtoint i8** %.arg_1 to i64
  %vr17 = add i64 %vr16, 8
  %vr18 = inttoptr i64 %vr17 to i8**
  %vr19 = load i8*, i8** %vr18, align 8
  %vr20 = call i32 @atoi(i8* %vr19)
  %vr25 = ptrtoint i8** %.arg_1 to i64
  %vr26 = add i64 %vr25, 16
  %vr27 = inttoptr i64 %vr26 to i8**
  %vr28 = load i8*, i8** %vr27, align 8
  %vr29 = call i32 @atoi(i8* %vr28)
  %vr34 = ptrtoint i8** %.arg_1 to i64
  %vr35 = add i64 %vr34, 24
  %vr36 = inttoptr i64 %vr35 to i8**
  %vr37 = load i8*, i8** %vr36, align 8
  %vr38 = call i32 @atoi(i8* %vr37)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str3, i32 0, i32 0), i32 %vr20, i32 %vr29, i32 %vr38)
  %vr43 = call i32 @tr(i32 1)
  %3 = icmp ne i32 %vr43, 0
  br i1 %3, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %vr45 = call i32 @fl(i32 2)
  br label %block_6

block_6:                                          ; preds = %block_5, %block_4
  %vr44 = phi i32 [ %vr43, %block_4 ], [ %vr45, %block_5 ]
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr44)
  %vr47 = call i32 @fl(i32 1)
  %5 = icmp ne i32 %vr47, 0
  br i1 %5, label %block_7, label %block_8

block_7:                                          ; preds = %block_6
  %vr49 = call i32 @fl(i32 2)
  br label %block_8

block_8:                                          ; preds = %block_7, %block_6
  %vr48 = phi i32 [ %vr47, %block_6 ], [ %vr49, %block_7 ]
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr48)
  %vr51 = call i32 @tr(i32 1)
  %7 = icmp ne i32 %vr51, 0
  br i1 %7, label %block_10, label %block_9

block_9:                                          ; preds = %block_8
  %vr53 = call i32 @fl(i32 2)
  br label %block_10

block_10:                                         ; preds = %block_9, %block_8
  %vr52 = phi i32 [ %vr51, %block_8 ], [ %vr53, %block_9 ]
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr52)
  %vr55 = call i32 @fl(i32 1)
  %9 = icmp ne i32 %vr55, 0
  br i1 %9, label %block_12, label %block_11

block_11:                                         ; preds = %block_10
  %vr57 = call i32 @tr(i32 2)
  br label %block_12

block_12:                                         ; preds = %block_11, %block_10
  %vr56 = phi i32 [ %vr55, %block_10 ], [ %vr57, %block_11 ]
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr56)
  %vr59 = call i32 @tr(i32 1)
  %11 = icmp ne i32 %vr59, 0
  br i1 %11, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr61 = call i32 @fl(i32 2)
  br label %block_14

block_14:                                         ; preds = %block_13, %block_12
  %vr60 = phi i32 [ %vr59, %block_12 ], [ %vr61, %block_13 ]
  %12 = icmp ne i32 %vr60, 0
  br i1 %12, label %block_16, label %block_15

block_15:                                         ; preds = %block_14
  %vr63 = call i32 @tr(i32 3)
  br label %block_16

block_16:                                         ; preds = %block_15, %block_14
  %vr62 = phi i32 [ %vr60, %block_14 ], [ %vr63, %block_15 ]
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr62)
  %vr65 = call i32 @fl(i32 1)
  %14 = icmp ne i32 %vr65, 0
  br i1 %14, label %block_17, label %block_18

block_17:                                         ; preds = %block_16
  %vr67 = call i32 @tr(i32 2)
  %15 = icmp ne i32 %vr67, 0
  br i1 %15, label %block_20, label %block_19

block_18:                                         ; preds = %block_20, %block_16
  %vr66 = phi i32 [ %vr65, %block_16 ], [ %vr68, %block_20 ]
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr66)
  %vr71 = call i32 @tr(i32 1)
  %17 = icmp ne i32 %vr71, 0
  br i1 %17, label %block_21, label %block_22

block_19:                                         ; preds = %block_17
  %vr69 = call i32 @tr(i32 3)
  br label %block_20

block_20:                                         ; preds = %block_19, %block_17
  %vr68 = phi i32 [ %vr67, %block_17 ], [ %vr69, %block_19 ]
  br label %block_18

block_21:                                         ; preds = %block_18
  %vr73 = call i32 @fl(i32 2)
  br label %block_22

block_22:                                         ; preds = %block_21, %block_18
  %vr72 = phi i32 [ %vr71, %block_18 ], [ %vr73, %block_21 ]
  %18 = icmp ne i32 %vr72, 0
  br i1 %18, label %block_23, label %block_24

block_23:                                         ; preds = %block_22
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str5, i32 0, i32 0))
  br label %block_25

block_24:                                         ; preds = %block_22
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str6, i32 0, i32 0))
  br label %block_25

block_25:                                         ; preds = %block_24, %block_23
  ret i32 0
}

