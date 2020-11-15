# ESP_Transparent



适用于ESP8266、ESP32的串口透传固件

模块启动后启动TCP监听8080端口。

使用TCP客户端连接模块IP地址:8080端口，即可实现TCP <==> UART的数据透传



使用 [WiFiManager](https://github.com/tzapu/WiFiManager.git) 库进行配网，可以使用WEB页面进行模块配网

- 当您的ESP启动时，它将它设置为Station模式，并尝试连接到以前保存的访问点
- 如果不成功（或没有保存以前的网络），它将把ESP移至接入点模式并启动DNS和WebServer（默认ip 192.168.4.1）
- 使用具有浏览器功能的任何支持wifi的设备（计算机，电话，平板电脑）连接到新创建的访问点

