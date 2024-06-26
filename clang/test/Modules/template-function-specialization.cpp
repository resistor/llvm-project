// RUN: rm -fr %t
// RUN: mkdir %t
// RUN: split-file %s %t
//
// RUN: %clang_cc1 -std=c++20 -emit-module-interface %t/foo.cppm -o %t/foo.pcm
// RUN: %clang_cc1 -std=c++20 -fprebuilt-module-path=%t %t/Use.cpp -verify -fsyntax-only

// RUN: %clang_cc1 -std=c++20 -emit-reduced-module-interface %t/foo.cppm -o %t/foo.pcm
// RUN: %clang_cc1 -std=c++20 -fprebuilt-module-path=%t %t/Use.cpp -verify -fsyntax-only -DREDUCED

//--- foo.cppm
module;
# 3 __FILE__ 1 // use the next physical line number here (and below)
template <typename T>
void foo() {
}

template <>
void foo<int>() {
}

template <typename T>
void foo2() {
}

template <>
void foo2<int>() {
}

template <typename T>
void foo3() {
}

template <>
void foo3<int>();

export module foo;
export using ::foo;
export using ::foo3;

export template <typename T>
void foo4() {
}

export template <>
void foo4<int>() {
}

//--- Use.cpp
import foo;
void use() {
  foo<short>();
  foo<int>();
#ifdef REDUCED
  // In reduced BMI, the foo2 template function is optimized out.
  foo2<short>(); // expected-error {{use of undeclared identifier 'foo2'}}
  foo2<int>();   // expected-error {{use of undeclared identifier 'foo2'}}
#else
  foo2<short>(); // expected-error {{missing '#include'; 'foo2' must be declared before it is used}}
                 // expected-note@* {{declaration here is not visible}}
  foo2<int>();   // expected-error {{missing '#include'; 'foo2' must be declared before it is used}}
                 // expected-note@* {{declaration here is not visible}}
#endif
  foo3<short>();
  foo3<int>();

  foo4<short>();
  foo4<int>();
}
