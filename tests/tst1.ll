; ModuleID = 'top'
source_filename = "top"

%struct12 = type { i32, i32 }

@.str0 = private unnamed_addr constant [9 x i8] c"R??: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str3 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1
@.str5 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str6 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str7 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str8 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str9 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str10 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str11 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
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
  br i1 %vr10, label %block_2, label %block_3

block_2:                                          ; preds = %block_1
  br label %block_3

block_3:                                          ; preds = %block_2, %block_1
  %vr139 = phi i32 [ %.arg_0, %block_2 ], [ %.arg_1, %block_1 ]
  %vr14 = add i32 %.arg_0, %vr139
  ret i32 %vr14
}

define i32 @main() {
block_4:
  %vr16 = load i32, i32* @gg, align 4
  %vr17 = alloca i32, align 4
  call void @store555(i32* %vr17)
  %vr20 = add i32 %vr16, 1
  %vr22 = icmp eq i32 %vr20, 7
  br i1 %vr22, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %vr23 = load i32, i32* %vr17, align 4
  %vr24 = add i32 %vr23, 3
  store i32 %vr24, i32* %vr17, align 4
  store i32 25, i32* @gg, align 4
  br label %block_7

block_6:                                          ; preds = %block_4
  %vr26 = call i32 @add(i32 %vr20, i32 5)
  br label %block_7

block_7:                                          ; preds = %block_6, %block_5
  %vr140 = phi i32 [ %vr20, %block_5 ], [ %vr26, %block_6 ]
  %vr28 = load i32, i32* %vr17, align 4
  %vr29 = call i32 @add(i32 %vr140, i32 %vr28)
  store i32 %vr29, i32* %vr17, align 4
  %vr31 = alloca [16 x i8], align 1
  %vr32 = bitcast [16 x i8]* %vr31 to i8*
  %vr35 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr36 = add i64 %vr35, 1
  %0 = call i8* @memcpy(i8* %vr32, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr36)
  %vr39 = bitcast [16 x i8]* %vr31 to i8*
  %vr40 = ptrtoint i8* %vr39 to i64
  %vr41 = add i64 %vr40, 2
  %vr42 = inttoptr i64 %vr41 to i8*
  store i8 115, i8* %vr42, align 1
  %vr44 = bitcast [16 x i8]* %vr31 to i8*
  %vr45 = ptrtoint i8* %vr44 to i64
  %vr46 = add i64 %vr45, 1
  %vr47 = inttoptr i64 %vr46 to i8*
  store i8 101, i8* %vr47, align 1
  %vr48 = bitcast [16 x i8]* %vr31 to i8*
  %vr49 = load i32, i32* %vr17, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr48, i32 %vr49)
  %vr51 = alloca %struct12, align 8
  %vr52 = load %struct12, %struct12* %vr51, align 4
  %vr53 = insertvalue %struct12 %vr52, i32 111, 0
  store %struct12 %vr53, %struct12* %vr51, align 4
  %vr54 = load %struct12, %struct12* %vr51, align 4
  %vr55 = insertvalue %struct12 %vr54, i32 222, 1
  store %struct12 %vr55, %struct12* %vr51, align 4
  %vr56 = load %struct12, %struct12* %vr51, align 4
  %vr57 = extractvalue %struct12 %vr56, 0
  %vr58 = load %struct12, %struct12* %vr51, align 4
  %vr59 = extractvalue %struct12 %vr58, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr57, i32 %vr59)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str3, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i8 97)
  br label %block_8

block_8:                                          ; preds = %block_9, %block_7
  %vr146 = phi i32 [ 10, %block_7 ], [ %vr79, %block_9 ]
  %6 = icmp ne i32 %vr146, 0
  br i1 %6, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr79 = sub i32 %vr146, 1
  br label %block_8

block_10:                                         ; preds = %block_8
  %vr80 = alloca [4 x i32], align 4
  br label %block_11

block_11:                                         ; preds = %block_12, %block_10
  %vr147.1 = phi i32 [ 0, %block_10 ], [ %vr95, %block_12 ]
  %vr83 = icmp slt i32 %vr147.1, 4
  br i1 %vr83, label %block_12, label %block_13

block_12:                                         ; preds = %block_11
  %vr86 = mul i32 %vr147.1, %vr147.1
  %vr87 = bitcast [4 x i32]* %vr80 to i32*
  %vr89 = zext i32 %vr147.1 to i64
  %vr90 = shl i64 %vr89, 2
  %vr91 = ptrtoint i32* %vr87 to i64
  %vr92 = add i64 %vr91, %vr90
  %vr93 = inttoptr i64 %vr92 to i32*
  store i32 %vr86, i32* %vr93, align 4
  %vr95 = add i32 %vr147.1, 1
  br label %block_11

