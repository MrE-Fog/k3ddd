#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_reader_test("K3DMeshReader","mesh.modifier.NurbsSkinnedSurfaceReference.k3d")

modifier = setup.document.new_node("NurbsSkinnedSurface")
modifier.along = 'y'
modifier.delete_original = True
modifier.mesh_selection = k3d.geometry.selection.create(1)

document.set_dependency(modifier.get_property("input_mesh"), setup.reader.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSkinnedSurface", 1)

