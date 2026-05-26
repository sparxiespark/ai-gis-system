# LLM服务模块

本项目包含了一个完整的C++ LLM服务子系统实现。

## 目录结构

```
service/llm/
├── LLMInterface.h       - 大模型统一接口规范
├── MockLLM.h            - 模拟LLM头文件
├── MockLLM.cpp          - 模拟LLM实现
├── PromptManager.h      - 提示词管理器头文件
├── PromptManager.cpp    - 提示词管理器实现
├── ResponseParser.h     - 响应解析器头文件
├── ResponseParser.cpp   - 响应解析器实现
├── LLMService.h         - LLM服务门面类头文件
└── LLMService.cpp       - LLM服务门面类实现
```

## 主要组件说明

### 1. LLMInterface.h
- 定义大模型的统一接口
- 确保不同模型实现可替换
- 包含纯虚函数 `query()`

### 2. MockLLM
- 实现 LLMInterface 接口
- 用于测试和开发的模拟实现
- 返回带有"【LLM分析】"前缀的模拟响应

### 3. PromptManager
- 负责构建和拼接提示词
- 整合用户问题和数据模型上下文
- 生成完整的查询提示

### 4. ResponseParser
- 清洗和解析LLM返回结果
- 移除特定前缀标记
- 格式化输出内容

### 5. LLMService
- 外观模式的门面类
- 整合所有LLM相关功能
- 提供简洁的 `ask()` 接口

## 使用说明

### 依赖项
- C++11 或更高版本
- Qt框架 (用于QString等类型)
- DataModel类 (需在../../model/目录下)

### 编译注意事项
1. 确保包含Qt库路径
2. 链接必要的Qt模块
3. 使用UTF-8编码编译(已通过pragma指令设置)

### 基本用法

```cpp
#include "service/llm/LLMService.h"
#include "service/llm/MockLLM.h"

// 创建LLM实例
LLMInterface* pLLM = new MockLLM();

// 创建LLM服务(注入模型和数据模型)
LLMService llmService(pLLM, pDataModel);

// 提问
std::string answer = llmService.ask("请分析当前数据趋势");
```

## 设计模式

- **策略模式**: LLMInterface抽象接口
- **外观模式**: LLMService简化子系统访问
- **依赖注入**: 通过构造函数注入依赖

## 扩展性

要添加新的LLM实现:
1. 继承LLMInterface
2. 实现query()方法
3. 在创建LLMService时注入新实现

## 注意事项

- LLMService会负责删除注入的LLM实例
- DataModel以只读方式传递,不应修改
- 所有字符串编码为UTF-8
- 适配了MSVC编译器的UTF-8设置
