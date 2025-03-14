# Common files

These files are used in all compilation targets. So do **NOT** do anything platform dependent.

If you have something platform dependent that needs to be used globally, I have set it up so that, depending on the target, only `windows_specific` or `linux_specific` will be added to the include paths, as such, you can segregate your code. The best option is to have a single .h file in `common/` and seperate .c files in the platform specific directories. The next best is to create seperate .h files that preform about the same.
