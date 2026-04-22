# 🎨 模块化 QSS 与代码解耦指南

## 1. 核心思想：为什么拒绝大锅饭 (base.qss)
将所有的样式塞进一个 `base.qss` 会带来随着项目增长而导致的**命名冲突**、**文件臃肿**和**加载变慢**的问题。
优秀的商业级工程要求**模块化**：每个由业务构成的 Page（如 `MusicPage`，`VideoPage`），不仅要有对应的 `.cpp` 和 `.h`，还必须有专属的 `.qss` 文件。

*   `MusicPage.cpp` <---> `MusicPage.qss`
*   `VideoPage.cpp` <---> `VideoPage.qss`
*   `HardwareControlPage.cpp` <---> `HardwareControlPage.qss`

## 2. 改造步骤拆解

### 步骤 A：清理 C++ 中的“杂质”
在 C++ 代码中，我们绝对不再写 `setStyleSheet("...")`，我们只赋予组件 **ObjectName**（用于 QSS 选择器定位）和动态改变的 **Property**（用于 QSS 状态判定）。

**错误写法：**
```cpp
m_ledBtn->setStyleSheet("QPushButton { border-image: url(:/off.svg); }");
```
**正确写法：**
```cpp
m_ledBtn->setObjectName("LedButton"); // 绑定 QSS ID
m_ledBtn->setProperty("isOn", false);   // 绑定自定义状态
```

### 步骤 B：在模块初始化注入 QSS
在页面类的构造函数（或专门的 `initStyle()` 方法）加载私有样式表：
```cpp
QFile file(":/res/style/HardwareControlPage.qss");
if(file.open(QFile::ReadOnly)) {
    this->setStyleSheet(QLatin1String(file.readAll()));
    file.close();
}
```

### 步骤 C：书写专属 QSS
针对绑定的 `ObjectName` 和属性进行 UI 美化：
```css
/* HardwareControlPage.qss */
#LedButton[isOn="false"] {
    border-image: url(:/off.svg);
}
#LedButton[isOn="true"] {
    border-image: url(:/on.svg);
}
```

## 3. 注意防坑（漏更配置）
创建独立的 `.qss` 文件后，千万不要忘记把它们注册到 `assets/resources.qrc` 中，否则 Qt 构建系统无法将其打包进二进制，运行时将读不到样式！