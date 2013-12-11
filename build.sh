# Copyright 2013 Thinkbox Software Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.




# This is a Linux build script to compile the PRT Exporter for Houdini 12.

# This program must be launched from the "Houdini Command Line Tools".
# You will need to link with OpenEXR, Zlib, and the PRT I/O Reference Library. This build file assumes OpenEXR is installed and that $HFS points to the Houdini install directory. Please modify this as needed.
# The prtio library is required. It can be downloaded from the Thinkbox Github.

hcustom -I $HOME/prtio/ -I $HFS/toolkit/include/zlib -I /usr/include/OpenEXR -L $HFS/dsolib -L /usr/lib exportPRT.cpp -l Half -l z -e
