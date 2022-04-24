; ModuleID = 'top'
source_filename = "top"

%struct12 = type { i32, i32 }

@.str0 = private unnamed_addr constant [9 x i8] c"R??: %d\0A\00", align 1
@.str1 = private unnamed_addr constant [7 x i8] c"%d %d\0A\00", align 1
@.str2 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str3 = private unnamed_addr constant [7 x i8] c"%c %c\0A\00", align 1
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
  %vr203 = load i32, i32* %.arg_0, align 4
  %vr204 = add i32 %vr203, 1
  store i32 %vr204, i32* %.arg_0, align 4
  ret void
}

define i32 @add(i32 %.arg_0, i32 %.arg_1) {
block_2:
  %vr211 = icmp eq i32 %.arg_0, %.arg_1
  br i1 %vr211, label %block_3, label %block_4

block_3:                                          ; preds = %block_2
  br label %block_4

block_4:                                          ; preds = %block_3, %block_2
  %vr219 = phi i32 [ %.arg_1, %block_2 ], [ %.arg_0, %block_3 ]
  %vr216 = add i32 %.arg_0, %vr219
  ret i32 %vr216
}

define i32 @mysqr(i32 %.arg_0) {
block_6:
  %vr224 = mul i32 %.arg_0, %.arg_0
  ret i32 %vr224
}

define i32 @main() {
block_8:
  %vr237 = load i32, i32* @gg, align 4
  %vr238 = alloca i32, align 4
  call void @store555(i32* %vr238)
  %vr241 = add i32 %vr237, 1
  %vr243 = icmp eq i32 %vr241, 7
  br i1 %vr243, label %block_9, label %block_10

block_9:                                          ; preds = %block_8
  %vr245 = load i32, i32* %vr238, align 4
  %vr247 = add i32 %vr245, 3
  store i32 %vr247, i32* %vr238, align 4
  store i32 25, i32* @gg, align 4
  br label %block_11

block_10:                                         ; preds = %block_8
  %vr251 = call i32 @add(i32 %vr241, i32 5)
  br label %block_11

block_11:                                         ; preds = %block_10, %block_9
  %vr475 = phi i32 [ %vr241, %block_9 ], [ %vr251, %block_10 ]
  %vr253 = load i32, i32* %vr238, align 4
  %vr254 = call i32 @add(i32 %vr475, i32 %vr253)
  store i32 %vr254, i32* %vr238, align 4
  %vr255 = alloca [16 x i8], align 1
  %vr256 = bitcast [16 x i8]* %vr255 to i8*
  %vr261 = call i64 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0))
  %vr262 = add i64 %vr261, 1
  %0 = call i8* @memcpy(i8* %vr256, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str0, i32 0, i32 0), i64 %vr262)
  %vr264 = bitcast [16 x i8]* %vr255 to i8*
  %vr265 = ptrtoint i8* %vr264 to i64
  %vr266 = add i64 %vr265, 2
  %vr267 = inttoptr i64 %vr266 to i8*
  store i8 115, i8* %vr267, align 1
  %vr269 = bitcast [16 x i8]* %vr255 to i8*
  %vr270 = ptrtoint i8* %vr269 to i64
  %vr271 = add i64 %vr270, 1
  %vr272 = inttoptr i64 %vr271 to i8*
  store i8 101, i8* %vr272, align 1
  %vr273 = bitcast [16 x i8]* %vr255 to i8*
  %vr274 = load i32, i32* %vr238, align 4
  %1 = call i32 (i8*, ...) @printf(i8* %vr273, i32 %vr274)
  %vr276 = alloca %struct12, align 8
  %vr277 = load %struct12, %struct12* %vr276, align 4
  %vr278 = insertvalue %struct12 %vr277, i32 111, 0
  store %struct12 %vr278, %struct12* %vr276, align 4
  %vr279 = load %struct12, %struct12* %vr276, align 4
  %vr280 = insertvalue %struct12 %vr279, i32 222, 1
  store %struct12 %vr280, %struct12* %vr276, align 4
  %vr281 = load %struct12, %struct12* %vr276, align 4
  %vr282 = extractvalue %struct12 %vr281, 0
  %vr283 = load %struct12, %struct12* %vr276, align 4
  %vr284 = extractvalue %struct12 %vr283, 1
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 %vr282, i32 %vr284)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 0x4007FFFFFE666666)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str2, i32 0, i32 0), double 3.000000e+00)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str1, i32 0, i32 0), i32 123, i32 -123)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.str3, i32 0, i32 0), i8 97, i8 97)
  br label %block_12

