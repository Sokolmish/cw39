; ModuleID = 'top'
source_filename = "top"

@.str0 = private unnamed_addr constant [9 x i8] c"Ref: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [7 x i8] c"%p %p\0A\00", align 1
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
  %vr7 = icmp eq i32 %.arg_0, %.arg_1
  br i1 %vr7, label %block_2, label %block_3

block_2:                                          ; preds = %block_1
  br label %block_3

block_3:                                          ; preds = %block_2, %block_1
  %vr52 = phi i32 [ %.arg_0, %block_2 ], [ %.arg_1, %block_1 ]
  %vr11 = add i32 %.arg_0, %vr52
  ret i32 %vr11
}

define i32 @main() {
block_4:
  %vr13 = load i32, i32* @gg, align 4
  %vr14 = alloca i32, align 4
  call void @store555(i32* %vr14)
  %vr17 = add i32 %vr13, 1
  %vr19 = icmp eq i32 %vr17, 7
  br i1 %vr19, label %block_5, label %block_6

block_5:                                          ; preds = %block_4
  %vr20 = load i32, i32* %vr14, align 4
  %vr21 = add i32 %vr20, 3
  store i32 %vr21, i32* %vr14, align 4
  store i32 25, i32* @gg, align 4
  br label %block_7

block_6:                                          ; preds = %block_4
  %vr23 = call i32 @add(i32 %vr17, i32 5)
  br label %block_7

block_7:                                          ; preds = %block_6, %block_5
  %vr53 = phi i32 [ %vr17, %block_5 ], [ %vr23, %block_6 ]
  %vr25 = load i32, i32* %vr14, align 4
  %vr26 = call i32 @add(i32 %vr53, i32 %vr25)
  store i32 %vr26, i32* %vr14, align 4
  %vr28 = alloca [16 x i8], align 1
  %vr29 = bitcast [16 x i8]* %vr28 to i8*
  %vr32 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr33 = add i64 %vr32, 1
  %0 = call i8* @memcpy(i8* %vr29, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr33)
  %vr36 = bitcast [16 x i8]* %vr28 to i8*
  %vr37 = ptrtoint i8* %vr36 to i64
  %vr38 = add i64 %vr37, 2
  %vr39 = inttoptr i64 %vr38 to i8*
  store i8 115, i8* %vr39, align 1
  %vr42 = bitcast [16 x i8]* %vr28 to i8*
  %vr43 = load i32, i32* %vr14, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr42, i32 %vr43)
  %vr45 = bitcast [16 x i8]* %vr28 to i8*
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i8* %vr45, i8* %vr39)
  ret i32 %vr53
}

