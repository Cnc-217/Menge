###controller处理输入
1.命令仿真启动（非unity）  
    1.1 直接调用Menge程序启动  
    1.2 Menge初始化，与socketServer建立连接，从server端得到仿真参数，更新，然后开始仿真  
    
2.命令仿真启动（unity）  
    2.1 调用unity程序启动   
    2.2 同上
    
3.action接收（post）  
    3.1 根据post得到的

4.查看Menge是否启动成功：查看sim的client是否为none  
    


###Menge和服务端的json交互规范：
server to Menge:  
发送json={"info"="getData"} 接收json={"info"="xxxxx","data"=vector}
发送json={"info"="updateParameter","data"=matrix} 接收json={"info"="xxxxx"}


###服务端接收post的规范（只用post）
