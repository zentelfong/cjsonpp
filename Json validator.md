
参考https://gitee.com/photondragon/webgeeker-validation

json的验证器也是json格式：

json数据如下：

{
    "name":"zhang",
    "age":18,
    "goodat":["basketball","pingpong"]
    "feature":{
        "tall":180,
        "weight":70
    }
}

校验的json如下：

{
    "name":"Str",
    "age":"IntIn:0,100",
    "goodat":["Str"],
    "feature":{
        "tall":"IntIn:0,300",
        "weight":"IntIn:0,300"
    }
}




