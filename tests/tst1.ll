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
@.str13 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
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
  %vr147 = phi i32 [ %.arg_0, %block_2 ], [ %.arg_1, %block_1 ]
  %vr14 = add i32 %.arg_0, %vr147
  ret i32 %vr14
}

define i32 @mysqr(i32 %.arg_0) {
block_4:
  %vr18 = mul i32 %.arg_0, %.arg_0
  ret i32 %vr18
}

define i32 @main() {
block_5:
  %vr20 = load i32, i32* @gg, align 4
  %vr21 = alloca i32, align 4
  call void @store555(i32* %vr21)
  %vr24 = add i32 %vr20, 1
  %vr26 = icmp eq i32 %vr24, 7
  br i1 %vr26, label %block_6, label %block_7

block_6:                                          ; preds = %block_5
  %vr27 = load i32, i32* %vr21, align 4
  %vr28 = add i32 %vr27, 3
  store i32 %vr28, i32* %vr21, align 4
  store i32 25, i32* @gg, align 4
  br label %block_8

block_7:                                          ; preds = %block_5
  %vr30 = call i32 @add(i32 %vr24, i32 5)
  br label %block_8

block_8:                                          ; preds = %block_7, %block_6
  %vr149 = phi i32 [ %vr24, %block_6 ], [ %vr30, %block_7 ]
  %vr32 = load i32, i32* %vr21, align 4
  %vr33 = call i32 @add(i32 %vr149, i32 %vr32)
  store i32 %vr33, i32* %vr21, align 4
  %vr35 = alloca [16 x i8], align 1
  %vr36 = bitcast [16 x i8]* %vr35 to i8*
  %vr39 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr40 = add i64 %vr39, 1
  %0 = call i8* @memcpy(i8* %vr36, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr40)
  %vr43 = bitcast [16 x i8]* %vr35 to i8*
  %vr44 = ptrtoint i8* %vr43 to i64
  %vr45 = add i64 %vr44, 2
  %vr46 = inttoptr i64 %vr45 to i8*
  store i8 115, i8* %vr46, align 1
  %vr48 = bitcast [16 x i8]* %vr35 to i8*
  %vr49 = ptrtoint i8* %vr48 to i64
  %vr50 = add i64 %vr49, 1
  %vr51 = inttoptr i64 %vr50 to i8*
  store i8 101, i8* %vr51, align 1
  %vr52 = bitcast [16 x i8]* %vr35 to i8*
  %vr53 = load i32, i32* %vr21, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr52, i32 %vr53)
  %vr55 = alloca %struct12, align 8
  %vr56 = load %struct12, %struct12* %vr55, align 4
  %vr57 = insertvalue %struct12 %vr56, i32 111, 0
  store %struct12 %vr57, %struct12* %vr55, align 4
  %vr58 = load %struct12, %struct12* %vr55, align 4
  %vr59 = insertvalue %struct12 %vr58, i32 222, 1
  store %struct12 %vr59, %struct12* %vr55, align 4
  %vr60 = load %struct12, %struct12* %vr55, align 4
  %vr61 = extractvalue %struct12 %vr60, 0
  %vr62 = load %struct12, %struct12* %vr55, align 4
  %vr63 = extractvalue %struct12 %vr62, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr61, i32 %vr63)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str3, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i8 97)
  br label %block_9

block_9:                                          ; preds = %block_10, %block_8
  %vr155 = phi i32 [ 10, %block_8 ], [ %vr83, %block_10 ]
  %6 = icmp ne i32 %vr155, 0
  br i1 %6, label %block_10, label %block_11

block_10:                                         ; preds = %block_9
  %vr83 = sub i32 %vr155, 1
  br label %block_9

block_11:                                         ; preds = %block_9
  %vr84 = alloca [4 x i32], align 4
  br label %block_12

block_12:                                         ; preds = %block_13, %block_11
  %vr156.1 = phi i32 [ 0, %block_11 ], [ %vr99, %block_13 ]
  %vr87 = icmp slt i32 %vr156.1, 4
  br i1 %vr87, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr90 = mul i32 %vr156.1, %vr156.1
  %vr91 = bitcast [4 x i32]* %vr84 to i32*
  %vr93 = zext i32 %vr156.1 to i64
  %vr94 = shl i64 %vr93, 2
  %vr95 = ptrtoint i32* %vr91 to i64
  %vr96 = add i64 %vr95, %vr94
  %vr97 = inttoptr i64 %vr96 to i32*
  store i32 %vr90, i32* %vr97, align 4
  %vr99 = add i32 %vr156.1, 1
  br label %block_12

