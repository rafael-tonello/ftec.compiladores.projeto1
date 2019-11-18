# Compiler written for the "compilers" discipline of the Computer Engineering course.


Grammar definition

```c++
    // start
    <E1>-> <var> | <E2>
    <E2> -> "while" | "if" | "(" | "{" | TokenName + "=" +<attribDef>
    <attribDef>-> <TokenNameOrData> | <TokenNameOrData> + <mathoperators> + <attribDef>
    <mathOperators> = + | - | * | / | ^
    <TokenNameOrData> -> TokenName | number | string

    //var declaration
    <var> -> "var" + <var declaration>
    <var declaration> -> "int" | "real" | <E1>
    "int" -> TokenName + "," + "int" | TokenName + "\n" + <var declaration>
    "real" -> TokenName + "," + "real" | TokenName + "\n" + <var description>

    //while block
    "while"-> <blockOfLogic> + <blockOfCode>
    <blockOfCode> -> "{" +<E2> + "}"

    //if block
    "if"-> <blockOfLogic> + <blockOfCode>
    <blockOfCode> -> "{" +<E2> + "}"

    <blockOfLogic> -> "(" + <parentesisD> + ")"

    <parentesisD> -> <blockOfLogic> | <TokenNameOrData> + <LogicOperator> + <TokenNameOrData>
    <logicOperator> -> > | < | >= | <= | == | !=
```

An example of code that should be interpreted

```c++
    var
        int cont, num
        real cont2 
        
    num = 0

    while(cont < 10) {
        cont2 = 3.1415 * contador ^ 2 
        if(cont < 5){
            num = num + cont2
        }
        else{
            cont = 0
        }
        
        cont = cont + 1
    }
```