block_13:                                         ; preds = %block_11
  br label %block_14

block_14:                                         ; preds = %block_17, %block_13
  %vr148 = phi i32 [ 0, %block_13 ], [ %vr100, %block_17 ]
  %vr98 = icmp slt i32 %vr148, 4
  br i1 %vr98, label %block_15, label %block_16

block_15:                                         ; preds = %block_14
  %vr101 = bitcast [4 x i32]* %vr80 to i32*
  %vr103 = zext i32 %vr148 to i64
  %vr104 = shl i64 %vr103, 2
  %vr105 = ptrtoint i32* %vr101 to i64
  %vr106 = add i64 %vr105, %vr104
  %vr107 = inttoptr i64 %vr106 to i32*
  %vr108 = load i32, i32* %vr107, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str5, i32 0, i32 0), i32 %vr108)
  br label %block_17

block_16:                                         ; preds = %block_14
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str6, i32 0, i32 0))
  br label %block_18

block_17:                                         ; preds = %block_15
  %vr100 = add i32 %vr148, 1
  br label %block_14

block_18:                                         ; preds = %block_22, %block_16
  %vr147.4 = phi i32 [ 3, %block_16 ], [ %vr112, %block_22 ]
  br i1 true, label %block_19, label %block_20

block_19:                                         ; preds = %block_18
  %vr112 = sub i32 %vr147.4, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str7, i32 0, i32 0), i32 %vr112)
  %vr116 = icmp eq i32 %vr112, 0
  br i1 %vr116, label %block_21, label %block_22

block_20:                                         ; preds = %block_21, %block_18
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str8, i32 0, i32 0))
  br label %block_24

block_21:                                         ; preds = %block_19
  br label %block_20

block_22:                                         ; preds = %block_19
  br label %block_18

block_24:                                         ; preds = %block_23, %block_20
  %vr124.0 = phi i32 [ 0, %block_20 ], [ %vr124.2, %block_23 ]
  %vr125.0 = phi i32 [ 0, %block_20 ], [ %vr125.2, %block_23 ]
  %vr147.8 = phi i32 [ 0, %block_20 ], [ %vr147.11, %block_23 ]
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str9, i32 0, i32 0), i32 %vr147.8)
  %vr123 = icmp eq i32 %vr147.8, 0
  br i1 %vr123, label %block_26, label %block_27

block_26:                                         ; preds = %block_24
  br label %block_28

block_27:                                         ; preds = %block_24
  %vr125.1 = sub i32 %vr147.8, 1
  br label %block_28

block_28:                                         ; preds = %block_27, %block_26
  %vr124.2 = phi i32 [ %vr124.0, %block_26 ], [ %vr147.8, %block_27 ]
  %vr125.2 = phi i32 [ %vr125.0, %block_26 ], [ %vr125.1, %block_27 ]
  %vr147.11 = phi i32 [ 2, %block_26 ], [ %vr125.1, %block_27 ]
  br label %block_23

block_23:                                         ; preds = %block_28
  %vr119 = icmp ne i32 %vr147.11, 0
  br i1 %vr119, label %block_24, label %block_25

block_25:                                         ; preds = %block_23
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str10, i32 0, i32 0))
  br label %block_29

block_29:                                         ; preds = %block_32, %block_25
  %vr149 = phi i32 [ 0, %block_25 ], [ %vr129, %block_32 ]
  br i1 true, label %block_30, label %block_31

block_30:                                         ; preds = %block_29
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str11, i32 0, i32 0), i32 %vr149)
  %vr133 = icmp eq i32 %vr149, 4
  br i1 %vr133, label %block_33, label %block_34

block_31:                                         ; preds = %block_33, %block_29
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str12, i32 0, i32 0))
  br label %block_35

block_33:                                         ; preds = %block_30
  br label %block_31

block_34:                                         ; preds = %block_30
  br label %block_32

block_35:                                         ; preds = %block_31
  br i1 true, label %block_36, label %block_37

block_32:                                         ; preds = %block_34
  %vr129 = add i32 %vr149, 1
  br label %block_29

block_36:                                         ; preds = %block_35
  ret i32 %vr140

block_37:                                         ; preds = %block_35
  ret i32 199
}

