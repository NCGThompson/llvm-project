// RUN: %clang_cc1 -triple i386-unknown-unknown -emit-llvm -disable-llvm-passes -Os -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple i386-unknown-unknown -emit-llvm -disable-llvm-passes -Os -std=c99 -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-unknown-unknown -emit-llvm -disable-llvm-passes -Os -std=c99 -o - %s | FileCheck %s
// CHECK: define{{.*}} signext i8 @f0(i32 noundef %x) [[NUW:#[0-9]+]]
// CHECK: define{{.*}} zeroext i8 @f1(i32 noundef %x) [[NUW]]
// CHECK: define{{.*}} void @f2(i8 noundef signext %x) [[NUW]]
// CHECK: define{{.*}} void @f3(i8 noundef zeroext %x) [[NUW]]
// CHECK: define{{.*}} signext i16 @f4(i32 noundef %x) [[NUW]]
// CHECK: define{{.*}} zeroext i16 @f5(i32 noundef %x) [[NUW]]
// CHECK: define{{.*}} void @f6(i16 noundef signext %x) [[NUW]]
// CHECK: define{{.*}} void @f7(i16 noundef zeroext %x) [[NUW]]

signed char f0(int x) { return x; }

unsigned char f1(int x) { return x; }

void f2(signed char x) { }

void f3(unsigned char x) { }

signed short f4(int x) { return x; }

unsigned short f5(int x) { return x; }

void f6(signed short x) { }

void f7(unsigned short x) { }

// CHECK-LABEL: define{{.*}} void @f8()
// CHECK: [[AI:#[0-9]+]]
// CHECK: {
void __attribute__((always_inline)) f8(void) { }

// CHECK: call void @f9_t()
// CHECK: [[NR:#[0-9]+]]
// CHECK: }
void __attribute__((noreturn)) f9_t(void);
void f9(void) { f9_t(); }

// CHECK: call void @f9a()
// CHECK: [[NR]]
// CHECK: }
_Noreturn void f9a(void);
void f9b(void) { f9a(); }

// FIXME: We should be setting nounwind on calls.
// CHECK: call i32 @f10_t()
// CHECK: [[NUW_RN:#[0-9]+]]
// CHECK: {
int __attribute__((const)) f10_t(void);
int f10(void) { return f10_t(); }
int f11(void) {
 exit:
  return f10_t();
}
int f12(int arg) {
  return arg ? 0 : f10_t();
}

// CHECK: define{{.*}} i32 @f13() [[NUW_OS_RN:#[0-9]+]]
int f13(void) __attribute__((const));
int f13(void){ return 0; }


// [irgen] clang isn't setting the optsize bit on functions
// CHECK-LABEL: define{{.*}} void @f15
// CHECK: [[NUW]]
// CHECK: {
void f15(void) {
}

// PR5254
// CHECK-LABEL: define{{.*}} void @f16
// CHECK: [[SR:#[0-9]+]]
// CHECK: {
void __attribute__((force_align_arg_pointer)) f16(void) {
}

// PR11038
// CHECK-LABEL: define{{.*}} void @f18()
// CHECK: [[RT:#[0-9]+]]
// CHECK: {
// CHECK: call void @f17()
// CHECK: [[RT_CALL:#[0-9]+]]
// CHECK: ret void
__attribute__ ((returns_twice)) void f17(void);
__attribute__ ((returns_twice)) void f18(void) {
        f17();
}

// CHECK-LABEL: define{{.*}} void @f19()
// CHECK: {
// CHECK: call i32 @setjmp(ptr noundef null)
// CHECK: [[RT_CALL]]
// CHECK: ret void
typedef int jmp_buf[((9 * 2) + 3 + 16)];
int setjmp(jmp_buf);
void f19(void) {
  setjmp(0);
}

// CHECK-LABEL: define{{.*}} void @f20()
// CHECK: {
// CHECK: call i32 @_setjmp(ptr noundef null)
// CHECK: [[RT_CALL]]
// CHECK: ret void
int _setjmp(jmp_buf);
void f20(void) {
  _setjmp(0);
}

// CHECK: attributes [[NUW]] = { nounwind optsize{{.*}} }
// CHECK: attributes [[AI]] = { alwaysinline nounwind optsize{{.*}} }
// CHECK: attributes [[NUW_OS_RN]] = { nounwind optsize willreturn memory(none){{.*}} }
// CHECK: attributes [[SR]] = { nounwind optsize{{.*}} "stackrealign"{{.*}} }
// CHECK: attributes [[RT]] = { nounwind optsize returns_twice{{.*}} }
// CHECK: attributes [[NR]] = { noreturn optsize }
// CHECK: attributes [[NUW_RN]] = { nounwind optsize willreturn memory(none) }
// CHECK: attributes [[RT_CALL]] = { optsize returns_twice }