block_12:                                         ; preds = %block_13, %block_11
  %vr484 = phi i32 [ 10, %block_11 ], [ %vr303, %block_13 ]
  %7 = icmp ne i32 %vr484, 0
  br i1 %7, label %block_13, label %block_14

block_13:                                         ; preds = %block_12
  %vr303 = sub i32 %vr484, 1
  br label %block_12

block_14:                                         ; preds = %block_12
  %vr304 = alloca [4 x i32], align 4
  br label %block_15

block_15:                                         ; preds = %block_16, %block_14
  %vr487 = phi i32 [ 0, %block_14 ], [ %vr332, %block_16 ]
  %vr308 = icmp slt i32 %vr487, 4
  br i1 %vr308, label %block_16, label %block_17

block_16:                                         ; preds = %block_15
  %vr314 = mul i32 %vr487, %vr487
  %vr316 = bitcast [4 x i32]* %vr304 to i32*
  %vr320 = zext i32 %vr487 to i64
  %vr322 = shl i64 %vr320, 2
  %vr324 = ptrtoint i32* %vr316 to i64
  %vr326 = add i64 %vr324, %vr322
  %vr328 = inttoptr i64 %vr326 to i32*
  store i32 %vr314, i32* %vr328, align 4
  %vr332 = add i32 %vr487, 1
  br label %block_15

block_17:                                         ; preds = %block_15
  br label %block_18

block_18:                                         ; preds = %block_21, %block_17
  %vr499 = phi i32 [ 0, %block_17 ], [ %vr340, %block_21 ]
  %vr336 = icmp slt i32 %vr499, 4
  br i1 %vr336, label %block_19, label %block_20

block_19:                                         ; preds = %block_18
  %vr342 = bitcast [4 x i32]* %vr304 to i32*
  %vr346 = zext i32 %vr499 to i64
  %vr348 = shl i64 %vr346, 2
  %vr350 = ptrtoint i32* %vr342 to i64
  %vr352 = add i64 %vr350, %vr348
  %vr354 = inttoptr i64 %vr352 to i32*
  %vr356 = load i32, i32* %vr354, align 4
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr356)
  br label %block_21

block_20:                                         ; preds = %block_18
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_22

block_21:                                         ; preds = %block_19
  %vr340 = add i32 %vr499, 1
  br label %block_18

block_22:                                         ; preds = %block_23, %block_20
  %vr489 = phi i32 [ 3, %block_20 ], [ %vr365, %block_23 ]
  br label %block_23

block_23:                                         ; preds = %block_22
  %vr365 = sub i32 %vr489, 1
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr365)
  %vr377 = icmp eq i32 %vr365, 0
  br i1 %vr377, label %block_24, label %block_22

block_24:                                         ; preds = %block_23
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_28

block_28:                                         ; preds = %block_27, %block_24
  %vr492 = phi i32 [ 0, %block_24 ], [ %vr493, %block_27 ]
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr492)
  %vr390 = icmp eq i32 %vr492, 0
  br i1 %vr390, label %block_30, label %block_31

block_30:                                         ; preds = %block_28
  br label %block_32

block_31:                                         ; preds = %block_28
  %vr396 = sub i32 %vr492, 1
  br label %block_32

block_32:                                         ; preds = %block_31, %block_30
  %vr493 = phi i32 [ 2, %block_30 ], [ %vr396, %block_31 ]
  br label %block_27

block_27:                                         ; preds = %block_32
  %vr382 = icmp ne i32 %vr493, 0
  br i1 %vr382, label %block_28, label %block_29

block_29:                                         ; preds = %block_27
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  br label %block_33

block_33:                                         ; preds = %block_36, %block_29
  %vr502 = phi i32 [ 0, %block_29 ], [ %vr401, %block_36 ]
  br label %block_34

