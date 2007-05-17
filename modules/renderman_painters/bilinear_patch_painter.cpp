// K-3D
// Copyright (c) 1995-2005, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Timothy M. Shead (tshead@k-3d.com)
*/

#include "array_helpers.h"

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/i18n.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/imesh_painter_ri.h>
#include <k3dsdk/node.h>
#include <k3dsdk/persistent.h>
#include <k3dsdk/renderable_ri.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/utility_gl.h>

namespace libk3drendermanpainters
{

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch_painter

class bilinear_patch_painter :
	public k3d::persistent<k3d::node>,
	public k3d::ri::imesh_painter
{
	typedef k3d::persistent<k3d::node> base;

public:
	bilinear_patch_painter(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document)
	{
	}

	void paint_mesh(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
		if(!k3d::validate_bilinear_patches(Mesh))
			return;

		const k3d::mesh::materials_t& patch_materials = *Mesh.bilinear_patches->patch_materials;
		const k3d::mesh::named_arrays& constant_data = Mesh.bilinear_patches->constant_data;
		const k3d::mesh::named_arrays& uniform_data = Mesh.bilinear_patches->uniform_data;
		const k3d::mesh::indices_t& patch_points = *Mesh.bilinear_patches->patch_points;
		const k3d::mesh::points_t& points = *Mesh.points;
		const k3d::mesh::named_arrays& varying_data = Mesh.bilinear_patches->varying_data;
		const k3d::mesh::named_arrays& vertex_data = Mesh.vertex_data;

		const size_t patch_begin = 0;
		const size_t patch_end = patch_begin + (patch_points.size() / 4);
		for(size_t patch = patch_begin; patch != patch_end; ++patch)
		{
			array_copier ri_constant_data;
			ri_constant_data.add_arrays(constant_data);

			array_copier ri_uniform_data;
			ri_uniform_data.add_arrays(uniform_data);

			array_copier ri_varying_data;
			ri_varying_data.add_arrays(varying_data);

			array_copier ri_vertex_data;
			ri_vertex_data.add_arrays(vertex_data);
			ri_vertex_data.add_array(k3d::ri::RI_P(), points);

			const size_t patch_points_begin = 4 * patch;
			const size_t patch_points_end = patch_points_begin + 4;
			for(size_t patch_point = patch_points_begin; patch_point != patch_points_end; ++patch_point)
				ri_vertex_data.push_back(patch_points[patch_point]);

			ri_constant_data.push_back(patch);
			ri_uniform_data.push_back(patch);
			ri_varying_data.insert(4 * patch, 4 * (patch + 1));

			k3d::ri::parameter_list ri_parameters;
			ri_constant_data.copy_to(k3d::ri::CONSTANT, ri_parameters);
			ri_uniform_data.copy_to(k3d::ri::UNIFORM, ri_parameters);
			ri_varying_data.copy_to(k3d::ri::VARYING, ri_parameters);
			ri_vertex_data.copy_to(k3d::ri::VERTEX, ri_parameters);

			k3d::ri::setup_material(patch_materials[patch], RenderState);
			RenderState.engine.RiPatchV("bilinear", ri_parameters);
		}
	}

	void paint_complete(const k3d::mesh& Mesh, const k3d::ri::render_state& RenderState)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<bilinear_patch_painter, k3d::interface_list<k3d::ri::imesh_painter > > factory(
			k3d::uuid(0x3f51b275, 0x5a664903, 0x96ed9168, 0x503e30f0),
			"RenderManBilinearPatchPainter",
			_("Renders bilinear patches"),
			"RenderMan Painters",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

/////////////////////////////////////////////////////////////////////////////
// bilinear_patch_painter_factory

k3d::iplugin_factory& bilinear_patch_painter_factory()
{
	return bilinear_patch_painter::get_factory();
}

} // namespace libk3drendermanpainters

