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
  %vr135 = phi i32 [ %.arg_0, %block_2 ], [ %.arg_1, %block_1 ]
  %vr10 = add i32 %.arg_0, %vr135
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
  %vr136 = phi i32 [ %vr16, %block_5 ], [ %vr22, %block_6 ]
  %vr24 = load i32, i32* %vr13, align 4
  %vr25 = call i32 @add(i32 %vr136, i32 %vr24)
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
  %vr47 = alloca %struct12, align 8
  %vr48 = load %struct12, %struct12* %vr47, align 4
  %vr49 = insertvalue %struct12 %vr48, i32 111, 0
  store %struct12 %vr49, %struct12* %vr47, align 4
  %vr50 = load %struct12, %struct12* %vr47, align 4
  %vr51 = insertvalue %struct12 %vr50, i32 222, 1
  store %struct12 %vr51, %struct12* %vr47, align 4
  %vr52 = load %struct12, %struct12* %vr47, align 4
  %vr53 = extractvalue %struct12 %vr52, 0
  %vr54 = load %struct12, %struct12* %vr47, align 4
  %vr55 = extractvalue %struct12 %vr54, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr53, i32 %vr55)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str3, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i8 97)
  br label %block_8

block_8:                                          ; preds = %block_9, %block_7
  %vr142 = phi i32 [ 10, %block_7 ], [ %vr75, %block_9 ]
  %6 = icmp ne i32 %vr142, 0
  br i1 %6, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr75 = sub i32 %vr142, 1
  br label %block_8

block_10:                                         ; preds = %block_8
  %vr76 = alloca [4 x i32], align 4
  br label %block_11

block_11:                                         ; preds = %block_12, %block_10
  %vr143.1 = phi i32 [ 0, %block_10 ], [ %vr91, %block_12 ]
  %vr79 = icmp slt i32 %vr143.1, 4
  br i1 %vr79, label %block_12, label %block_13

block_12:                                         ; preds = %block_11
  %vr82 = mul i32 %vr143.1, %vr143.1
  %vr83 = bitcast [4 x i32]* %vr76 to i32*
  %vr85 = zext i32 %vr143.1 to i64
  %vr86 = mul i64 %vr85, 4
  %vr87 = ptrtoint i32* %vr83 to i64
  %vr88 = add i64 %vr87, %vr86
  %vr89 = inttoptr i64 %vr88 to i32*
  store i32 %vr82, i32* %vr89, align 4
  %vr91 = add i32 %vr143.1, 1
  br label %block_11

block_13:                                         ; preds = %block_11
  br label %block_14

block_14:                                         ; preds = %block_17, %block_13
  %vr144 = phi i32 [ 0, %block_13 ], [ %vr96, %block_17 ]
  %vr94 = icmp slt i32 %vr144, 4
  br i1 %vr94, label %block_15, label %block_16

block_15:                                         ; preds = %block_14
  %vr97 = bitcast [4 x i32]* %vr76 to i32*
  %vr99 = zext i32 %vr144 to i64
  %vr100 = mul i64 %vr99, 4
  %vr101 = ptrtoint i32* %vr97 to i64
  %vr102 = add i64 %vr101, %vr100
  %vr103 = inttoptr i64 %vr102 to i32*
  %vr104 = load i32, i32* %vr103, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str5, i32 0, i32 0), i32 %vr104)
  br label %block_17

block_16:                                         ; preds = %block_14
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str6, i32 0, i32 0))
  br label %block_18

block_17:                                         ; preds = %block_15
  %vr96 = add i32 %vr144, 1
  br label %block_14

block_18:                                         ; preds = %block_22, %block_16
  %vr143.4 = phi i32 [ 3, %block_16 ], [ %vr108, %block_22 ]
  br i1 true, label %block_19, label %block_20

block_19:                                         ; preds = %block_18
  %vr108 = sub i32 %vr143.4, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str7, i32 0, i32 0), i32 %vr108)
  %vr112 = icmp eq i32 %vr108, 0
  br i1 %vr112, label %block_21, label %block_22

block_20:                                         ; preds = %block_21, %block_18
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str8, i32 0, i32 0))
  br label %block_24

block_21:                                         ; preds = %block_19
  br label %block_20

block_22:                                         ; preds = %block_19
  br label %block_18

block_24:                                         ; preds = %block_23, %block_20
  %vr120.0 = phi i32 [ 0, %block_20 ], [ %vr120.2, %block_23 ]
  %vr121.0 = phi i32 [ 0, %block_20 ], [ %vr121.2, %block_23 ]
  %vr143.8 = phi i32 [ 0, %block_20 ], [ %vr143.11, %block_23 ]
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str9, i32 0, i32 0), i32 %vr143.8)
  %vr119 = icmp eq i32 %vr143.8, 0
  br i1 %vr119, label %block_26, label %block_27

block_26:                                         ; preds = %block_24
  br label %block_28

block_27:                                         ; preds = %block_24
  %vr121.1 = sub i32 %vr143.8, 1
  br label %block_28

block_28:                                         ; preds = %block_27, %block_26
  %vr120.2 = phi i32 [ %vr120.0, %block_26 ], [ %vr143.8, %block_27 ]
  %vr121.2 = phi i32 [ %vr121.0, %block_26 ], [ %vr121.1, %block_27 ]
  %vr143.11 = phi i32 [ 2, %block_26 ], [ %vr121.1, %block_27 ]
  br label %block_23

block_23:                                         ; preds = %block_28
  %vr115 = icmp ne i32 %vr143.11, 0
  br i1 %vr115, label %block_24, label %block_25

block_25:                                         ; preds = %block_23
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str10, i32 0, i32 0))
  br label %block_29

block_29:                                         ; preds = %block_32, %block_25
  %vr145 = phi i32 [ 0, %block_25 ], [ %vr125, %block_32 ]
  br i1 true, label %block_30, label %block_31

block_30:                                         ; preds = %block_29
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str11, i32 0, i32 0), i32 %vr145)
  %vr129 = icmp eq i32 %vr145, 4
  br i1 %vr129, label %block_33, label %block_34

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
  %vr125 = add i32 %vr145, 1
  br label %block_29

block_36:                                         ; preds = %block_35
  ret i32 %vr136

block_37:                                         ; preds = %block_35
  ret i32 199
}

