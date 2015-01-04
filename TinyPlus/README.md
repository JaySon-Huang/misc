## 词法分析、语法分析、生成中间代码

### 词法分析
状态转移图见"token状态转化图.pdf"

### 语法分析
采用递归下降分析的方法(符合LL(1)文法)
表达式的解析用自底向上(算符优先文法)
```
program -> declarations stmt-sequence

declarations -> decl;declarations | e  
    decl -> type-specifier varlist  
        type-specifier -> int | bool | string  
        varlist        -> identifier [, varlist ]  

stmt-sequenct -> statement;[stmt-sequence]  
    statement -> if-stmt | repeat-stmt | while-stmt  
                  | assign-stmt | read-stmt | write-stmt  
        if-stmt     -> if logical-or-exp then stmt-sequence  
                        [ else stmt-sequence ] end  
        while-stmt  -> while logical-or-exp do stmt-sequenceend  
        repeat-stmt -> repeat stmt-sequence until logical-or-exp  
        read-stmt   -> read  identifier  
        write-stmt  -> write identifier  
        assign-stmt -> identifier := expression  
  
bool-exp -> logical-or-exp  
    logical-or-exp  -> logical-and-exp [ or logical-or-exp ]  
    logical-and-exp -> logical-not-exp [ and logical-and-exp ]  
    logical-not-exp -> comparison-exp  [ not expression ]  
    comparison-exp -> add-exp [ comparison-op comparison-exp ]  
        comparison-op  -> < | = | > | >= | <=   
    add-exp -> mul-exp [ addop add-exp ]  
        addop -> + |-  
    mul-exp -> factor [ mulop mul-exp ]  
        mulop -> * |/  
    factor -> number | string | identifier   
               | true | false | ( logical-or-exp )  
```


* FIXME
  * 语法错误:
    * 开始符号和后跟符号错误；
    * 标识符错误，例如 int 后面不是标识符;
    * 括号匹配错误，例如( 和 ) 不匹配
    * 符号错误，例如在赋值语句中，符号应该为 ‘:=’, 在比较表达式中符号应该为 ‘=’ 
  * 语义错误:
    * 使用标识符前没有声明，或标识符被多次声明；
    * 条件表达式的类型不是bool
    * 运算对象的类型不相同
    * 赋值号左边和右边的类型不相同
  * 短路运算
    * if, while, repeat 的布尔判别式是把整个表达式的值求出来再进行判断,没有用短路运算

* * *

* 编译命令

    g++ tinyp_structs.cpp symboltable.cpp lexical_handlers.cpp tinyp_lexer.cpp tinyp_parser.cpp syntax_tree.cpp tinyp_codegen.cpp main.cpp -o compiler -O2

* 运行
    
    ./compiler
    
