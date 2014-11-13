<h1>
	HoudiniPRTExporter
</h1>
<p>
	Python module for Houdini that allows users to export Houdini particles to Thinkbox PRT format.
</p>
<p>
	This plugin supports Houdini 12 and Houdini 13. 
</p>
<h3>
	Download Binary:
</h3>
<ul>
	<li>
		PRT Exporter downloads: https://thinkboxsoftware.box.com/s/l8sggb6upk3m4ko5irn8
	</li>
	<li>
		For Windows, the Houdini 13 download will only work on Houdini 13.0.547 or later.
	</li>
</ul>
<h3>
	Building from Source: 
</h3>
<ul>
	<li>
		Please, read the file build-instructions.txt.
	</li>
</ul>
<h3>
	Install instructions:
</h3>
<ul>
	<li>
		Ensure the plugin has either been built from source, or the pre-built binary has been downloaded.
	</li>
	<li>
		On Windows and Linux, copy the exportPRT.dll or exportPRT.so to: {Houdini Install Directory}/houdini/dso/
	</li>
	<li>
		On OS X, copy the exportPRT.dylib to: {Your Home Directory}/Library/Preferences/houdini/{Your Version Of Houdini}/dso/ or to: Users/Shared/houdini/{Your Version of Houdini}/dso/.
	</li>
</ul>
<h3>
	Running instructions:
</h3>
<ul>
	<li>
		Once the plugin has been installed, Houdini will have a new Python module called "Krakatoa".
	</li>
	<li>
		Launch Houdini, and create a new scene with a particle system.
	<li>
		To perform an export, open Python inside Houdini and use the following commands (Substitute your own node path and file path):
		<pre>
import Krakatoa
Krakatoa.exportParticles( hou.node( "/obj/particle_emitter1/popnet1" ), "my_path/output_particles.prt" ) (for Houdini 12)
or
Krakatoa.exportParticles( hou.node( "/obj/location_particles/import_location" ), "my_path/output_particles.prt" ) (for Houdini 13)
		</pre>
	</li>
</ul>
