; ModuleID = 'top'
source_filename = "top"

%struct12 = type { i32, i32 }

@.str0 = private unnamed_addr constant [9 x i8] c"R??: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str3 = private unnamed_addr constant [4 x i8] c"%c\0A\00", align 1
@.str4 = private unnamed_addr constant [4 x i8] c"%d \00", align 1
@.str5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@.str6 = private unnamed_addr constant [10 x i8] c"5^2 = %d\0A\00", align 1
@.str7 = private unnamed_addr constant [13 x i8] c"%d %d %d %d\0A\00", align 1
@gg = internal global i32 142, align 8

declare i32 @printf(i8*, ...)

declare i8* @memcpy(i8*, i8*, i64)

declare i64 @strlen(i8*)

declare i32 @atoi(i8*)

define void @store555(i32* %.arg_0) {
block_0:
  store i32 554, i32* %.arg_0, align 4
  %vr201 = load i32, i32* %.arg_0, align 4
  %vr202 = add i32 %vr201, 1
  store i32 %vr202, i32* %.arg_0, align 4
  ret void
}

define i32 @add(i32 %.arg_0, i32 %.arg_1) {
block_2:
  %vr209 = icmp eq i32 %.arg_0, %.arg_1
  br i1 %vr209, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  br label %block_4

block_4:                                          ; preds = %block_3, %block_2
  %vr217 = phi i32 [ %.arg_1, %block_2 ], [ %.arg_0, %block_3 ]
  %vr214 = add i32 %.arg_0, %vr217
  ret i32 %vr214
}

define i32 @mysqr(i32 %.arg_0) {
block_6:
  %vr222 = mul i32 %.arg_0, %.arg_0
  ret i32 %vr222
}

define i32 @main() {
block_8:
  %vr235 = load i32, i32* @gg, align 4
  %vr236 = alloca i32, align 4
  call void @store555(i32* %vr236)
  %vr239 = add i32 %vr235, 1
  %vr241 = icmp eq i32 %vr239, 7
  br i1 %vr241, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr243 = load i32, i32* %vr236, align 4
  %vr245 = add i32 %vr243, 3
  store i32 %vr245, i32* %vr236, align 4
  store i32 25, i32* @gg, align 4
  br label %block_11

block_10:                                         ; preds = %block_8
  %vr249 = call i32 @add(i32 %vr239, i32 5)
  br label %block_11

block_11:                                         ; preds = %block_10, %block_9
  %vr471 = phi i32 [ %vr239, %block_9 ], [ %vr249, %block_10 ]
  %vr251 = load i32, i32* %vr236, align 4
  %vr252 = call i32 @add(i32 %vr471, i32 %vr251)
  store i32 %vr252, i32* %vr236, align 4
  %vr253 = alloca [16 x i8], align 1
  %vr254 = bitcast [16 x i8]* %vr253 to i8*
  %vr259 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr260 = add i64 %vr259, 1
  %0 = call i8* @memcpy(i8* %vr254, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr260)
  %vr262 = bitcast [16 x i8]* %vr253 to i8*
  %vr263 = ptrtoint i8* %vr262 to i64
  %vr264 = add i64 %vr263, 2
  %vr265 = inttoptr i64 %vr264 to i8*
  store i8 115, i8* %vr265, align 1
  %vr267 = bitcast [16 x i8]* %vr253 to i8*
  %vr268 = ptrtoint i8* %vr267 to i64
  %vr269 = add i64 %vr268, 1
  %vr270 = inttoptr i64 %vr269 to i8*
  store i8 101, i8* %vr270, align 1
  %vr271 = bitcast [16 x i8]* %vr253 to i8*
  %vr272 = load i32, i32* %vr236, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr271, i32 %vr272)
  %vr274 = alloca %struct12, align 8
  %vr275 = load %struct12, %struct12* %vr274, align 4
  %vr276 = insertvalue %struct12 %vr275, i32 111, 0
  store %struct12 %vr276, %struct12* %vr274, align 4
  %vr277 = load %struct12, %struct12* %vr274, align 4
  %vr278 = insertvalue %struct12 %vr277, i32 222, 1
  store %struct12 %vr278, %struct12* %vr274, align 4
  %vr279 = load %struct12, %struct12* %vr274, align 4
  %vr280 = extractvalue %struct12 %vr279, 0
  %vr281 = load %struct12, %struct12* %vr274, align 4
  %vr282 = extractvalue %struct12 %vr281, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr280, i32 %vr282)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 123, i32 -123)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str3, i32 0, i32 0), i8 97)
  br label %block_12

