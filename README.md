# Some informations about this project
Compiler written for the "compilers" discipline of the Computer Engineering
course.

# How to compile
To compile this project, you need to navigate (in your terminal) to project root folder and make the file "compile.sh" an executable.
To do it, just run this code in your terminal:

```shell
    cd /path/to/project/root/directory
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
    [■] <E2> -> <while> | <if> | <blockOfCode> | TokenName + "=" +<attribDef> | {exit}
    //<attribDef>-> <attribDef2> | <attribDef2> + ("+"|"-") + <TokenNameOrData>
    //<attribDef2>-> <TokenNameOrData> | <attribDef2> + ("*"|"/") + <attribDef2>
    [ ] <attribDef> -> string | <m1>
    [ ] <m1> -> <m2> + ('+' | '-') + <m2> + '^' <m1> | 
                <m2> + ('+' | '-') <m2> + ('+'|'-')> <m1> |
                <m2> + ('+'|'-') <m2> |
                <m2>

    [ ] <m2> -> 
                <m3> + ('*'|'/') <m3> + ('*'|'/') <m1> |
                <m3> + ('*'|'/') <m3> |
                <m3>


    [ ] <m3> -> <m4> + '^' + <m4> |
                <m4> + '^' + <m4> +'^' + <m1> |
                <m4>
    [ ] <m4> -> "(" + <m1> +")" | <TokenNameOrData>

    [■] <TokenNameOrData> -> TokenName | number | string
    <attribEnd> -> TokenNameOrData + \n | {something} \n

    //var declaration
    [■] <var> -> "var"  + \n + <var declaration>
    [■] <var declaration> -> "\t" + <int> | "\t" + <real> | "\n" {exit}
    [■] <int> -> "int" + <TokenNamesList> + "\n" + <var declaration>
    [■] <real> -> "real" + <TokenNamesList> + "\n" + <var declaration>
    [■] <TokenNamesList> -> TokenName + "\n" | Tokename + "," + <TokenNamesList>

    //while block
    [■] <while>-> "while" + <blockOfLogic> + <blockOfCode>
    [■] <blockOfCode> -> "{" +<E2> + "}"

    //if block
    [■] <if>-> <blockOfLogic> + <blockOfCode>

    [■] <blockOfLogic> -> "(" + <parentesisD> + ")"
    [■] <logicTokenNameOrData> -> attribDef | blockOfLogic

    [■] <parentesisD> -> <attribDef> + <LogicOperator> + <attribDef>
    [■] <logicOperator> -> > | < | >= | <= | == | !=
```

An example of code that should be interpreted

```c++
    var
        int cont, num
        real cont2 
        num = 0

    while(cont < 10) {
        cont2 = 3.1415 * cont ^ 2 
        if(cont < 5){
            num = num + cont2
        }
        else{
            cont = 0
        }
        
        cont = cont + 1
    }
```

An example of output
```assembly
DEFINE INT CONT
DEFINE INT NUM
DEFINE REAL CONT2
ATTRIB NUM 0
:TMP_01
CHECK_EQUALS CONT 10 TMP_02
GOTO TMP_03
:TMP_02
MATH_POW CONT 2 TMP_05
MATH_MUL TEMP_05 3.1415 TMP_06
ATTRIB CONT2 TMP_06
CHECK_LESSTHAN CONT 5 TEMP_07
GOTO TMP_08
:TMP_07
MATH_SUM NUM CONT2 TMP_10
ATTRIB NUM TMP_10
GOTO TMP_09
:TMP_08
ATTRIB CONT 0
:TMP_09
MATH_SUM CONT 1 TMP_11
ATTRIB CONT TMP_11
GOTO TMP_01
GOTO :TMP_04
:TMP_03
:TMP_04

```

The same example of output, but idented and commented
```assembly
;int cont, num
DEFINE INT CONT
DEFINE INT NUM
;real cont2
DEFINE REAL CONT2

;num = 0
ATTRIB NUM 0

;while
:TMP_01
    ;(cont < 10)
    CHECK_EQUALS CONT 10 TMP_02
    GOTO TMP_03
    
    ;{
    :TMP_02
        ;cont2 = 3.1415 * cont ^ 2
        MATH_POW CONT 2 TMP_05
        MATH_MUL TEMP_05 3.1415 TMP_06
        ATTRIB CONT2 TMP_06

        ;if (cont < 5)
        CHECK_LESSTHAN CONT 5 TEMP_07
        GOTO TMP_08
        ;{
        :TMP_07
            ;num = num + cont2
            MATH_SUM NUM CONT2 TMP_10
            ATTRIB NUM TMP_10

        GOTO TMP_09
        ;}else{
        :TMP_08
            ;cont = 0
            ATTRIB CONT 0
        
        ;}
        :TMP_09
        ;cont = cont +1    
        MATH_SUM CONT 1 TMP_11
        ATTRIB CONT TMP_11
        GOTO TMP_01
    ;}
    GOTO :TMP_04
    :TMP_03

:TMP_04
```


<m1> -> 
     m1.1->   <m2> + '+-' <m2> + '^' + <m1> |
       --------------------------------- <m2> + '+-' <m2> + '*/' <m1> 
     m1.3->   <m2> + '+-' <m2> + '+-' <m1> |
     m1.4->   <m2> + '+-' <m2> |
     m1.5->   <m2>

<m2> -> 
   ---------------------------------- m2.1-> <m3> + '*/' <m3> + '^' + <m1>
    m2.2-> <m3> + '*/' <m3> + '*/' <m1> |
    m2.3-> <m3> + '*/' <m3> |
    m2.4-> <m3>


<m3> -> <m4> + '^' + <m4>
        <m4> + '^' + <m4> +'^' + <m1>


<m4> -> "(" + <m1> +")" | nordt

