![image](https://github.com/Amir-BK/Unreal_SwitchOnVector/assets/6035249/f0e4e40f-633a-49cd-a231-42d12cceb602)


![image](https://github.com/Amir-BK/Unreal_SwitchOnVector/assets/6035249/fa6d96e6-f615-41f2-8d76-1b62ffb9f909)

![image](https://github.com/Amir-BK/ExtraBlueprintSwitchStatements/assets/6035249/fff1f6a4-8669-439f-98e6-c63462e25908)


![image](https://github.com/Amir-BK/ExtraBlueprintSwitchStatements/assets/6035249/b8ea6d0c-1e2c-4a50-bfba-b14586a24b18)

Switch on Int array (allows switching on an arbitrary collection of ints rather than consecutive ints as with the default switch on int node):

![image](https://github.com/Amir-BK/ExtraBlueprintSwitchStatements/assets/6035249/ca89a1de-2bc4-491a-9425-1af55af783fd)


Custom blueprint nodes which inherit from K2Node_Switch and extend it.

K2Node_Switch creates a map of 'boolterms' that map between the output execution pins and functions that can be evaluated to a boolean, when the default node is shown and connected all other statements need to be evaluated, only if they return false will the default fire. The inner working of the K2Node_Switch are a bit confusing but the various examples in this repo should be helpful, essentially the BP compiler 'collects' the value assosciated with the pin through the 'GetTextForExport' methods we return a string from which the compiler can reconstruct our desired variable, as the comparator method itself needs to be static we use this string to pass on 'extra data' such as the tolerance in the float functions, etc, any function that can be made static and can return a bool can be used to create a blueprint switch statement. 
