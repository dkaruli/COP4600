**Instructions Assignment 5**

-Steps-
1.Open Terminal
2.Move the directory the assignment files are located in by typing in terminal ->
cd ~/(name of folder containing assignment)/
3.hit Enter
4.type in terminal -> make
5.hit Enter
6.Step 4 will compile the file, proceed to installation
7.To install type in terminal -> sudo insmod char-device.ko
8.hit Enter
9.Type dmesg in terminal and hit enter
10. Step 9 will provide the module output till that point, do this to check if installation was succesfull and to retrieve the Major Number.
11.Perform tests
12.When finished uninstall
13.To uninstall type into terminal -> sudo rmmod char-device
14.hit enter
15.type dmesg in terminal and hit enter
16.step 15 will provide the log of the module until that point, check this to see if uninstalled properly
