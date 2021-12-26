; ModuleID = 'top'
source_filename = "top"

@.str0 = private unnamed_addr constant [10 x i8] c"[true %d]\00", align 1
@.str1 = private unnamed_addr constant [11 x i8] c"[false %d]\00", align 1
@.str2 = private unnamed_addr constant [22 x i8] c"Wrong arguments count\00", align 1
@.str3 = private unnamed_addr constant [10 x i8] c"%d %d %d\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str5 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str6 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str7 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str8 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str9 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str10 = private unnamed_addr constant [6 x i8] c"true\0A\00", align 1
@.str11 = private unnamed_addr constant [7 x i8] c"false\0A\00", align 1

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
  br i1 %vr9, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str2, i32 0, i32 0))
  ret i32 1

block_4:                                          ; preds = %block_2
  %vr15 = ptrtoint i8** %.arg_1 to i64
  %vr16 = add i64 %vr15, 8
  %vr17 = inttoptr i64 %vr16 to i8**
  %vr18 = load i8*, i8** %vr17, align 8
  %vr19 = call i32 @atoi(i8* %vr18)
  %vr24 = ptrtoint i8** %.arg_1 to i64
  %vr25 = add i64 %vr24, 16
  %vr26 = inttoptr i64 %vr25 to i8**
  %vr27 = load i8*, i8** %vr26, align 8
  %vr28 = call i32 @atoi(i8* %vr27)
  %vr33 = ptrtoint i8** %.arg_1 to i64
  %vr34 = add i64 %vr33, 24
  %vr35 = inttoptr i64 %vr34 to i8**
  %vr36 = load i8*, i8** %vr35, align 8
  %vr37 = call i32 @atoi(i8* %vr36)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str3, i32 0, i32 0), i32 %vr19, i32 %vr28, i32 %vr37)
  %vr42 = call i32 @tr(i32 1)
  %2 = icmp ne i32 %vr42, 0
  br i1 %2, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %vr44 = call i32 @fl(i32 2)
  br label %block_6

block_6:                                          ; preds = %block_5, %block_4
  %vr43 = phi i32 [ %vr42, %block_4 ], [ %vr44, %block_5 ]
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr43)
  %vr46 = call i32 @fl(i32 1)
  %4 = icmp ne i32 %vr46, 0
  br i1 %4, label %block_7, label %block_8

block_7:                                          ; preds = %block_6
  %vr48 = call i32 @fl(i32 2)
  br label %block_8

block_8:                                          ; preds = %block_7, %block_6
  %vr47 = phi i32 [ %vr46, %block_6 ], [ %vr48, %block_7 ]
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str5, i32 0, i32 0), i32 %vr47)
  %vr50 = call i32 @tr(i32 1)
  %6 = icmp ne i32 %vr50, 0
  br i1 %6, label %block_10, label %block_9

block_9:                                          ; preds = %block_8
  %vr52 = call i32 @fl(i32 2)
  br label %block_10

block_10:                                         ; preds = %block_9, %block_8
  %vr51 = phi i32 [ %vr50, %block_8 ], [ %vr52, %block_9 ]
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str6, i32 0, i32 0), i32 %vr51)
  %vr54 = call i32 @fl(i32 1)
  %8 = icmp ne i32 %vr54, 0
  br i1 %8, label %block_12, label %block_11

block_11:                                         ; preds = %block_10
  %vr56 = call i32 @tr(i32 2)
  br label %block_12

block_12:                                         ; preds = %block_11, %block_10
  %vr55 = phi i32 [ %vr54, %block_10 ], [ %vr56, %block_11 ]
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str7, i32 0, i32 0), i32 %vr55)
  %vr58 = call i32 @tr(i32 1)
  %10 = icmp ne i32 %vr58, 0
  br i1 %10, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr60 = call i32 @fl(i32 2)
  br label %block_14

block_14:                                         ; preds = %block_13, %block_12
  %vr59 = phi i32 [ %vr58, %block_12 ], [ %vr60, %block_13 ]
  %11 = icmp ne i32 %vr59, 0
  br i1 %11, label %block_16, label %block_15

block_15:                                         ; preds = %block_14
  %vr62 = call i32 @tr(i32 3)
  br label %block_16

block_16:                                         ; preds = %block_15, %block_14
  %vr61 = phi i32 [ %vr59, %block_14 ], [ %vr62, %block_15 ]
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str8, i32 0, i32 0), i32 %vr61)
  %vr64 = call i32 @fl(i32 1)
  %13 = icmp ne i32 %vr64, 0
  br i1 %13, label %block_17, label %block_18

block_17:                                         ; preds = %block_16
  %vr66 = call i32 @tr(i32 2)
  br label %block_18

block_18:                                         ; preds = %block_17, %block_16
  %vr65 = phi i32 [ %vr64, %block_16 ], [ %vr66, %block_17 ]
  %14 = icmp ne i32 %vr65, 0
  br i1 %14, label %block_20, label %block_19

block_19:                                         ; preds = %block_18
  %vr68 = call i32 @tr(i32 3)
  br label %block_20

block_20:                                         ; preds = %block_19, %block_18
  %vr67 = phi i32 [ %vr65, %block_18 ], [ %vr68, %block_19 ]
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str9, i32 0, i32 0), i32 %vr67)
  %vr70 = call i32 @tr(i32 1)
  %16 = icmp ne i32 %vr70, 0
  br i1 %16, label %block_21, label %block_22

block_21:                                         ; preds = %block_20
  %vr72 = call i32 @fl(i32 2)
  br label %block_22

block_22:                                         ; preds = %block_21, %block_20
  %vr71 = phi i32 [ %vr70, %block_20 ], [ %vr72, %block_21 ]
  %17 = icmp ne i32 %vr71, 0
  br i1 %17, label %block_23, label %block_24

block_23:                                         ; preds = %block_22
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @.str10, i32 0, i32 0))
  br label %block_25

block_24:                                         ; preds = %block_22
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str11, i32 0, i32 0))
  br label %block_25

block_25:                                         ; preds = %block_24, %block_23
  ret i32 0
}

