import Blender
import math, sys
from Blender import NMesh, Object, Scene
from math import *

PATH ="/home/carlos/"      # La ruta donde se salvara el OREj
nframes = 50

object = Object.GetSelected()
objname = object[0].name
meshname = object[0].data.name
mesh = NMesh.GetRaw(meshname)

filename = PATH + objname + ".orj"
file = open(filename, "w")
std = sys.stdout        # Asociamos stdout al fichero .orj
sys.stdout = file

print "# Objeto OREj:", objname
print "# Vertices Totales:", len(mesh.verts)
print "# Caras Totales:", len(mesh.faces)

# Escribimos toda la lista de vertices (v x y z)
for vertex in mesh.verts:
	x, y, z = vertex.co
	print "v %f %f %f" % (x, y, z)
# Escribimos toda la lista de caras (f v1 v2 v3 ...)
for face in mesh.faces:
	print "f",
	face.v.reverse()    # Invertimos las normales 
	for vertex in face.v:
		print vertex.index + 1,
	print
# Lista de parametros uv de cada cara (t u1 v1 u2 v2...)
for face in mesh.faces:
	nvertex = 0
	face.v.reverse()    
	print "t",
	for vertex in face.v:		
		print face.uv[nvertex][0], face.uv[nvertex][1],
		nvertex = nvertex + 1
	print 

scn= Scene.GetCurrent()
context=scn.getRenderingContext()

for f in range(nframes):
	context.cFrame=f
	matrix = object[0].getMatrix()
	print "# FRAME ",f
	# Escribimos la matriz de transformacion para ese frame
	print "m  %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f" % (matrix[0][0],matrix[0][1],matrix[0][2],matrix[0][3],matrix[1][0],matrix[1][1],matrix[1][2],matrix[1][3],matrix[2][0],matrix[2][1],matrix[2][2],matrix[2][3],matrix[3][0],matrix[3][1],matrix[3][2],matrix[3][3])

sys.stdout = std
file.close()

print "Fichero salvado:", filename