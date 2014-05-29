gmcl_external
=============

Load files from anywhere on your computer.

Functions
=============

* includeExternal
* runEncrypted
* encrypt
* encryptFile

Usage
=============
**includeExternal** takes two arguments, the file or directory to include. If you specify a directory for the first argument, it will include every file in that directory. If you specify a file, it will only run that file. The second argument is a boolean. If it is true, it will recursively include every file in every directory in that directory.

**runEncrypted** takes one argument, the encrypted code to run. Returns false if an error occurs. 

**encrypt** takes one argument, the code to encrypt. It will copy the encrypted code to your clipboard. It also returns the encrypted code.

**encryptFile** has the same functionality as includeExternal, except it will not run the files, it will encrypt them.

**AN IMPORTANT THING TO NOTE IS THAT YOU SHOULD NOT HAVE BACKSLASHES IN THE PATHS. You could probably escape the backslashes, but it's a lot more easier to just use forwardslashes.** You should also NOT have any periods in the directory (besides the period preceding the lua extension).

C:\whatever\IDK\weiner.lua = no bueno

C:/whatever/IDK/weiner.lua = bueno, use it like this.
