misc(杂项) -- 各种作为试验API性质/课程实验的代码
====

## Crypto
S-DES实现、简单的RSA实现

* * *

## TinyPlus
Tiny+ 词法分析器

* * *

## PythonCallC
Python 和 C 之间交互的demo程序

* * *

## PKI 实验

### 实验一
使用`OpenSSL`的二进制文件建立自己的CA  

* 准备实验环境, 编写配置文件
* 生成根证书
* 生成普通用户的秘钥对(.key文件), 并生成证书请求
* CA给普通用户签发个人证书

### 实验二
使用`OpenSSL`提供的库函数来:

* 读取个人数字证书内容
* 验证普通用户证书中CA的数字签名是否有效

### 实验三
使用`OpenSSL`提供的库函数来:

* 生成RSA密钥对, 保存公钥私钥
* 对文件进行Hash(这里使用MD5), 并使用生成的私钥对其进行签名, 保存签名到文件中
* 使用公钥对生成的数字签名进行验证
