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
block_2:
  %vr10 = icmp eq i32 %.arg_0, %.arg_1
  br i1 %vr10, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  br label %block_4

block_4:                                          ; preds = %block_3, %block_2
  %vr145 = phi i32 [ %.arg_1, %block_2 ], [ %.arg_0, %block_3 ]
  %vr14 = add i32 %.arg_0, %vr145
  ret i32 %vr14
}

define i32 @mysqr(i32 %.arg_0) {
block_6:
  %vr18 = mul i32 %.arg_0, %.arg_0
  ret i32 %vr18
}

define i32 @main() {
block_8:
  %vr20 = load i32, i32* @gg, align 4
  %vr21 = alloca i32, align 4
  call void @store555(i32* %vr21)
  %vr24 = add i32 %vr20, 1
  %vr26 = icmp eq i32 %vr24, 7
  br i1 %vr26, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr27 = load i32, i32* %vr21, align 4
  %vr28 = add i32 %vr27, 3
  store i32 %vr28, i32* %vr21, align 4
  store i32 25, i32* @gg, align 4
  br label %block_11

block_10:                                         ; preds = %block_8
  %vr30 = call i32 @add(i32 %vr24, i32 5)
  br label %block_11

block_11:                                         ; preds = %block_10, %block_9
  %vr147 = phi i32 [ %vr24, %block_9 ], [ %vr30, %block_10 ]
  %vr32 = load i32, i32* %vr21, align 4
  %vr33 = call i32 @add(i32 %vr147, i32 %vr32)
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
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str3, i32 0, i32 0), i8 97)
  br label %block_12

block_12:                                         ; preds = %block_13, %block_11
  %vr153 = phi i32 [ 10, %block_11 ], [ %vr81, %block_13 ]
  %6 = icmp ne i32 %vr153, 0
  br i1 %6, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr81 = sub i32 %vr153, 1
  br label %block_12

block_14:                                         ; preds = %block_12
  %vr82 = alloca [4 x i32], align 4
  br label %block_15

block_15:                                         ; preds = %block_16, %block_14
  %vr154.1 = phi i32 [ 0, %block_14 ], [ %vr97, %block_16 ]
  %vr85 = icmp slt i32 %vr154.1, 4
  br i1 %vr85, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr88 = mul i32 %vr154.1, %vr154.1
  %vr89 = bitcast [4 x i32]* %vr82 to i32*
  %vr91 = zext i32 %vr154.1 to i64
  %vr92 = shl i64 %vr91, 2
  %vr93 = ptrtoint i32* %vr89 to i64
  %vr94 = add i64 %vr93, %vr92
  %vr95 = inttoptr i64 %vr94 to i32*
  store i32 %vr88, i32* %vr95, align 4
  %vr97 = add i32 %vr154.1, 1
  br label %block_15

block_17:                                         ; preds = %block_15
  br label %block_18

block_18:                                         ; preds = %block_21, %block_17
  %vr155 = phi i32 [ 0, %block_17 ], [ %vr102, %block_21 ]
  %vr100 = icmp slt i32 %vr155, 4
  br i1 %vr100, label %block_19, label %block_20

block_19:                                         ; preds = %block_18
  %vr103 = bitcast [4 x i32]* %vr82 to i32*
  %vr105 = zext i32 %vr155 to i64
  %vr106 = shl i64 %vr105, 2
  %vr107 = ptrtoint i32* %vr103 to i64
  %vr108 = add i64 %vr107, %vr106
  %vr109 = inttoptr i64 %vr108 to i32*
  %vr110 = load i32, i32* %vr109, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr110)
  br label %block_21

block_20:                                         ; preds = %block_18
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_22

block_21:                                         ; preds = %block_19
  %vr102 = add i32 %vr155, 1
  br label %block_18

block_22:                                         ; preds = %block_23, %block_20
  %vr154.4 = phi i32 [ 3, %block_20 ], [ %vr114, %block_23 ]
  br label %block_23

block_23:                                         ; preds = %block_22
  %vr114 = sub i32 %vr154.4, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr114)
  %vr118 = icmp eq i32 %vr114, 0
  br i1 %vr118, label %block_24, label %block_22

block_24:                                         ; preds = %block_23
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_28

block_28:                                         ; preds = %block_27, %block_24
  %vr126.0 = phi i32 [ undef, %block_24 ], [ %vr126.2, %block_27 ]
  %vr127.0 = phi i32 [ undef, %block_24 ], [ %vr127.2, %block_27 ]
  %vr154.8 = phi i32 [ 0, %block_24 ], [ %vr154.11, %block_27 ]
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr154.8)
  %vr125 = icmp eq i32 %vr154.8, 0
  br i1 %vr125, label %block_30, label %block_31

block_30:                                         ; preds = %block_28
  br label %block_32

block_31:                                         ; preds = %block_28
  %vr127.1 = sub i32 %vr154.8, 1
  br label %block_32

block_32:                                         ; preds = %block_31, %block_30
  %vr126.2 = phi i32 [ %vr126.0, %block_30 ], [ %vr154.8, %block_31 ]
  %vr127.2 = phi i32 [ %vr127.0, %block_30 ], [ %vr127.1, %block_31 ]
  %vr154.11 = phi i32 [ 2, %block_30 ], [ %vr127.1, %block_31 ]
  br label %block_27

block_27:                                         ; preds = %block_32
  %vr121 = icmp ne i32 %vr154.11, 0
  br i1 %vr121, label %block_28, label %block_29

block_29:                                         ; preds = %block_27
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_33

block_33:                                         ; preds = %block_36, %block_29
  %vr156 = phi i32 [ 0, %block_29 ], [ %vr131, %block_36 ]
  br label %block_34

block_34:                                         ; preds = %block_33
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr156)
  %vr135 = icmp eq i32 %vr156, 4
  br i1 %vr135, label %block_35, label %block_36

block_35:                                         ; preds = %block_34
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  %vr139 = call i32 @mysqr(i32 5)
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str6, i32 0, i32 0), i32 %vr139)
  br label %block_40

block_36:                                         ; preds = %block_34
  %vr131 = add i32 %vr156, 1
  br label %block_33

block_40:                                         ; preds = %block_35
  ret i32 %vr147
}

