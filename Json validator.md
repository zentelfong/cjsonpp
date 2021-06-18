## json数据格式校验

​	在开发过程中使用json格式进行数据传输时，往往会遇到json的数据类型不正确或者缺少某些字段导致程序崩溃问题。解决这些问题往往是在取值时添加一些类型判断，而这些往往是繁琐的而且容易遗漏的，能不能有更好的方式解决这个问题呢！答案是肯定的，我们可以在解析json数据前，先使用schema来校验格式是不是我们需要的类型，如果不是直接返回错误，这样只需维护好schema即可大大增强程序稳定性。

​	json schema是复杂的而且不直观，下面介绍一种简单的json验证方式。json的验证器也是json格式如下：


json数据如下：

```json
{
    "name":"zhang",
    "age":18,
    "goodat":["basketball","pingpong"],
    "feature":{
        "tall":180,
        "weight":70
    }
}
```

校验的schema定义如下：

```json
{
    "name":"string [0,30]",
    "age":"int [0,100]",
    "goodat":["string"],
    "feature":{
        "tall":"optinal int (0,300)",
        "weight":"int (0,300)"
    }
}
```

通过以上可以看出数据和schema的格式是一致的，字段对应的字符串内容为数据格式，包括是否可选，数据类型和值的范围。

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

​	!{1,2,3} 表示不在集合内

​	

##### 示例如下：

1. int
2. int [1,100]
3. optioal int (1,100)
4. float (0,1)
5. string [10,20]  表示字符串长度范围

6. int >0 表示大于0



C++的json验证器实现

[github]: https://github.com/zentelfong/cjsonpp

