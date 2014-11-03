@REM   	Copyright 2013 Thinkbox Software Inc.
@REM   	.
@REM   	Licensed under the Apache License, Version 2.0 (the "License");
@REM   	you may not use this file except in compliance with the License.
@REM   	You may obtain a copy of the License at
@REM   	.
@REM       	http://www.apache.org/licenses/LICENSE-2.0
@REM   	.
@REM   	Unless required by applicable law or agreed to in writing, software
@REM   	distributed under the License is distributed on an "AS IS" BASIS,
@REM   	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@REM   	See the License for the specific language governing permissions and
@REM   	limitations under the License.

@REM   	This is a Windows build script to compile the PRT Exporter for Houdini 12 or Houdini 13.

@REM   	This program must be launched from the "Houdini Command Line Tools".
@REM 	You will need to link with PRT-IO-Library Reference Library. It can be downloaded from the Thinkbox Github.
@REM   	You will need to link with OpenEXR and Zlib Libaries. Normally, those libraries are included in your Houdini install directory.
@REM	This build file assumes PRT-IO-Library is in the user's home directory and that %HFS% points to your Houdini install directory. Please modify this as needed.

hcustom -I %userprofile%\PRT-IO-Library\ -I %HFS%\toolkit\include\OpenEXR\ -I %HFS%\toolkit\include\zlib -L %HFS%\custom\houdini\dsolib\ -l Half.lib exportPRT.cpp
