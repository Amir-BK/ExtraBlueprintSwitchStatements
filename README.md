![image](https://github.com/Amir-BK/Unreal_SwitchOnVector/assets/6035249/f0e4e40f-633a-49cd-a231-42d12cceb602)

Bonus! Switch on Float:

![image](https://github.com/Amir-BK/Unreal_SwitchOnVector/assets/6035249/fa6d96e6-f615-41f2-8d76-1b62ffb9f909)


A custom blueprint node which inherits from K2Node_Switch and extends it to compare between two vectors while allowing input of a tolerance value.

All the code is contained the single K2Node_SwitchOnVector header and cpp file, if you seek to extend this or make your own custom switch nodes, it wasn't obvious to me that the switch node creates the value for the case statement using the export text method, and this is also how I add the tolerance value to the 'inner case struct' so that the comparator function can be kept static.  