block_34:                                         ; preds = %block_33
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str4, i32 0, i32 0), i32 %vr502)
  %vr413 = icmp eq i32 %vr502, 4
  br i1 %vr413, label %block_35, label %block_36

block_35:                                         ; preds = %block_34
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str5, i32 0, i32 0))
  %vr416 = call i32 @mysqr(i32 5)
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str6, i32 0, i32 0), i32 %vr416)
  %vr418 = alloca [4 x i32], align 4
  %vr419 = bitcast [4 x i32]* %vr418 to i32*
  %vr420 = ptrtoint i32* %vr419 to i64
  %vr422 = inttoptr i64 %vr420 to i32*
  store i32 3, i32* %vr422, align 4
  %vr423 = bitcast [4 x i32]* %vr418 to i32*
  %vr424 = ptrtoint i32* %vr423 to i64
  %vr425 = add i64 %vr424, 4
  %vr426 = inttoptr i64 %vr425 to i32*
  store i32 0, i32* %vr426, align 4
  %vr427 = bitcast [4 x i32]* %vr418 to i32*
  %vr428 = ptrtoint i32* %vr427 to i64
  %vr429 = add i64 %vr428, 8
  %vr430 = inttoptr i64 %vr429 to i32*
  store i32 0, i32* %vr430, align 4
  %vr431 = bitcast [4 x i32]* %vr418 to i32*
  %vr432 = ptrtoint i32* %vr431 to i64
  %vr433 = add i64 %vr432, 12
  %vr434 = inttoptr i64 %vr433 to i32*
  store i32 0, i32* %vr434, align 4
  %vr435 = bitcast [4 x i32]* %vr418 to i32*
  %vr436 = ptrtoint i32* %vr435 to i64
  %vr437 = add i64 %vr436, 8
  %vr438 = inttoptr i64 %vr437 to i32*
  store i32 9, i32* %vr438, align 4
  %vr439 = bitcast [4 x i32]* %vr418 to i32*
  %vr440 = ptrtoint i32* %vr439 to i64
  %vr441 = add i64 %vr440, 4
  %vr442 = inttoptr i64 %vr441 to i32*
  store i32 6, i32* %vr442, align 4
  %vr443 = bitcast [4 x i32]* %vr418 to i32*
  %vr444 = ptrtoint i32* %vr443 to i64
  %vr445 = add i64 %vr444, 16
  %vr446 = inttoptr i64 %vr445 to i32*
  %vr447 = ptrtoint i32* %vr446 to i64
  %vr448 = sub i64 %vr447, 4
  %vr449 = inttoptr i64 %vr448 to i32*
  store i32 12, i32* %vr449, align 4
  %vr450 = bitcast [4 x i32]* %vr418 to i32*
  %vr451 = ptrtoint i32* %vr450 to i64
  %vr453 = inttoptr i64 %vr451 to i32*
  %vr454 = load i32, i32* %vr453, align 4
  %vr455 = bitcast [4 x i32]* %vr418 to i32*
  %vr456 = ptrtoint i32* %vr455 to i64
  %vr457 = add i64 %vr456, 4
  %vr458 = inttoptr i64 %vr457 to i32*
  %vr459 = load i32, i32* %vr458, align 4
  %vr460 = bitcast [4 x i32]* %vr418 to i32*
  %vr461 = ptrtoint i32* %vr460 to i64
  %vr462 = add i64 %vr461, 8
  %vr463 = inttoptr i64 %vr462 to i32*
  %vr464 = load i32, i32* %vr463, align 4
  %vr465 = bitcast [4 x i32]* %vr418 to i32*
  %vr466 = ptrtoint i32* %vr465 to i64
  %vr467 = add i64 %vr466, 12
  %vr468 = inttoptr i64 %vr467 to i32*
  %vr469 = load i32, i32* %vr468, align 4
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str7, i32 0, i32 0), i32 %vr454, i32 %vr459, i32 %vr464, i32 %vr469)
  br label %block_40

block_36:                                         ; preds = %block_34
  %vr401 = add i32 %vr502, 1
  br label %block_33

block_40:                                         ; preds = %block_35
  ret i32 %vr475
}

