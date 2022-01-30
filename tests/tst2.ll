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
block_1:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @.str1, i32 0, i32 0), i32 %.arg_0)
  ret i32 0
}

define i32 @main(i32 %.arg_0, i8** %.arg_1) {
block_2:
  %vr9 = icmp ne i32 %.arg_0, 4
  br i1 %vr9, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str2, i32 0, i32 0))
  ret i32 1

block_4:                                          ; preds = %block_2
  %vr13 = ptrtoint i8** %.arg_1 to i64
  %vr14 = add i64 %vr13, 8
  %vr15 = inttoptr i64 %vr14 to i8**
  %vr16 = load i8*, i8** %vr15, align 8
  %vr17 = call i32 @atoi(i8* %vr16)
  %vr20 = ptrtoint i8** %.arg_1 to i64
  %vr21 = add i64 %vr20, 16
  %vr22 = inttoptr i64 %vr21 to i8**
  %vr23 = load i8*, i8** %vr22, align 8
  %vr24 = call i32 @atoi(i8* %vr23)
  %vr27 = ptrtoint i8** %.arg_1 to i64
  %vr28 = add i64 %vr27, 24
  %vr29 = inttoptr i64 %vr28 to i8**
  %vr30 = load i8*, i8** %vr29, align 8
  %vr31 = call i32 @atoi(i8* %vr30)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str3, i32 0, i32 0), i32 %vr17, i32 %vr24, i32 %vr31)
  %vr36 = call i32 @tr(i32 1)
  %2 = icmp ne i32 %vr36, 0
  br i1 %2, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %vr38 = call i32 @fl(i32 2)
  br label %block_6

block_6:                                          ; preds = %block_5, %block_4
  %vr37 = phi i32 [ %vr36, %block_4 ], [ %vr38, %block_5 ]
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr37)
  %vr40 = call i32 @fl(i32 1)
  %4 = icmp ne i32 %vr40, 0
  br i1 %4, label %block_7, label %block_8

block_7:                                          ; preds = %block_6
  %vr42 = call i32 @fl(i32 2)
  br label %block_8

block_8:                                          ; preds = %block_7, %block_6
  %vr41 = phi i32 [ %vr40, %block_6 ], [ %vr42, %block_7 ]
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr41)
  %vr44 = call i32 @tr(i32 1)
  %6 = icmp ne i32 %vr44, 0
  br i1 %6, label %block_10, label %block_9

block_9:                                          ; preds = %block_8
  %vr46 = call i32 @fl(i32 2)
  br label %block_10

block_10:                                         ; preds = %block_9, %block_8
  %vr45 = phi i32 [ %vr44, %block_8 ], [ %vr46, %block_9 ]
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr45)
  %vr48 = call i32 @fl(i32 1)
  %8 = icmp ne i32 %vr48, 0
  br i1 %8, label %block_12, label %block_11

block_11:                                         ; preds = %block_10
  %vr50 = call i32 @tr(i32 2)
  br label %block_12

block_12:                                         ; preds = %block_11, %block_10
  %vr49 = phi i32 [ %vr48, %block_10 ], [ %vr50, %block_11 ]
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr49)
  %vr52 = call i32 @tr(i32 1)
  %10 = icmp ne i32 %vr52, 0
  br i1 %10, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr54 = call i32 @fl(i32 2)
  br label %block_14

block_14:                                         ; preds = %block_13, %block_12
  %vr53 = phi i32 [ %vr52, %block_12 ], [ %vr54, %block_13 ]
  %11 = icmp ne i32 %vr53, 0
  br i1 %11, label %block_16, label %block_15

block_15:                                         ; preds = %block_14
  %vr56 = call i32 @tr(i32 3)
  br label %block_16

block_16:                                         ; preds = %block_15, %block_14
  %vr55 = phi i32 [ %vr53, %block_14 ], [ %vr56, %block_15 ]
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr55)
  %vr58 = call i32 @fl(i32 1)
  %13 = icmp ne i32 %vr58, 0
  br i1 %13, label %block_17, label %block_18

block_17:                                         ; preds = %block_16
  %vr60 = call i32 @tr(i32 2)
  %14 = icmp ne i32 %vr60, 0
  br i1 %14, label %block_20, label %block_19

block_18:                                         ; preds = %block_20, %block_16
  %vr59 = phi i32 [ %vr58, %block_16 ], [ %vr61, %block_20 ]
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr59)
  %vr64 = call i32 @tr(i32 1)
  %16 = icmp ne i32 %vr64, 0
  br i1 %16, label %block_21, label %block_22

block_19:                                         ; preds = %block_17
  %vr62 = call i32 @tr(i32 3)
  br label %block_20

block_20:                                         ; preds = %block_19, %block_17
  %vr61 = phi i32 [ %vr60, %block_17 ], [ %vr62, %block_19 ]
  br label %block_18

block_21:                                         ; preds = %block_18
  %vr66 = call i32 @fl(i32 2)
  br label %block_22

block_22:                                         ; preds = %block_21, %block_18
  %vr65 = phi i32 [ %vr64, %block_18 ], [ %vr66, %block_21 ]
  %17 = icmp ne i32 %vr65, 0
  br i1 %17, label %block_23, label %block_24

block_23:                                         ; preds = %block_22
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str5, i32 0, i32 0))
  br label %block_25

block_24:                                         ; preds = %block_22
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str6, i32 0, i32 0))
  br label %block_25

block_25:                                         ; preds = %block_24, %block_23
  ret i32 0
}

