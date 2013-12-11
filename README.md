<h1>
	HoudiniPRTExporter
</h1>
<p>
	Python module for Houdini 12 that allows users to export Houdini particles to Thinkbox PRT format.
</p>
<h3>
	Download Binary:
</h3>
<ul>
	<li>
		PRT Exporter for Windows download: https://thinkboxsoftware.box.com/shared/static/yg9wosstpujuhlvmf5rv.zip
	</li>
	<li>
		PRT Exporter for Linux download: https://thinkboxsoftware.box.com/shared/static/a0h85xykamx2p35vtgjs.zip
	</li>
</ul>
<h3>
	Building from Source:
</h3>
<ul>
	<li>
		Prerequisites:
		<ul>
			<li>
				Ensure Houdini 12 is installed.
			</li>
			<li>
				Ensure that OpenEXR libraries and headers are installed. http://www.openexr.com/
			</li>
			<li>
				Ensure that Zlib libraries and headers are installed. http://www.zlib.net/
			</li>
			<li>
				Ensure that the PRT I/O reference library headers are copied locally. https://github.com/ThinkboxSoftware/PRT-IO-Library
			</li>
		</ul>
	</li>
	<li>
		Create a local copy of this repository.
	</li>
	<li>
		Modify build.bat (Windows) or build.sh (Linux) to point to the prerequisite libraries.
	</li>
	<li>
		Launch the "Houdini Command Line Tools".
	</li>
	<li>
		Run build.bat or build.sh from the "Houdini Command Line Tools".
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
		Copy the exportPRT.dll (Windows) or exportPRT.so (Linux) to: {Houdini Install Directory}/houdini/dso/
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
Krakatoa.exportParticles( hou.node( "/obj/particle_emitter1/popnet1" ), "my_path/output_particles.prt" )
		</pre>
	</li>
</ul>
