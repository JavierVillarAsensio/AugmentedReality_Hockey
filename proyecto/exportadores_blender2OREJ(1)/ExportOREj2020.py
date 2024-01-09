# Javier Albusac 
# Tested in Blender 2.9

import bpy
import bmesh
import math, sys, os
from math import *

PATH = os.path.expanduser("~")      # La ruta donde se salvara el OREj
# Get the active mesh
me = bpy.context.object.data

# Get a BMesh representation
bm = bmesh.new()
bm.from_mesh(me)

bm = bmesh.from_edit_mesh(me)

# Triangular el objeto para que sea interpretado por OpenGL correctamente
bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method="BEAUTY", ngon_method="BEAUTY")


filename = os.path.join(PATH, me.name + ".orj")
with open(filename, "w") as f:
    f.write("# Objeto OREj: {}\n".format(me.name))
    f.write("# Vertices Totales: {}\n".format(len(bm.verts)))
    f.write("# Caras Totales: {}\n".format(len(bm.faces)))

    # Escribimos toda la lista de vertices (v x y z)
    for vertex in bm.verts:
        x, y, z = vertex.co
        f.write('v {:f} {:f} {:f}\n'.format(x, y ,z))
    # Escribimos toda la lista de caras (f v1 v2 v3 ...)
    #invertimos las normales
    bmesh.ops.reverse_faces(bm, faces=bm.faces)
    for face in bm.faces:
        f.write("f ") 
        for vertex in face.verts:
                f.write("{} ".format(vertex.index + 1))
        f.write("\n")
    # Lista de parametros uv de cada cara (t u1 v1 u2 v2...)
    uv_lay = bm.loops.layers.uv.active

    if uv_lay is not None:
        for face in bm.faces:
                f.write("t ")
                for loop in face.loops:
                        uv = loop[uv_lay].uv
                        f.write('{:.2f} {:.2f} '.format(uv[0], uv[1]))
                f.write("\n")

print("Fichero salvado:", filename)

