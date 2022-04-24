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
  %vr148 = load i32, i32* %.arg_0, align 4
  %vr149 = add i32 %vr148, 1
  store i32 %vr149, i32* %.arg_0, align 4
  ret void
}

define i32 @add(i32 %.arg_0, i32 %.arg_1) {
block_2:
  %vr156 = icmp eq i32 %.arg_0, %.arg_1
  br i1 %vr156, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  br label %block_4

block_4:                                          ; preds = %block_3, %block_2
  %vr164 = phi i32 [ %.arg_1, %block_2 ], [ %.arg_0, %block_3 ]
  %vr161 = add i32 %.arg_0, %vr164
  ret i32 %vr161
}

define i32 @mysqr(i32 %.arg_0) {
block_6:
  %vr169 = mul i32 %.arg_0, %.arg_0
  ret i32 %vr169
}

define i32 @main() {
block_8:
  %vr182 = load i32, i32* @gg, align 4
  %vr183 = alloca i32, align 4
  call void @store555(i32* %vr183)
  %vr186 = add i32 %vr182, 1
  %vr188 = icmp eq i32 %vr186, 7
  br i1 %vr188, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr190 = load i32, i32* %vr183, align 4
  %vr192 = add i32 %vr190, 3
  store i32 %vr192, i32* %vr183, align 4
  store i32 25, i32* @gg, align 4
  br label %block_11

block_10:                                         ; preds = %block_8
  %vr196 = call i32 @add(i32 %vr186, i32 5)
  br label %block_11

block_11:                                         ; preds = %block_10, %block_9
  %vr365 = phi i32 [ %vr186, %block_9 ], [ %vr196, %block_10 ]
  %vr198 = load i32, i32* %vr183, align 4
  %vr199 = call i32 @add(i32 %vr365, i32 %vr198)
  store i32 %vr199, i32* %vr183, align 4
  %vr200 = alloca [16 x i8], align 1
  %vr201 = bitcast [16 x i8]* %vr200 to i8*
  %vr206 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr207 = add i64 %vr206, 1
  %0 = call i8* @memcpy(i8* %vr201, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr207)
  %vr209 = bitcast [16 x i8]* %vr200 to i8*
  %vr210 = ptrtoint i8* %vr209 to i64
  %vr211 = add i64 %vr210, 2
  %vr212 = inttoptr i64 %vr211 to i8*
  store i8 115, i8* %vr212, align 1
  %vr214 = bitcast [16 x i8]* %vr200 to i8*
  %vr215 = ptrtoint i8* %vr214 to i64
  %vr216 = add i64 %vr215, 1
  %vr217 = inttoptr i64 %vr216 to i8*
  store i8 101, i8* %vr217, align 1
  %vr218 = bitcast [16 x i8]* %vr200 to i8*
  %vr219 = load i32, i32* %vr183, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr218, i32 %vr219)
  %vr221 = alloca %struct12, align 8
  %vr222 = load %struct12, %struct12* %vr221, align 4
  %vr223 = insertvalue %struct12 %vr222, i32 111, 0
  store %struct12 %vr223, %struct12* %vr221, align 4
  %vr224 = load %struct12, %struct12* %vr221, align 4
  %vr225 = insertvalue %struct12 %vr224, i32 222, 1
  store %struct12 %vr225, %struct12* %vr221, align 4
  %vr226 = load %struct12, %struct12* %vr221, align 4
  %vr227 = extractvalue %struct12 %vr226, 0
  %vr228 = load %struct12, %struct12* %vr221, align 4
  %vr229 = extractvalue %struct12 %vr228, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr227, i32 %vr229)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str3, i32 0, i32 0), i8 97)
  br label %block_12

block_12:                                         ; preds = %block_13, %block_11
  %vr374 = phi i32 [ 10, %block_11 ], [ %vr246, %block_13 ]
  %6 = icmp ne i32 %vr374, 0
  br i1 %6, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr246 = sub i32 %vr374, 1
  br label %block_12

