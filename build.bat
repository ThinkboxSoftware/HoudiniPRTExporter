@REM   Copyright 2013 Thinkbox Software Inc.
@REM   .
@REM   Licensed under the Apache License, Version 2.0 (the "License");
@REM   you may not use this file except in compliance with the License.
@REM   You may obtain a copy of the License at
@REM   .
@REM       http://www.apache.org/licenses/LICENSE-2.0
@REM   .
@REM   Unless required by applicable law or agreed to in writing, software
@REM   distributed under the License is distributed on an "AS IS" BASIS,
@REM   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@REM   See the License for the specific language governing permissions and
@REM   limitations under the License.




@REM   This is a Windows build script to compile the PRT Exporter for Houdini 12.

@REM   This program must be launched from the "Houdini Command Line Tools".
@REM   You will need to link with OpenEXR, Zlib, and the PRT I/O Reference Library. This build file assumes they are in the user's home directory. Please modify this as needed.
@REM   The prtio library is required. It can be downloaded from the Thinkbox Github.

@REM If vcvarsall.bat is not found, you need to install Visual Studio 2005
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86_amd64

hcustom -I %userprofile%\prtio\ -I  %userprofile%\OpenEXR\include\ -I  %userprofile%\zlib\include -L  %userprofile%\OpenEXR\lib-vc90-x64\ -l Half.lib exportPRT.cpp
if errorlevel 1 (
  echo.
  echo.
  echo ** ERROR **
  echo You likely need to start the command prompt using the Houdini Command Line Tools from the Start menu.
  echo.
  echo.
  pause
)
