HoudiniPRTExporter
==================

Python module for Houdini that allows users to export Houdini particles to
Thinkbox PRT format.

This plugin supports Houdini 16.


## Download Binary

PRT Exporter downloads: https://thinkboxsoftware.box.com/s/l8sggb6upk3m4ko5irn8


## Building from Source

See [BUILDING.md](BUILDING.md).


## Install instructions

Ensure the plugin has either been built from source, or the pre-built binary has
been downloaded.

On Windows and Linux, copy `exportPRT.dll` or `exportPRT.so` to
`{Houdini Install Directory}/houdini/dso/`.

On macOS, copy `exportPRT.dylib` to
`{Your Home Directory}/Library/Preferences/houdini/{Your Version Of Houdini}/dso/`
or to `/Users/Shared/houdini/{Your Version of Houdini}/dso/`.


## Running instructions

Once the plugin has been installed, Houdini will have a new Python module called
`Krakatoa`.

Launch Houdini, and create a new scene with a particle system.

To perform an export, open Python inside Houdini and use the following commands
(substitute your own node path and file path):

```python
import Krakatoa

Krakatoa.exportParticles(hou.node('/obj/fireworks_particles/import_fireworks'),
                         'my_path/output_particles.prt')
```
