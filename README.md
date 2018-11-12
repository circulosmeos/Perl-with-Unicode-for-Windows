# Perl with Unicode for Windows

This repository contains instructions for recompiling the `perl.exe` executable **for Windows** (of the chosen Perl distribution), in order to make it **compatible with command line Unicode characters in Windows**:   
Please note that actual distributions have not cared for Unicode treatment of Unicode in Windows, where it uses a UTF-16 codification, whereas the Perl cmdline executable as ported from linux (where this Unicode issues doesn't happen!) can't cope UTF-16 (or any other codification) at all in Windows.

The original patch comes from *A. Sinan Unur*'s brightening post *[Fixing Perl's Unicode problems on the command line on Windows: A trilogy in N parts](https://www.nu42.com/2017/02/perl-unicode-windows-trilogy-one.html)* and it is licensed under [Artistic License 2.0](https://opensource.org/licenses/Artistic-2.0).

## Requirements

* Note that I'll be using *libre* [mingw-w64 gcc](https://mingw-w64.org/doku.php) compiler: in Windows it is installed using [Win-builds](http://win-builds.org/doku.php/download_and_installation_from_windows).

* A Perl installation: actually I've only checked this patch with [ActiveState Perl](https://www.activestate.com/activeperl), though it should be portable *as is* to any other distribution ([Strawberry](http://strawberryperl.com/), compiled from sources, or whatever). I'll try to compile this in other distributions asap. Version 5.24.2.2403 (from source) and 5.24.0 MSWin32-x64-multi-thread (from installation) has been tested. [Sources](http://downloads.activestate.com/ActivePerl/releases/) aren't needed as they're installed with the distribution you choose.

* *A. Sinan Unur*'s patch to runperl.c. It makes a wrapper around Perl's `main()` with an [additional function called `wmain()`](https://msdn.microsoft.com/en-us/library/bky3b5dh.aspx) which copes with translation from Windows' Unicode to Unicode UTF-8 codification used by Perl. You can use the runperl.c code of this repository, or apply the the `runperl.patch` patch to your original `runperl.c` from source: 
	`$ patch runperl.c runperl.patch`

## Compilation

Supposing (change paths as convenient):

* mingw-w64 installed at `C:\mingw-w64\` 
* Perl installed at `C:\Perl64\`
* (If Perl is not installed there, but just compiled from source there, without installing, change `-LC:\Perl64\bin` with `-LC:\Perl64`)
* runperl.c **patched** on current path

First, go to `C:\Perl64\bin` (`C:\Perl64` if Perl's been just compiled there), and copy `perl524.dll` as `libperl524.dll`:
	
	C:\Perl> cd bin
	C:\Perl\bin> copy perl524.dll libperl524.dll

Now, just `cd` to the location of your patched `runperl.c` file and execute:
	
	C:\mingw-w64\bin\gcc.exe -I C:\Perl64\lib\CORE -I C:\mingw-w64\x86_64-w64-mingw32\include -LC:\Perl64\lib\CORE -LC:\Perl64\bin -llibperl524 -municode runperl.c -o perl_unicode.exe

This will generate `perl_unicode.exe`.


## Test perl_unicode.exe

* Copy the previously noted `libperl524.dll` to the same path as `perl_unicode.exe`

* If Perl is not installed but just compiled, you will need to set this environment variable:   
	`set PERL5LIB=C:\Perl64\lib`
	
* Check output from this input command (it contains various Unicode chars as a parameter to `perl_unicode.exe` at the end):
	`perl_unicode.exe -Mopen=:std,:utf8 -E "say for @ARGV" kârlıの`   
Output should be:
    `kârlıの`

Note that codepage may need to be changed to Unicode in some command-line examples: in this case type `chcp 65001` on your MSDOS prompt.
This occurs if no indication is made to Perl as to what kind of input we are using.   
But if you properly code your Perl script with appropriate input/ouput management, and replace `perl.exe` with this executable in your installation, you can directly use Unicode chars as parameters: I've used two scripts of mine (that were longing for this patch!) to check this: [rnm (recursive renaming for Windows and linux)](https://github.com/circulosmeos/rnm) and [cpr (recursive copying for Windows and linux)](https://github.com/circulosmeos/cpr).

## Binary releases

Please note that these binaries are distributed *as is* without any guarantee explicit nor implied.

* [ActivePerl v5.24](https://github.com/circulosmeos/Perl-with-Unicode-for-Windows/releases/tag/ActivePerl-v5.24)

## Pending business !

I'll upload more binaries and a longer explanation of the process *asap*.

## author

* Original patch has been developed by [A. Sinan Unur](https://www.nu42.com/2017/02/perl-unicode-windows-trilogy-one.html) under [Artistic License 2.0](https://opensource.org/licenses/Artistic-2.0)
* these instructions: [circulosmeos](https://github.com/circulosmeos)

## licence

Please note that original patch by [A. Sinan Unur](https://www.nu42.com/2017/02/perl-unicode-windows-trilogy-one.html) is licensed under [Artistic License 2.0](https://opensource.org/licenses/Artistic-2.0).   
Artistic License has been [largely in use in the Perl community](https://en.wikipedia.org/wiki/Artistic_License#Artistic_License_2.0).   

The original patch by [A. Sinan Unur](https://www.nu42.com/2017/02/perl-unicode-windows-trilogy-one.html) and the one used in these instructions differ in the following header lines:
	
	#define WINVER 0x0600
	#define _WIN32_WINNT 0x0600
	#include "winnls.h"
