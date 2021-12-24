; ModuleID = 'top'
source_filename = "top"

%struct2 = type { i32, i32 }

@.str0 = private unnamed_addr constant [9 x i8] c"R??: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [4 x i8] c"%p\0A\00", align 1
@.str2 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
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
  %vr63 = phi i32 [ %.arg_0, %block_2 ], [ %.arg_1, %block_1 ]
  %vr10 = add i32 %.arg_0, %vr63
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
  %vr64 = phi i32 [ %vr16, %block_5 ], [ %vr22, %block_6 ]
  %vr24 = load i32, i32* %vr13, align 4
  %vr25 = call i32 @add(i32 %vr64, i32 %vr24)
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
  %vr47 = bitcast [16 x i8]* %vr27 to i8*
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str1, i32 0, i32 0), i8* %vr47)
  %vr49 = alloca %struct2, align 8
  %vr50 = load %struct2, %struct2* %vr49, align 4
  %vr51 = insertvalue %struct2 %vr50, i32 111, 0
  store %struct2 %vr51, %struct2* %vr49, align 4
  %vr52 = load %struct2, %struct2* %vr49, align 4
  %vr53 = insertvalue %struct2 %vr52, i32 222, 1
  store %struct2 %vr53, %struct2* %vr49, align 4
  %vr54 = load %struct2, %struct2* %vr49, align 4
  %vr55 = extractvalue %struct2 %vr54, 0
  %vr56 = load %struct2, %struct2* %vr49, align 4
  %vr57 = extractvalue %struct2 %vr56, 1
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str2, i32 0, i32 0), i32 %vr55, i32 %vr57)
  ret i32 %vr64
}