block_14:                                         ; preds = %block_12
  %vr247 = alloca [4 x i32], align 4
  br label %block_15

block_15:                                         ; preds = %block_16, %block_14
  %vr377 = phi i32 [ 0, %block_14 ], [ %vr275, %block_16 ]
  %vr251 = icmp slt i32 %vr377, 4
  br i1 %vr251, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr257 = mul i32 %vr377, %vr377
  %vr259 = bitcast [4 x i32]* %vr247 to i32*
  %vr263 = zext i32 %vr377 to i64
  %vr265 = shl i64 %vr263, 2
  %vr267 = ptrtoint i32* %vr259 to i64
  %vr269 = add i64 %vr267, %vr265
  %vr271 = inttoptr i64 %vr269 to i32*
  store i32 %vr257, i32* %vr271, align 4
  %vr275 = add i32 %vr377, 1
  br label %block_15

block_17:                                         ; preds = %block_15
  br label %block_18

block_18:                                         ; preds = %block_21, %block_17
  %vr389 = phi i32 [ 0, %block_17 ], [ %vr283, %block_21 ]
  %vr279 = icmp slt i32 %vr389, 4
  br i1 %vr279, label %block_19, label %block_20

block_19:                                         ; preds = %block_18
  %vr285 = bitcast [4 x i32]* %vr247 to i32*
  %vr289 = zext i32 %vr389 to i64
  %vr291 = shl i64 %vr289, 2
  %vr293 = ptrtoint i32* %vr285 to i64
  %vr295 = add i64 %vr293, %vr291
  %vr297 = inttoptr i64 %vr295 to i32*
  %vr299 = load i32, i32* %vr297, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr299)
  br label %block_21

block_20:                                         ; preds = %block_18
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_22

block_21:                                         ; preds = %block_19
  %vr283 = add i32 %vr389, 1
  br label %block_18

block_22:                                         ; preds = %block_23, %block_20
  %vr379 = phi i32 [ 3, %block_20 ], [ %vr308, %block_23 ]
  br label %block_23

block_23:                                         ; preds = %block_22
  %vr308 = sub i32 %vr379, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr308)
  %vr320 = icmp eq i32 %vr308, 0
  br i1 %vr320, label %block_24, label %block_22

block_24:                                         ; preds = %block_23
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_28

block_28:                                         ; preds = %block_27, %block_24
  %vr382 = phi i32 [ 0, %block_24 ], [ %vr383, %block_27 ]
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr382)
  %vr333 = icmp eq i32 %vr382, 0
  br i1 %vr333, label %block_30, label %block_31

block_30:                                         ; preds = %block_28
  br label %block_32

block_31:                                         ; preds = %block_28
  %vr339 = sub i32 %vr382, 1
  br label %block_32

block_32:                                         ; preds = %block_31, %block_30
  %vr383 = phi i32 [ 2, %block_30 ], [ %vr339, %block_31 ]
  br label %block_27

block_27:                                         ; preds = %block_32
  %vr325 = icmp ne i32 %vr383, 0
  br i1 %vr325, label %block_28, label %block_29

block_29:                                         ; preds = %block_27
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_33

block_33:                                         ; preds = %block_36, %block_29
  %vr392 = phi i32 [ 0, %block_29 ], [ %vr344, %block_36 ]
  br label %block_34

block_34:                                         ; preds = %block_33
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr392)
  %vr356 = icmp eq i32 %vr392, 4
  br i1 %vr356, label %block_35, label %block_36

block_35:                                         ; preds = %block_34
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  %vr359 = call i32 @mysqr(i32 5)
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str6, i32 0, i32 0), i32 %vr359)
  br label %block_40

block_36:                                         ; preds = %block_34
  %vr344 = add i32 %vr392, 1
  br label %block_33

block_40:                                         ; preds = %block_35
  ret i32 %vr365
}

