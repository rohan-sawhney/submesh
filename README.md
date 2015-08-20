# submesh 
Breaks a mesh into submeshes through a face based and edge based approach. Face based approach works with non-manifold meshes
but expects triangle faces and unique vertices as input. Edge based approach works with only orientable manifold meshes expects unique vertices as input but works on arbitary faces.

Note: Requires Eigen 3.2.4 and assumes it is in /usr/local/Cellar/eigen/3.2.4/include/eigen3/
