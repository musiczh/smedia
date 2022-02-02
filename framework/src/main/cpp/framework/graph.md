```json
{
  "services": [
    {
      "service": "service",
      "name": "name",
      "options": {
        "enable": true,
        "num": 1
      }
    }
  ],
  "executors": [
    {
      "executor": "executor",
      "name": "default",
      "options": {
        "threadNum": 1
      }
    }
  ],
  "nodes": [
    {
      "functor": "OESTexReaderFunctor",
      "name": "oesNode",
      "outputs": [
        "video:v1"
      ]
    },
    {
      "functor": "RenderFunctor",
      "name": "renderNode",
      "inputs": [
        "video:v1"
      ]
    }
  ]
}
```
# 图json配置文档

### 概述
图的结构以及功能需要通过json来进行配置，在init的时候传入内部自动进行解析，并生成一张图。

### 整体json结构
```json
{
  "services": [
    {}
  ],
  "executors": [
    {}
  ],
  "nodes": [
    {}
  ]
}
```
json最外层是一个json对象，内部包含了三个key，其对应的value类型均为数组：

- services：配置图中所有节点共享的类对象。如GLContext，在此对象中进行渲染环境的统一。
- executors：节点调度线程池。默认是通过广度优先单线程调度节点，可以通过此处配置不同的线程池来调度节点。
- nodes：图中的节点

### service
```json
{
  "services": [
    {
      "service": "service",
      "name": "name",
      "options": {
        "enable": true,
        "num": 1
      }
    }
  ],
  "executors": [],
  "nodes": []
}
```

service可以配置多个对象，用户通过继承接口开发自己需要的service类，供全局获取。

- service：service的类名，Graph会创建对应的对象并存储，供全局节点获取。**注意，service类需要先注册，否则无法被Graph创建** 。
- name：service的名称，获取的时候通过名称来获取。如果缺少此字段，则默认采用service 类名作为名称。如果存在多个相同的
  service缺少名称，则会报错。
- options：此处可以配置对service的一些初始化配置，暂时不支持代码动态配置，这些option最终会在service初始化作为参数
  传递进去。