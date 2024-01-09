import bpy
import bmesh
import math, sys
from math import *

PATH ="/home/javier/"      # La ruta donde se salvara el OREj
nframes = 1

# Get the active mesh
me = bpy.context.object.data

# Get a BMesh representation
bm = bmesh.new()
bm.from_mesh(me)

bm = bmesh.from_edit_mesh(me)


filename = PATH + me.name + ".orj"
file = open(filename, "w")
std = sys.stdout        # Asociamos stdout al fichero .orj
sys.stdout = file

print ("# Objeto OREj:", me.name)
print ("# Vertices Totales:", len(bm.verts))
print ("# Caras Totales:", len(bm.faces))

# Escribimos toda la lista de vertices (v x y z)
for vertex in bm.verts:
	x, y, z = vertex.co
	print ('v {:f} {:f} {:f}'.format(x, y ,z))
# Escribimos toda la lista de caras (f v1 v2 v3 ...)
#invertimos las normales
#bmesh.ops.reverse_faces(bm, faces=bm.faces)
for face in bm.faces:
	print ("f", end=" ")	
	for vertex in face.verts:
		print (vertex.index + 1, end=" ")
	print()
# Lista de parametros uv de cada cara (t u1 v1 u2 v2...)
uv_lay = bm.loops.layers.uv.active

for face in bm.faces:        
        print ("t", end=" ")
        for loop in face.loops:
                uv = loop[uv_lay].uv
                print('{:.2f} {:.2f}'.format(uv[0], uv[1]), end= " ")
        print()   
        

#scn= bpy.context.scene
#context=scn.getRenderingContext()

#for f in range(nframes):
#	context.cFrame=f
#	matrix = me.getMatrix()
#	print ("# FRAME ",f)
	# Escribimos la matriz de transformacion para ese frame
#	print ('m  {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f} {:f}'.format(matrix[0][0],matrix[0][1],matrix[0][2],matrix[0][3],matrix[1][0],matrix[1][1],matrix[1][2],matrix[1][3],matrix[2][0],matrix[2][1],matrix[2][2],matrix[2][3],matrix[3][0],matrix[3][1],matrix[3][2],matrix[3][3]))

sys.stdout = std
file.close()

print ("Fichero salvado:", filename)
