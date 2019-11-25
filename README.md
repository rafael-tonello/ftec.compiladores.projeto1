# Some informations about this project
Compiler written for the "compilers" discipline of the Computer Engineering
course.

# How to compile
To compile this project, you need to make the file "compile.sh" a executable.
To do this, just run this code at your terminal:

```shell
    chmod +x ./compile.sh
```

After this, you just need to run the script "compile.sh":

```shell
    ./compile.sh
```

If all run without problems, a file called MyCompiler.app will appears in the
folder 'bin'.

# Grammar definition

```c++
    // start
    <E1>-> <var> | <E2>
    <E2> -> <while> | <if> | "{" + E2 + "}" | TokenName + "=" +<attribDef>
    <attribDef>-> <TokenNameOrData> | <TokenNameOrData> + <mathoperators> + <attribDef>
    <mathOperators> = + | - | * | / | ^
    <TokenNameOrData> -> TokenName | number | string

    //var declaration
    <var> -> "var" + <var declaration>
    <var declaration> -> "\t" + <int> | "\t" + <real> | "\n" + <E2>
    <int> -> "int" + <TokenNamesList> + "\n" + <var declaration>
    <real> -> "real" + <TokenNamesList> + "\n" + <var declaration>
    <TokenNamesList> -> TokenName | Tokename + "," + <TokenNamesList>

    //while block
    <while>-> "while" + <blockOfLogic> + <blockOfCode>
    <blockOfCode> -> "{" +<E2> + "}"

    //if block
    <if>-> <blockOfLogic> + <blockOfCode>
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

