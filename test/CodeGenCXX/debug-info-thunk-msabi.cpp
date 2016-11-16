// RUN: %clang_cc1 %s -triple i386-pc-windows-msvc19.0.0 -emit-llvm \
// RUN:     -debug-info-kind=line-tables-only -fms-extensions -o - | FileCheck %s
class __declspec(dllexport) A {
  A(int * = new int) {}
};
// CHECK: define {{.*}}void @"\01??_FA@@AAEXXZ"
// CHECK-SAME: !dbg ![[SP:[0-9]+]]
// CHECK-NOT: {{ret }}
// CHECK: call x86_thiscallcc %class.A* @"\01??0A@@AAE@PAH@Z"(%class.A* %this1, i32* %0)
// CHECK-SAME: !dbg ![[DBG:[0-9]+]]
// CHECK: ret void, !dbg ![[DBG1:[0-9]+]]
//
// CHECK: ![[SP]] = distinct !DISubprogram(
// CHECK-SAME:          line: 4
// CHECK-SAME:          isDefinition: true
// CHECK-SAME:          DIFlagArtificial
// CHECK-SAME:          ){{$}}
//
// CHECK: ![[DBG1]] = !DILocation(line: 0
// CHECK: ![[DBG]] = !DILocation(line: 0
