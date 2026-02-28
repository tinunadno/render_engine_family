#!/usr/bin/env python3
"""
Generate a 3D heart model with two materials.
Left side uses Material.001 (1.png), right side uses Material (2.png)
"""

import math

def heart_x(u, v):
    """Heart x coordinate: x = 16 * sin^3(u) * thickness"""
    thickness = v * 3  # Scale thickness
    return 16 * (math.sin(u) ** 3) * thickness

def heart_y(u, v):
    """Heart y coordinate: y = (13*cos(u) - 5*cos(2u) - 2*cos(3u) - cos(4u)) * thickness"""
    thickness = v * 3
    return (13 * math.cos(u) - 5 * math.cos(2*u) - 2 * math.cos(3*u) - math.cos(4*u)) * thickness

def heart_z(u, v):
    """Heart z coordinate: z = depth"""
    return v * 3

# Parameters
u_steps = 50  # Resolution around the heart
v_steps = 20  # Resolution through thickness
u_range = (0, 2 * math.pi)
v_range = (0.5, 1.0)  # Only front half for better visibility

# Generate vertices
vertices = []
uv_coords = []
normals = []

for v_idx in range(v_steps):
    v = v_range[0] + (v_range[1] - v_range[0]) * v_idx / (v_steps - 1)

    for u_idx in range(u_steps):
        u = u_range[0] + (u_range[1] - u_range[0]) * u_idx / (u_steps - 1)

        x = heart_x(u, v)
        y = heart_y(u, v)
        z = heart_z(u, v)

        vertices.append(f"v {x:.4f} {y:.4f} {z:.4f}")

        # UV coordinates based on position
        uv_u = u / (2 * math.pi)
        uv_v = (v - v_range[0]) / (v_range[1] - v_range[0])
        uv_coords.append(f"vt {uv_u:.4f} {uv_v:.4f}")

        # Normal pointing outward (simplified, pointing in +z direction)
        normals.append("vn 0.0 0.0 1.0")

# Write OBJ file
with open("heart.obj", "w") as f:
    f.write("# 3D Heart Model\n")
    f.write("# Left side (x < 0): Material.001 (1.png)\n")
    f.write("# Right side (x >= 0): Material (2.png)\n")
    f.write("mtllib heart.mtl\n")
    f.write("\n")

    # Write vertices, UVs, normals
    for v in vertices:
        f.write(v + "\n")
    for uv in uv_coords:
        f.write(uv + "\n")
    for n in normals:
        f.write(n + "\n")
    f.write("\n")

    # Collect faces by material
    material_001_faces = []  # Left side (x < 0)
    material_faces = []       # Right side (x >= 0)

    for v_idx in range(v_steps - 1):
        for u_idx in range(u_steps - 1):
            # Get vertex indices
            i0 = v_idx * u_steps + u_idx + 1  # +1 for OBJ 1-based indexing
            i1 = i0 + 1
            i2 = i0 + u_steps
            i3 = i2 + 1

            # Get vertex coordinates
            x0, y0, z0 = map(float, vertices[i0-1].split()[1:])

            # Determine material based on x coordinate
            face_data = (i0, i1, i2, i3)

            if x0 < 0:
                material_001_faces.append(face_data)  # Left side - 1.png
            else:
                material_faces.append(face_data)       # Right side - 2.png

    # Write Material.001 faces (left side)
    f.write("usemtl Material.001\n")
    for face in material_001_faces:
        i0, i1, i2, i3 = face
        f.write(f"f {i0}/{i0}/{i0} {i1}/{i1}/{i1} {i2}/{i2}/{i2}\n")
        f.write(f"f {i1}/{i1}/{i1} {i3}/{i3}/{i3} {i2}/{i2}/{i2}\n")

    # Write Material faces (right side)
    f.write("usemtl Material\n")
    for face in material_faces:
        i0, i1, i2, i3 = face
        f.write(f"f {i0}/{i0}/{i0} {i1}/{i1}/{i1} {i2}/{i2}/{i2}\n")
        f.write(f"f {i1}/{i1}/{i1} {i3}/{i3}/{i3} {i2}/{i2}/{i2}\n")

print(f"Generated heart.obj with {len(vertices)} vertices")
print(f"Material.001 (1.png) faces: {len(material_001_faces) * 2}")
print(f"Material (2.png) faces: {len(material_faces) * 2}")
print("Material split: x < 0 -> Material.001 (1.png), x >= 0 -> Material (2.png)")
