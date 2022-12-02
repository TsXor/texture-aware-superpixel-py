## 编译
运行`setup.py build_ext --inplace`  
打包目前还没整好。但是已经测试可用。  
打包需要安装`cython`和`cmake`，还需要一个C编译器，`msvc`、`mingw`、`gcc`都行。  
(如果使用`mingw`，开头的指令后面需要加上`--compiler=mingw32`)  
当然不装cmake也行，但是你需要手动把`TSSP.c`编译成静态库放在`csrc`下。  

暂时不想整打包了，你可以在编译之后手动安装：删掉`TSSP/build`，然后把文件夹`TSSP`复制一份到你的python包目录下。  

## 速度和效果
HybridGradient部分还可以，但是超像素分块部分比较慢。  
然而不关我事，这段是作者写的C，我只是写了个wrapper。  
实际效果似乎不太好。  

## 一些细节
~~原来的`TSSP.c`是Matlab扩展，因此需要Matlab的`mex.h`，我整了个`fakemex.h`模仿其功能。~~  
现在已经去matlab化，`fakemex.h`留作纪念。  