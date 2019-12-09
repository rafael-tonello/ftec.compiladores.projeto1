# Some informations about this project
Compiler written for the "compilers" discipline of the Computer Engineering
course.

# How to compile
To compile this project, you need to make the file "compile.sh" an executable.
To do it, just run this code in your terminal:

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
    // The "E1" is the code entry point
    [◧] <E1>-> <var> + <E2>| <E2>
    [ ] <E2> -> <while> | <if> | <blockOfCode> | TokenName + "=" +<attribDef> | {exit}
    //<attribDef>-> <attribDef2> | <attribDef2> + ("+"|"-") + <TokenNameOrData>
    //<attribDef2>-> <TokenNameOrData> | <attribDef2> + ("*"|"/") + <attribDef2>
    [ ] <attribDef>-> <attribDef2> | <attribDef2> + <mathLevel1> + <TokenNameOrData>
    [ ] <attribDef2>-> <TokenNameOrData> | <attribDef3> + <mathLevel2> + <attribDef>
    [ ] <attribDef2>-> <TokenNameOrData> | <attribDef3> + <mathLevel3> + <attribDef2>
    [ ] <mathLevel1> -> + | - 
    [ ] <mathLevel2> -> * | / | 
    [ ] <mathLevel3> -> ^
    [ ] <TokenNameOrData> -> TokenName | number | string

    //var declaration
    [■] <var> -> "var"  + \n + <var declaration>
    [■] <var declaration> -> "\t" + <int> | "\t" + <real> | "\n" {exit}
    [■] <int> -> "int" + <TokenNamesList> + "\n" + <var declaration>
    [■] <real> -> "real" + <TokenNamesList> + "\n" + <var declaration>
    [■] <TokenNamesList> -> TokenName + "\n" | Tokename + "," + <TokenNamesList>

    //while block
    [ ] <while>-> "while" + <blockOfLogic> + <blockOfCode>
    [ ] <blockOfCode> -> "{" +<E2> + "}"

    //if block
    [ ] <if>-> <blockOfLogic> + <blockOfCode>

    [ ] <blockOfLogic> -> "(" + <parentesisD> + ")"
    [ ] <logicTokenNameOrData> -> attribDef | blockOfLogic

    [ ] <parentesisD> -> <blockOfLogic> | <logicTokenNameOrData> + <LogicOperator> + <logicTokenNameOrData> 
    [ ] <logicOperator> -> > | < | >= | <= | == | !=
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