block_14:                                         ; preds = %block_12
  br label %block_15

block_15:                                         ; preds = %block_18, %block_14
  %vr157 = phi i32 [ 0, %block_14 ], [ %vr104, %block_18 ]
  %vr102 = icmp slt i32 %vr157, 4
  br i1 %vr102, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr105 = bitcast [4 x i32]* %vr84 to i32*
  %vr107 = zext i32 %vr157 to i64
  %vr108 = shl i64 %vr107, 2
  %vr109 = ptrtoint i32* %vr105 to i64
  %vr110 = add i64 %vr109, %vr108
  %vr111 = inttoptr i64 %vr110 to i32*
  %vr112 = load i32, i32* %vr111, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str5, i32 0, i32 0), i32 %vr112)
  br label %block_18

block_17:                                         ; preds = %block_15
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str6, i32 0, i32 0))
  br label %block_19

block_18:                                         ; preds = %block_16
  %vr104 = add i32 %vr157, 1
  br label %block_15

block_19:                                         ; preds = %block_23, %block_17
  %vr156.4 = phi i32 [ 3, %block_17 ], [ %vr116, %block_23 ]
  br i1 true, label %block_20, label %block_21

block_20:                                         ; preds = %block_19
  %vr116 = sub i32 %vr156.4, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str7, i32 0, i32 0), i32 %vr116)
  %vr120 = icmp eq i32 %vr116, 0
  br i1 %vr120, label %block_22, label %block_23

block_21:                                         ; preds = %block_22, %block_19
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str8, i32 0, i32 0))
  br label %block_25

block_22:                                         ; preds = %block_20
  br label %block_21

block_23:                                         ; preds = %block_20
  br label %block_19

block_25:                                         ; preds = %block_24, %block_21
  %vr128.0 = phi i32 [ 0, %block_21 ], [ %vr128.2, %block_24 ]
  %vr129.0 = phi i32 [ 0, %block_21 ], [ %vr129.2, %block_24 ]
  %vr156.8 = phi i32 [ 0, %block_21 ], [ %vr156.11, %block_24 ]
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str9, i32 0, i32 0), i32 %vr156.8)
  %vr127 = icmp eq i32 %vr156.8, 0
  br i1 %vr127, label %block_27, label %block_28

block_27:                                         ; preds = %block_25
  br label %block_29

block_28:                                         ; preds = %block_25
  %vr129.1 = sub i32 %vr156.8, 1
  br label %block_29

block_29:                                         ; preds = %block_28, %block_27
  %vr128.2 = phi i32 [ %vr128.0, %block_27 ], [ %vr156.8, %block_28 ]
  %vr129.2 = phi i32 [ %vr129.0, %block_27 ], [ %vr129.1, %block_28 ]
  %vr156.11 = phi i32 [ 2, %block_27 ], [ %vr129.1, %block_28 ]
  br label %block_24

block_24:                                         ; preds = %block_29
  %vr123 = icmp ne i32 %vr156.11, 0
  br i1 %vr123, label %block_25, label %block_26

block_26:                                         ; preds = %block_24
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str10, i32 0, i32 0))
  br label %block_30

block_30:                                         ; preds = %block_33, %block_26
  %vr158 = phi i32 [ 0, %block_26 ], [ %vr133, %block_33 ]
  br i1 true, label %block_31, label %block_32

block_31:                                         ; preds = %block_30
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str11, i32 0, i32 0), i32 %vr158)
  %vr137 = icmp eq i32 %vr158, 4
  br i1 %vr137, label %block_34, label %block_35

block_32:                                         ; preds = %block_34, %block_30
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str12, i32 0, i32 0))
  %vr141 = call i32 @mysqr(i32 5)
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str13, i32 0, i32 0), i32 %vr141)
  br label %block_36

block_34:                                         ; preds = %block_31
  br label %block_32

block_35:                                         ; preds = %block_31
  br label %block_33

block_36:                                         ; preds = %block_32
  br i1 true, label %block_37, label %block_38

block_33:                                         ; preds = %block_35
  %vr133 = add i32 %vr158, 1
  br label %block_30

block_37:                                         ; preds = %block_36
  ret i32 %vr149

block_38:                                         ; preds = %block_36
  ret i32 199
}

