# Evolution of Sparkler
## Environment

Type | Version
:--: | :--:
Operating System | Windows 10
Compiler | Visual C++ 2019
GUI | ImGui 1.81
Back-end | GLFW & OpenGL 3
Maths | GLM 0.9.9.8

## 命名规范
### **1. 代码**
* 变量相关
  * 类型名：大驼峰、变量名：小驼峰（便于和类型名区分）
  * 常量名：全大写，下划线分割
  * 命名空间名：全小写
  * 模板类型名：单个大写字母
    * 含义应在注释中给出
  * 全局变量名：域作用符 `::` 开头
    * 尽量避免全局变量，用`单例 Singleton`
  * 常用命名
    * `get/setXXX` 直接访问属性
    * `computeXXX` 计算内容
    * `initializeXXX` **`【！】不要 initXXX`**
    * `GUI 的控件变量加上类型后缀`
      * 如 mainWindow、fileMenu、loginText……
    * `集合加 s`（数组）
    * 循环变量 i, j, k, etc
    * `前缀 is, has, can, should` 等用来表示 bool 变量/函数方法

* 函数相关
  * 函数方法名：小驼峰，动词开头（和变量名形式一样，但函数需带参数列表，编译器可以区分）
  * 有返回类型的函数方法以返回内容命名，void 类型函数以 what they do 命名

* 类相关
  * 私有变量加 `_` 后缀（后缀比前缀有更好的可读性）
  * `前缀 prefix n` 表示对象的数量
  * `后缀 suffix No` 表示对象编号
  * 异常类要加`后缀 suffix with Exception`
    * 如 `class AccessException;`

* Tips
  * 用到简称和首字母缩写的时候按命名规范，不要用全大写
    * 如 `exportHtmlSource();`
  * 名字长度应和作用域成正相关
  * 类的方法名不要冗余
    * 如 `line.getLength(); // Not line.getLineLength();`
  * 布尔变量/函数名不要用否定
  * 不要缩写普通单词（init、cmd、comp(ute)），不要扩写常用缩写（HTML、CPU）
  * 用`对称词`
    * `get/set, add/remove, create/destroy, start/stop, insert/delete,`
    * `increment/decrement, old/new, begin/end, first/last, up/down, min/max,`
    * `next/previous, old/new, open/close, show/hide, suspend/resume, etc.`
* 常用
### **2. 文件**
* 文件名全小写，`_` 分割
* .h 声明，.cpp 定义
* 文件内容不超过 80 列
* 换行
  * 段落后换行
  * 运算符后换行
  * 新行与上一行表达式开头对齐
* 头文件保护
    ```cpp
    #ifndef CLASSNAME_H
    #define CLASSNAME_H
    #endif // CLASSNAME_H
    ```

## 单例模式
* [Stack Overflow - C++ Singleton design pattern](https://stackoverflow.com/questions/1008019/c-singleton-design-pattern)

# TODO Flag
* UI input usage
* Compare Areas

<!-- TODO: 5. statements 之后 -->

# References
* 代码格式化：[Clang-Format](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
* 命名规范：
  * [C++ Programming Style Guidelines](https://www.cise.ufl.edu/~mschneid/Research/C++%20Programming%20Style%20Guidelines.htm)
  * [Google Style Guides](https://github.com/google/styleguide)、[Google 开源项目风格指南 - 中文版](https://github.com/zh-google-styleguide/zh-google-styleguide)

