## json数据格式校验

json的验证器也是json格式：


json数据如下：

```json
{
    "name":"zhang",
    "age":18,
    "goodat":["basketball","pingpong"]
    "feature":{
        "tall":180,
        "weight":70
    }
}
```

校验的json如下：

```json
{
    "name":"string [0,30]",
    "age":"int [0,100]",
    "goodat":["optinal string"],
    "feature":{
        "tall":"int (0,300)",
        "weight":"int (0,300)"
    }
}
```

##### 数据类型

​	int,float,bool,string

##### 是否可选，默认为必选
​	optional 可省略为opt

##### 范围区间表示

​	(1,100) 表示 1 < x < 100
​	[1,100) 表示 1 <= x < 100
​	(1,100] 表示 1 < x <= 100
​	[1,100] 表示 1 <= x <= 100
​	{1,2,3,4,5} 表示在集合内

##### 示例如下：

1. int
2. int [1,100]
3. optioal int (1,100)
4. float (0,1)
5. string [10,20]  表示字符串长度范围





