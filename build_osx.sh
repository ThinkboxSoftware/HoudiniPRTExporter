# Copyright 2017 Thinkbox Software Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This is a macOS build script to compile the PRT Exporter for Houdini 16.

# This program must be launched from the "Houdini Command Line Tools" or from a Terminal where Houdini's environment has been initialized.
# You will need to link with PRT-IO-Library Reference Library. It can be downloaded from the Thinkbox Github.
# You will need to link with OpenEXR and Zlib Libaries. Normally, those libraries are included in your Houdini install directory.
# This build file assumes PRT-IO-Library is in the user's home directory and that $HFS points to your Houdini install directory. Please modify this as needed.

hcustom -I $HOME/PRT-IO-Library -I $HFS/toolkit/include -I $HFS/toolkit/include/OpenEXR -L $HFS/../Libraries -l Half -l z exportPRT.cpp
