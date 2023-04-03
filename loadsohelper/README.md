## 简介 ##

用于辅助加载so

## 使用方法 ##

1. 基于 load_so_helper 里面的基本结构定义自己的 so 结构体（参考lib.h）
2. 调用 LoadSOHelper 可以自动加载so里面的方法
3. so 使用完成后请调用 CloseSOHelper 释放对应的 so