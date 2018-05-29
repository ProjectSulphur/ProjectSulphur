MADE BY RIKO OPHORST, MARCH 2018.

DISCLAIMER:
- This is a temporary workaround the fact that the editor is not at a stable point yet.
- This tool uses a version of the Sulphur Builder compiled on Friday, March 23, 2018 at 3:10 PM.
- There are quite some nooks & crannies related to the output of the Sulphur Builder.
- In future updates of Sulphur Builder, most of the stuff will be ironed out.

KNOWN BUGS (as of Friday, March 23, 2018):
- The Sulphur Builder outputs the assets in two locations. Both in the builder folder & in the specified output folder. The output in the builder folder gets cleaned after processing finishes, though.
- The Sulphur Builder is likely not to output all assets properly. Sometimes you'll notice that there's assets missing after processing, which is a bug in Sulphur Builder, and not in the tooling you find in this folder.
- When running the tooling in this folder, you'll notice that in the command line it tries to convert literally every single file it finds in the given input folder. This is because of a bug in the Sulphur Builder.
- If you run the convert_project.bat on a folder in which there is a file with no contents whatsoever, the shader pipeline will crash on it.
- While the Sulphur Builder will check the input folder (obviously), it will also simultaneously check the current working directory (aka the folder in which this readme is located). 

USAGE:
- Run the "convert_project.bat" file by double clicking it from Windows File Explorer or via command line - whatever is your preferred way of invoking batch files.
- You will be prompted to enter a path to your assets (the "input folder").
	This path can be absolute and relative.
	It's recommended to use a relative path.
	The path to the input folder MUST end in a forward slash (" / "). If you don't do that, expect lots of errors. :pray:
	The path to the input folder MUST lead to a folder in which there is two different folders: an "unprocessed" folder and a "processed" folder.
	Inside of the "unprocessed" folder, you will keep all your assets for your project.
	Inside of the "processed" folder, you will find all the output that the Sulphur Builder generates during processing.
	Hit Enter to confirm your input folder.
- Next up, you will be asked to enter the NAME of the default VERTEX shader you want to use.
	This is required because in order to process the models, the Sulphur Builder must know what vertex shader to assign to them.
- Next up, you will be asked to enter the NAME of the default PIXEL shader you want to use.
	This is required because in order to process the models, the Sulphur Builder must know what pixel shader to assign to them.
- Once you've done that, grab some coffee, go for a smoke & come back in 10 minutes or so. 
	Asset processing unfortunately takes a while...

WHAT IS ACTUALLY HAPPENING?
- When you run the "convert_project.bat" file, obviously a batch file gets executed.
- The first step is prompting you for a path to the input folder.
- Next, you are prompted for the default vertex shader to use for all models.
- Next, you are prompted for the default pixel shader to use for all models.
- Next, a clean command is ran on the "processed" folder inside your input folder.
	This happens because the Sulphur Builder currently doesn't update assets that already exist. Instead, it will just create duplicates of them.
- Next, all shaders in the input folder will get processed using the "--convert_shaders -dir -r -output" command.
- Next, all models in the input folder will get processed using the "--convert_models -dir -r -output -vertex -pixel" command.
- Next, all animations in the input folder will get processed using the "--convert_animations -dir -r -output" command.
- Next, all skeletons in the input folder will get processed using the "--convert_skeletons -dir -r -output" command.
- Next, all textures in the input folder will get processed using the "--convert_textures -dir -r -output" command.
- Next, all scripts in the input folder will get processed using the "--convert_scripts -dir -r -output" command.
- Finally, a clean operation gets executed on the contents of the builder folder.
	See the first of the set of "KNOWN BUGS" for an explanation.
- Asset processing done!