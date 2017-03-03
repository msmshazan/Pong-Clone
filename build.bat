@echo off
set CompilerFlags= /Z7 /FC /nologo /Od -fp:fast -Gm- -GR- -EHa- /Oi -WX -W4 -wd4189 -wd4702 -wd4505 -wd4100 -wd4456 -wd4244 -wd4201
set LinkerFlags=-subsystem:Console 
set bits=x64
set LibraryLocation= ..\deps\lib\%bits%\

mkdir build > NUL 2> NUL
pushd build
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" %bits%
del *.pdb > NUL 2> NUL
ctime -begin sdl.ctm
cl %CompilerFlags% /MTd ..\code\gamecode.cpp -Fmgamecode.map -LD /link -incremental:no -opt:ref -PDB:handmade_%RANDOM%.pdb -EXPORT:GameUpdateAndRender -out:GameCode.dll
cl %CompilerFlags% /MD ..\code\sdllayer.cpp /I..\deps\include   /link -incremental:no %LibraryLocation%SDL2.lib %LibraryLocation%SDL2main.lib opengl32.lib  %LinkerFlags% -out:Pong.exe
ctime -end sdl.ctm
popd
