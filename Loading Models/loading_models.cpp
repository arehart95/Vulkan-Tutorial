/*	The program is now ready to render textured 3D meshes but the current geometry in the vertices
	and indices arrays is not very interesting yet. From here we are going to extend the program to
	load the vertices and indices from an actual model file to make the graphics card actually do
	some work. 
	
	Lots of graphics API tutorials have the reader write their own OBJ loader. However this presents
	a problem like any remotely interesting 3D application will require features that are not 
	supported by this file format, such as skeletal animation. We will load mesh data from an OBJ 
	model in this chapter, but we will focus more on integrating the mesh data with the program
	itself rather than the details of loading it from a file.
*/