block_12:                                         ; preds = %block_13, %block_11
  %vr480 = phi i32 [ 10, %block_11 ], [ %vr299, %block_13 ]
  %6 = icmp ne i32 %vr480, 0
  br i1 %6, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr299 = sub i32 %vr480, 1
  br label %block_12

block_14:                                         ; preds = %block_12
  %vr300 = alloca [4 x i32], align 4
  br label %block_15

block_15:                                         ; preds = %block_16, %block_14
  %vr483 = phi i32 [ 0, %block_14 ], [ %vr328, %block_16 ]
  %vr304 = icmp slt i32 %vr483, 4
  br i1 %vr304, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr310 = mul i32 %vr483, %vr483
  %vr312 = bitcast [4 x i32]* %vr300 to i32*
  %vr316 = zext i32 %vr483 to i64
  %vr318 = shl i64 %vr316, 2
  %vr320 = ptrtoint i32* %vr312 to i64
  %vr322 = add i64 %vr320, %vr318
  %vr324 = inttoptr i64 %vr322 to i32*
  store i32 %vr310, i32* %vr324, align 4
  %vr328 = add i32 %vr483, 1
  br label %block_15

block_17:                                         ; preds = %block_15
  br label %block_18

block_18:                                         ; preds = %block_21, %block_17
  %vr495 = phi i32 [ 0, %block_17 ], [ %vr336, %block_21 ]
  %vr332 = icmp slt i32 %vr495, 4
  br i1 %vr332, label %block_19, label %block_20

block_19:                                         ; preds = %block_18
  %vr338 = bitcast [4 x i32]* %vr300 to i32*
  %vr342 = zext i32 %vr495 to i64
  %vr344 = shl i64 %vr342, 2
  %vr346 = ptrtoint i32* %vr338 to i64
  %vr348 = add i64 %vr346, %vr344
  %vr350 = inttoptr i64 %vr348 to i32*
  %vr352 = load i32, i32* %vr350, align 4
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr352)
  br label %block_21

block_20:                                         ; preds = %block_18
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_22

block_21:                                         ; preds = %block_19
  %vr336 = add i32 %vr495, 1
  br label %block_18

block_22:                                         ; preds = %block_23, %block_20
  %vr485 = phi i32 [ 3, %block_20 ], [ %vr361, %block_23 ]
  br label %block_23

block_23:                                         ; preds = %block_22
  %vr361 = sub i32 %vr485, 1
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr361)
  %vr373 = icmp eq i32 %vr361, 0
  br i1 %vr373, label %block_24, label %block_22

block_24:                                         ; preds = %block_23
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_28

block_28:                                         ; preds = %block_27, %block_24
  %vr488 = phi i32 [ 0, %block_24 ], [ %vr489, %block_27 ]
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr488)
  %vr386 = icmp eq i32 %vr488, 0
  br i1 %vr386, label %block_30, label %block_31

block_30:                                         ; preds = %block_28
  br label %block_32

block_31:                                         ; preds = %block_28
  %vr392 = sub i32 %vr488, 1
  br label %block_32

block_32:                                         ; preds = %block_31, %block_30
  %vr489 = phi i32 [ 2, %block_30 ], [ %vr392, %block_31 ]
  br label %block_27

block_27:                                         ; preds = %block_32
  %vr378 = icmp ne i32 %vr489, 0
  br i1 %vr378, label %block_28, label %block_29

block_29:                                         ; preds = %block_27
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_33

block_33:                                         ; preds = %block_36, %block_29
  %vr498 = phi i32 [ 0, %block_29 ], [ %vr397, %block_36 ]
  br label %block_34

