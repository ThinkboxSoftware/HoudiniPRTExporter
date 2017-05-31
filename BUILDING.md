Building HoudiniPRTExporter from source
======================================= 

## Prerequisites

1. Ensure Houdini 16 is installed.
2. Ensure that the PRT I/O reference library headers are copied locally.
   https://github.com/ThinkboxSoftware/PRT-IO-Library
3. You might need to install OpenEXR libraries and headers.
   http://www.openexr.com/
4. You might need to install Zlib libraries and headers. http://www.zlib.net/


## Instructions

1. Create a local copy of this repository.
2. Modify `build.bat` (Windows), `build_linux.sh` (Linux) or `build_osx.sh`
   (macOS) to point to the prerequisite libraries. Normally, OpenEXR and zlib
   libraries are included in your Houdini Installation Folder.
3. Launch the "Houdini Command Line Tools". Alternatively, on Linux and on
   macOS, you can open a Command Line Terminal and call `source houdini_setup`
   from
   `/Library/Frameworks/Houdini.framework/Versions/{Your Houdini Version}/Resources`
   on macOS or from `{Your Houdini Installation Folder}` on Linux.
5. Run `build.bat`, `build_linux.sh` or `build_osx.sh` from the "Houdini Command
   Line Tools" or from the Command Line Terminal where Houdini's environment has
   been initialized.
