
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
# smedia框架使用文档

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

### Node

```json
{
  "services": [],
  "executors": [],
  "nodes": [
    {
      "functor": "CameraFunctor",
      "name": "cameraNode",
      "executor":"default",
      "outputs": [
        "video:v1"
      ]
    },
    {
      "functor": "OESTexReaderFunctor",
      "name": "oesNode",
      "inputs": [
        "video:v1"
      ]
    }

  ]
}
```

nodes标签代表的是整个图中的节点，每一个node为一个节点。
- functor: functor的注册名，此为框架寻找并创建对应类实例的名字。c++层框架提供了注册的逻辑，functor即为类名。而平台相关的functor则经Expander拓展接口，由外部自己负责创建。
- name: 指定节点的名字。每个节点都必须有独一无二的name，若指定了重复的name，会导致初始化失败。
- executor: 可以指定node执行的线程池,默认值为"default"，表示使用默认executor
- outputs & inputs: 节点的输出tag。这里需要画个篇幅介绍一下。

```json
{
  "nodes": [
    {
      "outputs": [
        "video:1:v1"
      ]
    },
    {
      "inputs": [
        "video:v1"
      ]
    }
  ]
}
```
inputs 和 outputs 标识的是一个节点的输入和输出端口，一个端口可以输出到多个其他的端口，也可以接收多个其他的端口的输入。一个字段的完整形态是：```tag:index:name``

- 表示一个端口采用两个字段：tag、index。tag类似于IP地址，而index类似于端口号。当一个节点需要输出时，需要指定tag和index，
  表示输出到指定端口。在json中的配置采用":"进行分开。index可以不指定，默认值为0，必须大于等于0。
- 最后一个字段为连接边的名字: name。把输出端口连接到输入端口，输出端口的数据就会流到输入端口，在json中采用name来标记这个连接。
  inputs or outputs 数组中每一个元素代表的就是一个连接，或者称为图的一条边
- 当省略index字段时，字段的组合为："tag:name"
- 由于inputs&outputs数组中表示的是连接，所以一个input肯定对应一个output，若name对应不上，则会解析失败。

### Executor

```json
{
  "services": [],
  "executors": [
    {
      "options": {
        "threadNum": 1
      }
    },
    {
      "executor": "ThreadPoolExecutor",
      "name": "multiThreadPool",
      "options": {
        "threadNum": 3
      }
    }
  ],
  "nodes": [
    {
      "functor": "CameraFunctor",
      "name": "cameraNode",
      "outputs": [
        "video:v1"
      ]
    },
    {
      "functor": "OESTexReaderFunctor",
      "name": "oesNode",
      "executor": "multiThreadPool",
      "inputs": [
        "video:v1"
      ]
    }
  ]
}
```

executor配置的是node调度的执行器。默认情况下，若没有设置executors，则内部会创建一个默认的executor来进行调度。
- executor: executor的注册名称。若不指定，则为内部的默认executor。
- name: executor的名称。若不指定。默认值为"default"。name为"default"的executor具有特殊意义，他表示所有node的默认执行器。
node中有executor字段，可以指定具体的executor，而没有指定的话，该字段的值默认为"default"。注意，只能有一个默认执行器，
  若存在两个没有name的executor执行器，则会发生名称冲突而解析失败。
- options: 对executor的配置，这个和node等相同，在executor初始化的时候会被传入。