block_34:                                         ; preds = %block_33
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr498)
  %vr409 = icmp eq i32 %vr498, 4
  br i1 %vr409, label %block_35, label %block_36

block_35:                                         ; preds = %block_34
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  %vr412 = call i32 @mysqr(i32 5)
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str6, i32 0, i32 0), i32 %vr412)
  %vr414 = alloca [4 x i32], align 4
  %vr415 = bitcast [4 x i32]* %vr414 to i32*
  %vr416 = ptrtoint i32* %vr415 to i64
  %vr418 = inttoptr i64 %vr416 to i32*
  store i32 3, i32* %vr418, align 4
  %vr419 = bitcast [4 x i32]* %vr414 to i32*
  %vr420 = ptrtoint i32* %vr419 to i64
  %vr421 = add i64 %vr420, 4
  %vr422 = inttoptr i64 %vr421 to i32*
  store i32 0, i32* %vr422, align 4
  %vr423 = bitcast [4 x i32]* %vr414 to i32*
  %vr424 = ptrtoint i32* %vr423 to i64
  %vr425 = add i64 %vr424, 8
  %vr426 = inttoptr i64 %vr425 to i32*
  store i32 0, i32* %vr426, align 4
  %vr427 = bitcast [4 x i32]* %vr414 to i32*
  %vr428 = ptrtoint i32* %vr427 to i64
  %vr429 = add i64 %vr428, 12
  %vr430 = inttoptr i64 %vr429 to i32*
  store i32 0, i32* %vr430, align 4
  %vr431 = bitcast [4 x i32]* %vr414 to i32*
  %vr432 = ptrtoint i32* %vr431 to i64
  %vr433 = add i64 %vr432, 8
  %vr434 = inttoptr i64 %vr433 to i32*
  store i32 9, i32* %vr434, align 4
  %vr435 = bitcast [4 x i32]* %vr414 to i32*
  %vr436 = ptrtoint i32* %vr435 to i64
  %vr437 = add i64 %vr436, 4
  %vr438 = inttoptr i64 %vr437 to i32*
  store i32 6, i32* %vr438, align 4
  %vr439 = bitcast [4 x i32]* %vr414 to i32*
  %vr440 = ptrtoint i32* %vr439 to i64
  %vr441 = add i64 %vr440, 16
  %vr442 = inttoptr i64 %vr441 to i32*
  %vr443 = ptrtoint i32* %vr442 to i64
  %vr444 = sub i64 %vr443, 4
  %vr445 = inttoptr i64 %vr444 to i32*
  store i32 12, i32* %vr445, align 4
  %vr446 = bitcast [4 x i32]* %vr414 to i32*
  %vr447 = ptrtoint i32* %vr446 to i64
  %vr449 = inttoptr i64 %vr447 to i32*
  %vr450 = load i32, i32* %vr449, align 4
  %vr451 = bitcast [4 x i32]* %vr414 to i32*
  %vr452 = ptrtoint i32* %vr451 to i64
  %vr453 = add i64 %vr452, 4
  %vr454 = inttoptr i64 %vr453 to i32*
  %vr455 = load i32, i32* %vr454, align 4
  %vr456 = bitcast [4 x i32]* %vr414 to i32*
  %vr457 = ptrtoint i32* %vr456 to i64
  %vr458 = add i64 %vr457, 8
  %vr459 = inttoptr i64 %vr458 to i32*
  %vr460 = load i32, i32* %vr459, align 4
  %vr461 = bitcast [4 x i32]* %vr414 to i32*
  %vr462 = ptrtoint i32* %vr461 to i64
  %vr463 = add i64 %vr462, 12
  %vr464 = inttoptr i64 %vr463 to i32*
  %vr465 = load i32, i32* %vr464, align 4
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str7, i32 0, i32 0), i32 %vr450, i32 %vr455, i32 %vr460, i32 %vr465)
  br label %block_40

block_36:                                         ; preds = %block_34
  %vr397 = add i32 %vr498, 1
  br label %block_33

block_40:                                         ; preds = %block_35
  ret i32 %vr471
}

